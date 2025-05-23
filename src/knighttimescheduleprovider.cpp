/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "knighttimescheduleprovider.h"
#include "knighttimescheduleprovider_p.h"

KNightTimeScheduleProvider::KNightTimeScheduleProvider(const QString &state, QObject *parent)
    : QObject(parent)
    , d(new KNightTimeScheduleProviderPrivate)
{
    d->subscription = KNightTimeScheduleSubscription::globalSubscription();
    connect(d->subscription.get(), &KNightTimeScheduleSubscription::refreshed, this, [this]() {
        const auto schedule = d->subscription->schedule().value_or(KNightTimeSchedule());
        if (d->schedule != schedule) {
            d->schedule = schedule;
            d->state = d->subscription->state();
            Q_EMIT scheduleChanged();
        }
    });

    if (auto schedule = d->subscription->schedule()) {
        d->schedule = std::move(*schedule);
        d->state = d->subscription->state();
    } else if (auto schedule = KNightTimeSchedule::fromState(state)) {
        d->schedule = std::move(*schedule);
        d->state = state;
    } else {
        d->schedule = KNightTimeSchedule::forecast(QDateTime::currentDateTime());
        d->state = d->schedule.toState();
    }
}

KNightTimeScheduleProvider::~KNightTimeScheduleProvider()
{
}

KNightTimeSchedule KNightTimeScheduleProvider::schedule() const
{
    return d->schedule;
}

QString KNightTimeScheduleProvider::state() const
{
    return d->state;
}
