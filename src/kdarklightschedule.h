/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "knighttime_export.h"

#include <QDateTime>

/*!
 * \class KDarkLightTransition
 * \inmodule KNightTime
 * \brief The KDarkLightTransition type represents a transition from day to night or vice versa.
 */
class KNIGHTTIME_EXPORT KDarkLightTransition
{
public:
    /*!
     * The Type enum specifies the type of a transition.
     *
     * \value Morning Transitioning from night time to daylight
     * \value Evening Transitioning from daylight to night time
     */
    enum Type {
        Morning,
        Evening,
    };

    /*!
     * The Relation enum specifies the relationship between this transition and a point in time.
     *
     * \value Upcoming The specified point in time comes before the transition
     * \value InProgress The specified point in time is between the start and the end of the transition
     * \value Passed The specified point in time is after the transition
     */
    enum Relation {
        Upcoming,
        InProgress,
        Passed,
    };

    /*!
     * Constructs an invalid KDarkLightTransition object.
     */
    KDarkLightTransition();

    /*!
     * Constructs a KDarkLightTransition object with the specified \a type, \a startDateTime,
     * and \a endDateTime.
     */
    KDarkLightTransition(Type type, const QDateTime &startDateTime, const QDateTime &endDateTime);

    auto operator<=>(const KDarkLightTransition &other) const = default;

    /*!
     * Checks how the specified \a dateTime relates to this transition.
     */
    Relation test(const QDateTime &dateTime) const;

    /*!
     * Returns the progress of the transition at the specified \a dateTime. The progress value is
     * in [0.0, 1.0] range, where 0.0 corresponds to the start of the transition, and 1.0 corresponds
     * to the end of the transition.
     */
    qreal progress(const QDateTime &dateTime) const;

    /*!
     * Returns the type of the transition.
     */
    Type type() const;

    /*!
     * Returns the date and time when the transition starts.
     */
    QDateTime startDateTime() const;

    /*!
     * Returns the date and time when the transition ends.
     */
    QDateTime endDateTime() const;

private:
    Type m_type;
    QDateTime m_startDateTime;
    QDateTime m_endDateTime;
};

/*!
 * \class KDarkLightCycle
 * \inmodule KNightTime
 * \brief The KDarkLightCycle provides information about the light-dark cycle for a single day.
 */
class KNIGHTTIME_EXPORT KDarkLightCycle
{
public:
    /*!
     * Constructs an invalid KDarkLightCycle.
     */
    KDarkLightCycle();

    /*!
     * Construcuts an KDarkLightCycle with specified \a noonDateTime, \a morning, and \a evening.
     */
    KDarkLightCycle(const QDateTime &noonDateTime, const KDarkLightTransition &morning, const KDarkLightTransition &evening);

    auto operator<=>(const KDarkLightCycle &other) const = default;

    /*!
     * Computes a new dark-light cycle that includes the specified \a referenceDateTime based on
     * the data in this cycle.
     */
    KDarkLightCycle extrapolated(const QDateTime &referenceDateTime) const;

    /*!
     * Returns the date and time of the noon.
     */
    QDateTime noonDateTime() const;

    /*!
     * Returns the morning transition.
     */
    KDarkLightTransition morning() const;

    /*!
     * Returns the evening transition.
     */
    KDarkLightTransition evening() const;

    /*!
     * Returns the next transition for the specified \a dateTime. If the given date and time comes
     * after the evening, a \c std::nullopt value will be returned.
     */
    std::optional<KDarkLightTransition> nextTransition(const QDateTime &dateTime) const;

    /*!
     * Returns the previous transition for the specified \a dateTime. If the given date and time comes
     * before the morning, a \c std::nullopt value will be returned.
     */
    std::optional<KDarkLightTransition> previousTransition(const QDateTime &dateTime) const;

private:
    QDateTime m_noonDateTime;
    KDarkLightTransition m_morning;
    KDarkLightTransition m_evening;
};

/*!
 * \class KDarkLightSchedule
 * \inmodule KNightTime
 * \brief The KDarkLightSchedule type represents a dark-light cycle schedule.
 *
 * Example usage:
 *
 * \code
 * const auto schedule = KDarkLightSchedule::forecast(QDateTime::currentDateTime(), 50.45, 30.52);
 *
 * qDebug() << "previous transition:" << schedule->previousTransition(QDateTime::currentDateTime());
 * qDebug() << "next transition:" << schedule->nextTransition(QDateTime::currentDateTime());
 * \endcode
 */
class KNIGHTTIME_EXPORT KDarkLightSchedule
{
public:
    /*!
     * Constructs a null schedule.
     */
    KDarkLightSchedule();

    /*!
     * Constructs a schedule with the specified \a cycles.
     */
    KDarkLightSchedule(const QList<KDarkLightCycle> &cycles);

    auto operator<=>(const KDarkLightSchedule &other) const = default;

    /*!
     * Retruns dark-light cycles stored in this schedule. A null schedule has no cycles in it.
     */
    QList<KDarkLightCycle> cycles() const;

    /*!
     * Finds the previous transition for the specified \a referenceDateTime. If this schedule is
     * null, a \c std::nullopt value will be returned.
     */
    std::optional<KDarkLightTransition> previousTransition(const QDateTime &referenceDateTime) const;

    /*!
     * Finds the next transition for the specified \a referenceDateTime. If this schedule is null,
     * a \c std::nullopt value will be returned.
     */
    std::optional<KDarkLightTransition> nextTransition(const QDateTime &referenceDateTime) const;

    /*!
     * Serializes the schedule in a string that can be stored in a config.
     */
    QString toState() const;

    /*!
     * Loads a schedule from the specified \a state string. If the \a state is empty or invalid, a
     * std::nullopt value will be returned.
     */
    static std::optional<KDarkLightSchedule> fromState(const QString &state);

    /*!
     * Computes the dark-light schedule for the next \a cycleCount days. The \a dateTime indicates the
     * current date and time. The \a morning and \a evening indicate when the morning and the evening
     * start, respectively. The \a transitionDuration indicates the duration of morning and evening.
     */
    static KDarkLightSchedule forecast(const QDateTime &dateTime, const QTime &morning = QTime(6, 0), const QTime &evening = QTime(18, 0), std::chrono::milliseconds transitionDuration = std::chrono::minutes(30), int cycleCount = 7);

    /*!
     * Computes the dark-light schedule for the next \a cycleCount days. Dark-light cycles are computed
     * based on the position of the Sun at the specified \a dateTime and location (\a latitude, \a longitude).
     * The latitude and the longitude are specified in the decimal degrees.
     *
     * This function may return \c std::nullopt at extreme latitudes if the Sun never rises or sets.
     */
    static std::optional<KDarkLightSchedule> forecast(const QDateTime &dateTime, qreal latitude, qreal longitude, int cycleCount = 7);

private:
    QList<KDarkLightCycle> m_cycles;
};

KNIGHTTIME_EXPORT QDebug operator<<(QDebug debug, const KDarkLightTransition &transition);
KNIGHTTIME_EXPORT QDebug operator<<(QDebug debug, const KDarkLightCycle &cycle);
KNIGHTTIME_EXPORT QDebug operator<<(QDebug debug, const KDarkLightSchedule &schedule);
