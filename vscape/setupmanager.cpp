#include <QDebug>
#include "setupmanager.h"

// SETUPMGR.SetupMgrCtrl.1          - {977C7146-FD15-11D0-AA2E-0020AF63DBFE}

/**
 * @brief SetupManager::SetupManager
 * @param parent
 */
SetupManager::SetupManager(int inspectionId, QObject *parent) : QObject(parent)
{
    m_systemStep = NULL;
    m_setupManager = new QAxWidget("{977C7146-FD15-11D0-AA2E-0020AF63DBFE}");
    m_setupManager->hide();
    m_setupManager->setDisabled(true);
    m_inspectionId = inspectionId;

    // setup Setup Manager exception handler
    connect(m_setupManager,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onSetupManagerException(int, const QString &, const QString &, const QString &))
            );

    connect(m_setupManager, SIGNAL(AcquireStarted()), this, SLOT(onAcquireStarted()));
    connect(m_setupManager, SIGNAL(AcquireDone()), this, SLOT(onAcquireDone()));

    m_setupManager->setProperty("ShowItemList", false);
    m_setupManager->setProperty("AutoRetrain", true);
    m_setupManager->setProperty("AutoRegenerate", true);
    m_acquireTimer = new QTimer(this);
    m_acquireTimer->setInterval(200);

    connect(m_acquireTimer, SIGNAL(timeout()), this, SLOT(onAcquireCheck()));
}

/**
 * @brief SetupManager::~SetupManager
 */
SetupManager::~SetupManager() {
    disconnectJob();
    delete this->m_setupManager;
}

/**
 * @brief SetupManager::connectJob
 * @param systemStep
 */
void SetupManager::connectJob(QAxObject *systemStep) {
    disconnectJob();

    m_systemStep = systemStep;

    qDebug() << "************* Connect Job";

    insp[0] = systemStep->querySubObject("Find(\"Step.Inspection\", 0)");
    insp[1] = this->insp[0]->querySubObject("Next()");
    insp[2] = this->insp[1]->querySubObject("Next()");
    insp[3] = this->insp[2]->querySubObject("Next()");
    insp[4] = this->insp[3]->querySubObject("Next()");
    if (insp[m_inspectionId] != NULL && ! insp[m_inspectionId]->isNull()) {
        unsigned int handle = insp[m_inspectionId]->property("Handle").toUInt();

        qDebug() << "************* handle " << handle << endl;

        if ( ! m_setupManager->dynamicCall("Edit(long hStep, short userMode)", handle, 0).toBool() ) {
            qCritical() << "Cannot set Setup Manager to edit Job";
        }
    }
}

/**
 * @brief SetupManager::disconnectJob
 */
void SetupManager::disconnectJob() {

    if (m_setupManager == NULL) {
        return;
    }

    m_setupManager->dynamicCall("Edit(0,0)");
}

/**
 * @brief SetupManager::onSetupManagerException
 * @param code
 * @param source
 * @param desc
 * @param help
 */
void SetupManager::onSetupManagerException(int code, const QString &source, const QString &desc, const QString &help) {
    qCritical() << "Error on Setup Manager.\nCode:" << code << "\nSource:" << source << "\nDescription:" << desc << "\nHelp:" << help;
}

/**
 * @brief SetupManager::acquireImage
 */
void SetupManager::acquireImage() {
    // get acquire status before calling Acquire
    if (m_setupManager->property("AcquireStatus").toBool() == false)
        return;

    m_setupManager->dynamicCall("Acquire(VARIANT_BOOL bSync, long syncTimeout)", false, 10000);
}

/*!
 * \brief SetupManager::cancelAcquireImage
 */
void SetupManager::cancelAcquireImage() {
    qDebug() << "Canceling Acquire image";
    m_setupManager->dynamicCall("AcquireStop()");
}

/*!
 * \brief SetupManager::acquireStatus
 * \return
 */
bool SetupManager::acquireStatus() {
    return m_setupManager->property("AcquireStatus").toBool();
}

/**
 * @brief SetupManager::onAcquireStarted
 */
void SetupManager::onAcquireStarted() {
    m_acquireTimer->start();
}

/**
 * @brief SetupManager::onAcquireDone
 */
void SetupManager::onAcquireDone() {
    m_acquireTimer->stop();
    getImage();
}

/**
 * @brief SetupManager::onAcquireTimeout
 */
void SetupManager::onAcquireCheck() {
    bool status = m_setupManager->property("AcquireStatus").toBool();
    if (status) {
        m_acquireTimer->stop();
        // the AcquireDone event does not fire unless we read the status until successful completion reached (true)
        // so the "onAcquireDone()" is automatically now
    }
}

/*!
 * \brief SetupManager::acquireRootImage
 * \param showGraphics
 */
void SetupManager::acquireRootImage(bool showGraphics) {
   acquireStepImage("root", false, showGraphics);
}

/*!
 * \brief SetupManager::acquireInputImage
 * \param symName
 */
void SetupManager::acquireStepImage(QString symName, bool useOutput, bool showGraphics) {

    QString cmd;

    if (symName.compare("root") == 0) {
        getImage(showGraphics);
        return;
    }
    else {
        cmd = "Find(\"" + symName + "\", 2)";
    }

    QAxObject *step = m_systemStep->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {
        acquireStepImage(step, useOutput, showGraphics);
        delete step;
    }
}


/*!
 * \brief SetupManager::acquireInputImage
 * \param symName
 */
void SetupManager::acquireStepImage(QAxObject *step, bool useOutput, bool showGraphics) {

    if (step == NULL || step->isNull()) {
        qCritical() << "Cannot get the step object to retrieve the image";
        return;
    }

    QAxObject *inputBuffer = step->querySubObject(useOutput ? "Datum(\"BufOut\")" : "Datum(\"BufIn\")");

    if (inputBuffer != NULL && ! inputBuffer->isNull()) {

        QAxObject *bufferDm;
        if (inputBuffer->property("IsReference").toBool()) {
            bufferDm = inputBuffer->querySubObject("Reference(1)");
        }
        else {
            bufferDm = inputBuffer;
            inputBuffer = NULL;
        }

        if (bufferDm != NULL && !bufferDm->isNull()) {

            int handle = bufferDm->property("Handle").toInt();
            int width = bufferDm->property("BufferWidth").toInt();
            int height = bufferDm->property("BufferHeight").toInt();

            if (handle && width > 0 && height > 0) {

                QPixmap image = Job::fromBufferDm(bufferDm, showGraphics);
                emit trainImageAcquired(&image);
            }
            else {
                qDebug() << "Invalid buffer input from " << step->property("Type") << handle << width << height;
            }

            delete bufferDm;
        }
        else {
            qDebug() << "Could not get buffer input from " << step->property("Type");
        }

        if (inputBuffer != NULL)
            delete inputBuffer;
    }
}


/*!
 * \brief SetupManager::getImage
 */
void SetupManager::getImage(bool showGraphics) {

//    qDebug() << "Setup Manager - getting image with graphics: " << showGraphics;

    QAxObject *snapshot = this->insp[this->m_inspectionId]->querySubObject("Find(\"Step.Snapshot\", 0)");
    if (snapshot != NULL && ! snapshot->isNull()) {

        QAxObject *bufferDm = snapshot->querySubObject("Datum(\"BufOut\")");
        if (bufferDm != NULL && !bufferDm->isNull()) {

            int handle = bufferDm->property("Handle").toInt();
            int width = bufferDm->property("BufferWidth").toInt();
            int height = bufferDm->property("BufferHeight").toInt();

            if (handle && width > 0 && height > 0) {

//                qDebug() << "Getting snapshot image from setup manager with graphics: " << showGraphics;
                QPixmap image = Job::fromBufferDm(bufferDm, showGraphics);
                emit trainImageAcquired(&image);
            }

            delete bufferDm;
        }

        delete snapshot;
    }
    else {
        qCritical() << "Cannot get the Snapshot object to retrieve the image";
    }
}

/*!
 * \brief SetupManager::getImageHSI
 * \param x
 * \param y
 * \return
 */
int* SetupManager::getImageHSI(int x, int y) {

    int hue = 0, saturation = 0, intensity = 0;

    QAxObject *snapshot = m_systemStep->querySubObject("Find(\"Step.Snapshot\", 0)");
    if (snapshot != NULL && ! snapshot->isNull()) {

        QAxObject *bufferDm = snapshot->querySubObject("Datum(\"BufOut\")");
        if (bufferDm != NULL && !bufferDm->isNull()) {

            int handle = bufferDm->property("Handle").toInt();
            int width = bufferDm->property("BufferWidth").toInt();
            int height = bufferDm->property("BufferHeight").toInt();

            if (handle && width > 0 && height > 0) {
                QImage image = Job::fromBufferDm(bufferDm).toImage();

                QColor color = image.pixelColor(x, y);
                hue = color.hslHueF() * 255;
                saturation = color.hslSaturationF() * 255;
                intensity = color.lightnessF() * 255;

                // color.getHsl(&hue, &saturation, &intensity);
            }
            else {
                qCritical() << "Error trying to get input image during color picking";
            }

            delete bufferDm;
        }

        delete snapshot;
    }

    int *result = new int[3];
    result[0] = hue;
    result[1] = saturation;
    result[2] = intensity;

    return result;
}

/*!
 * \brief SetupManager::selectStep
 * \param step
 */
void SetupManager::selectStep(QAxObject *step) {
    unsigned int handle = step->property("Handle").toUInt();
    m_setupManager->dynamicCall("SelectStep(long hStep)", handle);
}

/*!
 * \brief SetupManager::regenerate
 */
void SetupManager::regenerate() {
    m_setupManager->dynamicCall("Regenerate(0)");
}

/*!
 * \brief SetupManager::refresh
 */
void SetupManager::refresh() {
    // m_setupManager->dynamicCall("TryoutCurrentItem()");
    m_setupManager->dynamicCall("RefreshView()");
}

/*!
 * \brief SetupManager::tryoutStep
 * \param step
 */
void SetupManager::tryoutStep(QAxObject *step) {

    selectStep(step);
    if ( ! m_setupManager->dynamicCall("TryoutCurrentItem()").toBool() ) {
        qCritical() << "Tryout step failed - " + step->property("Name").toString();
    }
    else {
        qDebug() << "Tryout step succeded - " + step->property("Name").toString();
    }

}

/*!
 * \brief SetupManager::tryOut
 * \param step
 */
void SetupManager::tryOut(QAxObject *step) {

    // tryoutStep(step);

    int childrenCount = step->property("Count").toInt();
    for (int i = 0; i < childrenCount; i++) {

        QAxObject *child = step->querySubObject("Item([in] VARIANT varIndex)", i + 1);

        if (child != NULL && ! child->isNull()) {

            // tryOut(child);
            tryoutStep(child);
            delete child;
        }
    }

}
