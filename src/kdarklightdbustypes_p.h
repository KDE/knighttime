/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "kdarklightschedule.h"

#include <QDBusArgument>
#include <QDebug>
#include <QTimeZone>

struct KNightTimeDbusDateTime
{
    qint64 timestamp;
    QByteArray timezone;

    QDateTime into() const;
    static KNightTimeDbusDateTime from(const QDateTime &dateTime);
};

struct KNightTimeDbusCycle
{
    KNightTimeDbusDateTime noon;
    KNightTimeDbusDateTime morningStart;
    KNightTimeDbusDateTime morningEnd;
    KNightTimeDbusDateTime eveningStart;
    KNightTimeDbusDateTime eveningEnd;

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

inline const QDBusArgument &operator<<(QDBusArgument &argument, const KNightTimeDbusDateTime &dateTime)
{
    argument << dateTime.timestamp;
    argument << dateTime.timezone;
    return argument;
}

inline const QDBusArgument &operator<<(QDBusArgument &argument, const KNightTimeDbusCycle &cycle)
{
    argument.beginStructure();
    argument << cycle.noon;
    argument << cycle.morningStart;
    argument << cycle.morningEnd;
    argument << cycle.eveningStart;
    argument << cycle.eveningEnd;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, KNightTimeDbusDateTime &dateTime)
{
    argument >> dateTime.timestamp;
    argument >> dateTime.timezone;
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, KNightTimeDbusCycle &cycle)
{
    argument.beginStructure();
    argument >> cycle.noon;
    argument >> cycle.morningStart;
    argument >> cycle.morningEnd;
    argument >> cycle.eveningStart;
    argument >> cycle.eveningEnd;
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

inline QDateTime KNightTimeDbusDateTime::into() const
{
    return QDateTime::fromMSecsSinceEpoch(timestamp, QTimeZone(timezone));
}

inline KNightTimeDbusDateTime KNightTimeDbusDateTime::from(const QDateTime &dateTime)
{
    return KNightTimeDbusDateTime{
        .timestamp = dateTime.toMSecsSinceEpoch(),
        .timezone = dateTime.timeZone().id(),
    };
}

inline KDarkLightCycle KNightTimeDbusCycle::into() const
{
    return KDarkLightCycle(noon.into(),
                           KDarkLightTransition(KDarkLightTransition::Morning, morningStart.into(), morningEnd.into()),
                           KDarkLightTransition(KDarkLightTransition::Evening, eveningStart.into(), eveningEnd.into()));
}

inline KNightTimeDbusCycle KNightTimeDbusCycle::from(const KDarkLightCycle &cycle)
{
    return KNightTimeDbusCycle{
        .noon = KNightTimeDbusDateTime::from(cycle.noonDateTime()),
        .morningStart = KNightTimeDbusDateTime::from(cycle.morning().startDateTime()),
        .morningEnd = KNightTimeDbusDateTime::from(cycle.morning().endDateTime()),
        .eveningStart = KNightTimeDbusDateTime::from(cycle.evening().startDateTime()),
        .eveningEnd = KNightTimeDbusDateTime::from(cycle.evening().endDateTime()),
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
