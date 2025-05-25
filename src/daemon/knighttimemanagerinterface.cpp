/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "knighttimemanagerinterface.h"
#include "knighttimedbustypes_p.h"
#include "knighttimemanager.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>

KNightTimeManagerInterface::KNightTimeManagerInterface(KNightTimeManager *manager, QObject *parent)
    : QObject(parent)
    , m_manager(manager)
    , m_serviceWatcher(new QDBusServiceWatcher(this))
{
    qDBusRegisterMetaType<KNightTimeDbusCycle>();
    qDBusRegisterMetaType<QList<KNightTimeDbusCycle>>();
    qDBusRegisterMetaType<KNightTimeDbusSchedule>();

    connect(m_manager, &KNightTimeManager::scheduleChanged, this, &KNightTimeManagerInterface::OnScheduleChanged);

    m_serviceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &KNightTimeManagerInterface::OnServiceUnregistered);

    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/kde/NightTime/Manager"), this, QDBusConnection::ExportScriptableContents);
}

uint KNightTimeManagerInterface::version() const
{
    return 1;
}

QVariantMap KNightTimeManagerInterface::Subscribe(const QVariantMap &options)
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

void KNightTimeManagerInterface::Unsubscribe(uint cookie)
{
    const QString subscriber = message().service();
    if (!m_subscribers.remove(subscriber, cookie)) {
        return;
    }

    if (!m_subscribers.contains(subscriber)) {
        m_serviceWatcher->removeWatchedService(subscriber);
    }
}

void KNightTimeManagerInterface::OnServiceUnregistered(const QString &serviceName)
{
    m_serviceWatcher->removeWatchedService(serviceName);
    m_subscribers.remove(serviceName);
}

void KNightTimeManagerInterface::OnScheduleChanged()
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