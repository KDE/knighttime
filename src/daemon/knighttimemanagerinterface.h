/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QDBusContext>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QVariant>

class KNightTimeCycle;
class KNightTimeManager;
class KNightTimeSchedule;

class KNightTimeManagerInterface : public QObject, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.NightTime.Manager")
    Q_PROPERTY(uint Version READ version)

public:
    explicit KNightTimeManagerInterface(KNightTimeManager *manager, QObject *parent = nullptr);

    uint version() const;

Q_SIGNALS:
    Q_SCRIPTABLE void Refreshed(const QVariantMap &data);

public Q_SLOTS:
    Q_SCRIPTABLE QVariantMap Subscribe();
    Q_SCRIPTABLE void Unsubscribe(uint cookie);

private Q_SLOTS:
    void OnScheduleChanged();
    void OnServiceUnregistered(const QString &serviceName);

private:
    KNightTimeManager *m_manager;
    QDBusServiceWatcher *m_serviceWatcher;
    QMultiMap<QString, uint> m_subscribers;
    uint m_lastCookie = 0;
};
