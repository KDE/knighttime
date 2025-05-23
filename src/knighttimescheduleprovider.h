/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "knighttimeschedule.h"

class KNightTimeScheduleProviderPrivate;

/*!
 * \class KNightTimeScheduleProvider
 * \inmodule KNightTime
 * \brief The KNightTimeScheduleProvider provides a convenient way to retrieve and manage dark-light cycle schedule.
 *
 * Example usage:
 *
 * \code
 * auto stateConfig = KSharedConfig::openStateConfig();
 * auto provider = new KNightTimeScheduleProvider(stateConfig->group(QStringLiteral("DarkLightCycle")).readEntry(QStringLiteral("State")));
 * connect(provider, &KNightTimeScheduleProvider::scheduleChanged, [provider, stateConfig]() {
 *     stateConfig->group(QStringLiteral("DarkLightCycle")).writeEntry(QStringLiteral("State"), provider->state());
 *     stateConfig->sync();
 *
 *     qDebug() << "next transition:" << provider->schedule().nextTransition(QDateTime::currentDateTime());
 * });
 *
 * qDebug() << "next transition:" << provider->schedule().nextTransition(QDateTime::currentDateTime());
 * \endcode
 */
class KNIGHTTIME_EXPORT KNightTimeScheduleProvider : public QObject
{
    Q_OBJECT

public:
    /*!
     * Constructs the schedule provider. If a \a state string is specified, the previous schedule
     * will be restored.
     *
     * If no state string is given, the provider will use the default schedule until the current one
     * is retrieved asynchronously. With the default schedule, morning lasts from 6:00AM to 6:30AM,
     * and evening lasts from 6:00PM to 6:30PM.
     */
    explicit KNightTimeScheduleProvider(const QString &state = QString(), QObject *parent = nullptr);
    ~KNightTimeScheduleProvider();

    /*!
     * Returns the dark-light cycle schedule.
     */
    KNightTimeSchedule schedule() const;

    /*!
     * Returns the dark-light cycle schedule state. The state string can be stored in a state config
     * to restore the schedule after a reboot. It can be used to reduce the number of unnecessary animations
     * at system startup.
     */
    QString state() const;

Q_SIGNALS:
    /*!
     * This signal is emitted when the schedule is refreshed.
     */
    void scheduleChanged();

private:
    std::unique_ptr<KNightTimeScheduleProviderPrivate> d;
};
