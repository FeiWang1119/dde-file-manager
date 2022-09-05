// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../dbusservice/dbusadaptor/vaultbruteforceprevention_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class VaultBruteForcePreventionAdaptor
 */

VaultBruteForcePreventionAdaptor::VaultBruteForcePreventionAdaptor(VaultBruteForcePrevention *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

VaultBruteForcePreventionAdaptor::~VaultBruteForcePreventionAdaptor()
{
    // destructor
}

int VaultBruteForcePreventionAdaptor::getLeftoverErrorInputTimes(int userID)
{
    // handle method call com.deepin.filemanager.daemon.VaultManager2.getLeftoverErrorInputTimes
    return parent()->getLeftoverErrorInputTimes(userID);
}

int VaultBruteForcePreventionAdaptor::getNeedWaitMinutes(int userID)
{
    // handle method call com.deepin.filemanager.daemon.VaultManager2.getNeedWaitMinutes
    return parent()->getNeedWaitMinutes(userID);
}

void VaultBruteForcePreventionAdaptor::leftoverErrorInputTimesMinusOne(int userID)
{
    // handle method call com.deepin.filemanager.daemon.VaultManager2.leftoverErrorInputTimesMinusOne
    parent()->leftoverErrorInputTimesMinusOne(userID);
}

void VaultBruteForcePreventionAdaptor::restoreLeftoverErrorInputTimes(int userID)
{
    // handle method call com.deepin.filemanager.daemon.VaultManager2.restoreLeftoverErrorInputTimes
    parent()->restoreLeftoverErrorInputTimes(userID);
}

void VaultBruteForcePreventionAdaptor::restoreNeedWaitMinutes(int userID)
{
    // handle method call com.deepin.filemanager.daemon.VaultManager2.restoreNeedWaitMinutes
    parent()->restoreNeedWaitMinutes(userID);
}

void VaultBruteForcePreventionAdaptor::startTimerOfRestorePasswordInput(int userID)
{
    // handle method call com.deepin.filemanager.daemon.VaultManager2.startTimerOfRestorePasswordInput
    parent()->startTimerOfRestorePasswordInput(userID);
}

