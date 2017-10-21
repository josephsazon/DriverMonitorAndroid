#ifndef DRIVERMONITORANDROID_H
#define DRIVERMONITORANDROID_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

#include <chat.h>

namespace Ui {
class DriverMonitorAndroid;
}

class DriverMonitorAndroid : public QMainWindow
{
    Q_OBJECT

public:
    explicit DriverMonitorAndroid(QWidget *parent = 0);
    ~DriverMonitorAndroid();

    void startGPS();

signals:
    void passToChat(const QString &message);

public slots:
    void positionUpdated(QGeoPositionInfo m_geoPositionInfo);
    void requestUpdate();
    void messageToCommand(const QString &message);

private slots:
    void on_OpenCamera_pushButton_clicked();

    void on_CloseCamera_pushButton_clicked();

private:
    Ui::DriverMonitorAndroid *ui;

    QGeoPositionInfo *geoPositionInfo;
    QGeoPositionInfoSource *geoPositionInfoSource;

    double speed;

    QString commandString;
    Chat *chat;

    QMediaPlayer *alertSound;
};

#endif // DRIVERMONITORANDROID_H
