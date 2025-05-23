/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "knighttimeschedule.h"

class KNightTimeScheduler
{
public:
    virtual ~KNightTimeScheduler();

    virtual KNightTimeSchedule schedule(const QDateTime &referenceDateTime) = 0;
};
