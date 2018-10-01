#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QScopedPointer>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void setCamera(const QCameraInfo &cameraInfo);

    void startCamera();
    void stopCamera();

    void takeImage();
    void imageSaved(int id,  const QString &filePath);

    void processCapturedImage(int requestId);

    void displayViewfinder();
    void displayCapturedImage();

    void readyForCapture(bool ready);

    void cicloImmagini();


protected:

   // void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    QScopedPointer<QCamera> m_camera; //Puntatore alla classe Qcamera che :Construct a QCamera with a parent.
    QScopedPointer<QCameraImageCapture> m_imageCapture;
    QScopedPointer<QMediaRecorder> m_mediaRecorder;

    QImageEncoderSettings m_imageSettings;
    QAudioEncoderSettings m_audioSettings;
    QVideoEncoderSettings m_videoSettings;
    QString m_videoContainerFormat;
    bool m_isCapturingImage = false;
    bool m_applicationExiting = false;



};

#endif // MAINWINDOW_H
