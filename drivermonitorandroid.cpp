#include "drivermonitorandroid.h"
#include "ui_drivermonitorandroid.h"

QVector<double> speedPlotData;
QVector<double> xPlotData;

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
    connect(chat, SIGNAL(clientActive(bool)), this, SLOT(clientStatus(bool)));

    alertSound = new QMediaPlayer(this);
    alertSound->setMedia(QUrl("qrc:/res/alarm.mp3"));

    ui->speedPlot->addGraph();
    ui->speedPlot->yAxis->setRange(0, 130);
    ui->speedPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag);
    ui->speedPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->speedPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->speedPlot->graph(0)->setPen(QPen(Qt::blue));
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
        speedPlotData.append(speed);
        xPlotData.append(speedPlotData.size());

        ui->speedPlot->graph(0)->setData(xPlotData, speedPlotData);
        ui->speedPlot->xAxis->setRange(speedPlotData.size(), 100, Qt::AlignRight);
        ui->speedPlot->replot();
        ui->speedPlot->update();

        if(client == true)
        {
            if(speed >= ui->ThresholdSpeed_horizontalSlider->value() || ui->ThresholdSpeedBypass_checkBox->isChecked())
                emit passToChat("Enable Distracted");
            else
                emit passToChat("Disable Distracted");
        }
    }
}

void DriverMonitorAndroid::requestUpdate()
{
    geoPositionInfoSource->requestUpdate();
}

void DriverMonitorAndroid::messageToCommand(const QString &message)
{
    qDebug() << message;
    time = QTime::currentTime();
    commandString += time.toString("hh:mm:ss ap") + ':' + message + '\n';
    ui->commandText->setText(commandString);
    if(message == "Alarm")
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


void DriverMonitorAndroid::on_ThresholdSpeed_horizontalSlider_valueChanged(int value)
{
    ui->ThresholdSpeedLabel->setText(QString::number(value));
}

void DriverMonitorAndroid::clientStatus(const bool &status)
{
    if(status == true)
        client = true;
    else
        client = false;
}

void DriverMonitorAndroid::on_ThresholdSpeedBypass_checkBox_toggled(bool checked)
{
    if(checked == true)
        emit passToChat("Enable Distracted");
    else
        emit passToChat("Disable Distracted");
}
