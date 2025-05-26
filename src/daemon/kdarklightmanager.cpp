/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "kdarklightmanager.h"
#include "kdarklightmanagerinterface.h"
#include "kdarklightsettings.h"
#include "kdarklightstate.h"
#include "ksolardarklightscheduler.h"
#include "ktimeddarklightscheduler.h"

#include <KConfigWatcher>
#include <KSharedConfig>
#include <KSystemClockSkewNotifier>

#include <chrono>

using namespace std::chrono_literals;

KDarkLightManager::KDarkLightManager(QObject *parent)
    : QObject(parent)
    , m_dbusInterface(std::make_unique<KDarkLightManagerInterface>(this))
    , m_settings(std::make_unique<KDarkLightSettings>())
    , m_state(std::make_unique<KDarkLightState>())
    , m_skewNotifier(std::make_unique<KSystemClockSkewNotifier>())
    , m_scheduleTimer(std::make_unique<QTimer>())
{
    auto configWatcher = KConfigWatcher::create(m_settings->sharedConfig());
    connect(configWatcher.get(), &KConfigWatcher::configChanged, this, &KDarkLightManager::reconfigure);

    m_scheduleTimer->setSingleShot(false);
    m_scheduleTimer->setInterval(24h);
    connect(m_scheduleTimer.get(), &QTimer::timeout, this, &KDarkLightManager::reschedule);

    m_skewNotifier->setActive(true);
    connect(m_skewNotifier.get(), &KSystemClockSkewNotifier::skewed, this, &KDarkLightManager::reschedule);
}

KDarkLightManager::~KDarkLightManager()
{
}

KDarkLightSchedule KDarkLightManager::schedule() const
{
    return m_schedule;
}

void KDarkLightManager::reconfigure()
{
    m_positionInfoSource.reset();
    m_scheduler.reset();

    switch (m_settings->source()) {
    case KDarkLightSettings::AutomaticLocation:
        if (auto source = QGeoPositionInfoSource::createDefaultSource(this)) {
            m_positionInfoSource.reset(source);
            m_positionInfoSource->setBackendProperty(QStringLiteral("desktopId"), QStringLiteral("org.kde.knighttimed"));
            connect(m_positionInfoSource.get(), &QGeoPositionInfoSource::errorOccurred, this, [this]() {
                m_scheduler = std::make_unique<KTimedDarkLightScheduler>(QTime::fromString(m_settings->sunriseStart()), QTime::fromString(m_settings->sunsetStart()), m_settings->transitionDuration());
                reschedule();
            });
            connect(m_positionInfoSource.get(), &QGeoPositionInfoSource::positionUpdated, this, [this](const QGeoPositionInfo &update) {
                const QGeoCoordinate coordinate = update.coordinate();
                m_state->setAvailable(true);
                m_state->setLatitude(coordinate.latitude());
                m_state->setLongitude(coordinate.longitude());
                m_state->save();

                const int minDistanceInMeters = 50000;
                const auto currentScheduler = dynamic_cast<KSolarDarkLightScheduler *>(m_scheduler.get());
                if (!currentScheduler || coordinate.distanceTo(currentScheduler->coordinate()) > minDistanceInMeters) {
                    m_scheduler = std::make_unique<KSolarDarkLightScheduler>(coordinate);
                    reschedule();
                }
            });

            m_positionInfoSource->startUpdates();
            if (m_state->available()) {
                m_scheduler = std::make_unique<KSolarDarkLightScheduler>(QGeoCoordinate(m_state->latitude(), m_state->longitude()));
                break;
            }
        }

        m_scheduler = std::make_unique<KTimedDarkLightScheduler>(QTime::fromString(m_settings->sunriseStart()), QTime::fromString(m_settings->sunsetStart()), m_settings->transitionDuration());
        break;
    case KDarkLightSettings::ManualLocation:
        m_scheduler = std::make_unique<KSolarDarkLightScheduler>(QGeoCoordinate(m_settings->latitude(), m_settings->longitude()));
        break;
    case KDarkLightSettings::ManualTimes:
        m_scheduler = std::make_unique<KTimedDarkLightScheduler>(QTime::fromString(m_settings->sunriseStart()), QTime::fromString(m_settings->sunsetStart()), m_settings->transitionDuration());
        break;
    }

    reschedule();
}

void KDarkLightManager::reschedule()
{
    const auto schedule = m_scheduler->schedule(QDateTime::currentDateTime());
    if (m_schedule != schedule) {
        m_schedule = schedule;
        Q_EMIT scheduleChanged();
    }
}
