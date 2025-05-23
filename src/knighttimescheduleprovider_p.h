/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "knighttimeschedule.h"
#include "knighttimeschedulesubscription_p.h"

class KNightTimeScheduleProviderPrivate
{
public:
    KNightTimeSchedule schedule;
    QString state;
    std::shared_ptr<KNightTimeScheduleSubscription> subscription;
};
