/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QObject>
#include <QTest>
#include <QTimeZone>

#include "knighttimeschedule.h"

using namespace std::chrono_literals;

class ScheduleTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void timedForecast();
    void solarForecast();
    void state();
    void previousTransition();
    void nextTransition();
};

void ScheduleTest::timedForecast()
{
    const KNightTimeSchedule schedule = KNightTimeSchedule::forecast(QDateTime(QDate(2025, 5, 25), QTime(12, 0)), QTime(6, 0), QTime(18, 0), 30min, 3);
    const QList<KNightTimeCycle> cycles{
        KNightTimeCycle(QDateTime(QDate(2025, 5, 24), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 24), QTime(6, 0)), QDateTime(QDate(2025, 5, 24), QTime(6, 30))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 24), QTime(18, 0)), QDateTime(QDate(2025, 5, 24), QTime(18, 30)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 25), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(18, 0)), QDateTime(QDate(2025, 5, 25), QTime(18, 30)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 26), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 26), QTime(6, 0)), QDateTime(QDate(2025, 5, 26), QTime(6, 30))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 26), QTime(18, 0)), QDateTime(QDate(2025, 5, 26), QTime(18, 30)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 27), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 27), QTime(6, 0)), QDateTime(QDate(2025, 5, 27), QTime(6, 30))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 27), QTime(18, 0)), QDateTime(QDate(2025, 5, 27), QTime(18, 30)))),
    };

    QCOMPARE(schedule.cycles(), cycles);
}

void ScheduleTest::solarForecast()
{
    const QTimeZone tz = QTimeZone::fromDurationAheadOfUtc(3h);

    QCOMPARE(KNightTimeSchedule::forecast(QDateTime(QDate(2025, 5, 25), QTime(12, 0), tz), 90, 0), std::nullopt);
    QCOMPARE(KNightTimeSchedule::forecast(QDateTime(QDate(2025, 5, 25), QTime(12, 0), tz), -90, 0), std::nullopt);

    const std::optional<KNightTimeSchedule> schedule = KNightTimeSchedule::forecast(QDateTime(QDate(2025, 5, 25), QTime(12, 0), tz), 50.45, 30.52, 3);
    const QList<KNightTimeCycle> cycles{
        KNightTimeCycle(QDateTime(QDate(2025, 5, 24), QTime(12, 54, 48), tz),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 24), QTime(4, 16, 33), tz), QDateTime(QDate(2025, 5, 24), QTime(4, 58, 34), tz)),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 24), QTime(20, 51, 1), tz), QDateTime(QDate(2025, 5, 24), QTime(21, 33, 3), tz))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 25), QTime(12, 54, 54), tz),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(4, 15, 17), tz), QDateTime(QDate(2025, 5, 25), QTime(4, 57, 31), tz)),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(20, 52, 16), tz), QDateTime(QDate(2025, 5, 25), QTime(21, 34, 30), tz))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 26), QTime(12, 55, 0), tz),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 26), QTime(4, 14, 4), tz), QDateTime(QDate(2025, 5, 26), QTime(4, 56, 30), tz)),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 26), QTime(20, 53, 30), tz), QDateTime(QDate(2025, 5, 26), QTime(21, 35, 56), tz))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 27), QTime(12, 55, 7), tz),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 27), QTime(4, 12, 54), tz), QDateTime(QDate(2025, 5, 27), QTime(4, 55, 32), tz)),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 27), QTime(20, 54, 42), tz), QDateTime(QDate(2025, 5, 27), QTime(21, 37, 20), tz))),
    };

    QCOMPARE(schedule->cycles(), cycles);
}

void ScheduleTest::state()
{
    QCOMPARE(KNightTimeSchedule::fromState(QString()), std::nullopt);
    QCOMPARE(KNightTimeSchedule::fromState(QStringLiteral("foobar")), std::nullopt);

    QCOMPARE(KNightTimeSchedule().toState(), QString());

    const auto schedule = KNightTimeSchedule::forecast(QDateTime::currentDateTime());
    QCOMPARE(KNightTimeSchedule::fromState(schedule.toState()), schedule);
}

void ScheduleTest::previousTransition()
{
    // May 25th is the current date. The schedule contains: yesterday's cycle, today's cycle, tomorrow's
    // cycle, and the cycle of a day after tomorrow.
    const int firstCycle = -1;
    const KNightTimeSchedule schedule({
        KNightTimeCycle(QDateTime(QDate(2025, 5, 24), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 24), QTime(5, 50)), QDateTime(QDate(2025, 5, 24), QTime(6, 20))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 24), QTime(18, 10)), QDateTime(QDate(2025, 5, 24), QTime(18, 40)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 25), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(18, 0)), QDateTime(QDate(2025, 5, 25), QTime(18, 30)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 26), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 26), QTime(6, 10)), QDateTime(QDate(2025, 5, 26), QTime(6, 40))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 26), QTime(17, 50)), QDateTime(QDate(2025, 5, 26), QTime(18, 20)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 27), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 27), QTime(6, 20)), QDateTime(QDate(2025, 5, 27), QTime(6, 50))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 27), QTime(17, 40)), QDateTime(QDate(2025, 5, 27), QTime(18, 10)))),
    });

    // Extrapolate transition times from the first cycle.
    for (int day = -3; day < -1; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDateTime referenceDateTime(QDate(2025, 5, 25).addDays(day), QTime(hour, 0));
            const auto previousTransition = schedule.previousTransition(referenceDateTime);

            const bool passedMorning = referenceDateTime.time().secsTo(QTime(5, 50)) <= 0;
            const bool passedEvening = referenceDateTime.time().secsTo(QTime(18, 10)) <= 0;

            if (!passedMorning) {
                QCOMPARE(previousTransition, KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(referenceDateTime.date().addDays(-1), QTime(18, 10)), QDateTime(referenceDateTime.date().addDays(-1), QTime(18, 40))));
            } else if (!passedEvening) {
                QCOMPARE(previousTransition, KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(referenceDateTime.date(), QTime(5, 50)), QDateTime(referenceDateTime.date(), QTime(6, 20))));
            } else {
                QCOMPARE(previousTransition, KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(referenceDateTime.date(), QTime(18, 10)), QDateTime(referenceDateTime.date(), QTime(18, 40))));
            }
        }
    }

    for (int day = -1; day < 3; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDateTime referenceDateTime(QDate(2025, 5, 25).addDays(day), QTime(hour, 0));
            const auto previousTransition = schedule.previousTransition(referenceDateTime);

            const KNightTimeCycle cycle = schedule.cycles().at(day - firstCycle);
            const bool passedMorning = referenceDateTime.time().secsTo(cycle.morning().startDateTime().time()) <= 0;
            const bool passedEvening = referenceDateTime.time().secsTo(cycle.evening().startDateTime().time()) <= 0;

            if (!passedMorning) {
                const int previousCycleIndex = day - firstCycle - 1;
                const KNightTimeCycle previousCycle = previousCycleIndex >= 0 ? schedule.cycles().at(previousCycleIndex) : cycle.extrapolated(referenceDateTime.addDays(-1));
                QCOMPARE(previousTransition, previousCycle.evening());
            } else if (!passedEvening) {
                QCOMPARE(previousTransition, cycle.morning());
            } else {
                QCOMPARE(previousTransition, cycle.evening());
            }
        }
    }

    // Extrapolate transition times from the last cycle.
    for (int day = 3; day < 5; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDateTime referenceDateTime(QDate(2025, 5, 25).addDays(day), QTime(hour, 0));
            const auto previousTransition = schedule.previousTransition(referenceDateTime);

            const bool passedMorning = referenceDateTime.time().secsTo(QTime(6, 20)) <= 0;
            const bool passedEvening = referenceDateTime.time().secsTo(QTime(17, 40)) <= 0;

            if (!passedMorning) {
                QCOMPARE(previousTransition, KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(referenceDateTime.date().addDays(-1), QTime(17, 40)), QDateTime(referenceDateTime.date().addDays(-1), QTime(18, 10))));
            } else if (!passedEvening) {
                QCOMPARE(previousTransition, KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(referenceDateTime.date(), QTime(6, 20)), QDateTime(referenceDateTime.date(), QTime(6, 50))));
            } else {
                QCOMPARE(previousTransition, KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(referenceDateTime.date(), QTime(17, 40)), QDateTime(referenceDateTime.date(), QTime(18, 10))));
            }
        }
    }
}

void ScheduleTest::nextTransition()
{
    // May 25th is the current date. The schedule contains: yesterday's cycle, today's cycle, tomorrow's
    // cycle, and the cycle of a day after tomorrow.
    const int firstCycle = -1;
    const KNightTimeSchedule schedule({
        KNightTimeCycle(QDateTime(QDate(2025, 5, 24), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 24), QTime(5, 50)), QDateTime(QDate(2025, 5, 24), QTime(6, 20))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 24), QTime(18, 10)), QDateTime(QDate(2025, 5, 24), QTime(18, 40)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 25), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(18, 0)), QDateTime(QDate(2025, 5, 25), QTime(18, 30)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 26), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 26), QTime(6, 10)), QDateTime(QDate(2025, 5, 26), QTime(6, 40))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 26), QTime(17, 50)), QDateTime(QDate(2025, 5, 26), QTime(18, 20)))),
        KNightTimeCycle(QDateTime(QDate(2025, 5, 27), QTime(12, 0)),
                        KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 27), QTime(6, 20)), QDateTime(QDate(2025, 5, 27), QTime(6, 50))),
                        KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(QDate(2025, 5, 27), QTime(17, 40)), QDateTime(QDate(2025, 5, 27), QTime(18, 10)))),
    });

    // Extrapolate transition times from the first cycle.
    for (int day = -3; day < -1; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDateTime referenceDateTime(QDate(2025, 5, 25).addDays(day), QTime(hour, 0));
            const auto nextTransition = schedule.nextTransition(referenceDateTime);

            const bool passedMorning = referenceDateTime.time().secsTo(QTime(5, 50)) <= 0;
            const bool passedEvening = referenceDateTime.time().secsTo(QTime(18, 10)) <= 0;

            if (!passedMorning) {
                QCOMPARE(nextTransition, KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(referenceDateTime.date(), QTime(5, 50)), QDateTime(referenceDateTime.date(), QTime(6, 20))));
            } else if (!passedEvening) {
                QCOMPARE(nextTransition, KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(referenceDateTime.date(), QTime(18, 10)), QDateTime(referenceDateTime.date(), QTime(18, 40))));
            } else {
                QCOMPARE(nextTransition, KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(referenceDateTime.date().addDays(1), QTime(5, 50)), QDateTime(referenceDateTime.date().addDays(1), QTime(6, 20))));
            }
        }
    }

    for (int day = -1; day < 3; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDateTime referenceDateTime(QDate(2025, 5, 25).addDays(day), QTime(hour, 0));
            const auto nextTransition = schedule.nextTransition(referenceDateTime);

            const KNightTimeCycle cycle = schedule.cycles().at(day - firstCycle);
            const bool passedMorning = referenceDateTime.time().secsTo(cycle.morning().startDateTime().time()) <= 0;
            const bool passedEvening = referenceDateTime.time().secsTo(cycle.evening().startDateTime().time()) <= 0;

            if (!passedMorning) {
                QCOMPARE(nextTransition, cycle.morning());
            } else if (!passedEvening) {
                QCOMPARE(nextTransition, cycle.evening());
            } else {
                const int nextCycleIndex = day - firstCycle + 1;
                const KNightTimeCycle nextCycle = nextCycleIndex < schedule.cycles().size() ? schedule.cycles().at(nextCycleIndex) : cycle.extrapolated(referenceDateTime.addDays(1));
                QCOMPARE(nextTransition, nextCycle.morning());
            }
        }
    }

    // Extrapolate transition times from the last cycle.
    for (int day = 3; day < 5; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDateTime referenceDateTime(QDate(2025, 5, 25).addDays(day), QTime(hour, 0));
            const auto nextTransition = schedule.nextTransition(referenceDateTime);

            const bool passedMorning = referenceDateTime.time().secsTo(QTime(6, 20)) <= 0;
            const bool passedEvening = referenceDateTime.time().secsTo(QTime(17, 40)) <= 0;

            if (!passedMorning) {
                QCOMPARE(nextTransition, KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(referenceDateTime.date(), QTime(6, 20)), QDateTime(referenceDateTime.date(), QTime(6, 50))));
            } else if (!passedEvening) {
                QCOMPARE(nextTransition, KNightTimeTransition(KNightTimeTransition::Evening, QDateTime(referenceDateTime.date(), QTime(17, 40)), QDateTime(referenceDateTime.date(), QTime(18, 10))));
            } else {
                QCOMPARE(nextTransition, KNightTimeTransition(KNightTimeTransition::Morning, QDateTime(referenceDateTime.date().addDays(1), QTime(6, 20)), QDateTime(referenceDateTime.date().addDays(1), QTime(6, 50))));
            }
        }
    }
}

QTEST_MAIN(ScheduleTest)

#include "schedule_test.moc"
