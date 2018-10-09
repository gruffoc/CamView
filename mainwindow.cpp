#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waveformgraph.h"

#include <QImageEncoderSettings>
#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPalette>
#include <QComboBox>
#include <QDebug>
#include <QCameraImageCapture>
#include <QMediaService>
#include <stdint.h>
#include <QtWidgets>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <QStringList>
#include <QFileDialog>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QChartView>
#include <QThread>

Q_DECLARE_METATYPE(QCameraInfo)

using namespace QtCharts;

QThread *th2;
WaveformGraph *work_process;

extern int measure_number;

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    measure_number = 0;

    //setting up the measure thread

    th2 = new QThread();
    work_process = new WaveformGraph();

    work_process->moveToThread(th2);

    connect(th2,SIGNAL(started()),work_process,SLOT(do_something()));
    th2->start();


    //Camera devices: SCELTA DEL DISPOSITIVO

    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            videoDeviceAction->setChecked(true);
        ui->menuDevices->addAction(videoDeviceAction);
    }
    setCamera(QCameraInfo::defaultCamera());


    connect(ui->startButton,SIGNAL(clicked()), this, SLOT(startCamera()));
    connect(ui->stopButton,SIGNAL(clicked()), this, SLOT(stopCamera()));
    connect(ui->startAcquisitionButton,SIGNAL(clicked()), this, SLOT(cicloImmagini()));

    connect(this,SLOT(startCamera()),this,SLOT(change_label()));
    //connect(th2,SIGNAL(change()),this,SLOT(change_label()));


    connect(ui->pushButton,SIGNAL(clicked()), this, SLOT(select()));
}

void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    m_camera.reset(new QCamera(cameraInfo));

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));

    QImageEncoderSettings imageSettings;
    //imageSettings.setCodec("image/png");

    QString width=ui->widthText->toPlainText();
    QString height=ui->heightText->toPlainText();
    imageSettings.setResolution(width.toInt(), height.toInt());
    m_imageCapture->setEncodingSettings(imageSettings);

    connect(m_camera.data(), &QCamera::stateChanged, this, &MainWindow::updateCameraState);

    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

    m_camera->setViewfinder(ui->viewfinder);

    m_camera->setCaptureMode(QCamera::CaptureStillImage);

    connect(ui->exposureCompensation, &QAbstractSlider::valueChanged, this, &MainWindow::setExposureCompensation);

    updateCameraState(m_camera->state());

    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, this, &MainWindow::readyForCapture);
    connect(m_imageCapture.data(), QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            this, &MainWindow::displayCaptureError);


    ui->startAcquisitionButton->setEnabled((m_camera->isCaptureModeSupported(QCamera::CaptureStillImage)));


    ui->stopButton->setEnabled(false);
    ui->startAcquisitionButton->setEnabled(false);

}

void MainWindow::startCamera()
{
    m_camera->start();
    ui->startButton->setText("Camera Started");
}

void MainWindow::stopCamera()
{
    m_camera->stop();
    ui->startButton->setText("Start");
}

void MainWindow::cicloImmagini()
{
    QString NumberFrame = ui->NumberFrameText->toPlainText();
    QString filePath = ui->fileTextEdit->toPlainText();
    QString FrameRate=ui->FrameRateText->toPlainText();
    QString FileName= ui->FileNameText->toPlainText();

    if (NumberFrame == ""){
        QMessageBox::information(0,"Open File", "ERRORE!\n NumberFrame non settato - indicare un NumberFrame ");
        return;
    }

    if (FrameRate  == ""){
        QMessageBox::information(0,"Open File", "ERRORE!\n FrameRate non settato - indicare un FrameRate ");
        return;
    }

    if (filePath  == ""){
        QMessageBox::information(0,"Open File", "ERRORE!\n Impossibile trovare la path indicata ");
        return;
    }


    int i=1;
    while(i <= NumberFrame.toInt() ){
        displayCapturedImage();
        ui->startAcquisitionButton->setText("Acquisition Started");
        ui->NumericText->setText(QString::number(i));
        m_isCapturingImage=true;
        readyForCapture(i);


        std::cout<<i<<"\n";

            if(QCameraImageCapture::NoError==0){

            m_camera->searchAndLock();

            m_imageCapture->capture(filePath+ FileName + QString::number(i));

            QTimer::singleShot(FrameRate.toInt(), this, &MainWindow::displayViewfinder);

            //ui->statusBar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(filePath)));

            ui->resultingText->clear();
            ui->resultingText->appendPlainText(filePath+FileName+QString::number(i));

            m_isCapturingImage=false;

            m_camera->unlock();


            i++;
            }else{
                continue;
            }

    ui->startAcquisitionButton->setText("Start Acquisition");

    std::cout<<NumberFrame.toInt()<<"\n";
    if (m_applicationExiting)
      close();

    }
}

void MainWindow::setExposureCompensation(int index)
{
    //QCameraExposure *exposure = m_camera->exposure();
    //exposure->setExposureMode(QCameraExposure::ExposureManual);
    m_camera->exposure()->setExposureCompensation(index*0.5);

}

void MainWindow::setImage()
{
    QImageEncoderSettings imageSettings;
    imageSettings.setCodec("image/png");
    imageSettings.setResolution(1600, 1200);

    m_imageCapture->setEncodingSettings(imageSettings);
}

void MainWindow::updateCameraState(QCamera::State state)
{
    switch (state) {
    case QCamera::ActiveState:
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        ui->startAcquisitionButton->setEnabled(true);

        break;
    case QCamera::UnloadedState:

    case QCamera::LoadedState:
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->startAcquisitionButton->setEnabled(false);
    }
}

void MainWindow::displayViewfinder()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::displayCapturedImage()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::readyForCapture(bool ready)
{
    ui->startAcquisitionButton->setEnabled(ready);
}


void MainWindow::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    m_isCapturingImage = false;
}


void MainWindow::displayCameraError()
{
    QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}


void MainWindow::select()
{
    QString directory =
        QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath()));
    ui->fileTextEdit->clear();
    ui->fileTextEdit->appendPlainText(directory);
}


void MainWindow::change_label()
{
    ui->label_9->setText("ciao");
}

