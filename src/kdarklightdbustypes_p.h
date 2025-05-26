/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "kdarklightschedule.h"

#include <QDBusArgument>
#include <QDebug>

struct KNightTimeDbusCycle
{
    qint64 noonTimestamp;
    qint64 morningStartTimestamp;
    qint64 morningEndTimestamp;
    qint64 eveningStartTimestamp;
    qint64 eveningEndTimestamp;

    KDarkLightCycle into() const;
    static KNightTimeDbusCycle from(const KDarkLightCycle &cycle);
};

struct KNightTimeDbusSchedule
{
    QString name;
    QDBusVariant data;

    KDarkLightSchedule into() const;
    static KNightTimeDbusSchedule from(const KDarkLightSchedule &schedule);
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

inline KDarkLightCycle KNightTimeDbusCycle::into() const
{
    return KDarkLightCycle(QDateTime::fromMSecsSinceEpoch(noonTimestamp),
                           KDarkLightTransition(KDarkLightTransition::Morning, QDateTime::fromMSecsSinceEpoch(morningStartTimestamp), QDateTime::fromMSecsSinceEpoch(morningEndTimestamp)),
                           KDarkLightTransition(KDarkLightTransition::Evening, QDateTime::fromMSecsSinceEpoch(eveningStartTimestamp), QDateTime::fromMSecsSinceEpoch(eveningEndTimestamp)));
}

inline KNightTimeDbusCycle KNightTimeDbusCycle::from(const KDarkLightCycle &cycle)
{
    return KNightTimeDbusCycle{
        .noonTimestamp = cycle.noonDateTime().toMSecsSinceEpoch(),
        .morningStartTimestamp = cycle.morning().startDateTime().toMSecsSinceEpoch(),
        .morningEndTimestamp = cycle.morning().endDateTime().toMSecsSinceEpoch(),
        .eveningStartTimestamp = cycle.evening().startDateTime().toMSecsSinceEpoch(),
        .eveningEndTimestamp = cycle.evening().endDateTime().toMSecsSinceEpoch(),
    };
}

inline KDarkLightSchedule KNightTimeDbusSchedule::into() const
{
    if (name != QLatin1String("dynamic")) {
        return KDarkLightSchedule();
    }

    const QList<KNightTimeDbusCycle> dbusCycles = qdbus_cast<QList<KNightTimeDbusCycle>>(data.variant().value<QDBusArgument>());
    QList<KDarkLightCycle> cycles;
    cycles.reserve(dbusCycles.size());
    for (const auto &dbusCycle : dbusCycles) {
        cycles.append(dbusCycle.into());
    }

    return KDarkLightSchedule(cycles);
}

inline KNightTimeDbusSchedule KNightTimeDbusSchedule::from(const KDarkLightSchedule &schedule)
{
    const QList<KDarkLightCycle> cycles = schedule.cycles();
    QList<KNightTimeDbusCycle> dbusCycles;
    dbusCycles.reserve(cycles.size());
    for (const KDarkLightCycle &cycle : cycles) {
        dbusCycles.append(KNightTimeDbusCycle::from(cycle));
    }

    return KNightTimeDbusSchedule{
        .name = QStringLiteral("dynamic"),
        .data = QDBusVariant(QVariant::fromValue(dbusCycles)),
    };
}

Q_DECLARE_METATYPE(KNightTimeDbusCycle)
Q_DECLARE_METATYPE(KNightTimeDbusSchedule)
