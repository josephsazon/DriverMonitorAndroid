#include "drivermonitorandroid.h"
#include "ui_drivermonitorandroid.h"

DriverMonitorAndroid::DriverMonitorAndroid(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DriverMonitorAndroid)
{
    ui->setupUi(this);

    geoPositionInfo = new QGeoPositionInfo();
    startGPS();

    connect(ui->RequestGpsUpdate_pushButton, SIGNAL(clicked()), this, SLOT(requestUpdate()));

    chat = new Chat(this);

    connect(this, SIGNAL(passToChat(QString)), chat, SLOT(sendClicked(QString)));
    connect(chat, SIGNAL(passToApp(QString)), this, SLOT(messageToCommand(QString)));

    alertSound = new QMediaPlayer(this);
    alertSound->setMedia(QUrl("qrc:/res/alarm.mp3"));
}

DriverMonitorAndroid::~DriverMonitorAndroid()
{
    delete ui;
}

void DriverMonitorAndroid::startGPS()
{
    if(!geoPositionInfoSource)
    {
        geoPositionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);
        geoPositionInfoSource->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);

        connect(geoPositionInfoSource, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
        geoPositionInfoSource->setUpdateInterval(200);
        geoPositionInfoSource->startUpdates();
    }
    else
    {
        geoPositionInfoSource->setUpdateInterval(5000);
        geoPositionInfoSource->startUpdates();
    }
}

void DriverMonitorAndroid::positionUpdated(QGeoPositionInfo m_geoPositionInfo)
{
    if(m_geoPositionInfo.isValid())
    {
        this->speed = m_geoPositionInfo.attribute(QGeoPositionInfo::GroundSpeed) * 3.6;
        ui->speed_lcdNumber->display(speed);
    }
}

void DriverMonitorAndroid::requestUpdate()
{
    geoPositionInfoSource->requestUpdate();
}

void DriverMonitorAndroid::messageToCommand(const QString &message)
{
    commandString += message + '\n';
    ui->commandText->setText(commandString);
    if(message == "Alarm\n")
    {
        //Sound Alarm
        if(alertSound->state() == QMediaPlayer::PlayingState)
            alertSound->setPosition(0);
        else
            alertSound->play();
    }
}

void DriverMonitorAndroid::on_OpenCamera_pushButton_clicked()
{
    emit passToChat("Open Camera");
}

void DriverMonitorAndroid::on_CloseCamera_pushButton_clicked()
{
    emit passToChat("Close Camera");
}

