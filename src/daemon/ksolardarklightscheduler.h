/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "kdarklightscheduler.h"

#include <QGeoCoordinate>

class KSolarDarkLightScheduler : public KDarkLightScheduler
{
public:
    explicit KSolarDarkLightScheduler(const QGeoCoordinate &coordinate);

    QGeoCoordinate coordinate() const;

    KDarkLightSchedule schedule(const QDateTime &referenceDateTime) override;

private:
    QGeoCoordinate m_coordinate;
};
