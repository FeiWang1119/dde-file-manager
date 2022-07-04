/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "vaulthelper.h"
#include "vaultdefine.h"
#include "pathmanager.h"
#include "views/vaultcreatepage.h"
#include "views/vaultunlockpages.h"
#include "views/vaultremovepages.h"
#include "views/vaultpropertyview/vaultpropertydialog.h"
#include "utils/encryption/vaultconfig.h"
#include "utils/vaultautolock.h"
#include "utils/servicemanager.h"
#include "utils/policy/policymanager.h"
#include "events/vaulteventcaller.h"
#include "dbus/vaultdbusutils.h"

#include "services/common/delegate/delegateservice.h"

#include "dfm-base/base/urlroute.h"
#include "dfm-base/utils/universalutils.h"
#include "dfm-base/utils/dialogmanager.h"
#include "dfm-base/dfm_event_defines.h"
#include "dfm-base/dfm_global_defines.h"
#include "dfm-base/base/application/settings.h"
#include "dfm-base/widgets/dfmwindow/filemanagerwindowsmanager.h"

#include <dfm-framework/event/event.h>

#include <dfm-io/dfmio_utils.h>

#include <QUrl>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QApplication>

Q_DECLARE_METATYPE(QList<QUrl> *)

DSB_FM_USE_NAMESPACE
DFMBASE_USE_NAMESPACE
DSC_USE_NAMESPACE
using namespace dfmplugin_vault;

QUrl VaultHelper::rootUrl()
{
    QUrl url;
    url.setScheme(scheme());
    QString path = "/";
    url.setPath(path);
    url.setHost("");
    return url;
}

QUrl VaultHelper::sourceRootUrl()
{
    QUrl url;
    url.setScheme(scheme());
    QString path = PathManager::makeVaultLocalPath(QString(""), kVaultDecryptDirName);
    url.setPath(path);
    url.setHost("");
    return url;
}

QUrl VaultHelper::pathToVaultVirtualUrl(const QString &path)
{
    QString localPath = instance()->sourceRootUrl().path();
    if (path.contains(localPath)) {
        QString virtualPath = path;
        virtualPath = virtualPath.replace(0, localPath.length(), "");
        QUrl virtualUrl;
        virtualUrl.setPath(virtualPath);
        virtualUrl.setScheme(scheme());
        virtualUrl.setHost("");
        return virtualUrl;
    }
    return QUrl();
}

/*!
 * \brief 用于右侧栏保险箱右键菜单创建
 * \param windowId 窗口ID
 * \param url      保险箱Url
 * \param globalPos 右键菜单显示坐标
 */
void VaultHelper::contenxtMenuHandle(quint64 windowId, const QUrl &url, const QPoint &globalPos)
{
    VaultHelper::instance()->appendWinID(windowId);
    QMenu *menu = createMenu();
    menu->exec(globalPos);
    delete menu;
}

/*!
 * \brief 用于右侧栏保险箱点击处理
 * \param windowId 窗口ID
 * \param url      保险箱Url
 */
void VaultHelper::siderItemClicked(quint64 windowId, const QUrl &url)
{
    QApplication::restoreOverrideCursor();
    VaultHelper::instance()->appendWinID(windowId);

    switch (instance()->state(PathManager::vaultLockPath())) {
    case VaultState::kNotExisted: {
        VaultHelper::instance()->creatVaultDialog();
    } break;
    case VaultState::kEncrypted: {
        VaultHelper::instance()->unlockVaultDialog();
    } break;
    case VaultState::kUnlocked:
        instance()->defaultCdAction(windowId, url);
        recordTime(kjsonGroupName, kjsonKeyInterviewItme);
        break;
    case VaultState::kUnderProcess:
    case VaultState::kBroken:
    case VaultState::kNotAvailable:
        break;
    }
}

VaultState VaultHelper::state(QString lockBaseDir)
{
    QString cryfsBinary = QStandardPaths::findExecutable("cryfs");
    if (cryfsBinary.isEmpty()) {
        // 记录保险箱状态
        return VaultState::kNotAvailable;
    }

    if (lockBaseDir.isEmpty()) {
        lockBaseDir = PathManager::vaultLockPath() + "cryfs.config";
    } else {
        if (lockBaseDir.endsWith("/"))
            lockBaseDir += "cryfs.config";
        else
            lockBaseDir += "/cryfs.config";
    }

    if (QFile::exists(lockBaseDir)) {
        QUrl baseDirUrl = QUrl::fromLocalFile(PathManager::vaultUnlockPath());
        const QString &fsType = DFMIO::DFMUtils::fsTypeFromUrl(baseDirUrl);
        if (fsType == "fuse.cryfs") {
            return VaultState::kUnlocked;
        }
        return VaultState::kEncrypted;
    } else {
        return VaultState::kNotExisted;
    }
}

void VaultHelper::defaultCdAction(const quint64 windowId, const QUrl &url)
{
    VaultEventCaller::sendItemActived(windowId, url);
}

void VaultHelper::openNewWindow(const QUrl &url)
{
    VaultEventCaller::sendOpenWindow(url);
}

bool VaultHelper::getVaultVersion()
{
    VaultConfig config;
    QString strVersion = config.get(kConfigNodeName, kConfigKeyVersion).toString();
    if (!strVersion.isEmpty() && strVersion != kConfigVaultVersion)
        return true;

    return false;
}

void VaultHelper::killVaultTasks()
{
}

quint64 VaultHelper::currentWindowId()
{
    return currentWinID;
}

void VaultHelper::removeWinID(const quint64 &winId)
{
    if (winIDs.contains(winId)) {
        winIDs.removeOne(winId);
    }
}

void VaultHelper::appendWinID(const quint64 &winId)
{
    currentWinID = winId;
    if (!winIDs.contains(winId))
        winIDs.append(winId);
}

QMenu *VaultHelper::createMenu()
{
    QMenu *menu = new QMenu;
    QMenu *timeMenu = new QMenu;
    switch (instance()->state(PathManager::vaultLockPath())) {
    case VaultState::kNotExisted:
        menu->addAction(QObject::tr("Create Vault"), VaultHelper::instance(), &VaultHelper::creatVaultDialog);
        break;
    case VaultState::kEncrypted:
        menu->addAction(QObject::tr("Unlock"), VaultHelper::instance(), &VaultHelper::unlockVaultDialog);
        break;
    case VaultState::kUnlocked: {
        menu->addAction(QObject::tr("Open"), VaultHelper::instance(), &VaultHelper::openWindow);

        menu->addAction(QObject::tr("Open in new window"), VaultHelper::instance(), &VaultHelper::newOpenWindow);

        menu->addSeparator();

        menu->addAction(QObject::tr("Lock"), VaultHelper::instance(), &VaultHelper::lockVault);

        QAction *timeLock = new QAction;
        timeLock->setText(QObject::tr("Auto lock"));
        VaultAutoLock::AutoLockState autoState = VaultAutoLock::instance()->getAutoLockState();
        QAction *actionNever = timeMenu->addAction(QObject::tr("Never"), []() {
            VaultAutoLock::instance()->autoLock(VaultAutoLock::AutoLockState::kNever);
        });
        actionNever->setCheckable(true);
        actionNever->setChecked(VaultAutoLock::AutoLockState::kNever == autoState ? true : false);
        timeMenu->addSeparator();
        QAction *actionFiveMins = timeMenu->addAction(QObject::tr("5 minutes"), []() {
            VaultAutoLock::instance()->autoLock(VaultAutoLock::AutoLockState::kFiveMinutes);
        });
        actionFiveMins->setCheckable(true);
        actionFiveMins->setChecked(VaultAutoLock::AutoLockState::kFiveMinutes == autoState ? true : false);
        QAction *actionTenMins = timeMenu->addAction(QObject::tr("10 minutes"), []() {
            VaultAutoLock::instance()->autoLock(VaultAutoLock::AutoLockState::kTenMinutes);
        });
        actionTenMins->setCheckable(true);
        actionTenMins->setChecked(VaultAutoLock::AutoLockState::kTenMinutes == autoState ? true : false);
        QAction *actionTwentyMins = timeMenu->addAction(QObject::tr("20 minutes"), []() {
            VaultAutoLock::instance()->autoLock(VaultAutoLock::AutoLockState::kTwentyMinutes);
        });
        actionTwentyMins->setCheckable(true);
        actionTwentyMins->setChecked(VaultAutoLock::AutoLockState::kTwentyMinutes == autoState ? true : false);
        timeLock->setMenu(timeMenu);

        menu->addMenu(timeMenu);

        menu->addSeparator();

        menu->addAction(QObject::tr("Delete File Vault"), VaultHelper::instance(), &VaultHelper::removeVaultDialog);

        menu->addAction(QObject::tr("Properties"), []() {
            VaultEventCaller::sendVaultProperty(VaultHelper::instance()->rootUrl());
        });
    } break;
    case VaultState::kUnderProcess:
    case VaultState::kBroken:
    case VaultState::kNotAvailable:
        break;
    }

    return menu;
}

QWidget *VaultHelper::createVaultPropertyDialog(const QUrl &url)
{
    static VaultPropertyDialog *vaultDialog = nullptr;
    bool flg = UniversalUtils::urlEquals(VaultHelper::instance()->rootUrl(), url);
    QUrl tempUrl = VaultHelper::instance()->sourceRootUrl();
    tempUrl.setScheme(url.scheme());
    bool flg1 = UniversalUtils::urlEquals(tempUrl, url);
    if (flg || flg1) {
        if (!vaultDialog) {
            vaultDialog = new VaultPropertyDialog();
            vaultDialog->selectFileUrl(url);
            connect(vaultDialog, &VaultPropertyDialog::finished, []() { vaultDialog = nullptr; });
            return vaultDialog;
        }
        return vaultDialog;
    }
    return nullptr;
}

QUrl VaultHelper::vaultToLocalUrl(const QUrl &url)
{
    if (url.scheme() != instance()->scheme())
        return QUrl();
    if (url.path().contains(instance()->sourceRootUrl().path())) {
        QUrl localUrl = QUrl::fromLocalFile(url.path());
        QFileInfo info(localUrl.path());
        return localUrl;
    } else {
        QUrl localUrl = QUrl::fromLocalFile(instance()->sourceRootUrl().path() + url.path());
        QFileInfo info(localUrl.path());
        return localUrl;
    }
}

void VaultHelper::createVault(QString &password)
{
    static bool flg = true;
    if (flg) {
        connect(ServiceManager::fileEncryptServiceInstance(), &FileEncryptService::signalCreateVaultState, VaultHelper::instance(), &VaultHelper::sigCreateVault);
        flg = false;
    }
    ServiceManager::fileEncryptServiceInstance()->createVault(PathManager::vaultLockPath(), PathManager::vaultUnlockPath(), password);
}

void VaultHelper::unlockVault(QString &password)
{
    static bool flg = true;
    if (flg) {
        connect(ServiceManager::fileEncryptServiceInstance(), &FileEncryptService::signalUnlockVaultState, VaultHelper::instance(), &VaultHelper::sigUnlocked);
        flg = false;
    }
    ServiceManager::fileEncryptServiceInstance()->unlockVault(PathManager::vaultLockPath(), PathManager::vaultUnlockPath(), password);
}

void VaultHelper::lockVault()
{
    static bool flg = true;
    if (flg) {
        connect(ServiceManager::fileEncryptServiceInstance(), &FileEncryptService::signalLockVaultState, VaultHelper::instance(), &VaultHelper::slotlockVault);
        flg = false;
    }
    ServiceManager::fileEncryptServiceInstance()->lockVault(PathManager::vaultUnlockPath());
}

void VaultHelper::creatVaultDialog()
{
    VaultPageBase *page = new VaultActiveView();
    page->moveToCenter();
    page->exec();
}

void VaultHelper::unlockVaultDialog()
{
    VaultUnlockPages *page = new VaultUnlockPages();
    page->pageSelect(PageType::kUnlockPage);
    page->moveToCenter();
    page->exec();
}

void VaultHelper::removeVaultDialog()
{
    VaultPageBase *page = new VaultRemovePages();
    page->moveToCenter();
    page->exec();
}

void VaultHelper::openWindow()
{
    defaultCdAction(VaultHelper::instance()->currentWindowId(),
                    VaultHelper::instance()->rootUrl());
}

void VaultHelper::openWidWindow(quint64 winID, const QUrl &url)
{
    VaultEventCaller::sendItemActived(winID, url);
}

void VaultHelper::newOpenWindow()
{
    openNewWindow(rootUrl());
}

QList<QUrl> VaultHelper::transUrlsToLocal(const QList<QUrl> &urls)
{
    QList<QUrl> urlsTrans {};
    bool ok = dpfHookSequence->run("dfmplugin_utils", "hook_UrlsTransform", urls, &urlsTrans);
    if (ok && !urlsTrans.isEmpty())
        return urlsTrans;

    return urls;
}

void VaultHelper::slotlockVault(int state)
{
    if (state == 0) {
        VaultAutoLock::instance()->slotLockVault(state);
        emit VaultHelper::instance()->sigLocked(state);
        QUrl url;
        url.setScheme(QString(Global::Scheme::kComputer));
        url.setPath("/");
        for (quint64 wid : winIDs) {
            defaultCdAction(wid, url);
        }
        recordTime(kjsonGroupName, kjsonKeyLockTime);
    }
}

VaultHelper *VaultHelper::instance()
{
    static VaultHelper vaultHelper;
    return &vaultHelper;
}

void VaultHelper::recordTime(const QString &group, const QString &key)
{
    Settings setting(kVaultTimeConfigFile);
    setting.setValue(group, key, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

bool VaultHelper::urlsToLocal(const QList<QUrl> &origins, QList<QUrl> *urls)
{
    if (!urls)
        return false;
    for (const QUrl &url : origins) {
        if (url.scheme() != VaultHelper::scheme())
            return false;
        (*urls).push_back(vaultToLocalUrl(url));
    }
    return true;
}

bool VaultHelper::cutFile(const quint64 windowId, const QList<QUrl> sources, const QUrl target, const AbstractJobHandler::JobFlags flags)
{
    if (target.scheme() != scheme())
        return false;

    QList<QUrl> actualUrls;
    for (const QUrl &url : sources) {
        if (FileUtils::isComputerDesktopFile(url) || FileUtils::isTrashDesktopFile(url)) {
            continue;
        } else {
            actualUrls << url;
        }
    }
    const QUrl url = transUrlsToLocal({ target }).first();
    dpfSignalDispatcher->publish(GlobalEventType::kCutFile, windowId, actualUrls, url, flags, nullptr);

    return true;
}

bool VaultHelper::copyFile(const quint64 windowId, const QList<QUrl> sources, const QUrl target, const AbstractJobHandler::JobFlags flags)
{
    if (target.scheme() != scheme())
        return false;

    QList<QUrl> actualUrls;
    for (const QUrl &url : sources) {
        if (FileUtils::isComputerDesktopFile(url) || FileUtils::isTrashDesktopFile(url)) {
            continue;
        } else {
            actualUrls << url;
        }
    }

    // if use &, transUrlsToLocal return value will free, and url is invalid, app crash, the same below
    const QUrl url = transUrlsToLocal({ target }).first();
    dpfSignalDispatcher->publish(GlobalEventType::kCopy, windowId, actualUrls, url, flags, nullptr);
    return true;
}

bool VaultHelper::moveToTrash(const quint64 windowId, const QList<QUrl> sources, const AbstractJobHandler::JobFlags flags)
{
    if (sources.isEmpty())
        return false;
    if (sources.first().scheme() != scheme())
        return false;

    Q_UNUSED(flags)
    QList<QUrl> redirectedFileUrls = transUrlsToLocal(sources);

    dpfSignalDispatcher->publish(GlobalEventType::kDeleteFiles,
                                 windowId,
                                 redirectedFileUrls, flags, nullptr);
    return true;
}

bool VaultHelper::deleteFile(const quint64 windowId, const QList<QUrl> sources, const AbstractJobHandler::JobFlags flags)
{
    if (sources.isEmpty())
        return false;
    if (sources.first().scheme() != scheme())
        return false;

    QList<QUrl> redirectedFileUrls = transUrlsToLocal(sources);

    dpfSignalDispatcher->publish(GlobalEventType::kDeleteFiles,
                                 windowId,
                                 redirectedFileUrls, flags, nullptr);
    return true;
}

bool VaultHelper::openFileInPlugin(quint64 windowId, const QList<QUrl> urls)
{
    if (urls.isEmpty())
        return false;
    if (urls.first().scheme() != scheme())
        return false;

    QList<QUrl> redirectedFileUrls = transUrlsToLocal(urls);

    if (!redirectedFileUrls.isEmpty())
        VaultEventCaller::sendOpenFiles(windowId, redirectedFileUrls);

    return true;
}

bool VaultHelper::renameFile(const quint64 windowId, const QUrl oldUrl, const QUrl newUrl, const AbstractJobHandler::JobFlags flags)
{
    if (oldUrl.scheme() != scheme())
        return false;

    const QUrl ourl = transUrlsToLocal({ oldUrl }).first();
    const QUrl nurl = transUrlsToLocal({ newUrl }).first();
    dpfSignalDispatcher->publish(GlobalEventType::kRenameFile, windowId, ourl, nurl, flags);

    return true;
}

bool VaultHelper::makeDir(const quint64 windowId, const QUrl url)
{
    if (url.scheme() != scheme())
        return false;

    const QUrl dirUrl = transUrlsToLocal({ url }).first();
    dpfSignalDispatcher->publish(GlobalEventType::kMkdir, windowId, dirUrl);

    return true;
}

bool VaultHelper::touchFile(const quint64 windowId, const QUrl url, const DFMGLOBAL_NAMESPACE::CreateFileType type, QString *error)
{
    if (url.scheme() != scheme())
        return false;

    const QUrl dirUrl = transUrlsToLocal({ url }).first();
    dpfSignalDispatcher->publish(GlobalEventType::kTouchFile,
                                 windowId, dirUrl, type, *error);
    return true;
}

bool VaultHelper::writeUrlsToClipboard(const quint64 windowId, const ClipBoard::ClipboardAction action, const QList<QUrl> urls)
{
    if (urls.isEmpty())
        return false;
    if (urls.first().scheme() != scheme())
        return false;

    QList<QUrl> redirectedFileUrls = transUrlsToLocal(urls);

    dpfSignalDispatcher->publish(GlobalEventType::kWriteUrlsToClipboard, windowId, action, redirectedFileUrls);

    return true;
}

bool VaultHelper::renameFiles(const quint64 windowId, const QList<QUrl> urls, const QPair<QString, QString> replacePair, bool flg)
{
    if (urls.isEmpty())
        return false;
    if (urls.first().scheme() != scheme())
        return false;

    QList<QUrl> actualUrls = transUrlsToLocal(urls);

    dpfSignalDispatcher->publish(GlobalEventType::kRenameFiles, windowId, actualUrls, replacePair, flg);

    return true;
}

bool VaultHelper::renameFilesAddText(const quint64 windowId, const QList<QUrl> urls, const QPair<QString, AbstractJobHandler::FileNameAddFlag> replacePair)
{
    if (urls.isEmpty())
        return false;
    if (urls.first().scheme() != scheme())
        return false;

    QList<QUrl> actualUrls = transUrlsToLocal(urls);

    dpfSignalDispatcher->publish(GlobalEventType::kRenameFiles, windowId, actualUrls, replacePair);

    return true;
}

VaultHelper::VaultHelper()
{
}
