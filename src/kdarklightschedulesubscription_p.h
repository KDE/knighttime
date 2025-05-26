/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "kdarklightschedule.h"

#include <QObject>

class KDarkLightScheduleSubscription : public QObject
{
    Q_OBJECT

public:
    explicit KDarkLightScheduleSubscription(QObject *parent = nullptr);
    ~KDarkLightScheduleSubscription() override;

    std::optional<KDarkLightSchedule> schedule() const;
    QString state() const;

    static std::shared_ptr<KDarkLightScheduleSubscription> globalSubscription();

Q_SIGNALS:
    void refreshed();

private Q_SLOTS:
    void OnSubscribed(const QVariantMap &data);
    void OnRefreshed(const QVariantMap &data);

private:
    void update(const QVariant &data);

    std::optional<KDarkLightSchedule> m_schedule;
    QString m_state;
    std::optional<uint> m_cookie;
};
