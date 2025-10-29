/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kdarklightschedule.h"

#include <KHolidays/SunEvents>

#include <QBuffer>

using namespace std::chrono_literals;

QDebug operator<<(QDebug debug, const KDarkLightTransition &transition)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "KNightTimeTransition(type = " << int(transition.type());
    debug << ", start = " << transition.startDateTime();
    debug << ", end = " << transition.endDateTime();
    debug << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const KDarkLightCycle &cycle)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "KNightTimeCycle(noon = " << cycle.noonDateTime();
    debug << ", morning = " << cycle.morning();
    debug << ", evening = " << cycle.evening();
    debug << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const KDarkLightSchedule &schedule)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "KNightTimeSchedule(cycles = " << schedule.cycles() << ")";
    return debug;
}

KDarkLightTransition::KDarkLightTransition()
    : m_type(Morning)
{
}

KDarkLightTransition::KDarkLightTransition(Type type, const QDateTime &startDateTime, const QDateTime &endDateTime)
    : m_type(type)
    , m_startDateTime(startDateTime)
    , m_endDateTime(endDateTime)
{
}

KDarkLightTransition::Relation KDarkLightTransition::test(const QDateTime &dateTime) const
{
    const int tolerance = 60;
    if (dateTime.secsTo(m_startDateTime) > tolerance) {
        return Relation::Upcoming;
    } else if (dateTime.secsTo(m_endDateTime) > tolerance) {
        return Relation::InProgress;
    } else {
        return Relation::Passed;
    }
}

qreal KDarkLightTransition::progress(const QDateTime &dateTime) const
{
    const qreal elapsed = m_startDateTime.secsTo(dateTime);
    const qreal total = m_startDateTime.secsTo(m_endDateTime);
    return std::clamp<qreal>(elapsed / total, 0.0, 1.0);
}

KDarkLightTransition::Type KDarkLightTransition::type() const
{
    return m_type;
}

QDateTime KDarkLightTransition::startDateTime() const
{
    return m_startDateTime;
}

QDateTime KDarkLightTransition::endDateTime() const
{
    return m_endDateTime;
}

KDarkLightCycle::KDarkLightCycle()
{
}

KDarkLightCycle::KDarkLightCycle(const QDateTime &noonDateTime, const KDarkLightTransition &morning, const KDarkLightTransition &evening)
    : m_noonDateTime(noonDateTime)
    , m_morning(morning)
    , m_evening(evening)
{
}

KDarkLightCycle KDarkLightCycle::extrapolated(const QDateTime &referenceDateTime) const
{
    const QDateTime localReferenceDateTime = referenceDateTime.toLocalTime();
    const QDateTime localNoonDateTime = m_noonDateTime.toLocalTime();
    const QDateTime newNoonDateTime(localReferenceDateTime.date(), localNoonDateTime.time());

    return KDarkLightCycle(newNoonDateTime,
                           KDarkLightTransition(KDarkLightTransition::Morning,
                                                newNoonDateTime.addMSecs(-m_morning.startDateTime().msecsTo(m_noonDateTime)),
                                                newNoonDateTime.addMSecs(-m_morning.endDateTime().msecsTo(m_noonDateTime))),
                           KDarkLightTransition(KDarkLightTransition::Evening,
                                                newNoonDateTime.addMSecs(-m_evening.startDateTime().msecsTo(m_noonDateTime)),
                                                newNoonDateTime.addMSecs(-m_evening.endDateTime().msecsTo(m_noonDateTime))));
}

QDateTime KDarkLightCycle::noonDateTime() const
{
    return m_noonDateTime;
}

KDarkLightTransition KDarkLightCycle::morning() const
{
    return m_morning;
}

KDarkLightTransition KDarkLightCycle::evening() const
{
    return m_evening;
}

std::optional<KDarkLightTransition> KDarkLightCycle::nextTransition(const QDateTime &dateTime) const
{
    switch (m_morning.test(dateTime)) {
    case KDarkLightTransition::Upcoming:
        return m_morning;
    case KDarkLightTransition::InProgress:
    case KDarkLightTransition::Passed:
        break;
    }

    switch (m_evening.test(dateTime)) {
    case KDarkLightTransition::Upcoming:
        return m_evening;
    case KDarkLightTransition::InProgress:
    case KDarkLightTransition::Passed:
        break;
    }

    return std::nullopt;
}

std::optional<KDarkLightTransition> KDarkLightCycle::previousTransition(const QDateTime &dateTime) const
{
    switch (m_evening.test(dateTime)) {
    case KDarkLightTransition::Upcoming:
        break;
    case KDarkLightTransition::InProgress:
    case KDarkLightTransition::Passed:
        return m_evening;
    }

    switch (m_morning.test(dateTime)) {
    case KDarkLightTransition::Upcoming:
        break;
    case KDarkLightTransition::InProgress:
    case KDarkLightTransition::Passed:
        return m_morning;
    }

    return std::nullopt;
}

KDarkLightSchedule::KDarkLightSchedule()
{
}

KDarkLightSchedule::KDarkLightSchedule(const QList<KDarkLightCycle> &cycles)
    : m_cycles(cycles)
{
}

QList<KDarkLightCycle> KDarkLightSchedule::cycles() const
{
    return m_cycles;
}

static std::pair<int, std::chrono::milliseconds> closestCycle(const QList<KDarkLightCycle> &cycles, const QDateTime &dateTime)
{
    int bestIndex = -1;
    std::chrono::milliseconds bestScore;

    for (int i = 0; i < cycles.size(); ++i) {
        const auto score = std::chrono::abs(cycles[i].noonDateTime() - dateTime);
        if (bestIndex == -1 || score < bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }

    return std::make_pair(bestIndex, bestScore);
}

std::optional<KDarkLightTransition> KDarkLightSchedule::previousTransition(const QDateTime &referenceDateTime) const
{
    const auto [index, diff] = closestCycle(m_cycles, referenceDateTime);
    if (index == -1) {
        return std::nullopt;
    }

    const auto &cycle = m_cycles[index];
    if (diff <= 12h) {
        if (const auto transition = cycle.previousTransition(referenceDateTime)) {
            return transition;
        }

        if (index > 0) {
            const auto &previousCycle = m_cycles[index - 1];
            if (const auto transition = previousCycle.previousTransition(referenceDateTime)) {
                return transition;
            }
        }
    }

    const auto extrapolatedCycle = cycle.extrapolated(referenceDateTime);
    if (const auto transition = extrapolatedCycle.previousTransition(referenceDateTime)) {
        return transition;
    }

    const auto extrapolatedEvening = extrapolatedCycle.evening();
    return KDarkLightTransition(extrapolatedEvening.type(), extrapolatedEvening.startDateTime().addDays(-1), extrapolatedEvening.endDateTime().addDays(-1));
}

std::optional<KDarkLightTransition> KDarkLightSchedule::nextTransition(const QDateTime &referenceDateTime) const
{
    const auto [index, diff] = closestCycle(m_cycles, referenceDateTime);
    if (index == -1) {
        return std::nullopt;
    }

    const auto &cycle = m_cycles[index];
    if (diff <= 12h) {
        if (const auto transition = cycle.nextTransition(referenceDateTime)) {
            return transition;
        }

        if (index + 1 < m_cycles.size()) {
            const auto &nextCycle = m_cycles[index + 1];
            if (const auto transition = nextCycle.nextTransition(referenceDateTime)) {
                return transition;
            }
        }
    }

    const auto extrapolatedCycle = cycle.extrapolated(referenceDateTime);
    if (const auto transition = extrapolatedCycle.nextTransition(referenceDateTime)) {
        return transition;
    }

    const auto extrapolatedMorning = extrapolatedCycle.morning();
    return KDarkLightTransition(extrapolatedMorning.type(), extrapolatedMorning.startDateTime().addDays(1), extrapolatedMorning.endDateTime().addDays(1));
}

static std::optional<KDarkLightTransition> deserializeTransition(QDataStream &stream)
{
    int type;
    qint64 start;
    qint64 end;

    stream >> type >> start >> end;

    if (!start || !end) {
        return std::nullopt;
    }

    switch (type) {
    case KDarkLightTransition::Morning:
    case KDarkLightTransition::Evening:
        break;
    default:
        return std::nullopt;
    }

    return KDarkLightTransition(KDarkLightTransition::Type(type), QDateTime::fromMSecsSinceEpoch(start), QDateTime::fromMSecsSinceEpoch(end));
}

static void serializeTransition(QDataStream &stream, const KDarkLightTransition &transition)
{
    stream << int(transition.type()) << transition.startDateTime().toMSecsSinceEpoch() << transition.endDateTime().toMSecsSinceEpoch();
}

static std::optional<KDarkLightCycle> deserializeCycle(QDataStream &stream)
{
    qint64 noon;
    stream >> noon;
    if (!noon) {
        return std::nullopt;
    }

    const auto morning = deserializeTransition(stream);
    if (!morning) {
        return std::nullopt;
    }

    const auto evening = deserializeTransition(stream);
    if (!evening) {
        return std::nullopt;
    }

    return KDarkLightCycle(QDateTime::fromMSecsSinceEpoch(noon), *morning, *evening);
}

static void serializeCycle(QDataStream &stream, const KDarkLightCycle &cycle)
{
    stream << cycle.noonDateTime().toMSecsSinceEpoch();
    serializeTransition(stream, cycle.morning());
    serializeTransition(stream, cycle.evening());
}

static std::optional<KDarkLightSchedule> deserializeSchedule(QDataStream &stream)
{
    int version;
    stream >> version;
    if (version != 1) {
        return std::nullopt;
    }

    QList<KDarkLightCycle> cycles;
    int size;
    stream >> size;
    for (int i = 0; i < size; ++i) {
        const auto cycle = deserializeCycle(stream);
        if (!cycle) {
            return std::nullopt;
        }
        cycles.append(*cycle);
    }

    return KDarkLightSchedule(cycles);
}

static void serializeSchedule(QDataStream &stream, const KDarkLightSchedule &schedule)
{
    stream << 1; // version

    const auto cycles = schedule.cycles();
    stream << int(cycles.size());
    for (const auto &cycle : cycles) {
        serializeCycle(stream, cycle);
    }
}

QString KDarkLightSchedule::toState() const
{
    if (m_cycles.isEmpty()) {
        return QString();
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    serializeSchedule(out, *this);

    return QString::fromLatin1(byteArray.toBase64());
}

std::optional<KDarkLightSchedule> KDarkLightSchedule::fromState(const QString &state)
{
    if (state.isEmpty()) {
        return std::nullopt;
    }
    const QByteArray base64 = QByteArray::fromBase64(state.toLatin1());
    QDataStream stream(base64);
    return deserializeSchedule(stream);
}

static int daylightDurationInSeconds(QTime morning, QTime evening)
{
    if (morning < evening) {
        return morning.secsTo(evening);
    } else {
        const int secondsInDay = 86400;
        return secondsInDay - evening.secsTo(morning);
    }
}

KDarkLightSchedule KDarkLightSchedule::forecast(const QDateTime &dateTime, QTime morning, QTime evening, std::chrono::milliseconds transitionDuration, int cycleCount)
{
    QList<KDarkLightCycle> cycles;
    cycles.reserve(cycleCount + 1);

    const int halfOfDaylight = daylightDurationInSeconds(morning, evening) / 2;
    const QTime noon = morning.addSecs(halfOfDaylight);
    const QDateTime localDateTime = dateTime.toLocalTime();
    for (int day = -1; day < cycleCount; ++day) {
        const QDateTime noonDataTime(localDateTime.date().addDays(day), noon);

        const QDateTime startOfMorning = noonDataTime.addSecs(-halfOfDaylight);
        const QDateTime endOfMorning = startOfMorning + transitionDuration;

        const QDateTime startOfEvening = noonDataTime.addSecs(halfOfDaylight);
        const QDateTime endOfEvening = startOfEvening + transitionDuration;

        cycles.append(KDarkLightCycle(noonDataTime,
                                      KDarkLightTransition(KDarkLightTransition::Morning, startOfMorning, endOfMorning),
                                      KDarkLightTransition(KDarkLightTransition::Evening, startOfEvening, endOfEvening)));
    }

    return KDarkLightSchedule(cycles);
}

std::optional<KDarkLightSchedule> KDarkLightSchedule::forecast(const QDateTime &dateTime, qreal latitude, qreal longitude, int cycleCount)
{
    QList<KDarkLightCycle> cycles;
    cycles.reserve(cycleCount + 1);

    for (int day = -1; day < cycleCount; ++day) {
        const KHolidays::SunEvents sunEvents(dateTime.addDays(day), latitude, longitude);

        const QDateTime civilDawn = sunEvents.civilDawn();
        if (!civilDawn.isValid()) {
            return std::nullopt;
        }

        const QDateTime sunrise = sunEvents.sunrise();
        if (!sunrise.isValid()) {
            return std::nullopt;
        }

        const QDateTime civilDusk = sunEvents.civilDusk();
        if (!civilDusk.isValid()) {
            return std::nullopt;
        }

        const QDateTime sunset = sunEvents.sunset();
        if (!sunset.isValid()) {
            return std::nullopt;
        }

        cycles.append(KDarkLightCycle(sunEvents.solarNoon(),
                                      KDarkLightTransition(KDarkLightTransition::Morning, civilDawn, sunrise),
                                      KDarkLightTransition(KDarkLightTransition::Evening, sunset, civilDusk)));
    }

    return KDarkLightSchedule(cycles);
}
