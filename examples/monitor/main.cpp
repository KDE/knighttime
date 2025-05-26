/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QCoreApplication>
#include <QDebug>

#include "kdarklightscheduleprovider.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    KDarkLightScheduleProvider provider;

    qDebug() << provider.schedule();
    QObject::connect(&provider, &KDarkLightScheduleProvider::scheduleChanged, [&provider]() {
        qDebug() << provider.schedule();
    });

    return app.exec();
}
