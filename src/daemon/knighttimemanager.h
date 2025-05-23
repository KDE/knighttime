/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "knighttimescheduler.h"

#include <QGeoPositionInfoSource>
#include <QTimer>

class KNightTimeManagerInterface;
class KNightTimeSettings;
class KNightTimeState;
class KSystemClockSkewNotifier;

class KNightTimeManager : public QObject
{
    Q_OBJECT

public:
    explicit KNightTimeManager(QObject *parent = nullptr);
    ~KNightTimeManager() override;

    KNightTimeSchedule schedule() const;

    void reconfigure();
    void reschedule();

Q_SIGNALS:
    void scheduleChanged();

private:
    std::unique_ptr<KNightTimeManagerInterface> m_dbusInterface;
    std::unique_ptr<KNightTimeSettings> m_settings;
    std::unique_ptr<KNightTimeState> m_state;
    std::unique_ptr<KNightTimeScheduler> m_scheduler;
    std::unique_ptr<KSystemClockSkewNotifier> m_skewNotifier;
    std::unique_ptr<QGeoPositionInfoSource> m_positionInfoSource;
    std::unique_ptr<QTimer> m_scheduleTimer;
    KNightTimeSchedule m_schedule;
};
