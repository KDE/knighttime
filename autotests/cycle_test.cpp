/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QObject>
#include <QTest>

#include "kdarklightschedule.h"

class CycleTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void extrapolated();
    void nextTransition();
    void previousTransition();
};

void CycleTest::extrapolated()
{
    const KDarkLightCycle originalCycle(QDateTime(QDate(2025, 5, 25), QTime(12, 0)),
                                        KDarkLightTransition(KDarkLightTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30))),
                                        KDarkLightTransition(KDarkLightTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(18, 0)), QDateTime(QDate(2025, 5, 25), QTime(18, 30))));

    for (int day = -1; day <= 1; ++day) {
        for (int hour = 0; hour < 24; ++hour) {
            const QDate date(2025, 5, 25 + day);
            const QDateTime referenceDateTime(date, QTime(hour, 0));
            const KDarkLightCycle exepectedCycle(QDateTime(date, QTime(12, 0)),
                                                 KDarkLightTransition(KDarkLightTransition::Morning, QDateTime(date, QTime(6, 0)), QDateTime(date, QTime(6, 30))),
                                                 KDarkLightTransition(KDarkLightTransition::Evening, QDateTime(date, QTime(18, 0)), QDateTime(date, QTime(18, 30))));

            QCOMPARE(originalCycle.extrapolated(referenceDateTime), exepectedCycle);
        }
    }
}

void CycleTest::nextTransition()
{
    const KDarkLightCycle cycle(QDateTime(QDate(2025, 5, 25), QTime(12, 0)),
                                KDarkLightTransition(KDarkLightTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30))),
                                KDarkLightTransition(KDarkLightTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(18, 0)), QDateTime(QDate(2025, 5, 25), QTime(18, 30))));

    QCOMPARE(cycle.nextTransition(QDateTime(QDate(2025, 5, 24), QTime(12, 0))), cycle.morning());

    for (int hour = 0; hour < 6; ++hour) {
        QCOMPARE(cycle.nextTransition(QDateTime(QDate(2025, 5, 25), QTime(hour, 0))), cycle.morning());
    }

    for (int hour = 6; hour < 18; ++hour) {
        QCOMPARE(cycle.nextTransition(QDateTime(QDate(2025, 5, 25), QTime(hour, 0))), cycle.evening());
    }

    for (int hour = 18; hour < 24; ++hour) {
        QCOMPARE(cycle.nextTransition(QDateTime(QDate(2025, 5, 25), QTime(hour, 0))), std::nullopt);
    }

    QCOMPARE(cycle.nextTransition(QDateTime(QDate(2025, 5, 26), QTime(12, 0))), std::nullopt);
}

void CycleTest::previousTransition()
{
    const KDarkLightCycle cycle(QDateTime(QDate(2025, 5, 25), QTime(12, 0)),
                                KDarkLightTransition(KDarkLightTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30))),
                                KDarkLightTransition(KDarkLightTransition::Evening, QDateTime(QDate(2025, 5, 25), QTime(18, 0)), QDateTime(QDate(2025, 5, 25), QTime(18, 30))));

    QCOMPARE(cycle.previousTransition(QDateTime(QDate(2025, 5, 24), QTime(12, 0))), std::nullopt);

    for (int hour = 0; hour < 6; ++hour) {
        QCOMPARE(cycle.previousTransition(QDateTime(QDate(2025, 5, 25), QTime(hour, 0))), std::nullopt);
    }

    for (int hour = 6; hour < 18; ++hour) {
        QCOMPARE(cycle.previousTransition(QDateTime(QDate(2025, 5, 25), QTime(hour, 0))), cycle.morning());
    }

    for (int hour = 18; hour < 24; ++hour) {
        QCOMPARE(cycle.previousTransition(QDateTime(QDate(2025, 5, 25), QTime(hour, 0))), cycle.evening());
    }

    QCOMPARE(cycle.previousTransition(QDateTime(QDate(2025, 5, 26), QTime(12, 0))), cycle.evening());
}

QTEST_MAIN(CycleTest)

#include "cycle_test.moc"
