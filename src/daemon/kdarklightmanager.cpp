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

#include <KSharedConfig>
#include <KSystemClockSkewNotifier>

#include <chrono>

using namespace std::chrono_literals;

static void migrateNightLightConfig(KDarkLightSettings *knighttimerc)
{
    const KSharedConfig::Ptr kwinrc = KSharedConfig::openConfig(QStringLiteral("kwinrc"));
    KConfigGroup nightLight(kwinrc.get(), QStringLiteral("NightColor"));

    QStringList keysToDelete{
        QStringLiteral("MorningBeginFixed"),
        QStringLiteral("EveningBeginFixed"),
        QStringLiteral("TransitionTime"),
        QStringLiteral("LatitudeAuto"),
        QStringLiteral("LatitudeFixed"),
        QStringLiteral("LongitudeAuto"),
        QStringLiteral("LongitudeFixed"),
    };

    // Note that the Mode should not be removed if its value is either DarkLight (default) or Constant.
    const QString mode = nightLight.readEntry(QStringLiteral("Mode"), QString());
    if (mode == QLatin1String("Times")) {
        keysToDelete << QStringLiteral("Mode");

        knighttimerc->setSource(KDarkLightSettings::Source::Times);
        knighttimerc->setSunriseStart(QTime::fromString(nightLight.readEntry(QStringLiteral("MorningBeginFixed"), QStringLiteral("0600")), QStringLiteral("hhmm")));
        knighttimerc->setSunsetStart(QTime::fromString(nightLight.readEntry(QStringLiteral("EveningBeginFixed"), QStringLiteral("1800")), QStringLiteral("hhmm")));
        knighttimerc->setTransitionDuration(nightLight.readEntry<int>(QStringLiteral("TransitionTime"), 30) * 60);
        knighttimerc->save();
    } else if (mode == QLatin1String("Automatic")) {
        keysToDelete << QStringLiteral("Mode");

        // Effectively, there is nothing to migrate, the automatic mode is the default.
        knighttimerc->setSource(KDarkLightSettings::Source::Location);
        knighttimerc->setAutomaticLocation(true);
        knighttimerc->save();
    } else if (mode == QLatin1String("Location")) {
        keysToDelete << QStringLiteral("Mode");

        knighttimerc->setSource(KDarkLightSettings::Source::Location);
        knighttimerc->setAutomaticLocation(false);
        knighttimerc->setManualLatitude(nightLight.readEntry(QStringLiteral("LatitudeFixed"), 0.0));
        knighttimerc->setManualLongitude(nightLight.readEntry(QStringLiteral("LongitudeFixed"), 0.0));
        knighttimerc->save();
    }

    for (const QString &key : std::as_const(keysToDelete)) {
        nightLight.deleteEntry(key);
    }
}

KDarkLightManager::KDarkLightManager(QObject *parent)
    : QObject(parent)
    , m_dbusInterface(std::make_unique<KDarkLightManagerInterface>(this))
    , m_settings(std::make_unique<KDarkLightSettings>(KSharedConfig::openConfig(QStringLiteral("knighttimerc"), KConfig::NoGlobals)))
    , m_state(std::make_unique<KDarkLightState>())
    , m_skewNotifier(std::make_unique<KSystemClockSkewNotifier>())
    , m_scheduleTimer(std::make_unique<QTimer>())
{
    migrateNightLightConfig(m_settings.get());

    m_configWatcher = KConfigWatcher::create(m_settings->sharedConfig());
    connect(m_configWatcher.get(), &KConfigWatcher::configChanged, this, [this]() {
        m_settings->read();
        reconfigure();
    });

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
    case KDarkLightSettings::Location: {
        if (m_settings->automaticLocation()) {
            const QVariantMap parameters{
                {QStringLiteral("desktopId"), QStringLiteral("org.kde.knighttimed")},
            };

            if (auto source = QGeoPositionInfoSource::createDefaultSource(parameters, this)) {
                m_positionInfoSource.reset(source);
                connect(m_positionInfoSource.get(), &QGeoPositionInfoSource::errorOccurred, this, [this]() {
                    m_scheduler = std::make_unique<KTimedDarkLightScheduler>(m_settings->sunriseStart(), m_settings->sunsetStart(), m_settings->transitionDuration());
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

            m_scheduler = std::make_unique<KTimedDarkLightScheduler>(m_settings->sunriseStart(), m_settings->sunsetStart(), m_settings->transitionDuration());
        } else {
            m_scheduler = std::make_unique<KSolarDarkLightScheduler>(QGeoCoordinate(m_settings->manualLatitude(), m_settings->manualLongitude()));
        }
        break;
    }
    case KDarkLightSettings::Times:
        m_scheduler = std::make_unique<KTimedDarkLightScheduler>(m_settings->sunriseStart(), m_settings->sunsetStart(), m_settings->transitionDuration());
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

#include "moc_kdarklightmanager.cpp"
