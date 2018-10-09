#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QImageEncoderSettings>
#include <QMediaRecorder>
#include <QScopedPointer>
#include <QImage>
#include <QImage>
#include <QCameraImageProcessing>
#include <QCameraImageProcessingControl>

#include <QMainWindow>

class QComboBox;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

//    ColorGradient {
//        id: surfaceGradient
//        ColorGradientStop { position: 0.0; color: "black" }
//        ColorGradientStop { position: 0.2; color: "red" }
//        ColorGradientStop { position: 0.5; color: "blue" }
//        ColorGradientStop { position: 0.8; color: "yellow" }
//        ColorGradientStop { position: 1.0; color: "white" }
//    }

//    QMenuBar {
//        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
//                                          stop:0 lightgray, stop:1 darkgray);
//    }


public:
    MainWindow();

private slots:
    void setCamera(const QCameraInfo &cameraInfo);

    void startCamera();
    void stopCamera();

    void displayViewfinder();
    void displayCapturedImage();

    void readyForCapture(bool ready);

    void cicloImmagini();

    void updateCameraState(QCamera::State);

    void setExposureCompensation(int index);

    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);
    void displayCameraError();

    void setImage();

    void select();

    void change_label();


protected:


private:



    Ui::MainWindow *ui;

    QScopedPointer<QCamera> m_camera; //Puntatore alla classe Qcamera che :Construct a QCamera with a parent.
    QScopedPointer<QCameraImageCapture> m_imageCapture;
    QImageEncoderSettings m_imageSettings;
    QString m_videoContainerFormat;
    bool m_isCapturingImage = false;
    bool m_applicationExiting = false;





};

#endif // MAINWINDOW_H
