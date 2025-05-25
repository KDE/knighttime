/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ksolarnighttimescheduler.h"

KSolarNightTimeScheduler::KSolarNightTimeScheduler(const QGeoCoordinate &coordinate)
    : m_coordinate(coordinate)
{
}

QGeoCoordinate KSolarNightTimeScheduler::coordinate() const
{
    return m_coordinate;
}

KNightTimeSchedule KSolarNightTimeScheduler::schedule(const QDateTime &referenceDateTime)
{
    if (auto schedule = KNightTimeSchedule::forecast(referenceDateTime, m_coordinate.latitude(), m_coordinate.longitude())) {
        return *schedule;
    }
    return KNightTimeSchedule::forecast(referenceDateTime);
}
