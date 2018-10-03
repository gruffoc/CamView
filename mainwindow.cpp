#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>

#include <QMessageBox>
#include <QPalette>

#include <stdint.h>

#include <QtWidgets>

#include <unistd.h>
#include <iostream>

Q_DECLARE_METATYPE(QCameraInfo)



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

    connect(m_camera.data(), &QCamera::stateChanged, this, &MainWindow::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));

    m_camera->setViewfinder(ui->viewfinder);

    m_camera->setCaptureMode(QCamera::CaptureStillImage);


    //questo ti servirà
    //connect(ui->exposureCompensation, &QAbstractSlider::valueChanged, this, &Camera::setExposureCompensation);


    updateCameraState(m_camera->state());


    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, this, &MainWindow::readyForCapture);
    //connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);
    //connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved);
    connect(m_imageCapture.data(), QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            this, &MainWindow::displayCaptureError);


    ui->startAcquisitionButton->setEnabled((m_camera->isCaptureModeSupported(QCamera::CaptureStillImage)));


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


//mi esce 'errore e la metà
void MainWindow::cicloImmagini()
{
    ui->startAcquisitionButton->setText("Acquisition Started");
    QString NumberFrame = ui->NumberFrameText->toPlainText();
    QString filePath = ui->plainTextEdit->toPlainText();
    QString FrameRate=ui->FrameRateText->toPlainText();



    int i=1;

    while(i < NumberFrame.toInt() ){
        displayCapturedImage();
        ui->NumericText->setText(QString::number(i));
        m_isCapturingImage=true;
        readyForCapture(i);

        std::cout<<i<<"\n";

            if(QCamera::NoError){
            m_camera->searchAndLock();
            //QString s = (filePath+QString::number(i));

            //QMessageBox::information(0,"prova",s);

            m_imageCapture->capture(filePath+QString::number(i));

            QTimer::singleShot(FrameRate.toInt(), this, &MainWindow::displayViewfinder);

            ui->statusBar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(filePath)));
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

void MainWindow::processCapturedImage(int requestId)
{
    Q_UNUSED(requestId);


    //If FrameRate=1, the display captured image for 1/1000 seconds.
    displayCapturedImage();

    QString FrameRate=ui->FrameRateText->toPlainText();

    QTimer::singleShot(FrameRate.toInt(), this, &MainWindow::displayViewfinder);
}


//void MainWindow::takeImage()
//{
//    QString filePath = ui->plainTextEdit->toPlainText();

//    m_isCapturingImage = true;
//    m_imageCapture->capture(filePath);


//}

//void MainWindow::imageSaved(int id,  const QString &filePath)
//{
//    Q_UNUSED(id);

//    ui->statusBar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(filePath)));
//    m_isCapturingImage = false;
//    if (m_applicationExiting)
//       close();
//}


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
