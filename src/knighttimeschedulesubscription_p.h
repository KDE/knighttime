/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "knighttimeschedule.h"

#include <QObject>

class KNightTimeScheduleSubscription : public QObject
{
    Q_OBJECT

public:
    explicit KNightTimeScheduleSubscription(QObject *parent = nullptr);
    ~KNightTimeScheduleSubscription() override;

    std::optional<KNightTimeSchedule> schedule() const;
    QString state() const;

    static std::shared_ptr<KNightTimeScheduleSubscription> globalSubscription();

Q_SIGNALS:
    void refreshed();

private Q_SLOTS:
    void OnSubscribed(const QVariantMap &data);
    void OnRefreshed(const QVariantMap &data);

private:
    void update(const QVariant &data);

    std::optional<KNightTimeSchedule> m_schedule;
    QString m_state;
    std::optional<uint> m_cookie;
};
