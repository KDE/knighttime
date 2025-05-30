/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ktimeddarklightscheduler.h"

KTimedDarkLightScheduler::KTimedDarkLightScheduler(QTime sunriseStart, QTime sunsetStart, int transitionDuration)
    : m_sunriseStart(sunriseStart)
    , m_sunsetStart(sunsetStart)
    , m_transitionDuration(transitionDuration)
{
}

KDarkLightSchedule KTimedDarkLightScheduler::schedule(const QDateTime &referenceDateTime)
{
    return KDarkLightSchedule::forecast(referenceDateTime, m_sunriseStart, m_sunsetStart, m_transitionDuration);
}
