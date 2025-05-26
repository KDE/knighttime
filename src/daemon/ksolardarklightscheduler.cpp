/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ksolardarklightscheduler.h"

KSolarDarkLightScheduler::KSolarDarkLightScheduler(const QGeoCoordinate &coordinate)
    : m_coordinate(coordinate)
{
}

QGeoCoordinate KSolarDarkLightScheduler::coordinate() const
{
    return m_coordinate;
}

KDarkLightSchedule KSolarDarkLightScheduler::schedule(const QDateTime &referenceDateTime)
{
    if (auto schedule = KDarkLightSchedule::forecast(referenceDateTime, m_coordinate.latitude(), m_coordinate.longitude())) {
        return *schedule;
    }
    return KDarkLightSchedule::forecast(referenceDateTime);
}
