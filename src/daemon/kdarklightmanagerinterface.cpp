/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "kdarklightmanagerinterface.h"
#include "kdarklightdbustypes_p.h"
#include "kdarklightmanager.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>

KDarkLightManagerInterface::KDarkLightManagerInterface(KDarkLightManager *manager, QObject *parent)
    : QObject(parent)
    , m_manager(manager)
    , m_serviceWatcher(new QDBusServiceWatcher(this))
{
    qDBusRegisterMetaType<KNightTimeDbusCycle>();
    qDBusRegisterMetaType<QList<KNightTimeDbusCycle>>();
    qDBusRegisterMetaType<KNightTimeDbusSchedule>();

    connect(m_manager, &KDarkLightManager::scheduleChanged, this, &KDarkLightManagerInterface::OnScheduleChanged);

    m_serviceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &KDarkLightManagerInterface::OnServiceUnregistered);

    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/kde/NightTime/Manager"), this, QDBusConnection::ExportScriptableContents);
}

uint KDarkLightManagerInterface::version() const
{
    return 1;
}

QVariantMap KDarkLightManagerInterface::Subscribe(const QVariantMap &options)
{
    const QString subscriber = message().service();
    m_serviceWatcher->addWatchedService(subscriber);

    const uint cookie = m_lastCookie++;
    m_subscribers.insert(subscriber, cookie);

    return QVariantMap{
        {QStringLiteral("Cookie"), cookie},
        {QStringLiteral("Schedule"), QVariant::fromValue(KNightTimeDbusSchedule::from(m_manager->schedule()))},
    };
}

void KDarkLightManagerInterface::Unsubscribe(uint cookie)
{
    const QString subscriber = message().service();
    if (!m_subscribers.remove(subscriber, cookie)) {
        return;
    }

    if (!m_subscribers.contains(subscriber)) {
        m_serviceWatcher->removeWatchedService(subscriber);
    }
}

void KDarkLightManagerInterface::OnServiceUnregistered(const QString &serviceName)
{
    m_serviceWatcher->removeWatchedService(serviceName);
    m_subscribers.remove(serviceName);
}

void KDarkLightManagerInterface::OnScheduleChanged()
{
    const auto subscribers = m_serviceWatcher->watchedServices();
    if (subscribers.isEmpty()) {
        return;
    }

    const QVariantMap data{
        {QStringLiteral("Schedule"), QVariant::fromValue(KNightTimeDbusSchedule::from(m_manager->schedule()))},
    };

    for (const QString &subscriber : subscribers) {
        auto signal = QDBusMessage::createTargetedSignal(subscriber, QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Refreshed"));
        signal.setArguments({data});
        QDBusConnection::sessionBus().send(signal);
    }
}

#include "moc_kdarklightmanagerinterface.cpp"
