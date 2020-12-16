#include <QDebug>
#include "camera.h"

#define XFER_DONE           0
#define XFER_IN_PROGRESS    1

// Vsobj.VsCoordinator.1 - {05E7251A-44E3-4165-A892-F65F37C1C199}

/**
 * @brief Camera::Camera
 * @param name
 * @param parent
 */
Camera::Camera(QString name, QObject *parent) : QObject(parent)
{
    m_cameraName = name;
    m_deviceReady = false;
    m_device = NULL;

    m_downloadTimer = new QTimer(this);
    m_downloadTimer->setInterval(200);
    connect(m_downloadTimer, SIGNAL(timeout()), this, SLOT(onDownloadCheck()));
}

/**
 * @brief Camera::~Camera
 */
Camera::~Camera() {
    if (m_device != NULL)
        delete m_device;

    if (m_vsCoordinator != NULL)
        delete m_vsCoordinator;
}

/*!
 * \brief Camera::getDevice
 * \return
 */
QAxObject *Camera::getDevice() {
    return m_device;
}

/**
 * @brief Camera::start
 */
void Camera::start() {
    m_vsCoordinator = new QAxObject("{05E7251A-44E3-4165-A892-F65F37C1C199}");

    // make the signal connections
    connect(m_vsCoordinator,
            SIGNAL(OnDeviceFocus(IDispatch*)),
            this,
            SLOT(onDeviceFocus(IDispatch*)));

    connect(m_vsCoordinator,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onCoordinatorException(int, const QString &, const QString &, const QString &))
            );

    // enable device focus event on discovery
    m_vsCoordinator->dynamicCall(
                "DeviceFocusSetOnDiscovery([in] BSTR bstrName, [in, optional, defaultvalue(-1)] long GroupID)",
                m_cameraName.toLatin1().data()
                );
}

/**
 * @brief Camera::onDeviceFocus
 * @param iDevice
 */
void Camera::onDeviceFocus(IDispatch* iDevice) {

    if (m_deviceReady)
        return;

    m_device = new QAxObject(iDevice);
    QString discoveredName = m_device->property("Name").toString();

    if (discoveredName.compare(m_cameraName, Qt::CaseInsensitive) != 0) {
        if (!m_device->isNull()) {
            m_device->deleteLater();
        }
        return;
    }

    m_deviceReady = true;

    qDebug() << "*******" << "Camera::onDeviceFocus(camera:" << m_device->property("Name").toString() << ")";       //created for tracking steps

    qDebug() << "$ Camera Ready:" << m_device->property("Name").toString();

    connect(m_device,
            SIGNAL(OnXferProgress(int, int, const QString &)),
            this,
            SLOT(onXferProgress(int,int, const QString&))
            );

    connect(m_device,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onDeviceException(int, const QString &, const QString &, const QString &))
            );

    emit cameraReady();
}

/**
 * @brief Camera::stopAllInspections
 */
void Camera::stopAllInspections() {
    m_device->dynamicCall("StopInspection(-1)");
}

/**
 * @brief Camera::onXferProgress
 * @param nState
 * @param nStatus
 * @param msg
 */
void Camera::onXferProgress(int nState, int nStatus, const QString &msg) {
    qDebug() << "Xfer progress" << nState << nStatus << msg;
}

/**
 * @brief Camera::onDeviceException
 * @param code
 * @param source
 * @param desc
 * @param help
 */
void Camera::onDeviceException(int code, const QString &source, const QString &desc, const QString &help) {
    qCritical() << "Error on Device.\nCode:" << code << "\nSource:" << source << "\nDescription:" << desc << "\nHelp:" << help;
}

/**
 * @brief Camera::onCoordinatorException
 * @param code
 * @param source
 * @param desc
 * @param help
 */
void Camera::onCoordinatorException(int code, const QString &source, const QString &desc, const QString &help) {
    qCritical() << "Error on Coordinator.\nCode:" << code << "\nSource:" << source << "\nDescription:" << desc << "\nHelp:" << help;
}

/**
 * @brief Camera::isDiscovered
 * @return
 */
bool Camera::isReady() {
    return m_deviceReady;
}

/**
 * @brief Camera::download
 * @param systemStep
 */
void Camera::download(QAxObject *systemStep) {
   unsigned int status = m_device->dynamicCall(
               "Download([in] IVisionSystemStep* objVS, [in, optional, defaultvalue(1)] long bAsync, [out, retval] long* pStatus)",
               QVariant::fromValue(systemStep), 1)
           .toInt() ;

   if (status == XFER_IN_PROGRESS) {
       m_downloadTimer->start();
       return;
   }

   emit downloadFinished(status == XFER_DONE);
}

/**
 * @brief Camera::onDownloadCheck
 */
void Camera::onDownloadCheck() {
    unsigned int status = m_device->dynamicCall("CheckXferStatus(200)").toInt();

    if (status != XFER_IN_PROGRESS) {
        m_downloadTimer->stop();
        emit downloadFinished(status == XFER_DONE);
    }
}
