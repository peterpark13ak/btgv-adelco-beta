#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QAxObject>
#include <QTimer>
#include <objbase.h>

class Camera : public QObject
{
    Q_OBJECT

public:
    explicit Camera(QString name, QObject *parent = 0);
    ~Camera();

    void start();
    bool isReady();
    void stopAllInspections();
    void download(QAxObject *systemStep);
    QAxObject *getDevice();

signals:
    void cameraReady();
    void downloadFinished(bool success);

public slots:

private slots:
    void onDeviceFocus(IDispatch*);
    void onXferProgress(int nState, int nStatus, const QString &msg);
    void onDeviceException(int, const QString &, const QString &, const QString &);
    void onCoordinatorException(int, const QString &, const QString &, const QString &);
    void onDownloadCheck();

private:
    QString m_cameraName;
    QAxObject *m_vsCoordinator;
    QAxObject *m_device;
    QTimer *m_downloadTimer;

    bool m_deviceReady;
};

#endif // CAMERA_H
