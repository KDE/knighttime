/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QCoreApplication>

#include <KDBusService>

#include "kdarklightmanager.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("NightTime"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));

    KDarkLightManager daemon;
    daemon.reconfigure();

    KDBusService dbusService(KDBusService::Unique);
    return app.exec();
}
