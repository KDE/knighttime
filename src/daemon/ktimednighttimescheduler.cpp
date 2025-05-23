/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ktimednighttimescheduler.h"

KTimedNightTimeScheduler::KTimedNightTimeScheduler(const QTime &sunriseStart, const QTime &sunsetStart, int transitionDuration)
    : m_sunriseStart(sunriseStart)
    , m_sunsetStart(sunsetStart)
    , m_transitionDuration(transitionDuration)
{
}

KNightTimeSchedule KTimedNightTimeScheduler::schedule(const QDateTime &referenceDateTime)
{
    return KNightTimeSchedule::forecast(referenceDateTime, m_sunriseStart, m_sunsetStart, m_transitionDuration);
}
