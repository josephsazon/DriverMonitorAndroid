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

    alertSound1 = new QMediaPlayer(this);
    alertSound1->setMedia(QUrl("qrc:/res/Alert.mp3"));
    //ui->alarmName->setText("alarm.mp3");

    alertSound2 = new QMediaPlayer(this);
    alertSound2->setMedia(QUrl("qrc:/res/pleaseFocusOnTheRoad.mp3"));

    ui->speedPlot->addGraph();
    ui->speedPlot->yAxis->setRange(0, 130);
    ui->speedPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag);
    ui->speedPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->speedPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->speedPlot->graph(0)->setPen(QPen(Qt::blue));

    QStringList headerName;
    headerName << "Time" << "Status" << "Elapsed Time(ms)";
    ui->tableDisplay->setColumnCount(3);
    ui->tableDisplay->setHorizontalHeaderLabels(headerName);
    ui->tableDisplay->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    commandString += time.toString("hh:mm:ss ap") + ": " + message + '\n';
    ui->commandText->setText(commandString);
    if(message == "AlarmA")
    {
        //Sound Alarm
        if(alertSound1->state() == QMediaPlayer::PlayingState || alertSound2->state() == QMediaPlayer::PlayingState)
            return;
        else
            alertSound1->play();
    }
    else if(message == "AlarmB")
    {
        if(alertSound2->state() == QMediaPlayer::PlayingState || alertSound1->state() == QMediaPlayer::PlayingState )
            return;
        else
            alertSound2->play();
    }
    else if(message.left(5) == "entry")
        readEntry(message);
}

void DriverMonitorAndroid::on_OpenCamera_pushButton_clicked()
{
    emit passToChat("Open Camera");
}

void DriverMonitorAndroid::on_CloseCamera_pushButton_clicked()
{
    emit passToChat("Close Camera");
}

//void DriverMonitorAndroid::on_changeAlarmButton_clicked()
//{
//    QFileDialog alarmFilePathDialog(this);
//    alarmFilePathDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).value(0, QDir::homePath()));
//    QString alarmFilePathUrl = "file://";
//    QString alarmFilePath = alarmFilePathDialog.getOpenFileName(this);

//    alarmFilePathUrl += alarmFilePath;
//    alertSound1->setMedia(QUrl(alarmFilePathUrl));

//    QFileInfo file(alarmFilePath);
//    //ui->alarmName->setText(file.fileName());
//}

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

void DriverMonitorAndroid::readEntry(const QString &message)
{
    QString time;
    QString instance;
    QString duration;
    time =  message.mid(5,11);
    if(message.mid(16, 14) == "Head turn left")
    {
        instance = message.mid(16, 14);
        duration = message.right(message.length() - 30);
    }
    else if(message.mid(16, 15) == "Head turn right")
    {
        instance = message.mid(16, 15);
        duration = message.right(message.length() - 31);
    }
    else if(message.mid(16, 5) == "Blink")
    {
        instance = message.mid(16, 5);
        duration = message.right(message.length() - 21);
    }
    else if(message.mid(16, 4) == "Yawn")
    {
        instance = message.mid(16, 4);
        duration = message.right(message.length() - 20);
    }
    else if(message.mid(16, 6) == "Drowsy")
    {
        instance = message.mid(16, 6);
        duration = message.right(message.length() - 22);
    }
    else if(message.mid(16, 10) == "Distracted")
    {
        instance = message.mid(16, 10);
        duration = message.right(message.length() - 26);
    }
    else if(message.mid(16, 6) == "Asleep")
    {
        instance = message.mid(16, 6);
        duration = message.right(message.length() - 22);
    }
    displayEntry(time, instance, duration);
}

void DriverMonitorAndroid::displayEntry(const QString &time, const QString &instance, const QString &duration)
{
    ui->tableDisplay->insertRow(ui->tableDisplay->rowCount());
    ui->tableDisplay->setItem(ui->tableDisplay->rowCount() - 1, 0, new QTableWidgetItem(time));
    ui->tableDisplay->setItem(ui->tableDisplay->rowCount() - 1, 1, new QTableWidgetItem(instance));
    ui->tableDisplay->setItem(ui->tableDisplay->rowCount() - 1, 2, new QTableWidgetItem(duration));

    ui->tableDisplay->item(ui->tableDisplay->rowCount() - 1, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableDisplay->item(ui->tableDisplay->rowCount() - 1, 1)->setTextAlignment(Qt::AlignCenter);
    ui->tableDisplay->item(ui->tableDisplay->rowCount() - 1, 2)->setTextAlignment(Qt::AlignCenter);

    ui->tableDisplay->scrollToBottom();
}


