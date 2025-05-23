/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ksolarnighttimescheduler.h"

KSolarNightTimeScheduler::KSolarNightTimeScheduler(qreal latitude, qreal longitude)
    : m_latitude(latitude)
    , m_longitude(longitude)
{
}

KNightTimeSchedule KSolarNightTimeScheduler::schedule(const QDateTime &referenceDateTime)
{
    if (auto schedule = KNightTimeSchedule::forecast(referenceDateTime, m_latitude, m_longitude)) {
        return *schedule;
    }
    return KNightTimeSchedule::forecast(referenceDateTime);
}
