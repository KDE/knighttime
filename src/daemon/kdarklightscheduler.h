/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "kdarklightschedule.h"

class KDarkLightScheduler
{
public:
    virtual ~KDarkLightScheduler();

    virtual KDarkLightSchedule schedule(const QDateTime &referenceDateTime) = 0;
};
