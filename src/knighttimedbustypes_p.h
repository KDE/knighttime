/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "knighttimeschedule.h"

#include <QDBusArgument>
#include <QDebug>

struct KNightTimeDbusCycle
{
    qint64 noonTimestamp;
    qint64 morningStartTimestamp;
    qint64 morningEndTimestamp;
    qint64 eveningStartTimestamp;
    qint64 eveningEndTimestamp;

    KNightTimeCycle into() const;
    static KNightTimeDbusCycle from(const KNightTimeCycle &cycle);
};

struct KNightTimeDbusSchedule
{
    QString name;
    QDBusVariant data;

    KNightTimeSchedule into() const;
    static KNightTimeDbusSchedule from(const KNightTimeSchedule &schedule);
};

inline const QDBusArgument &operator<<(QDBusArgument &argument, const KNightTimeDbusCycle &cycle)
{
    argument.beginStructure();
    argument << cycle.noonTimestamp;
    argument << cycle.morningStartTimestamp;
    argument << cycle.morningEndTimestamp;
    argument << cycle.eveningStartTimestamp;
    argument << cycle.eveningEndTimestamp;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, KNightTimeDbusCycle &cycle)
{
    argument.beginStructure();
    argument >> cycle.noonTimestamp;
    argument >> cycle.morningStartTimestamp;
    argument >> cycle.morningEndTimestamp;
    argument >> cycle.eveningStartTimestamp;
    argument >> cycle.eveningEndTimestamp;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator<<(QDBusArgument &argument, const KNightTimeDbusSchedule &schedule)
{
    argument.beginStructure();
    argument << schedule.name;
    argument << schedule.data;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, KNightTimeDbusSchedule &cycle)
{
    argument.beginStructure();
    argument >> cycle.name;
    argument >> cycle.data;
    argument.endStructure();
    return argument;
}

inline KNightTimeCycle KNightTimeDbusCycle::into() const
{
    return KNightTimeCycle(QDateTime::fromMSecsSinceEpoch(noonTimestamp),
                           KNightTimeTransition(KNightTimeTransition::Morning, QDateTime::fromMSecsSinceEpoch(morningStartTimestamp), QDateTime::fromMSecsSinceEpoch(morningEndTimestamp)),
                           KNightTimeTransition(KNightTimeTransition::Evening, QDateTime::fromMSecsSinceEpoch(eveningStartTimestamp), QDateTime::fromMSecsSinceEpoch(eveningEndTimestamp)));
}

inline KNightTimeDbusCycle KNightTimeDbusCycle::from(const KNightTimeCycle &cycle)
{
    return KNightTimeDbusCycle{
        .noonTimestamp = cycle.noonDateTime().toMSecsSinceEpoch(),
        .morningStartTimestamp = cycle.morning().startDateTime().toMSecsSinceEpoch(),
        .morningEndTimestamp = cycle.morning().endDateTime().toMSecsSinceEpoch(),
        .eveningStartTimestamp = cycle.evening().startDateTime().toMSecsSinceEpoch(),
        .eveningEndTimestamp = cycle.evening().endDateTime().toMSecsSinceEpoch(),
    };
}

inline KNightTimeSchedule KNightTimeDbusSchedule::into() const
{
    if (name != QLatin1String("sunrise-sunset")) {
        return KNightTimeSchedule();
    }

    const QList<KNightTimeDbusCycle> dbusCycles = qdbus_cast<QList<KNightTimeDbusCycle>>(data.variant().value<QDBusArgument>());
    QList<KNightTimeCycle> cycles;
    cycles.reserve(dbusCycles.size());
    for (const auto &dbusCycle : dbusCycles) {
        cycles.append(dbusCycle.into());
    }

    return KNightTimeSchedule(cycles);
}

inline KNightTimeDbusSchedule KNightTimeDbusSchedule::from(const KNightTimeSchedule &schedule)
{
    const QList<KNightTimeCycle> cycles = schedule.cycles();
    QList<KNightTimeDbusCycle> dbusCycles;
    dbusCycles.reserve(cycles.size());
    for (const KNightTimeCycle &cycle : cycles) {
        dbusCycles.append(KNightTimeDbusCycle::from(cycle));
    }

    return KNightTimeDbusSchedule{
        .name = QStringLiteral("sunrise-sunset"),
        .data = QDBusVariant(QVariant::fromValue(dbusCycles)),
    };
}

Q_DECLARE_METATYPE(KNightTimeDbusCycle)
Q_DECLARE_METATYPE(KNightTimeDbusSchedule)
