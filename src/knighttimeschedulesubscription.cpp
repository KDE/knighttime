/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "knighttimeschedulesubscription_p.h"
#include "knighttimedbustypes_p.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QPointer>

std::shared_ptr<KNightTimeScheduleSubscription> KNightTimeScheduleSubscription::globalSubscription()
{
    static std::weak_ptr<KNightTimeScheduleSubscription> singleton;
    if (auto subscription = singleton.lock()) {
        return subscription;
    }

    auto subscription = std::make_shared<KNightTimeScheduleSubscription>();
    singleton = subscription;

    return subscription;
}

KNightTimeScheduleSubscription::KNightTimeScheduleSubscription(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Refreshed"), this, SLOT(OnRefreshed(QVariantMap)));

    auto message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Subscribe"));
    auto pendingCall = QDBusConnection::sessionBus().asyncCall(message);

    auto watcher = new QDBusPendingCallWatcher(pendingCall);
    auto self = QPointer(this);
    connect(watcher, &QDBusPendingCallWatcher::finished, qApp, [self](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();

        if (watcher->isError()) {
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

KNightTimeScheduleSubscription::~KNightTimeScheduleSubscription()
{
    if (m_cookie) {
        auto message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.NightTime"), QStringLiteral("/org/kde/NightTime/Manager"), QStringLiteral("org.kde.NightTime.Manager"), QStringLiteral("Unsubscribe"));
        message.setArguments({m_cookie.value()});
    }
}

std::optional<KNightTimeSchedule> KNightTimeScheduleSubscription::schedule() const
{
    return m_schedule;
}

QString KNightTimeScheduleSubscription::state() const
{
    return m_state;
}

void KNightTimeScheduleSubscription::OnSubscribed(const QVariantMap &data)
{
    if (auto it = data.find(QStringLiteral("Cookie")); it != data.end()) {
        m_cookie = it->toUInt();
    } else {
        return;
    }

    update(data[QStringLiteral("Schedule")]);
}

void KNightTimeScheduleSubscription::OnRefreshed(const QVariantMap &data)
{
    if (m_cookie) {
        update(data[QStringLiteral("Schedule")]);
    }
}

void KNightTimeScheduleSubscription::update(const QVariant &data)
{
    const auto dbusSchedule = qdbus_cast<KNightTimeDbusSchedule>(data.value<QDBusArgument>());
    m_schedule = dbusSchedule.into();
    m_state = m_schedule->toState();
    Q_EMIT refreshed();
}
