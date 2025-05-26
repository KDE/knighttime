/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QObject>
#include <QTest>

#include "kdarklightschedule.h"

class TransitionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void relation();
    void progress();
};

void TransitionTest::relation()
{
    const KDarkLightTransition transition(KDarkLightTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30)));

    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(5, 0))), KDarkLightTransition::Upcoming);
    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 26), QTime(5, 0))), KDarkLightTransition::Passed);

    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(6, 0))), KDarkLightTransition::InProgress);
    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(6, 15))), KDarkLightTransition::InProgress);
    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(6, 30))), KDarkLightTransition::Passed);

    QCOMPARE(transition.test(QDateTime(QDate(2025, 5, 25), QTime(7, 0))), KDarkLightTransition::Passed);
}

void TransitionTest::progress()
{
    const KDarkLightTransition transition(KDarkLightTransition::Morning, QDateTime(QDate(2025, 5, 25), QTime(6, 0)), QDateTime(QDate(2025, 5, 25), QTime(6, 30)));

    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(5, 0))), 0.0);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(6, 0))), 0.0);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(6, 15))), 0.5);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(6, 30))), 1.0);
    QCOMPARE(transition.progress(QDateTime(QDate(2025, 5, 25), QTime(7, 0))), 1.0);
}

QTEST_MAIN(TransitionTest)

#include "transition_test.moc"
