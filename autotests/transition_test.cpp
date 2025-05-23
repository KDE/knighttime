/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QObject>
#include <QTest>

#include "knighttimeschedule.h"

class TransitionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void relation();
    void progress();
};

void TransitionTest::relation()
{
    const KNightTimeTransition transition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30)));

    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(5, 0))), KNightTimeTransition::Upcoming);
    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 26), QTime(5, 0))), KNightTimeTransition::Passed);

    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(6, 0))), KNightTimeTransition::InProgress);
    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(6, 15))), KNightTimeTransition::InProgress);
    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(6, 30))), KNightTimeTransition::Passed);

    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(7, 0))), KNightTimeTransition::Passed);
}

void TransitionTest::progress()
{
    const KNightTimeTransition transition(KNightTimeTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30)));

    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(5, 0))), 0.0);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(6, 0))), 0.0);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(6, 15))), 0.5);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(6, 30))), 1.0);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(7, 0))), 1.0);
}

QTEST_MAIN(TransitionTest)

#include "transition_test.moc"
