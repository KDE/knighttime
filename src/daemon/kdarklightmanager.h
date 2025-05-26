/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "kdarklightscheduler.h"

#include <QGeoPositionInfoSource>
#include <QTimer>

class KDarkLightManagerInterface;
class KDarkLightSettings;
class KDarkLightState;
class KSystemClockSkewNotifier;

class KDarkLightManager : public QObject
{
    Q_OBJECT

public:
    explicit KDarkLightManager(QObject *parent = nullptr);
    ~KDarkLightManager() override;

    KDarkLightSchedule schedule() const;

    void reconfigure();
    void reschedule();

Q_SIGNALS:
    void scheduleChanged();

private:
    std::unique_ptr<KDarkLightManagerInterface> m_dbusInterface;
    std::unique_ptr<KDarkLightSettings> m_settings;
    std::unique_ptr<KDarkLightState> m_state;
    std::unique_ptr<KDarkLightScheduler> m_scheduler;
    std::unique_ptr<KSystemClockSkewNotifier> m_skewNotifier;
    std::unique_ptr<QGeoPositionInfoSource> m_positionInfoSource;
    std::unique_ptr<QTimer> m_scheduleTimer;
    KDarkLightSchedule m_schedule;
};
