/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "knighttimescheduler.h"

class KSolarNightTimeScheduler : public KNightTimeScheduler
{
public:
    KSolarNightTimeScheduler(qreal latitude, qreal longitude);

    KNightTimeSchedule schedule(const QDateTime &referenceDateTime) override;

private:
    qreal m_latitude;
    qreal m_longitude;
};
