/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QCoreApplication>
#include <QDebug>

#include "knighttimescheduleprovider.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    KNightTimeScheduleProvider provider;

    qDebug() << provider.schedule();
    QObject::connect(&provider, &KNightTimeScheduleProvider::scheduleChanged, [&provider]() {
        qDebug() << provider.schedule();
    });

    return app.exec();
}
