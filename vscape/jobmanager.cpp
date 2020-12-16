#include <QDebug>
#include "jobmanager.h"

#include "datumproperties.h"

/**
 * @brief JobManager::JobManager
 * @param path
 * @param avp
 * @param parent
 */
JobManager::JobManager(QSettings *settings, QObject *parent) : QObject(parent)
{
    m_settings = settings;

    // get application settings
    // ------------------------------------------------------------------------------------
    m_settings->beginGroup("Vscape");
    m_avpPath = m_settings->value("avpPath", "c:/btgv-adelco/avp").toString();
    m_cameraName = m_settings->value("camera", "SoftSys1").toString();
    m_settings->endGroup();
    // ------------------------------------------------------------------------------------

    prepareDatums();

    m_avp = "";                         // AVP filename loaded (or to load on startup)
    m_editMode = false;
    m_runMode = false;
    m_loadInProgress = false;
    m_avpLoaded = false;
    m_stepProperties = NULL;

    m_job = new Job(this);

    m_camera = new Camera(m_cameraName, this);
    connect(m_camera, SIGNAL(cameraReady()), this, SLOT(onCameraReady()));
    connect(m_camera, SIGNAL(downloadFinished(bool)), this, SLOT(onAvpReady(bool)));

    for (int i = 0 ; i < 4 ; i ++)
    {
        m_setupManager[i] = NULL;
    }

    {
        m_reportConnection[0] = new ReportConnection(m_settings, 0, this);
        connect(m_reportConnection[0], SIGNAL(newImage(QPixmap*, bool)), this, SLOT(onNewImage1(QPixmap*, bool)));
        connect(m_reportConnection[0], SIGNAL(newResult(bool, unsigned char)), this, SLOT(onNewResult(bool, unsigned char)));
        connect(m_reportConnection[0], SIGNAL(newStatistics(QString)), this, SIGNAL(newStatistics(QString)));
    }
    {
        m_reportConnection[1] = new ReportConnection(m_settings, 1, this);
        connect(m_reportConnection[1], SIGNAL(newImage(QPixmap*, bool)), this, SLOT(onNewImage2(QPixmap*, bool)));
        connect(m_reportConnection[1], SIGNAL(newResult(bool, unsigned char)), this, SLOT(onNewResult(bool, unsigned char)));
        connect(m_reportConnection[1], SIGNAL(newStatistics(QString)), this, SIGNAL(newStatistics(QString)));
    }
    {
        m_reportConnection[2] = new ReportConnection(m_settings, 2, this);
        connect(m_reportConnection[2], SIGNAL(newImage(QPixmap*, bool)), this, SLOT(onNewImage3(QPixmap*, bool)));
        connect(m_reportConnection[2], SIGNAL(newResult(bool, unsigned char)), this, SLOT(onNewResult(bool, unsigned char)));
        connect(m_reportConnection[2], SIGNAL(newStatistics(QString)), this, SIGNAL(newStatistics(QString)));
    }
    {
        m_reportConnection[3] = new ReportConnection(m_settings, 3, this);
        connect(m_reportConnection[3], SIGNAL(newImage(QPixmap*, bool)), this, SLOT(onNewImage4(QPixmap*, bool)));
        connect(m_reportConnection[3], SIGNAL(newResult(bool, unsigned char)), this, SLOT(onNewResult(bool, unsigned char)));
        connect(m_reportConnection[3], SIGNAL(newStatistics(QString)), this, SIGNAL(newStatistics(QString)));
    }
    {
        m_reportConnection[4] = new ReportConnection(m_settings, 4, this);
    }

    connect(m_reportConnection[0], SIGNAL(newFailedImage(QPixmap*, int)), this, SLOT(onNewFailedImage(QPixmap*, int)));
    connect(m_reportConnection[1], SIGNAL(newFailedImage(QPixmap*, int)), this, SLOT(onNewFailedImage(QPixmap*, int)));
    connect(m_reportConnection[2], SIGNAL(newFailedImage(QPixmap*, int)), this, SLOT(onNewFailedImage(QPixmap*, int)));
    connect(m_reportConnection[3], SIGNAL(newFailedImage(QPixmap*, int)), this, SLOT(onNewFailedImage(QPixmap*, int)));
}

/**
 * @brief JobManager::~JobManager
 */
JobManager::~JobManager() {
    cleanUp();

    for (int i = 0 ; i < 4 ; i ++)
    {
        delete m_setupManager[i];
    }
    delete m_job;
}

/*!
 * \brief JobManager::getGlobalDatums
 * \return
 */
QList<DatumType> JobManager::getGlobalDatums(int inspectionId) {
    updateGlobalDatumValues(inspectionId);
    return m_globalDatums->values();
}

/*!
 * \brief JobManager::prepareGlobalDatums
 */
void JobManager::prepareDatums() {

    // read global datum definitions from settings
    m_settings->beginGroup("Vscape");
    QStringList globalDatumsStr = m_settings->value("globalDatums", "").toStringList();
    QStringList datumDefaultsStr = m_settings->value("datumDefaults", "").toStringList();
    m_settings->endGroup();

    int i;
    QStringList parts;

    // Global Datums
    // ======================================================================================
    m_globalDatums = new QMap<QString, DatumType>();

    for (i = 0; i < globalDatumsStr.length(); i++) {
        parts = globalDatumsStr[i].split("/");
        if (parts.length() < 4) {
            continue;
        }

        QString type = parts[3];

        DatumType datumType;
        datumType.name = parts[0] + ":" + parts[1];
        datumType.label = parts[2];

        qDebug() << "***************** Datum: " << datumType.name << ' ' << datumType.label;

        if (type.compare("double") == 0) {
            datumType.type = doubleType;
        }
        else if (type.compare("integer") == 0) {
            datumType.type = integerType;
        }

        datumType.scale = 1;
        datumType.minValue = 0;
        datumType.maxValue = INT_MAX;

        QStringList limits;

        switch (datumType.type) {
        case doubleType:
        case integerType:
            if (parts.length() < 5)
                continue;

            limits = parts[4].split("-");
            datumType.scale = QString(limits[0]).toInt();
            datumType.minValue = QString(limits[1]).toInt();
            datumType.maxValue = QString(limits[2]).toInt();
            break;
        default:
            break;
        }

        m_globalDatums->insert(datumType.name, datumType);
    }

    // Custom Datums default values
    // ======================================================================================
    for(i = 0; i < datumDefaultsStr.length(); i++) {
        parts = datumDefaultsStr[i].split("/");
        if (parts.length() < 3) {
            continue;
        }

        QString stepSym = parts[0];
        QString datumSym = parts[1];
        QString datumDefaultValue = parts[2];

        if (g_stepDatums.contains(stepSym)) {
            QList<StepDatum> *datums = &(g_stepDatums[stepSym]);
            for (int di = 0; di < datums->length(); di++) {
                if (QString::compare(datumSym, datums->at(di).symName) == 0) {
                    (*datums)[di].value = QString(datumDefaultValue).toInt();
                    break;
                }
            }
        }
    }
}

/*!
 * \brief JobManager::updateGlobalDatumValues
 */
void JobManager::updateGlobalDatumValues(int inspectionId) {

    QMap<QString, DatumType>::iterator i;

    for (i = m_globalDatums->begin(); i != m_globalDatums->end(); ++i) {

        QStringList parts = i.value().name.split(":");

        qDebug() << "********************** Update Datums:";

        if (parts.length() < 2) {
            qCritical() << "Invalid code in global datums: " + i.value().name;
            return;
        }

        switch (i.value().type) {
        case doubleType:
            i.value().doubleValue = m_job->getAvpNumericParameter(inspectionId, parts[0], parts[1]).toDouble();
            break;
        case integerType:
            i.value().intValue = m_job->getAvpNumericParameter(inspectionId, parts[0], parts[1]).toInt();
            break;
        default:
            break;
        }
    }
}

/*!
 * \brief JobManager::trainAll
 */
void JobManager::trainAll() {
    m_job->walkJob(m_stepProperties, trainSteps);
}

/*!
 * \brief JobManager::tryOutSteps
 */
void JobManager::tryOutSteps(QString stepPath) {

    QAxObject *system = m_job->getSystemStep();
    QAxObject *step = system->querySubObject(QString("Find(\"" + stepPath + "\", 2)").toLatin1().data());
    if (step != NULL && ! step->isNull()) {

        for (int i = 0 ; i < 4 ; i ++)
        {
            m_setupManager[i]->tryOut(step);
        }

        delete step;
    }
    delete system;
}

/*!
 * \brief JobManager::train
 * \param stepPath
 */
void JobManager::train(QString stepPath) {
    m_job->train(stepPath);
}

/*!
 * \brief JobManager::trainParent
 * \param stepPath
 */
void JobManager::trainParent(QString stepPath) {
    m_job->trainParent(stepPath);
}

/*!
 * \brief JobManager::tryout
 * \param stepPath
 */
void JobManager::tryout(QString stepPath) {

    QAxObject *system = m_job->getSystemStep();
    QAxObject *step = system->querySubObject(QString("Find(\"" + stepPath + "\", 2)").toLatin1().data());
    if (step != NULL && ! step->isNull()) {

        for (int i = 0 ; i < 4 ; i ++)
        {
            m_setupManager[i]->tryoutStep(step);
        }

        delete step;
    }
    delete system;
}

/*!
 * \brief JobManager::deleteStep
 * \param stepPath
 */
void JobManager::deleteStep(QString stepPath) {

    QAxObject *system = m_job->getSystemStep();
    QAxObject *step = system->querySubObject(QString("Find(\"" + stepPath + "\", 2)").toLatin1().data());
    if (step != NULL && ! step->isNull()) {

        for (int i = 0 ; i < 4 ; i ++)
        {
            m_setupManager[i]->disconnectJob();
            m_job->deleteStep(step);
            m_setupManager[i]->connectJob(m_job->getSystemStep());
        }

        delete step;
    }
    delete system;
}

/*!
 * \brief JobManager::getConfigurableSteps
 * \return
 */
QList<StepProps> *JobManager::getConfigurableSteps(bool withUpdate) {

    // check if step properties list is already populated
    if (m_stepProperties != NULL && ! withUpdate) {
        return m_stepProperties;
    }

    if (withUpdate && m_stepProperties != NULL) {
        m_stepProperties->clear();
        delete m_stepProperties;
        m_stepProperties = NULL;
    }

    if (m_stepProperties == NULL) {
        m_stepProperties = new QList<StepProps>();
    }

    // walk Job and execute command
    m_job->walkJob(m_stepProperties, scanForSteps);

    return m_stepProperties;
}

/*!
 * \brief JobManager::getStep
 * \param stepPath
 * \return
 */
StepProps JobManager::getStepProps(QString stepPath) {

    QString cmd;
    StepProps stepProps, parentProps;
    QAxObject *system;
    QAxObject *step;

    cmd = "Find(\"" + stepPath + "\", 2)";

    // check if the object is the root (single symbol name in the path)
    if (stepPath.indexOf(".") < 0) {

        system = m_job->getSystemStep();
        step = system->querySubObject(cmd.toLatin1().data());

        parentProps.level = 1;
        parentProps.path = step->property("NameSym").toString();
        stepProps = StepProperties::fromStepObject(step, parentProps);

        delete step;
        delete system;

        return stepProps;
    }

    stepProps = StepProps { "", "", 0, "", "", 0, false, false, false, false, false, false, false, false, false, false, false, false, false, {0, 0, 0, 0}, false, 0, 0, 0, {} };

    system = m_job->getSystemStep();
    step = system->querySubObject(cmd.toLatin1().data());

    if (step != NULL && ! step->isNull()) {

        QString parentPath = stepPath.left(stepPath.lastIndexOf('.'));
        parentProps = getStepProps(parentPath);

        stepProps = StepProperties::fromStepObject(step, parentProps);
        delete step;
    }
    else {
        qCritical() << "Error trying to get step from path: " << stepPath;
    }

    delete system;

    return stepProps;
}

/*!
 * \brief JobManager::setAvp
 * \param avp
 */
void JobManager::setAvp(QString avp) {
    m_avp = avp;
}

/**
 * @brief JobManager::start
 */
void JobManager::start() {
    m_camera->start();
}

/*!
 * \brief JobManager::saveAVP
 * \param fileName
 */
void JobManager::saveAVP(QString fileName) {

    QAxObject *jobStep = m_job->getJobStep();

    QString avpFilePath = m_avpPath + "/" + fileName;
    jobStep->dynamicCall(QString("SaveAll(\""+ avpFilePath +"\")").toLatin1().data());
    qDebug() << "Saved " + avpFilePath;
}

/**
 * @brief JobManager::loadAVP
 * @param fileName
 */
void JobManager::loadAVP(QString fileName) {
    qDebug() << "*******" << "JobManager::loadAVP";       //created for tracking steps

    m_avpInProgress = fileName;
    QString fullAVPPath = m_avpPath + "/" + fileName;

    if (m_loadInProgress) {
        qDebug() << "! Loading in progress - cannot load " + fullAVPPath;
        return;
    }

    if (m_avpLoaded && m_avp.compare(fileName, Qt::CaseInsensitive) == 0) {
        qDebug() << "! Already loaded - no action necessary: " + fileName;
        emit ready();
        return;
    }

    if (! m_camera->isReady() || m_job == NULL || fileName.isEmpty() || m_avpPath.isEmpty()) {
        qCritical() << "Cannot load AVP file " + fullAVPPath;
        return;
    }

    qDebug() << "> Start loading " + fullAVPPath;
    emit viewMessage("Loading Job: " + fullAVPPath);

    m_loadInProgress = true;

    cleanUp();
    m_job->load(fullAVPPath);

    QAxObject *systemStep = m_job->getSystemStep();
    if (systemStep != NULL && ! systemStep->isNull()) {

        m_camera->download(systemStep);
        delete systemStep;
    }
}

/**
 * @brief JobManager::cleanUp
 */
void JobManager::cleanUp() {

    // clear edit and run Mode so that when requested again to perform the necessary actions
    m_editMode = false;
    m_runMode = false;

    // remove all entries from configurable step list
    if (m_stepProperties != NULL) {
        m_stepProperties->clear();
        delete m_stepProperties;
        m_stepProperties = NULL;
    }

    // disconnect setup manager
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL) {
            m_setupManager[i]->cancelAcquireImage();
            m_setupManager[i]->disconnectJob();
        }
    }

    // disconnect reports
    for (int i = 0 ; i < 4 ; i ++)
    {
        m_reportConnection[i]->disconnectReport();
    }

    //stop all inspections
    if (m_camera != NULL)
        m_camera->stopAllInspections();

    // clear the job from memory
    if (m_job != NULL)
        m_job->removeAllJobs();

    m_avpLoaded = false;
}

/**
 * @brief JobManager::onAvpReady
 * @param success
 */
void JobManager::onAvpReady(bool success) {
    qDebug() << ((success ? "< Download finished ": "Error while downloading job to camera ") + m_avpInProgress);
    qDebug() << "*******" << "JobManager::onAvpReady";       //created for tracking steps

    if (success) {
        m_avpLoaded = true;
        m_avp = m_avpInProgress;
    }
    m_loadInProgress = false;

//    if (m_setupManager == NULL) {
//        m_setupManager = new SetupManager(this);
//        connect(m_setupManager, SIGNAL(trainImageAcquired(QPixmap*)), this, SLOT(onTrainImageAcquired(QPixmap*)));
//    }

    emit ready();
    emit viewMessage("Job loaded: " + m_avp);
}

/**
 * @brief JobManager::setPath
 * @param path
 */
void JobManager::setPath(QString path) {
    m_avpPath = path;
}

/**
 * @brief JobManager::getPath
 * @return
 */
QString JobManager::getPath() {
    return m_avpPath;
}

/**
 * @brief JobManager::onCameraReady
 */
void JobManager::onCameraReady() {
    loadAVP(m_avp);
}

/**
 * @brief JobManager::walkSteps
 */
void JobManager::walkSteps() {
    // m_job->walkJob();
}

/**
 * @brief JobManager::editMode
 */
void JobManager::editMode() {

    if (m_editMode) {
        qInfo() << "Already in Edit Mode - no action required";
        return;
    }
    else {
        qInfo() << "Entering AVP Edit Mode";
    }

    for (int i = 0 ; i < 4 ; i ++)
    {
        m_reportConnection[i]->disconnectReport();
        m_camera->stopAllInspections();
    }

    if (m_setupManager[0] == NULL) {
            m_setupManager[0] = new SetupManager(0, this);
            m_setupManager[1] = new SetupManager(1, this);
            m_setupManager[2] = new SetupManager(2, this);
            m_setupManager[3] = new SetupManager(3, this);
            connect(m_setupManager[0], SIGNAL(trainImageAcquired(QPixmap*)), this, SLOT(onTrainImageAcquired1(QPixmap*)));
            connect(m_setupManager[1], SIGNAL(trainImageAcquired(QPixmap*)), this, SLOT(onTrainImageAcquired2(QPixmap*)));
            connect(m_setupManager[2], SIGNAL(trainImageAcquired(QPixmap*)), this, SLOT(onTrainImageAcquired3(QPixmap*)));
            connect(m_setupManager[3], SIGNAL(trainImageAcquired(QPixmap*)), this, SLOT(onTrainImageAcquired4(QPixmap*)));
    }

    m_setupManager[0]->connectJob(m_job->getJobStep());
    m_setupManager[1]->connectJob(m_job->getJobStep());
    m_setupManager[2]->connectJob(m_job->getJobStep());
    m_setupManager[3]->connectJob(m_job->getJobStep());

    m_editMode = true;
    m_runMode = false;
}

/*!
 * \brief JobManager::runMode
 */
void JobManager::runMode() {

    if (m_runMode) {
        qInfo() << "Already in Run Mode - no action required";
        return;
    }
    else {
        qInfo() << "Entering AVP Run Mode";
    }

    // disconnect setup manager
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL) {
            m_setupManager[i]->cancelAcquireImage();
            m_setupManager[i]->disconnectJob();
        }
    }

    for (int i = 0 ; i < 4 ; i ++)
    {
        // download the job again to the device
        m_camera->download(m_job->getSystemStep());

        // connect reports
        m_reportConnection[i]->connectReport(
                    m_camera->getDevice(),
                    m_job->getSystemStep(),
                    i + 1,
                    m_batchConfiguration->getNoOfProducts()
                    );
    }


    m_runMode = true;
    m_editMode = false;
}

/*!
 * \brief JobManager::acquireImage
 */
void JobManager::acquireImage() {
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL)
            m_setupManager[i]->acquireImage();
    }
}

/*!
 * \brief JobManager::acquireInputImage
 * \param symName
 */
void JobManager::acquireStepImage(QString path, bool useOutput, bool showGraphics) {

    QAxObject *system = m_job->getSystemStep();
    QAxObject *step = system->querySubObject(QString("Find(\"" + path + "\", 2)").toLatin1().data());
    if (step != NULL && ! step->isNull()) {

        for (int i = 0 ; i < 4 ; i ++)
        {
            if (m_setupManager[i] != NULL)
            {
                m_setupManager[i]->selectStep(step);
                m_setupManager[i]->acquireStepImage(step, useOutput, showGraphics);
            }
        }

        delete step;
    }
    delete system;
}

/*!
 * \brief JobManager::acquireRootImage
 */
void JobManager::acquireRootImage(bool showGraphics) {
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL)
        {
            m_setupManager[i]->getImage(showGraphics);
        }
    }
}

/*!
 * \brief JobManager::getImageHSI
 * \param x
 * \param y
 * \return
 */
int* JobManager::getImageHSI(int x, int y) {
    return m_setupManager[0]->getImageHSI(x, y);
}

/*!
 * \brief JobManager::cancelAcquireImage
 */
void JobManager::cancelAcquireImage() {
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL)
        {
            m_setupManager[i]->cancelAcquireImage();
        }
    }
}

/*!
 * \brief JobManager::acquireStatus
 */
bool JobManager::acquireStatus() {
    return m_setupManager[0]->acquireStatus();
}

/**
 * @brief JobManager::onTrainImageAcquired
 */
void JobManager::onTrainImageAcquired1(QPixmap *image) {
    emit trainImageAcquired1(image);
    emit trainReady();
}
void JobManager::onTrainImageAcquired2(QPixmap *image) {
    emit trainImageAcquired2(image);
    emit trainReady();
}
void JobManager::onTrainImageAcquired3(QPixmap *image) {
    emit trainImageAcquired3(image);
    emit trainReady();
}
void JobManager::onTrainImageAcquired4(QPixmap *image) {
    emit trainImageAcquired4(image);
    emit trainReady();
}

/*!
 * \brief JobManager::onNewImage
 * \param image
 * \param result
 */
void JobManager::onNewImage1(QPixmap *image, bool result) {
    emit newImage1(image, result);
    emit imageReady(result);
}
void JobManager::onNewImage2(QPixmap *image, bool result) {
    emit newImage2(image, result);
    emit imageReady(result);
}
void JobManager::onNewImage3(QPixmap *image, bool result) {
    emit newImage3(image, result);
    emit imageReady(result);
}
void JobManager::onNewImage4(QPixmap *image, bool result) {
    emit newImage4(image, result);
    emit imageReady(result);
}

/**
 * @brief JobManager::getImage
 */
void JobManager::getImage() {
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL)
        {
            m_setupManager[i]->getImage();
        }
    }
}

/*!
 * \brief JobManager::updateROI
 * \param stepProps
 */
void JobManager::updateROI(StepProps stepProps) {
    m_job->updateROI(stepProps);
}

/*!
 * \brief JobManager::updateROIs
 * \param steps
 */
void JobManager::updateROIs(QList<StepProps> steps) {
    for (int i = 0; i < steps.size(); i++) {
        m_job->updateROI(steps[i]);
    }
}

/*!
 * \brief JobManager::onNewResult
 * \param result
 */
void JobManager::onNewResult(bool result, unsigned char resultMask) {
    emit newResult(result, resultMask);
}

/*!
 * \brief JobManager::setAvpNumericParam
 * \param code
 * \param value
 */
void JobManager::setAvpNumericParam(int inspectionId, QString code, int value) {

    QVariant targetValue;
    double doubleValue;
    int intValue;

    // get the symbolic names of step and datum from code
    QStringList parts;
    parts = code.split(":");

    if (parts.length() < 2) {
        qCritical() << "Invalid datum code " + code;
        return;
    }

    if (m_globalDatums->contains(code)) {

        DatumType dt = m_globalDatums->value(code);

        switch (dt.type) {
        case doubleType:
            doubleValue = value * 1.0 / dt.scale;
            targetValue = QVariant::fromValue(doubleValue);
            break;
        case integerType:
            intValue = value / dt.scale;
            targetValue = QVariant::fromValue(intValue);
        default:
            break;
        }
    }

    qDebug() << "> Setting value to " + code + " " + targetValue.toString();

    m_job->setAvpNumericParameter(inspectionId, parts[0], parts[1], targetValue);
}

/*!
 * \brief JobManager::setDatumValue
 * \param stepPath
 * \param stepDatum
 */
void JobManager::setDatumValue(QString stepPath, StepDatum stepDatum) {
    m_job->setDatumValue(stepPath, stepDatum);

    // TODO: update m_stepProperties
}

/*!
 * \brief JobManager::regenerate
 */
void JobManager::regenerate() {
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL)
        {
            m_setupManager[i]->regenerate();
        }
    }
    // m_job->regenerate();
}

void JobManager::refresh() {
    for (int i = 0 ; i < 4 ; i ++)
    {
        if (m_setupManager[i] != NULL)
        {
            m_setupManager[i]->refresh();
        }
    }
}

/*!
 * \brief JobManager::getAvpNumericParam
 * \param code
 * \return
 */
QVariant JobManager::getAvpNumericParam(int inspectionId, QString code) {

    QVariant sourceValue, targetValue;
    double doubleValue;
    int intValue;

    targetValue = QVariant::fromValue(0);

    // get the symbolic names of step and datum from code
    QStringList parts;
    parts = code.split(":");

    if (parts.length() < 2) {
        qCritical() << "Invalid datum code " + code;
        return QVariant::fromValue(NULL);
    }

    sourceValue = m_job->getAvpNumericParameter(inspectionId, parts[0], parts[1]);

    if (m_globalDatums->contains(code)) {

        DatumType dt = m_globalDatums->value(code);

        switch (dt.type) {
        case doubleType:
            doubleValue = sourceValue.toDouble() * dt.scale;
            targetValue = QVariant::fromValue(doubleValue);
            break;
        case integerType:
            intValue = sourceValue.toInt() * dt.scale;
            targetValue = QVariant::fromValue(intValue);
        default:
            break;
        }
    }
    return targetValue;
}

/*!
 * \brief JobManager::setBatchConfiguration
 *        Use this function to set the effectin Batch Configuration. The supplied
 *        configuration will be used for all subsequent inspection process executions.

 * \param conf the current batch configuration data.
 */
void JobManager::setBatchConfiguration(BatchConfiguration *conf) {

    qDebug() << "Change batch configuration";

    m_batchConfiguration = new BatchConfiguration(conf, this);
}

void JobManager::onNewFailedImage(QPixmap *pix, int providerId) {
    qDebug() << "***** NewFailedImage Reached : " << providerId;
    emit newFailedImage(pix, providerId);
}
