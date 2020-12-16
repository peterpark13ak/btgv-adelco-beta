#include <QDebug>
#include "reportconnection.h"

// Avpreport.AvpReportConnection.1 - {DE40F272-B890-41BD-96DA-426537BD2EF2}

/*!
 * \brief ReportConnection::ReportConnection
 * \param parent
 */
ReportConnection::ReportConnection(QSettings *settings, int reportId, QObject *parent) : QObject(parent)
{
    m_connected = false;
    this->m_reportId = reportId;

    // get application settings
    // ------------------------------------------------------------------------------------
    settings->beginGroup("Vscape");
    m_imagePath = settings->value("imagePath", "c:/btgv-adelco/images").toString();
    m_imageSave = settings->value("imageSave", "false").toBool();
    m_saveOnlyFailed = settings->value("imageOnlyFailed", "true").toBool();

    m_prodStepNames.append(settings->value("Prod1StepName", "Product01").toString());
    m_prodStepNames.append(settings->value("Prod2StepName", "Product02").toString());
    m_prodStepNames.append(settings->value("Prod3StepName", "Product03").toString());
    m_prodStepNames.append(settings->value("Prod4StepName", "Product04").toString());
    settings->endGroup();
    // ------------------------------------------------------------------------------------

    m_reportConnection = new QAxObject("{DE40F272-B890-41BD-96DA-426537BD2EF2}");

    // OnNewReport([in] IAvpInspReport* rptObj, [in] VARIANT_BOOL bGoingToFreeze);
    connect(m_reportConnection,
            SIGNAL(OnNewReport(IDispatch*, bool)),
            this,
            SLOT(onNewReport(IDispatch*, bool))
            );

    connect(m_reportConnection,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            this,
            SLOT(onReportConnectionException(int, const QString &, const QString &, const QString &))
            );
}

/*!
 * \brief ReportConnection::onCoordinatorException
 * \param code
 * \param source
 * \param desc
 * \param help
 */
void ReportConnection::onReportConnectionException(int code, const QString &source, const QString &desc, const QString &help) {
    qCritical() << "Error on Report Connection.\nCode:" << code << "\nSource:" << source << "\nDescription:" << desc << "\nHelp:" << help;
}

/*!
 * \brief ReportConnection::connect
 * \param device
 * \param inspectionIndex
 * \param noOfProducts the number of products inspected in each inspection cycle
 */
bool ReportConnection::connectReport(QAxObject *device, QAxObject *system, int inspectionIndex, uint noOfProducts) {

    m_overruns = 0;

    if (m_reportConnection->dynamicCall("Connected()").toBool())
        m_reportConnection->dynamicCall("Disconnect()");

    // Connect the Report Connection to the specified inspection on the specified device
    QString connectArg = "Connect(\""
            + device->dynamicCall("Name").toString() + "\", "
            + QString::number(inspectionIndex)
            + ")";
    m_reportConnection->dynamicCall(connectArg.toLatin1().data());

    // Add the first snapshot buffer to our report
    addImageToReport(system);

    // Add product check to report
    m_prodStepPaths.clear();
    for (uint pIdx = 0; pIdx < noOfProducts; pIdx++) {
        // get the full path of the corresponding step responsible for the product inspection result
        // the result is hold in the Status datum of the step
        QString stepName = m_prodStepNames.at(pIdx);
        QString path = Job::getStepFullPath(system, stepName);
        path = path + ".Status";
        m_prodStepPaths.append(path);
        QString dataRecAddArg = "DataRecordAdd(\"" + path + "\")";
        m_reportConnection->dynamicCall(dataRecAddArg.toLatin1().data());
    }

    // Start All
    device->dynamicCall("StartInspection()");

    return m_reportConnection->dynamicCall("Connected()").toBool();
}

/*!
 * \brief ReportConnection::disconnect
 */
void ReportConnection::disconnectReport() {
    m_reportConnection->dynamicCall("Disconnect()");
}

/*!
 * \brief ReportConnection::AddImageToReport
 * \param parentStep
 */
void ReportConnection::addImageToReport(QAxObject *system)
{
    qDebug() << "- Adding Image to Report";

    QAxObject *step = system->querySubObject("Item([in] VARIANT varIndex)", 1);

    //find the first snapshot step under the Step "parentStep"
    QVariantList params;
    params << "Step.Snapshot";
    params << 0;
    QAxObject *snapshot = step->querySubObject("Find(QString, EnumAvpFindOption, EnumAvpStepCategory)", params);

    //Add the output buffer of the snapshot to list of data records being uploaded by the inspection.
    //  you must specify the symbolic name of the step and the datum to be uploaded.
    //    e.g. "Snapshot1.BufOut", where BufOut is the symbolic name of the snapshot's output buffer
    if(snapshot != NULL) {
        qDebug() << "- Adding Data Record";

        QString dataRecAddArg = "DataRecordAdd(\"" + snapshot->property("NameSym").toString() + ".BufOut\")";

        m_reportConnection->dynamicCall(dataRecAddArg.toLatin1().data());
    }

    if (snapshot != NULL)
        delete snapshot;

    delete step;
}

/*!
 * \brief ReportConnection::onNewReport
 * \param iDisp
 * \param bToFreeze
 */
void ReportConnection::onNewReport(IDispatch* iDisp, bool bToFreeze) {
    Q_UNUSED(bToFreeze);

    if (iDisp == 0) {
        return;
    }

    // IAvpInspReport
    QAxObject *inspection = new QAxObject(iDisp);
    qDebug() << "********" << "ReportConnection::onNewReport";
    QAxObject *stats = inspection->querySubObject("Stats");

    bool overrun = false;
    if (! stats->isNull()) {
        int processOverruns = stats->property("ProcessOverruns").toInt();
        int triggerOverruns = stats->property("TriggerOverruns").toInt();
        int fifoOverruns = stats->property("FifoOverruns").toInt();

        long newOverruns = processOverruns + triggerOverruns + fifoOverruns;

        if (newOverruns > m_overruns ) {
            qDebug() << "!! Overrun";
            m_overruns = newOverruns;
            overrun = true;
        }
    }

    // prepare results
    QJsonArray resultList = {};

    bool result = updateStats(stats, resultList);
    unsigned char resultMask = 0x0;

    // append results for the inspection to statistics
    QAxObject *results = inspection->querySubObject("Results");
    if (results != NULL && ! results->isNull()) {
        resultMask = updateResults(results, resultList);
        delete results;
    }

    emit newResult(result, resultMask);

    QAxObject *images = inspection->querySubObject("Images");

    if (images != NULL && ! images->isNull()) {
        int imageCount = images->property("Count").toInt();

        //hold a reference to the most recent report, so our images won't get destroyed
        if (imageCount > 0 && ! overrun) {
            QAxObject *bufferDm = images->querySubObject("Item([in] VARIANT varIndex)", 1);

            if (! bufferDm->isNull()) {

                int handle = bufferDm->property("Handle").toInt();
                int width = bufferDm->property("BufferWidth").toInt();
                int height = bufferDm->property("BufferHeight").toInt();

                if (handle && width > 0 && height > 0) {
                    QPixmap image = Job::fromBufferDm(bufferDm, true);
                    emit newImage(&image, result);
                    if (!result) {
                        emit newFailedImage(&image, this->m_reportId);
                    }
                }

                // save the image
                if (m_imageSave && ((m_saveOnlyFailed && !result) || ! m_saveOnlyFailed)) {
                    QString timeStamp = QDateTime::currentDateTime().toString("hh-mm-ss-zzz");
                    QString imgFileSave = "SaveImage(\"" + m_imagePath + "/" + timeStamp + "-blstr.tif\", 1)";
                    bufferDm->dynamicCall(imgFileSave.toLatin1().data());
                }

                delete bufferDm;
            }
        }
        delete images;
    }

    QJsonDocument doc(resultList);
    emit newStatistics(doc.toJson());

    delete stats;
    delete inspection;

    return;
}

/*!
 * \brief ReportConnection::updateStats
 * \param stats
 * \param results
 * \return
 */
bool ReportConnection::updateStats(QAxObject *stats, QJsonArray &results)
{
    bool passed = stats->dynamicCall("Passed").toBool();

    int cycleCount = stats->dynamicCall("CycleCount").toInt();
    int cycleTime = stats->dynamicCall("CycleTime").toInt();
    int processTime = stats->dynamicCall("ProcessTime").toInt();
    int processTimeMax = stats->dynamicCall("ProcessTimeMax").toInt();
    int passedCount = stats->dynamicCall("PassedCount").toInt();
    int failedCount = stats->dynamicCall("FailedCount").toInt();
    int processOverruns = stats->dynamicCall("ProcessOverruns").toInt();

    QJsonObject title;
    QJsonObject newValue;

    title    = { {"id", "statsheader" }, {"label", "Statistics"}, {"value", ""}, {"isGroupLabel", true} };
    results.append(title);
    newValue = { {"id", "cyclecount"  }, {"label", "Cycle count"}, {"value", cycleCount}, {"isGroupLabel", false} };
    results.append(newValue);
    newValue = { {"id", "cycletime"   }, {"label", "Cycle time"}, {"value", cycleTime}, {"isGroupLabel", false} };
    results.append(newValue);
    newValue = { {"id", "processtime" }, {"label", "Process time"}, {"value", processTime}, {"isGroupLabel", false} };
    results.append(newValue);
    newValue = { {"id", "maxproctime" }, {"label", "Maximum process time"}, {"value", processTimeMax}, {"isGroupLabel", false} };
    results.append(newValue);
    newValue = { {"id", "passedcount" }, {"label", "Passed count"}, {"value", passedCount}, {"isGroupLabel", false} };
    results.append(newValue);
    newValue = { {"id", "failedcount" }, {"label", "Failed count"}, {"value", failedCount}, {"isGroupLabel", false} };
    results.append(newValue);
    newValue = { {"id", "procoverrun" }, {"label", "Process overruns"}, {"value", processOverruns}, {"isGroupLabel", false} };
    results.append(newValue);

    return passed;
}

/*!
 * \brief ReportConnection::UpdateResults
 * \param results
 * \param values
 */
unsigned char ReportConnection::updateResults(QAxObject *results, QJsonArray &values) {

    int resultCount = results->property("Count").toInt();
    if (resultCount < 1) {
        return 0;
    }

    QJsonObject title;
    QJsonObject newValue;

    title = { {"id", "resultsheader" }, {"label", "Results"}, {"value", ""}, {"isGroupLabel", true} };
    values.append(title);

    unsigned char inspectionResult = 0;

    for(int i = 0; i < resultCount; i++) {
        QAxObject *dataRecord = results->querySubObject("Item([in] VARIANT varIndex)", i+1);

        // find the datarecord in product paths list
        QString symName = dataRecord->property("NameSym").toString();

        int pIdx = m_prodStepPaths.indexOf(symName);
        if ( pIdx >= 0 ) {
            bool bValue = dataRecord->property("Value").toBool();
            if (!bValue) {
                inspectionResult = inspectionResult | (0x01 << pIdx);
            }
        }

        if (dataRecord != NULL && ! dataRecord->isNull()) {
            newValue = {
                {"id", "result" + i},
                {"label", dataRecord->property("NameSym").toString()},
                {"value", dataRecord->dynamicCall("ValueAsString()").toString()},
                {"isGroupLabel", false}
            };
            values.append(newValue);
            delete dataRecord;
        }
    }

    return inspectionResult;
}
