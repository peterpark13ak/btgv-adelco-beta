#include "plcconnection.h"

/**
 * @brief PLCConnection::PLCConnection
 * @param settings
 * @param parent
 */
PLCConnection::PLCConnection(QSettings *st, QObject *parent) : QObject(parent)
{
    m_settings = st;

    // get application settings
    // ------------------------------------------------------------------------------------
    m_settings->beginGroup("PLC");
    m_serverAddress = m_settings->value("plcAddress", "127.0.0.1").toString();
    m_serverPort = m_settings->value("plcPort", 2000).toInt();
    m_pollingPeriod = m_settings->value("pollingPeriod", 1000).toInt();
    m_cyclePulses = m_settings->value("cyclePulses", 4000).toInt();
    m_triggerOffset = m_settings->value("triggerOffset", 0).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("Thresholds");
    m_noAnswerThreshold = m_settings->value("NoAnswerThreshold", 100).toInt();
    m_failedThreshold = m_settings->value("FailedThreshold", 100).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("Switches");
    m_continueOnError = m_settings->value("ContinueOnError", true).toBool();
    m_settings->endGroup();
    // ------------------------------------------------------------------------------------

    // prepare system configuration
    // ------------------------------------------------------------------------------------
    m_systemConfigAppliedToPLC = false;
    m_systemConfiguration = new SystemConfiguration(this);
    m_systemConfiguration->setNoAnswerThreshold(m_noAnswerThreshold);
    m_systemConfiguration->setFailedThreshold(m_failedThreshold);
    m_systemConfiguration->setContinueOnError(m_continueOnError);
    // ------------------------------------------------------------------------------------

    m_running = false;
    m_sampleSeq = 0;
    m_connected = false;
    m_plcSocket = new QTcpSocket(this);
    m_systemStatus = new SystemStatus(this);

    connect(m_plcSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_plcSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(m_plcSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(m_plcSocket, SIGNAL(readyRead()), this, SLOT(newData()));
}

void PLCConnection::updateLocalSystemConfiguration(SystemConfiguration *config) {
    m_systemConfiguration->setNoAnswerThreshold(config->getNoAnswerThreshold());
    m_systemConfiguration->setFailedThreshold(config->getFailedThreshold());
    m_systemConfiguration->setContinueOnError(config->getContinueOnError());
}

/**
 * @brief PLCConnection::plcConnect
 */
void PLCConnection::plcConnect() {
    qDebug() << "> Trying to connect to PLC ...";
    m_plcSocket->connectToHost(m_serverAddress, m_serverPort);
}

/**
 * @brief PLCConnection::connected
 */
void PLCConnection::connected() {
    qInfo() << "< Connected to PLC";
    m_connected = true;
    emit plcConnected();
    emit viewMessage("Connected to PLC");

    // update PLC with system configuration on PLC startup
    if (! m_systemConfigAppliedToPLC) {
        sendSystemConfiguration(m_systemConfiguration);
        m_systemConfigAppliedToPLC = true;
    }
}

/**
 * @brief PLCConnection::disconnected
 */
void PLCConnection::disconnected() {
    qWarning() << "Disconnected from PLC";
    m_connected = false;
    emit plcDisconnected();
    emit viewMessage("Disconnected from PLC");
}

/**
 * @brief PLCConnection::connectionError
 * @param error
 */
void PLCConnection::connectionError(QAbstractSocket::SocketError error) {
    qWarning() << "Connection error:" << m_plcSocket->errorString() << "- trying to reconnect...";
    emit viewMessage("Connection error: " + m_plcSocket->errorString());

    switch (error) {
    case QTcpSocket::RemoteHostClosedError:
            m_plcSocket->disconnectFromHost();
    case QTcpSocket::ConnectionRefusedError:
    case QTcpSocket::UnfinishedSocketOperationError:
            m_plcSocket->abort();
            m_connected = false;
            QTimer::singleShot(1000, this, SLOT(plcConnect()));
            break;
    default:
            break;
    }
}

/**
 * @brief newData
 */
void PLCConnection::newData() {

    int length;
    unsigned char command;

    length = m_plcSocket->bytesAvailable();
    if (length < 1) {
        qWarning() << "PLC communication - No input data available";
        return;
    }

    QDataStream in(m_plcSocket);
    in.setByteOrder(QDataStream::BigEndian);
    // SystemStatus systemStatus;
    bool newRunningStatus;

    // get command
    in >> command;

    switch(command) {

    // get Status Data
    case 'Q':
        fillInSystemStatus(in, *m_systemStatus);

        newRunningStatus = (m_systemStatus->statusFlags & SYSTEM_STARTED_FLAG) == 1;
        if ( newRunningStatus != m_running ) {
            m_running = newRunningStatus;
            qInfo() << (m_running ? "Process Started" : "Process Stopped");
        }

        emit plcStatus(m_systemStatus);
        break;
    }

    // clear input buffer
    m_plcSocket->readAll();
}

/**
 * @brief PLCConnection::sendAnswer
 * @param seq
 */
void PLCConnection::sendAnswer(unsigned char seq) {
    unsigned char command = 'P';

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.writeRawData((const char *)&command, 1);
    out.writeRawData((const char *)&seq, sizeof(seq));
    out.writeRawData(QByteArray(24, 0x00).data(), 24);

    m_plcSocket->write(block);
    m_plcSocket->flush();
}

/**
 * @brief PLCConnection::sendAck
 */
void PLCConnection::sendAck(unsigned char seq) {
    unsigned char command = 'A';
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.writeRawData((const char *)&command, 1);
    out.writeRawData((const char *)&seq, sizeof(seq));
    out.writeRawData(QByteArray(24, 0x00).data(), 24);

    m_plcSocket->write(block);
    m_plcSocket->flush();
}

/**
 * @brief PLCConnection::sendSimpleCommand
 * @param command
 */
void PLCConnection::sendSimpleCommand(unsigned char command) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.writeRawData((const char *)&command, 1);
    out.writeRawData(QByteArray(25, 0x00).data(), 25);

    m_plcSocket->write(block);
    m_plcSocket->flush();

    // Do not log Q commands
    if ((char)command == 'Q')
       return;

    qDebug() << "> Command" << (char)command << "sent";
}

/**
 * @brief PLCConnection::start
 */
void PLCConnection::start() {
    sendSimpleCommand('I');
    qInfo() << "Start command sent";

    this->m_sampleSeq = 0;
}

/**
 * @brief PLCConnection::stop
 */
void PLCConnection::stop() {
    sendSimpleCommand('O');
    qInfo() << "Stop command sent";
    emit stopped();
}

/**
 * @brief PLCConnection::sendBatchConfiguration
 */
void PLCConnection::sendBatchConfiguration(BatchConfiguration *batchConf) {

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << (qint8)'C';
    out << (qint8)0x01;     // argument 1
    out << (quint16)0x0;    // unused arguments 2, 3

    out << (quint16)0x0;
    out << (quint16)0x0;

    out << (quint16)batchConf->getCameraToReject();
    out << (quint16)batchConf->getTrigger1Offset();
    out << (quint16)batchConf->getTrigger2Offset();
    out << (quint16)batchConf->getReject1Offset();
    out << (quint16)batchConf->getRejectDistance();
    out << (quint16)batchConf->getRejectDistance();
    out << (quint16)batchConf->getRejectDistance();
    out << (quint16)batchConf->getRejectDuration();

    out << (quint16)0;
    m_plcSocket->write(block);
    m_plcSocket->flush();

    /*
    quint16 getCameraToReject()         { return cameraToReject; }
    quint16 getTrigger1Offset()         { return trigger1Offset; }
    quint16 getTrigger2Offset()         { return trigger2Offset; }
    quint16 getReject1Offset()          { return reject1Offset; }
    quint16 getRejectDistance()         { return rejectDistance; }
    quint16 getRejectDuration()         { return rejectDuration; }
    */

    qInfo() << "> Batch Configuration Sent " << block.toHex();
}

/*!
 * \brief PLCConnection::sendSystemConfiguration
 * \param systemConf
 */
void PLCConnection::sendSystemConfiguration(SystemConfiguration *systemConf) {

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << (qint8)'C';
    out << (qint8)0x00;     // argument 1
    out << (quint16)0;      // unused arguments 2, 3
    out << (quint16)systemConf->getNoAnswerThreshold();
    out << (quint16)systemConf->getFailedThreshold();
    out << (quint16)0; // systemConf->getCameraToReject();
    out << (quint16)0; // systemConf->getTrigger1Offset();
    out << (quint16)0; // systemConf->getTrigger2Offset();
    out << (quint16)0; // systemConf->getReject1Offset();
    out << (quint16)0; // systemConf->getRejectDistance();
    out << (quint16)0; // systemConf->getRejectDistance();
    out << (quint16)0; // systemConf->getRejectDistance();
    out << (quint16)0; // systemConf->getRejectDuration();
    out << (quint16)0;
    m_plcSocket->write(block);
    m_plcSocket->flush();

    // ! Spyros should embed these two properties to the system-configuration block
    setContinueOnError(systemConf->getContinueOnError());

    saveSystemConfiguration(systemConf);
    qInfo() << "> System Configuration Sent " << block.toHex();
}

/*!
 * \brief PLCConnection::getSystemConfiguration
 * \return
 */
SystemConfiguration *PLCConnection::getSystemConfiguration() {
    return m_systemConfiguration;
}

/**
 * @brief PLCConnection::sendCommand
 * @param command
 * @param arg
 */
void PLCConnection::sendCommand(unsigned char command, unsigned char arg) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.writeRawData((const char *)&command, 1);
    out.writeRawData((const char *)&arg, 1);
    out.writeRawData(QByteArray(24, 0x00).data(), 24);

    m_plcSocket->write(block);
    m_plcSocket->flush();

    qDebug() << "> Command" << (char)command << "sent with arg" << arg;
    qDebug() << "*******" << "PLCConnection::sendCommand";       //created for tracking steps

}

/**
 * @brief PLCConnection::startTest
 * @param test
 */
void PLCConnection::startTest(int test) {
    sendCommand('E', test);
}

/**
 * @brief PLCConnection::stopTest
 * @param test
 */
void PLCConnection::stopTest(int test) {
    sendCommand('D', test);
}

/*!
 * \brief PLCConnection::getPLCStatus
 */
void PLCConnection::getPLCStatus() {

    if (!m_connected)
        return;

    sendSimpleCommand('Q');
}

/**
 * @brief PLCConnection::isConnected
 * @return
 */
bool PLCConnection::isConnected() {
    return m_connected;
}

/**
 * @brief PLCConnection::fillInSystemStatus
 * @param in
 * @param sStatus
 */
void PLCConnection::fillInSystemStatus(QDataStream &in, SystemStatus &status) {

    in.skipRawData(1);                      // skip Q command unused argument
    in >> (quint8&)status.statusFlags;
    in >> (quint8&)status.reservedA;

    readPermanentCounters(in, status);
    readBatchCounters(in, status);
    readDailyCounters(in, status);

    in >> (quint16&)status.encoder;
    in >> (quint8&)status.warningsA;
    in >> (quint8&)status.warningsB;

    in >> (quint8&)status.errorsA;
    in >> (quint8&)status.errorsB;
    in >> (quint8&)status.errorsC;
    in >> (quint8&)status.errorsD;
}

/**
 * @brief PLCConnection::readPermanentCounters
 * @param in
 * @param status
 */
void PLCConnection::readPermanentCounters(QDataStream &in, SystemStatus &status) {

    // read 64 bytes
    in >> status.permCameraTriggers;
    in >> status.permAnswers;
    in >> status.permNoAnswerFails;
    in >> status.permPassed01;
    in >> status.permPassed02;
    in >> status.permPassed03;
    in >> status.permPassed04;
    in >> status.permFails01;
    in >> status.permFails02;
    in >> status.permFails03;
    in >> status.permFails04;
    in >> status.permExternalRejections;
    in >> status.permRejectionsAgree;
    in.skipRawData(12);
}

/**
 * @brief PLCConnection::readGlobalCounters
 * @param in
 * @param status
 */
void PLCConnection::readGlobalCounters(QDataStream &in, SystemStatus &status) {

    in >> status.globalCameraTriggers;
    in >> status.globalAnswers;
    in >> status.globalNoAnswerFails;
    in >> status.globalPassed01;
    in >> status.globalPassed02;
    in >> status.globalPassed03;
    in >> status.globalPassed04;
    in >> status.globalFails01;
    in >> status.globalFails02;
    in >> status.globalFails03;
    in >> status.globalFails04;
    in >> status.globalExternalRejections;
    in >> status.globalRejectionsAgree;
    in.skipRawData(12);
}

/**
 * @brief PLCConnection::readBatchCounters
 * @param in
 * @param status
 */
void PLCConnection::readBatchCounters(QDataStream &in, SystemStatus &status) {

    in >> status.batchCameraTriggers;
    in >> status.batchAnswers;
    in >> status.batchNoAnswerFails;
    in >> status.batchPassed01;
    in >> status.batchPassed02;
    in >> status.batchPassed03;
    in >> status.batchPassed04;
    in >> status.batchFails01;
    in >> status.batchFails02;
    in >> status.batchFails03;
    in >> status.batchFails04;
    in >> status.batchExternalRejections;
    in >> status.batchRejectionsAgree;
    in.skipRawData(12);
}

/**
 * @brief PLCConnection::readDailyCounters
 * @param in
 * @param status
 */
void PLCConnection::readDailyCounters(QDataStream &in, SystemStatus &status) {

    in >> status.dailyCameraTriggers;
    in >> status.dailyAnswers;
    in >> status.dailyNoAnswerFails;
    in >> status.dailyPassed01;
    in >> status.dailyPassed02;
    in >> status.dailyPassed03;
    in >> status.dailyPassed04;
    in >> status.dailyFails01;
    in >> status.dailyFails02;
    in >> status.dailyFails03;
    in >> status.dailyFails04;
    in >> status.dailyExternalRejections;
    in >> status.dailyRejectionsAgree;
    in.skipRawData(12);

}

/*!
 * \brief PLCConnection::saveSystemConfiguration
 * \param sys
 */
void PLCConnection::saveSystemConfiguration(SystemConfiguration *sys) {

    updateLocalSystemConfiguration(sys);

    m_settings->beginGroup("Thresholds");
    m_settings->setValue("NoAnswerThreshold", sys->getNoAnswerThreshold());
    m_settings->setValue("FailedThreshold", sys->getFailedThreshold());
    m_settings->endGroup();

    m_settings->beginGroup("Switches");
    m_settings->setValue("ContinueOnError", sys->getContinueOnError());
    m_settings->endGroup();
}

/**
 * @brief PLCConnection::setManualMode
 * @param manualMode
 */
void PLCConnection::setManualMode(bool manualMode) {
    sendCommand('M', manualMode ? 1 : 0);
}

/**
 * @brief PLCConnection::onNewResult
 * @param passed
 */
void PLCConnection::onNewResult(bool passed, unsigned char resultMask) {

    unsigned char command = (passed && resultMask == 0) ? 'P' : 'F';

    this->m_sampleSeq = (quint8)(this->m_sampleSeq + 1) == 0 ? 1 : this->m_sampleSeq + 1;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.writeRawData((const char *)&command, 1);
    out.writeRawData((const char *)&(this->m_sampleSeq), sizeof(this->m_sampleSeq));
    out.writeRawData((const char *)&resultMask, 1);          // failed mask
    out.writeRawData(QByteArray(23, 0x00).data(), 23);

    m_plcSocket->write(block);
    m_plcSocket->flush();

    QString logMsg = (passed ? " > Passed blister " : " > Failed blister ")
            + QString::number(this->m_sampleSeq);
    qDebug() << logMsg.toLatin1() << " / mask: " << resultMask;
}

/*!
 * \brief PLCConnection::resetCounters
 * \param group
 */
void PLCConnection::resetCounters(int group) {
    sendCommand('B', group);
}

/**
 * @brief PLCConnection::onResetErrors
 */
void PLCConnection::resetErrors() {
    sendSimpleCommand('R');
}

/**
 * @brief PLCConnection::setContinueOnError
 * @param mode
 */
void PLCConnection::setContinueOnError(bool mode) {
    sendCommand('N', mode);
}

/**
 * @brief PLCConnection::setStrobeAlwaysOn
 * @param mode
 */
void PLCConnection::setStrobeAlwaysOn(bool mode) {
    sendCommand('L', mode);
}

/*!
 * \brief PLCConnection::getCyclePulses
 * \return
 */
int PLCConnection::getCyclePulses() {
    return m_cyclePulses;
}

/*!
 * \brief PLCConnection::getTriggerOffset
 * \return
 */
int PLCConnection::getTriggerOffset() {
    return m_triggerOffset;
}
