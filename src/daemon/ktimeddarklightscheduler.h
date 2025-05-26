/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "kdarklightscheduler.h"

class KTimedDarkLightScheduler : public KDarkLightScheduler
{
public:
    KTimedDarkLightScheduler(const QTime &sunriseStart, const QTime &sunsetStart, int transitionDuration);

    KDarkLightSchedule schedule(const QDateTime &referenceDateTime) override;

private:
    QTime m_sunriseStart;
    QTime m_sunsetStart;
    std::chrono::seconds m_transitionDuration;
};
