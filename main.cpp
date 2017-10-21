#include "drivermonitorandroid.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DriverMonitorAndroid w;
    w.show();

    return a.exec();
}
