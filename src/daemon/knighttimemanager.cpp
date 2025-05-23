/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "knighttimemanager.h"
#include "knighttimemanagerinterface.h"
#include "knighttimesettings.h"
#include "knighttimestate.h"
#include "ksolarnighttimescheduler.h"
#include "ktimednighttimescheduler.h"

#include <KConfigWatcher>
#include <KSharedConfig>
#include <KSystemClockSkewNotifier>

#include <chrono>

using namespace std::chrono_literals;

KNightTimeManager::KNightTimeManager(QObject *parent)
    : QObject(parent)
    , m_dbusInterface(std::make_unique<KNightTimeManagerInterface>(this))
    , m_settings(std::make_unique<KNightTimeSettings>())
    , m_state(std::make_unique<KNightTimeState>())
    , m_skewNotifier(std::make_unique<KSystemClockSkewNotifier>())
    , m_scheduleTimer(std::make_unique<QTimer>())
{
    auto configWatcher = KConfigWatcher::create(m_settings->sharedConfig());
    connect(configWatcher.get(), &KConfigWatcher::configChanged, this, &KNightTimeManager::reconfigure);

    m_scheduleTimer->setSingleShot(false);
    m_scheduleTimer->setInterval(24h);
    connect(m_scheduleTimer.get(), &QTimer::timeout, this, &KNightTimeManager::reschedule);

    m_skewNotifier->setActive(true);
    connect(m_skewNotifier.get(), &KSystemClockSkewNotifier::skewed, this, &KNightTimeManager::reschedule);
}

KNightTimeManager::~KNightTimeManager()
{
}

KNightTimeSchedule KNightTimeManager::schedule() const
{
    return m_schedule;
}

void KNightTimeManager::reconfigure()
{
    m_positionInfoSource.reset();
    m_scheduler.reset();

    switch (m_settings->source()) {
    case KNightTimeSettings::AutomaticLocation:
        if (auto source = QGeoPositionInfoSource::createDefaultSource(this)) {
            m_positionInfoSource.reset(source);
            m_positionInfoSource->setBackendProperty(QStringLiteral("desktopId"), QStringLiteral("org.kde.knighttimed"));
            connect(m_positionInfoSource.get(), &QGeoPositionInfoSource::errorOccurred, this, [this]() {
                m_scheduler = std::make_unique<KTimedNightTimeScheduler>(QTime::fromString(m_settings->sunriseStart()), QTime::fromString(m_settings->sunsetStart()), m_settings->transitionDuration());
                reschedule();
            });
            connect(m_positionInfoSource.get(), &QGeoPositionInfoSource::positionUpdated, this, [this](const QGeoPositionInfo &update) {
                const QGeoCoordinate coordinate = update.coordinate();
                m_state->setAvailable(true);
                m_state->setLatitude(coordinate.latitude());
                m_state->setLongitude(coordinate.longitude());
                m_state->save();

                m_scheduler = std::make_unique<KSolarNightTimeScheduler>(coordinate.latitude(), coordinate.longitude());
                reschedule();
            });

            m_positionInfoSource->startUpdates();
            if (m_state->available()) {
                m_scheduler = std::make_unique<KSolarNightTimeScheduler>(m_state->latitude(), m_state->longitude());
                break;
            }
        }

        m_scheduler = std::make_unique<KTimedNightTimeScheduler>(QTime::fromString(m_settings->sunriseStart()), QTime::fromString(m_settings->sunsetStart()), m_settings->transitionDuration());
        break;
    case KNightTimeSettings::ManualLocation:
        m_scheduler = std::make_unique<KSolarNightTimeScheduler>(m_settings->latitude(), m_settings->longitude());
        break;
    case KNightTimeSettings::ManualTimes:
        m_scheduler = std::make_unique<KTimedNightTimeScheduler>(QTime::fromString(m_settings->sunriseStart()), QTime::fromString(m_settings->sunsetStart()), m_settings->transitionDuration());
        break;
    }

    reschedule();
}

void KNightTimeManager::reschedule()
{
    const auto schedule = m_scheduler->schedule(QDateTime::currentDateTime());
    if (m_schedule != schedule) {
        m_schedule = schedule;
        Q_EMIT scheduleChanged();
    }
}
