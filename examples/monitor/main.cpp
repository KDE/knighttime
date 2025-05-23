#include <QCoreApplication>
#include <QDebug>

#include "knighttimescheduleprovider.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    KNightTimeScheduleProvider provider;

    qDebug() << provider.schedule();
    QObject::connect(&provider, &KNightTimeScheduleProvider::scheduleChanged, [&provider]() {
        qDebug() << provider.schedule();
    });

    return app.exec();
}
