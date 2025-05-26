/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kdarklightscheduleprovider.h"
#include "kdarklightscheduleprovider_p.h"

KDarkLightScheduleProvider::KDarkLightScheduleProvider(const QString &state, QObject *parent)
    : QObject(parent)
    , d(new KDarkLightScheduleProviderPrivate)
{
    d->subscription = KDarkLightScheduleSubscription::globalSubscription();
    connect(d->subscription.get(), &KDarkLightScheduleSubscription::refreshed, this, [this]() {
        const auto schedule = d->subscription->schedule().value_or(KDarkLightSchedule());
        if (d->schedule != schedule) {
            d->schedule = schedule;
            d->state = d->subscription->state();
            Q_EMIT scheduleChanged();
        }
    });

    if (auto schedule = d->subscription->schedule()) {
        d->schedule = std::move(*schedule);
        d->state = d->subscription->state();
    } else if (auto schedule = KDarkLightSchedule::fromState(state)) {
        d->schedule = std::move(*schedule);
        d->state = state;
    } else {
        d->schedule = KDarkLightSchedule::forecast(QDateTime::currentDateTime());
        d->state = d->schedule.toState();
    }
}

KDarkLightScheduleProvider::~KDarkLightScheduleProvider()
{
}

KDarkLightSchedule KDarkLightScheduleProvider::schedule() const
{
    return d->schedule;
}

QString KDarkLightScheduleProvider::state() const
{
    return d->state;
}

#include "moc_kdarklightscheduleprovider.cpp"
