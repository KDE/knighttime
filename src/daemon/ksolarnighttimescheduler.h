/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "knighttimescheduler.h"

#include <QGeoCoordinate>

class KSolarNightTimeScheduler : public KNightTimeScheduler
{
public:
    explicit KSolarNightTimeScheduler(const QGeoCoordinate &coordinate);

    QGeoCoordinate coordinate() const;

    KNightTimeSchedule schedule(const QDateTime &referenceDateTime) override;

private:
    QGeoCoordinate m_coordinate;
};
