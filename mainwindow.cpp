#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>

#include <QMessageBox>
#include <QPalette>

#include <stdint.h>
#include <unistd.h>

#include <QtWidgets>
#include <QMessageBox>
#include <QDebug>


Q_DECLARE_METATYPE(QCameraInfo)

//int i=1;

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}


void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    m_camera.reset(new QCamera(cameraInfo));

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));

    m_camera->setViewfinder(ui->viewfinder);

    m_camera->setCaptureMode(QCamera::CaptureStillImage);

  // connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);
  // connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved);
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

//aggiungere errore per frame rate=0


void MainWindow::cicloImmagini()
{

    /*Attenzione al punto in cui viene definito l'iteratore i
      l'hai messo come variabile globale, queusto fa si che si possano
      creare dei refusi. Per i cicli, usa delle variabili locali che
      hanno uno scope solo all'interno del ciclo*/

    ui->startAcquisitionButton->setText("Acquisition Started");
    QString NumberFrame = ui->NumberFrameText->toPlainText();
    QString filePath = ui->plainTextEdit->toPlainText();
    int i = 0;

    while(i < NumberFrame.toInt() ){
        //displayCapturedImage();
        if(QCamera::UnavailableStatus != 0){
        m_camera->searchAndLock();
        QString s = (filePath+QString::number(i));

        //QMessageBox::information(0,"prova",s);

        m_imageCapture->capture(filePath+QString::number(i));

        m_camera->unlock();
        std::cout<<i<<"\n";
        i++;
        }else{
            continue;
        }
        //m_camera.reset(new QCamera(QCameraInfo::defaultCamera()));
        //m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));
        //m_camera->setViewfinder(ui->viewfinder);

    }

    /*while ( i < NumberFrame.toInt()){

        ui-> NumericText->setText(QString::number(i));

        processCapturedImage(i);
        takeImage();
        imageSaved(i, filePath);

        i=i+1;

    }*/




    m_isCapturingImage = false;
    ui->startAcquisitionButton->setText("Acquisition");
    i=0;
    ui-> NumericText->setText("0");
    if (m_applicationExiting)
       close(); //?? Questo non lo capisco...

}

void MainWindow::processCapturedImage(int requestId)
{
    Q_UNUSED(requestId);

    //Qui succede qualcosa di strano, molto strano..... non capisco bene l'azione del timer
    //
    //If FrameRate=1, the display captured image for 1/1000 seconds.
    displayCapturedImage();

    QString FrameRate=ui->FrameRateText->toPlainText();

    QTimer::singleShot(FrameRate.toInt(), this, &MainWindow::displayViewfinder);
}


void MainWindow::takeImage()
{
    QString filePath = ui->plainTextEdit->toPlainText();

    m_isCapturingImage = true;
    m_imageCapture->capture(filePath); // alla fine è questo il vero capture, no?


}

void MainWindow::imageSaved(int id,  const QString &filePath)
{
    Q_UNUSED(id);

    ui->statusBar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(filePath)));
    m_isCapturingImage = false;
    if (m_applicationExiting)
       close(); // questo non mi è chiaro per nulla .....


    //QString NumberFrame = ui->NumberFrameText->toPlainText();



//    if (i < NumberFrame.toInt()){

//        processCapturedImage(i);

//        takeImage();
//        i=i+1;
//        ui-> NumericText->setText(QString::number(i));
//    }
//    else {
//        m_isCapturingImage = false;
//        ui->startAcquisitionButton->setText("Acquisition");
//        i=1;
//        ui-> NumericText->setText("0");
//        if (m_applicationExiting)
//            close();
//    }

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


