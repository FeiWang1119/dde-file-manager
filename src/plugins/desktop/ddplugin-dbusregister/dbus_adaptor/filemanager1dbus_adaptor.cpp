/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i ../../plugins/desktop/ddplugin-dbusregister/filemanager1dbus.h -c FileManager1Adaptor -l FileManager1DBus -a ../../plugins/desktop/ddplugin-dbusregister/dbus_adaptor/filemanager1dbus_adaptor filemanager1dbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "../../plugins/desktop/ddplugin-dbusregister/dbus_adaptor/filemanager1dbus_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class FileManager1Adaptor
 */

FileManager1Adaptor::FileManager1Adaptor(FileManager1DBus *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

FileManager1Adaptor::~FileManager1Adaptor()
{
    // destructor
}

void FileManager1Adaptor::ShowFolders(const QStringList &URIs, const QString &StartupId)
{
    // handle method call org.freedesktop.FileManager1.ShowFolders
    parent()->ShowFolders(URIs, StartupId);
}

void FileManager1Adaptor::ShowItemProperties(const QStringList &URIs, const QString &StartupId)
{
    // handle method call org.freedesktop.FileManager1.ShowItemProperties
    parent()->ShowItemProperties(URIs, StartupId);
}

void FileManager1Adaptor::ShowItems(const QStringList &URIs, const QString &StartupId)
{
    // handle method call org.freedesktop.FileManager1.ShowItems
    parent()->ShowItems(URIs, StartupId);
}

void FileManager1Adaptor::Trash(const QStringList &URIs)
{
    // handle method call org.freedesktop.FileManager1.Trash
    parent()->Trash(URIs);
}

