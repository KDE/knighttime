/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "knighttimeschedule.h"

#include <KHolidays/SunEvents>

#include <QBuffer>

using namespace std::chrono_literals;

QDebug operator<<(QDebug debug, const KNightTimeTransition &transition)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "KNightTimeTransition(type = " << int(transition.type());
    debug << ", start = " << transition.startDateTime();
    debug << ", end = " << transition.endDateTime();
    debug << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const KNightTimeCycle &cycle)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "KNightTimeCycle(noon = " << cycle.noonDateTime();
    debug << ", morning = " << cycle.morning();
    debug << ", evening = " << cycle.evening();
    debug << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const KNightTimeSchedule &schedule)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "KNightTimeSchedule(cycles = " << schedule.cycles() << ")";
    return debug;
}

KNightTimeTransition::KNightTimeTransition()
    : m_type(Morning)
{
}

KNightTimeTransition::KNightTimeTransition(Type type, const QDateTime &startDateTime, const QDateTime &endDateTime)
    : m_type(type)
    , m_startDateTime(startDateTime)
    , m_endDateTime(endDateTime)
{
}

KNightTimeTransition::Relation KNightTimeTransition::test(const QDateTime &dateTime) const
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

qreal KNightTimeTransition::progress(const QDateTime &dateTime) const
{
    const qreal elapsed = m_startDateTime.secsTo(dateTime);
    const qreal total = m_startDateTime.secsTo(m_endDateTime);
    return std::clamp<qreal>(elapsed / total, 0.0, 1.0);
}

KNightTimeTransition::Type KNightTimeTransition::type() const
{
    return m_type;
}

QDateTime KNightTimeTransition::startDateTime() const
{
    return m_startDateTime;
}

QDateTime KNightTimeTransition::endDateTime() const
{
    return m_endDateTime;
}

KNightTimeCycle::KNightTimeCycle()
{
}

KNightTimeCycle::KNightTimeCycle(const QDateTime &noonDateTime, const KNightTimeTransition &morning, const KNightTimeTransition &evening)
    : m_noonDateTime(noonDateTime)
    , m_morning(morning)
    , m_evening(evening)
{
}

KNightTimeCycle KNightTimeCycle::extrapolated(const QDateTime &referenceDateTime) const
{
    const QDateTime localReferenceDateTime = referenceDateTime.toLocalTime();
    const QDateTime localNoonDateTime = m_noonDateTime.toLocalTime();
    const QDateTime newNoonDateTime(localReferenceDateTime.date(), localNoonDateTime.time());

    return KNightTimeCycle(newNoonDateTime,
                           KNightTimeTransition(KNightTimeTransition::Morning,
                                                newNoonDateTime.addMSecs(-m_morning.startDateTime().msecsTo(m_noonDateTime)),
                                                newNoonDateTime.addMSecs(-m_morning.endDateTime().msecsTo(m_noonDateTime))),
                           KNightTimeTransition(KNightTimeTransition::Evening,
                                                newNoonDateTime.addMSecs(-m_evening.startDateTime().msecsTo(m_noonDateTime)),
                                                newNoonDateTime.addMSecs(-m_evening.endDateTime().msecsTo(m_noonDateTime))));
}

QDateTime KNightTimeCycle::noonDateTime() const
{
    return m_noonDateTime;
}

KNightTimeTransition KNightTimeCycle::morning() const
{
    return m_morning;
}

KNightTimeTransition KNightTimeCycle::evening() const
{
    return m_evening;
}

std::optional<KNightTimeTransition> KNightTimeCycle::nextTransition(const QDateTime &dateTime) const
{
    switch (m_morning.test(dateTime)) {
    case KNightTimeTransition::Upcoming:
        return m_morning;
    case KNightTimeTransition::InProgress:
    case KNightTimeTransition::Passed:
        break;
    }

    switch (m_evening.test(dateTime)) {
    case KNightTimeTransition::Upcoming:
        return m_evening;
    case KNightTimeTransition::InProgress:
    case KNightTimeTransition::Passed:
        break;
    }

    return std::nullopt;
}

std::optional<KNightTimeTransition> KNightTimeCycle::previousTransition(const QDateTime &dateTime) const
{
    switch (m_evening.test(dateTime)) {
    case KNightTimeTransition::Upcoming:
        break;
    case KNightTimeTransition::InProgress:
    case KNightTimeTransition::Passed:
        return m_evening;
    }

    switch (m_morning.test(dateTime)) {
    case KNightTimeTransition::Upcoming:
        break;
    case KNightTimeTransition::InProgress:
    case KNightTimeTransition::Passed:
        return m_morning;
    }

    return std::nullopt;
}

KNightTimeSchedule::KNightTimeSchedule()
{
}

KNightTimeSchedule::KNightTimeSchedule(const QList<KNightTimeCycle> &cycles)
    : m_cycles(cycles)
{
}

QList<KNightTimeCycle> KNightTimeSchedule::cycles() const
{
    return m_cycles;
}

static std::pair<int, std::chrono::milliseconds> closestCycle(const QList<KNightTimeCycle> &cycles, const QDateTime &dateTime)
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

std::optional<KNightTimeTransition> KNightTimeSchedule::previousTransition(const QDateTime &referenceDateTime) const
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
    return KNightTimeTransition(extrapolatedEvening.type(), extrapolatedEvening.startDateTime().addDays(-1), extrapolatedEvening.endDateTime().addDays(-1));
}

std::optional<KNightTimeTransition> KNightTimeSchedule::nextTransition(const QDateTime &referenceDateTime) const
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
    return KNightTimeTransition(extrapolatedMorning.type(), extrapolatedMorning.startDateTime().addDays(1), extrapolatedMorning.endDateTime().addDays(1));
}

static std::optional<KNightTimeTransition> deserializeTransition(QDataStream &stream)
{
    int type;
    qint64 start;
    qint64 end;

    stream >> type >> start >> end;

    if (!start || !end) {
        return std::nullopt;
    }

    switch (type) {
    case KNightTimeTransition::Morning:
    case KNightTimeTransition::Evening:
        break;
    default:
        return std::nullopt;
    }

    return KNightTimeTransition(KNightTimeTransition::Type(type), QDateTime::fromMSecsSinceEpoch(start), QDateTime::fromMSecsSinceEpoch(end));
}

static void serializeTransition(QDataStream &stream, const KNightTimeTransition &transition)
{
    stream << int(transition.type()) << transition.startDateTime().toMSecsSinceEpoch() << transition.endDateTime().toMSecsSinceEpoch();
}

static std::optional<KNightTimeCycle> deserializeCycle(QDataStream &stream)
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

    return KNightTimeCycle(QDateTime::fromMSecsSinceEpoch(noon), *morning, *evening);
}

static void serializeCycle(QDataStream &stream, const KNightTimeCycle &cycle)
{
    stream << cycle.noonDateTime().toMSecsSinceEpoch();
    serializeTransition(stream, cycle.morning());
    serializeTransition(stream, cycle.evening());
}

static std::optional<KNightTimeSchedule> deserializeSchedule(QDataStream &stream)
{
    int version;
    stream >> version;
    if (version != 1) {
        return std::nullopt;
    }

    QList<KNightTimeCycle> cycles;
    int size;
    stream >> size;
    for (int i = 0; i < size; ++i) {
        const auto cycle = deserializeCycle(stream);
        if (!cycle) {
            return std::nullopt;
        }
        cycles.append(*cycle);
    }

    return KNightTimeSchedule(cycles);
}

static void serializeSchedule(QDataStream &stream, const KNightTimeSchedule &schedule)
{
    stream << 1; // version

    const auto cycles = schedule.cycles();
    stream << int(cycles.size());
    for (const auto &cycle : cycles) {
        serializeCycle(stream, cycle);
    }
}

QString KNightTimeSchedule::toState() const
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

std::optional<KNightTimeSchedule> KNightTimeSchedule::fromState(const QString &state)
{
    if (state.isEmpty()) {
        return std::nullopt;
    }
    const QByteArray base64 = QByteArray::fromBase64(state.toLatin1());
    QDataStream stream(base64);
    return deserializeSchedule(stream);
}

KNightTimeSchedule KNightTimeSchedule::forecast(const QDateTime &dateTime, const QTime &morning, const QTime &evening, std::chrono::milliseconds transitionDuration, int cycleCount)
{
    QList<KNightTimeCycle> cycles;
    cycles.reserve(cycleCount + 1);

    const int halfOfDaylight = morning.secsTo(evening) / 2;
    const QTime noon = morning.addSecs(halfOfDaylight);
    const QDateTime localDateTime = dateTime.toLocalTime();
    for (int day = -1; day < cycleCount; ++day) {
        const QDateTime noonDataTime(localDateTime.date().addDays(day), noon);

        const QDateTime startOfMorning = noonDataTime.addSecs(-halfOfDaylight);
        const QDateTime endOfMorning = startOfMorning + transitionDuration;

        const QDateTime startOfEvening = noonDataTime.addSecs(halfOfDaylight);
        const QDateTime endOfEvening = startOfEvening + transitionDuration;

        cycles.append(KNightTimeCycle(noonDataTime,
                                      KNightTimeTransition(KNightTimeTransition::Morning, startOfMorning, endOfMorning),
                                      KNightTimeTransition(KNightTimeTransition::Evening, startOfEvening, endOfEvening)));
    }

    return KNightTimeSchedule(cycles);
}

std::optional<KNightTimeSchedule> KNightTimeSchedule::forecast(const QDateTime &dateTime, qreal latitude, qreal longitude, int cycleCount)
{
    QList<KNightTimeCycle> cycles;
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

        cycles.append(KNightTimeCycle(sunEvents.solarNoon(),
                                      KNightTimeTransition(KNightTimeTransition::Morning, civilDawn, sunrise),
                                      KNightTimeTransition(KNightTimeTransition::Evening, sunset, civilDusk)));
    }

    return KNightTimeSchedule(cycles);
}
