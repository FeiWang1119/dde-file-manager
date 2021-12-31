/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huanyu<huanyub@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
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
#ifndef FILEVIEW_P_H
#define FILEVIEW_P_H

#include "fileview.h"

#include <QObject>
#include <QUrl>

namespace GlobalPrivate {
const int kIconViewSpacing { 5 };
const int kListViewSpacing { 1 };
const int kListViewMinimumWidth { 80 };
const int kListViewDefaultWidth { 120 };
const int kListViewIconSize { 24 };
const QList<int> kIconSizeList { 48, 64, 96, 128, 256 };
}   // namespace GlobalPrivate

DPWORKSPACE_BEGIN_NAMESPACE

class StatusBar;
class HeaderView;
class BaseItemDelegate;
class FileSortFilterProxyModel;
class FileViewPrivate
{
    friend class FileView;
    FileView *const q;

    QAtomicInteger<bool> allowedAdjustColumnSize = true;
    QHash<int, BaseItemDelegate *> delegates;
    StatusBar *statusBar = nullptr;
    HeaderView *headerView = nullptr;
    FileSortFilterProxyModel *proxyModel = nullptr;
    QTimer *updateStatusBarTimer = nullptr;
    QUrl url;

    FileView::ViewMode configViewMode = FileView::ViewMode::IconMode;
    int configIconSizeLevel = 1;

    explicit FileViewPrivate(FileView *qq);
    int iconModeColumnCount(int itemWidth = 0) const;
    QUrl modelIndexUrl(const QModelIndex &index) const;

    void initIconModeView();
    void initListModeView();

    void updateListModeColumnWidth();
};

DPWORKSPACE_END_NAMESPACE

#endif   // FILEVIEW_P_H
