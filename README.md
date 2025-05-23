# KNightTime

The `KNightTime` provides the dark-light cycle schedule information. It can be used to implement
features such as adjusting the screen color temperature based on time of day, etc.

## Features

The `KNightTime` offers a system service that provides the schedule and a client library to subscribe
to the updates from the daemon.

Example usage:

```cpp
#include <KNightTimeScheduleProvider>
#include <KSharedConfig>

auto stateConfig = KSharedConfig::openStateConfig();
auto provider = new KNightTimeScheduleProvider(stateConfig->group(QStringLiteral("DarkLightCycle")).readEntry(QStringLiteral("State")));
connect(provider, &KNightTimeScheduleProvider::scheduleChanged, [provider, stateConfig]() {
    stateConfig->group(QStringLiteral("DarkLightCycle")).writeEntry(QStringLiteral("State"), provider->state());
    stateConfig->sync();

    qDebug() << "next transition:" << provider->schedule().nextTransition(QDateTime::currentDateTime());
});

qDebug() << "next transition:" << provider->schedule().nextTransition(QDateTime::currentDateTime());
```

## License

This project is licenced under LGPL v2.1+, you can find all the information under `LICENSES/`.
