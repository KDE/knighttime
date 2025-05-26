/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kdarklightschedulesubscription_p.h"
#include "kdarklightdbustypes_p.h"
#include "knighttimelogging.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QPointer>

std::shared_ptr<KDarkLightScheduleSubscription> KDarkLightScheduleSubscription::globalSubscription()
{
    static std::weak_ptr<KDarkLightScheduleSubscription> singleton;
    if (auto subscription = singleton.lock()) {
        return subscription;
    }

    auto subscription = std::make_shared<KDarkLightScheduleSubscription>();
    singleton = subscription;

    return subscription;
}

KDarkLightScheduleSubscription::KDarkLightScheduleSubscription(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Refreshed"), this, SLOT(OnRefreshed(QVariantMap)));

    auto message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Subscribe"));
    message.setArguments({QVariantMap()});
    auto pendingCall = QDBusConnection::sessionBus().asyncCall(message);

    auto watcher = new QDBusPendingCallWatcher(pendingCall);
    auto self = QPointer(this);
    connect(watcher, &QDBusPendingCallWatcher::finished, qApp, [self](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();

        if (watcher->isError()) {
            qCWarning(KNIGHTTIME) << "Subscribe() failed:" << watcher->error();
            return;
        }

        QDBusPendingReply<QVariantMap> reply = *watcher;
        if (self) {
            self->OnSubscribed(reply.value());
        } else {
            auto message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Unsubscribe"));
            message.setArguments({reply.value()[QStringLiteral("Cookie")]});
            QDBusConnection::sessionBus().asyncCall(message);
        }
    });
}

KDarkLightScheduleSubscription::~KDarkLightScheduleSubscription()
{
    if (m_cookie) {
        auto message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Unsubscribe"));
        message.setArguments({m_cookie.value()});
    }
}

std::optional<KDarkLightSchedule> KDarkLightScheduleSubscription::schedule() const
{
    return m_schedule;
}

QString KDarkLightScheduleSubscription::state() const
{
    return m_state;
}

void KDarkLightScheduleSubscription::OnSubscribed(const QVariantMap &data)
{
    if (auto it = data.find(QStringLiteral("Cookie")); it != data.end()) {
        m_cookie = it->toUInt();
    } else {
        qCWarning(KNIGHTTIME) << "Subscribe() reply contains no Cookie. Available data:" << data;
        return;
    }

    if (auto it = data.find(QStringLiteral("Schedule")); it != data.end()) {
        update(*it);
    } else {
        qCWarning(KNIGHTTIME) << "Subscribe() reply contains no Schedule. Available data:" << data;
    }
}

void KDarkLightScheduleSubscription::OnRefreshed(const QVariantMap &data)
{
    if (m_cookie) {
        update(data[QStringLiteral("Schedule")]);
    }
}

void KDarkLightScheduleSubscription::update(const QVariant &data)
{
    const auto dbusSchedule = qdbus_cast<KNightTimeDbusSchedule>(data.value<QDBusArgument>());
    m_schedule = dbusSchedule.into();
    m_state = m_schedule->toState();
    Q_EMIT refreshed();
}

#include "moc_kdarklightschedulesubscription_p.cpp"
