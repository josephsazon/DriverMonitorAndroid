#ifndef DRIVERMONITORANDROID_H
#define DRIVERMONITORANDROID_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QTime>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

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
    void readEntry(const QString &message);
    void displayEntry(const QString &time, const QString &instance, const QString &duration);

signals:
    void passToChat(const QString &message);

public slots:
    void positionUpdated(QGeoPositionInfo m_geoPositionInfo);
    void requestUpdate();
    void messageToCommand(const QString &message);
    void clientStatus(const bool &status);    

private slots:
    void on_OpenCamera_pushButton_clicked();

    void on_CloseCamera_pushButton_clicked();

    void on_ThresholdSpeed_horizontalSlider_valueChanged(int value);

    void on_ThresholdSpeedBypass_checkBox_toggled(bool checked);

    //void on_changeAlarmButton_clicked();

private:
    Ui::DriverMonitorAndroid *ui;

    QGeoPositionInfo *geoPositionInfo;
    QGeoPositionInfoSource *geoPositionInfoSource;

    double speed;

    QString commandString;
    Chat *chat;

    QMediaPlayer *alertSound1;
    QMediaPlayer *alertSound2;

    bool client;

    QTime time;
};

#endif // DRIVERMONITORANDROID_H
