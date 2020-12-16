#include <QFileInfo>
#include <QWidget>
#include <QQuickItem>
#include <QQmlContext>
#include "uibackend.h"

#include <systemconfiguration.h>
#include <batchconfiguration.h>

// TODO:
/*
#include <counterblock.h>
#include <systemstatus.h>
#include <labeledvalue.h>
#include <labeledvaluelist.h>
*/

/*!
 * \brief UIBackend::UIBackend
 * \param st an initialized application settings object
 */
UIBackend::UIBackend(UIController *controller, QSettings *st, TrainImageProvider **trainImageProvider, BufferImageProvider **bufferImageProvider, QWindow *parent) : QQuickView(parent)
{
    m_controller = controller;
    m_settings = st;

    for (int i = 0 ; i < 4 ; i ++)
    {
        m_trainImageProvider[i] = trainImageProvider[i];
    }
    for (int i = 0 ; i < 5 ; i ++)
    {
        m_bufferImageProvider[i] = bufferImageProvider[i];
    }

    initializeTypes();
    setVisualProperties();

    // inject controller to the QML context
    rootContext()->setContextProperty("uiController", m_controller);

    // wait for the UI to get ready
    connect(this, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(onStatusChanged(QQuickView::Status)));
}

/*!
 * \brief UIBackend::~UIBackend
 */
UIBackend::~UIBackend() {
    // TODO:
    // m_imgProvider->deleteLater();
}

/*!
 * \brief UIBackend::onStatusChanged
 * \param status
 */
void UIBackend::onStatusChanged(QQuickView::Status status) {

    if (status != QQuickView::Ready) {
            return;
    }
    m_controller->initialize();

    connect(m_controller, SIGNAL(clearFailedImages()), this, SLOT(onClearFailedImages()) );
}

/*!
 * \brief UIBackend::onClearFailedImages
 */
void UIBackend::onClearFailedImages() {
    for (int i = 0 ; i < 5 ; i ++)
    {
        m_bufferImageProvider[i]->clearFailedImages();
    }
}

/**
 * @brief UIBackend::setWinProperties
 */
void UIBackend::setVisualProperties() {
    this->setColor(QColor("#000000"));
    this->setResizeMode(QQuickView::SizeRootObjectToView);
}

/**
 * @brief UIBackend::show
 */
void UIBackend::show() {
    setSource(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));
}

/*!
 * \brief UIBackend::getTrainImageProvider
 * \return
 */
TrainImageProvider *UIBackend::getTrainImageProvider(int id) {
    return m_trainImageProvider[id];
}

/*!
 * \brief UIBackend::getBufferImageProvider
 * \return
 */
BufferImageProvider *UIBackend::getBufferImageProvider(int id) {
    return m_bufferImageProvider[id];
}

/**
 * @brief UIBackend::initializeTypes
 */
void UIBackend::initializeTypes() {

    // m_bufferImageProvider = new BufferImageProvider();
    // m_trainImageProvider = new TrainImageProvider(this);

    for (int i = 0 ;  i < 4 ; i ++)
    {
        QString str("train");
        str.append('1' + i);
        this->engine()->addImageProvider(str.toLatin1().data(), m_trainImageProvider[i]);
    }
    for (int i = 0 ; i < 4 ; i ++)
    {
        QString str("buffer");
        str.append('1' + i);
        this->engine()->addImageProvider(str.toLatin1().data(), m_bufferImageProvider[i]);
    }
    this->engine()->addImageProvider("buffer", m_bufferImageProvider[4]);

    qmlRegisterType<SystemStatus>("com.darebit.blisterocv",         1, 0, "SystemStatus");
    qmlRegisterType<SystemConfiguration>("com.darebit.blisterocv",  1, 0, "SystemConfiguration");
    qmlRegisterType<BatchConfiguration>("com.darebit.blisterocv",   1, 0, "BatchConfiguration");

    // TODO:
    /*
    qmlRegisterType<LabeledValue>("com.darebit.blisterocv",         1, 0, "LabeledValue");
    qmlRegisterType<LabeledValueList>("com.darebit.blisterocv",     1, 0, "LabeledValueList");
    */
}
