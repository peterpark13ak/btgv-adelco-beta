#include "uicontroller.h"
#include <systemmanager.h>

UIController::UIController(
        QString configId,
        PLCConnection *plcConnection,
        ConfigurationManager *manager,
        JobManager *jobmanager,
        TrainImageProvider **trainImageProvider,
        QObject *parent) : QObject(parent)
{
    m_activeConfigurationId = configId;
    m_ready = false;
    m_plcRunning = false;
    m_plcError = false;
    m_plcWarning = false;

    for (int i = 0 ; i < 4 ; i ++)
    {
        m_trainImageProvider[i] = trainImageProvider[i];
    }

    m_plcConnection = plcConnection;
    m_configurationManager = manager;
    m_jobManager = jobmanager;
    m_jobReady = false;

    m_cyclePulses = m_plcConnection->getCyclePulses();
    m_triggerOffset = m_plcConnection->getTriggerOffset();

    newMessage("Application started");

    connect(m_plcConnection, SIGNAL(plcConnected()), this, SIGNAL(plcConnect()));
    connect(m_plcConnection, SIGNAL(plcDisconnected()), this, SIGNAL(plcDisconnect()));
    connect(m_plcConnection, SIGNAL(plcStatus(SystemStatus*)), this, SLOT(onPlcStatus(SystemStatus*)));

    // connect message signals from all sources
    connect(m_plcConnection, SIGNAL(viewMessage(QString)), this, SIGNAL(newMessage(QString)));
    connect(m_jobManager, SIGNAL(viewMessage(QString)), this, SIGNAL(newMessage(QString)));

    // connect job manager signals
    connect(m_jobManager, SIGNAL(ready()), this, SLOT(onJobReady()));
    connect(m_jobManager, SIGNAL(trainReady()), this, SLOT(onTrainReady()));
    connect(m_jobManager, SIGNAL(stepImageReady()), this, SIGNAL(stepImageReady()));
    connect(m_jobManager, SIGNAL(imageReady(bool)), this, SIGNAL(imageReady(bool)));
    connect(m_jobManager, SIGNAL(newResult(bool, unsigned char)), m_plcConnection, SLOT(onNewResult(bool, unsigned char)));
    connect(m_jobManager, SIGNAL(newStatistics(QString)), this, SIGNAL(newStatistics(QString)));
}

/*!
 * \brief UIController::setEditMode
 */
void UIController::setEditMode() {
    m_jobManager->editMode();

    emit jobReady(true);
}

/*!
 * \brief UIController::setRunMode
 */
void UIController::setRunMode() {
    m_jobManager->runMode();

    emit jobReady(true);
}

/*!
 * \brief UIController::acquireImage
 */
void UIController::acquireImage() {
    m_jobManager->acquireImage();
}

/*!
 * \brief UIController::acquireStepImage
 * \param symName
 */
void UIController::acquireStepImage(QString symName, bool useOutput, bool showGraphics) {
    m_jobManager->acquireStepImage(symName, useOutput, showGraphics);
}

/*!
 * \brief UIController::acquireRootImage
 */
void UIController::acquireRootImage(bool showGraphics) {
   m_jobManager->acquireRootImage(showGraphics);
}

/*!
 * \brief UIController::getImageHSI
 * \param x the image x coordinate
 * \param y the image y coordinate
 * \return the HSI as JSON serialized objet { hue: int, saturation: int, intensity: int }
 */
QString UIController::getImageHSI(int x, int y) {
    // int* colorComponents = m_jobManager->getImageHSI(x, y);

    int colorComponents[3];

    // int* colorComponents =
    m_trainImageProvider[0]->getImageHSI(x, y, colorComponents);

    QJsonObject hsi = {
        { "hue",        colorComponents[0] },
        { "saturation", colorComponents[1] },
        { "intensity",  colorComponents[2] }
    };

    QJsonDocument doc(hsi);
    return doc.toJson();
}

/*!
 * \brief UIController::cancelAcquireImage
 */
void UIController::cancelAcquireImage() {
    m_jobManager->cancelAcquireImage();
}

/*!
 * \brief UIController::acquireStatus
 * \return
 */
bool UIController::acquireStatus() {
    return m_jobManager->acquireStatus();
}

/*!
 * \brief UIController::onJobReady
 */
void UIController::onJobReady() {
    emit jobReady(true);
}

/*!
 * \brief UIController::isJobReady
 * \return
 */
bool UIController::isJobReady() {
    return m_jobReady;
}

/*!
 * \brief UIController::onTrainReady
 */
void UIController::onTrainReady() {

    emit trainImageReady();
    return;
}

/*!
 * \brief UIController::setAvpNumericParam
 * \param code
 * \param value
 */
void UIController::setAvpNumericParam(int inspectionId, QString code, int value) {
    m_jobManager->setAvpNumericParam(inspectionId, code, value);
}

/*!
 * \brief UIController::setDatumValue
 * \param stepPath
 * \param datumJson
 */
void UIController::setDatumValue(QString stepPath, QString datumJson) {

    StepDatum stepDatum = DatumProperties::datumFromJson(datumJson);
    m_jobManager->setDatumValue(stepPath, stepDatum);
}

/*!
 * \brief UIController::updateROI
 * \param stepModel
 */
void UIController::updateROI(QString stepModel) {
    StepProps stepProps = StepProperties::fromJson(stepModel);

    m_jobManager->updateROI(stepProps);
}

/*!
 * \brief UIController::updateROIs
 * \param stepsModel
 */
void UIController::updateROIs(QString stepsModel) {
    QList<StepProps> stepsProps = StepProperties::listFromJson(stepsModel);

    m_jobManager->updateROIs(stepsProps);
}

/*!
 * \brief UIController::getGlobalDatums
 * \return
 */
QString UIController::getGlobalDatums(int inspectionId) {
    QList<DatumType> globalDatums = m_jobManager->getGlobalDatums(inspectionId);

    QJsonArray datums = {};
    for (int i = 0; i < globalDatums.length(); i++) {
        datums.append( DatumProperties::asJson( globalDatums[i]) );
    }

    QJsonDocument doc(datums);
    return doc.toJson();
}

/*!
 * \brief UIController::getConfigurableSteps
 * \return
 */
QString UIController::getConfigurableSteps(bool withUpdate) {
    QList<StepProps> *stepProperties = m_jobManager->getConfigurableSteps(withUpdate);

    QJsonArray steps = {};
    for (int i = 0; i < stepProperties->length(); i++) {
        steps.append( StepProperties::asJson( stepProperties->at(i) ));
    }

    QJsonDocument doc(steps);
    return doc.toJson();
}

/*!
 * \brief UIController::getStep
 * \param path
 * \return
 */
QString UIController::getStep(QString stepPath) {
   StepProps stepProperties = m_jobManager->getStepProps(stepPath);

   QJsonObject stepJsonObject = StepProperties::asJson(stepProperties);

   QJsonDocument doc(stepJsonObject);
   return doc.toJson();
}

/*!
 * \brief UIController::trainAll
 */
void UIController::trainAll() {
    m_jobManager->trainAll();
}

/*!
 * \brief UIController::regenerate
 */
void UIController::regenerate(QString requestor) {
   m_jobManager->regenerate();
   m_jobManager->refresh();

   qDebug() << ">> Manager Ready for " << requestor;
   emit managerReady(requestor);
}


/*!
 * \brief UIController::tryOutSteps
 */
void UIController::tryOutSteps(QString startStepModel, QString requestor) {

    StepProps stepProps = StepProperties::fromJson(startStepModel);

    m_jobManager->tryOutSteps(stepProps.path);

    qDebug() << ">> Manager Ready for " << requestor;
    emit managerReady(requestor);
}

/*!
 * \brief UIController::train
 * \param stepPath
 */
void UIController::train(QString stepPath) {
    m_jobManager->train(stepPath);
}

/*!
 * \brief UIController::trainParent
 * \param stepPath
 */
void UIController::trainParent(QString stepPath) {
    m_jobManager->trainParent(stepPath);
}

/*!
 * \brief UIController::tryout
 * \param stepPath
 */
void UIController::tryout(QString stepPath, QString requestor) {
    m_jobManager->tryout(stepPath);

    qDebug() << ">> Manager Ready for " << requestor;
    emit managerReady(requestor);}

/*!
 * \brief UIController::deleteStep
 * \param stepPath
 */
void UIController::deleteStep(QString stepPath) {
    m_jobManager->deleteStep(stepPath);
}

/*!
 * \brief UIController::setActiveConfigurationId
 * \param id
 */
void UIController::setActiveConfigurationId(QString id) {
    m_activeConfigurationId = id;
}

/*!
 * \brief UIController::getActiveConfigurationId
 * \return
 */
QString UIController::getActiveConfigurationId() {
   return m_activeConfigurationId;
}

/*!
 * \brief UIController::initialize
 */
void UIController::initialize() {
    m_plcConnection->plcConnect();
    m_ready = true;

    QString avp = m_configurationManager->getAvpById(m_activeConfigurationId);
    m_jobManager->setAvp(avp);

    m_jobManager->start();
}

/*!
 * \brief UIController::requestPLCStatus
 */
void UIController::requestPLCStatus() {
    m_plcConnection->getPLCStatus();
}

/*!
 * \brief UIController::requestPLCStart
 */
void UIController::requestPLCStart() {
    m_plcConnection->start();
}

/*!
 * \brief UIController::requestPLCStop
 */
void UIController::requestPLCStop() {
    m_plcConnection->stop();
}

/*!
 * \brief UIController::resetCounters
 * \param group
 */
void UIController::resetCounters(int group) {
    m_plcConnection->resetCounters(group);
}

/*!
 * \brief UIController::resetErrors
 */
void UIController::resetErrors() {
    m_plcConnection->resetErrors();
}

/*!
 * \brief UIController::startTest
 * \param code
 */
void UIController::startTest(int code) {
    m_plcConnection->startTest(code);
}

/*!
 * \brief UIController::stopTest
 * \param code
 */
void UIController::stopTest(int code) {
    m_plcConnection->stopTest(code);
}

/*!
 * \brief UIController::setManualMode
 * \param manualMode
 */
void UIController::setManualMode(bool manualMode) {
    m_plcConnection->setManualMode(manualMode);
}

/*!
 * \brief UIController::saveSystemConfiguration
 * \param config
 */
void UIController::saveSystemConfiguration(SystemConfiguration *config) {
    m_plcConnection->sendSystemConfiguration(config);
}

/*!
 * \brief UIController::getSystemConfiguration
 * \return
 */
SystemConfiguration *UIController::getSystemConfiguration() {
    return m_plcConnection->getSystemConfiguration();
}

/*!
 * \brief UIController::onPlcStatus
 * \param systemStatus
 */
void UIController::onPlcStatus(SystemStatus *systemStatus) {
    emit plcStatus(systemStatus);

    quint8 flags = systemStatus->getStatusFlags();

    // handle Running status changes
    // -------------------------------------------------------------
     bool newRunningStatus = flags & SYSTEM_STARTED_FLAG;

     if (newRunningStatus != m_plcRunning) {
        m_plcRunning = newRunningStatus;

        if (m_plcRunning) {
            emit plcStarted();
            emit newMessage("PLC started");
        }
        else {
            emit plcStopped();
            emit newMessage("PLC stopped");
        }
    }

    // handle Error status changes
    // -------------------------------------------------------------
    bool newErrorStatus = flags & SYSTEM_ERROR_FLAG;

    if (newErrorStatus != m_plcError) {
        m_plcError = newErrorStatus;
        emit plcErrorStatus(m_plcError);
    }

    // handle Warning status changes
    // -------------------------------------------------------------
    bool newWarningStatus = flags & SYSTEM_WARNING_FLAG;

    if (newWarningStatus != m_plcWarning) {
        m_plcWarning = newWarningStatus;
        emit plcWarningStatus(m_plcWarning);
    }
}

// ------------------------------------------------------------------
// Configuration Management functions
// ------------------------------------------------------------------
/*!
 * \brief UIController::getConfigurationNames
 * \return
 */
QStringList UIController::getConfigurationNames() {
    return m_configurationManager->getConfigurationNames();
}

/*!
 * \brief UIController::getReadOnlyConfigurations
 * \return
 */
QStringList UIController::getReadOnlyConfigurations() {
    return m_configurationManager->getReadOnlyConfigurations();
}

/*!
 * \brief UIController::getConfigurationById
 * \param id
 * \param config
 */
BatchConfiguration *UIController::getConfigurationById(QString id) {

    BatchConfiguration *config = new BatchConfiguration();

    // convert pulses to degress (x100)
    // -------------------------------------------------------------
    m_configurationManager->getConfigurationById(id, config);
//    config->setCameraTrigger ( (quint32)( ((m_cyclePulses + config->getCameraTrigger()  - m_triggerOffset) % m_cyclePulses) * 36000.0 / m_cyclePulses + 0.5) );
//    config->setLightStrobeOff( (quint32)( ((m_cyclePulses + config->getLightStrobeOff() - m_triggerOffset) % m_cyclePulses) * 36000.0 / m_cyclePulses + 0.5) );
//    config->setRejectValveOn ( (quint32)( ((m_cyclePulses + config->getRejectValveOn()  - m_triggerOffset) % m_cyclePulses) * 36000.0 / m_cyclePulses + 0.5) );
//    config->setRejectValveOff( (quint32)( ((m_cyclePulses + config->getRejectValveOff() - m_triggerOffset) % m_cyclePulses) * 36000.0 / m_cyclePulses + 0.5) );

    qDebug() << "getting the batch configuration of " << id;
    qDebug() << "*******" << "UIController::getConfigurationById(id:" << id << ")";       //created for tracking steps

    return config;
}

/*!
 * \brief UIController::useConfiguration
 * \param config
 * \param loadAvp
 */
void UIController::useBatchConfiguration(BatchConfiguration *config, bool loadAvp) {

    // convert degress (x100) to pulses
    // -------------------------------------------------------------
    BatchConfiguration *newConfig = new BatchConfiguration(config, config);
//    newConfig->setCameraTrigger ( (quint32)(config->getCameraTrigger()  * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
//    newConfig->setLightStrobeOff( (quint32)(config->getLightStrobeOff() * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
//    newConfig->setRejectValveOn ( (quint32)(config->getRejectValveOn()  * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
//    newConfig->setRejectValveOff( (quint32)(config->getRejectValveOff() * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
    m_plcConnection->sendBatchConfiguration(newConfig);
    delete newConfig;

    if (! loadAvp) {
        return;
    }

    emit jobReady(false);

    m_activeConfigurationId = config->getName();
    m_configurationManager->storeConfigurationId(m_activeConfigurationId);

    QString fileName = m_activeConfigurationId + ".avp";
    m_jobManager->setBatchConfiguration(config);
    m_jobManager->loadAVP(fileName);
    qDebug() << " use BatchConfiguration : AVP load";
}

/*!
 * \brief UIController::loadAvp
 * \param code
 */
void UIController::loadAvp(QString code, bool editMode) {
    // TODO: check the necessity of the editMode parameter
    // Q_UNUSED(editMode);
    qDebug() << "*******" << "UIController::loadAvpcode(code:" << code << ")";       //created for tracking steps

    emit jobReady(false);
    if (editMode) {
        QString fileName = code + ".avp";
        m_jobManager->loadAVP(fileName);
    }
    else {
        BatchConfiguration *conf = getConfigurationById(code);
        useBatchConfiguration(conf, true);
    }
}

/*!
 * \brief UIController::saveAvp
 * \param code
 */
void UIController::saveAvp(QString code) {

    QString fileName = code + ".avp";
    m_jobManager->saveAVP(fileName);
}

/*!
 * \brief UIController::updateConfiguration
 * \param config
 */
void UIController::updateConfiguration(BatchConfiguration *config) {
    qDebug() << "Updating the Batch-Configuration database entry for " + config->getName();

    BatchConfiguration *newConfig = new BatchConfiguration(config, config);
    convertDegressToPulses(newConfig);
    m_configurationManager->updateConfiguration(newConfig);
    delete newConfig;
}

/*!
 * \brief UIController::saveConfiguration
 * \param config
 */
void UIController::saveConfiguration(BatchConfiguration *config) {
    qDebug() << "Saving the Batch-Configuration database entry for " + config->getName();

    BatchConfiguration *newConfig = new BatchConfiguration(config, config);
    convertDegressToPulses(newConfig);
    m_configurationManager->saveConfiguration(newConfig);
    delete newConfig;
}

/*!
 * \brief UIController::convertDegressToPulses
 * \param config
 */
void UIController::convertDegressToPulses(BatchConfiguration *config) {
    Q_UNUSED(config)

    // convert degress (x100) to pulses
    // -------------------------------------------------------------
//    config->setCameraTrigger ( (quint32)(config->getCameraTrigger()  * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
//    config->setLightStrobeOff( (quint32)(config->getLightStrobeOff() * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
//    config->setRejectValveOn ( (quint32)(config->getRejectValveOn()  * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
//    config->setRejectValveOff( (quint32)(config->getRejectValveOff() * m_cyclePulses / 36000.0 + m_triggerOffset + 0.5) % m_cyclePulses  );
}

/*!
 * \brief UIController::configurationNameExists
 * \param name
 */
bool UIController::configurationNameExists(QString name) {
    return m_configurationManager->nameExists(name);
}

/*!
 * \brief UIController::copyAvp
 * \param name1
 * \param name2
 */
void UIController::copyAvp(QString name1, QString name2) {
    m_configurationManager->copyAvp(name1, name2);
}

/*!
 * \brief UIController::deleteConfiguration
 * \param name
 */
void UIController::deleteConfiguration(QString name) {
    m_configurationManager->deleteConfiguration(name);
}

/*!
 * \brief UIController::shutDown
 */
void UIController::shutDown() {
    m_plcConnection->stop();
    SystemManager::shutDown();
}
