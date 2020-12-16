#include "stepproperties.h"

/*!
 * \brief StepProperties::StepProperties
 * \param parent
 */
StepProperties::StepProperties(QObject *parent) : QObject(parent) {
}

/*!
 * \brief StepProperties::onStepException
 * \param source
 * \param desc
 * \param help
 */
void StepProperties::onStepException(int, const QString &source, const QString &desc, const QString &help) {
    Q_UNUSED(source)
    Q_UNUSED(desc)
    Q_UNUSED(help)

    qDebug() << "Step exception while getting datum: " << source;
}

/**
 * @brief StepProperties::~StepProperties
 */
StepProperties::~StepProperties() {
}

/*!
 * \brief StepProperties::asJson
 * \param stepProps
 * \return
 */
QJsonObject StepProperties::asJson(StepProps stepProps) {

    QJsonObject props = {
        { "symName",            stepProps.symName },
        { "name",               stepProps.name },
        { "type",               stepProps.type },
        { "path",               stepProps.path },
        { "parentPath",         stepProps.parentPath },
        { "level",              stepProps.level },
        { "relativeToParent",   stepProps.relativeToParent },
        { "useInputImage",      stepProps.useInputImage },
        { "useOutputImage",     stepProps.useOutputImage },
        { "showGraphics",       stepProps.showGraphics },
        { "tryChildren",        stepProps.tryChildren },
        { "trainable",          stepProps.trainable },
        { "trained",            stepProps.trained },
        { "editRoi",            stepProps.editRoi },
        { "showParent",         stepProps.showParent },
        { "editGroup",          stepProps.editGroup },
        { "allowDelete",        stepProps.allowDelete },
        { "childOnly",          stepProps.childOnly },
        { "autoTrainParent",    stepProps.autoTrainParent },
        { "parentX",            stepProps.parentX },
        { "parentY",            stepProps.parentY }
    };

    if (stepProps.editRoi) {
        props.insert("roiValue", QJsonValue({
            stepProps.roiValue[0],
            stepProps.roiValue[1],
            stepProps.roiValue[2],
            stepProps.roiValue[3]}
        ));
    }

    // convert the datums list to json array -----------------------------------------
    QJsonArray datumsJson;
    for (int i = 0; i < stepProps.datums.size(); i++) {
        QJsonObject datum = DatumProperties::asJson(stepProps.datums[i]);
        datumsJson.append(datum);
    }
    props.insert("datums", datumsJson);

    return props;
}

/*!
 * \brief StepProperties::fromJson
 * \param stepJson
 * \return
 */
StepProps StepProperties::fromJsonObject(QJsonObject jsonObject) {

    StepProps stepProps;

    stepProps.symName           = jsonObject["symName"].toString();
    stepProps.name              = jsonObject["name"].toString();
    stepProps.type              = jsonObject["type"].toString();
    stepProps.path              = jsonObject["path"].toString();
    stepProps.parentPath        = jsonObject["parentPath"].toString();
    stepProps.level             = jsonObject["level"].toInt();
    stepProps.relativeToParent  = jsonObject["relativeToParent"].toBool();
    stepProps.useInputImage     = jsonObject["useInputImage"].toBool();
    stepProps.useOutputImage    = jsonObject["useOutputImage"].toBool();
    stepProps.showGraphics      = jsonObject["showGraphics"].toBool();
    stepProps.tryChildren       = jsonObject["tryChildren"].toBool();
    stepProps.trainable         = jsonObject["trainable"].toBool();
    stepProps.editRoi           = jsonObject["editRoi"].toBool();
    stepProps.showParent        = jsonObject["showParent"].toBool();
    stepProps.editGroup         = jsonObject["editGroup"].toBool();
    stepProps.allowDelete       = jsonObject["allowDelete"].toBool();
    stepProps.childOnly         = jsonObject["childOnly"].toBool();
    stepProps.autoTrainParent   = jsonObject["autoTrainParent"].toBool();
    stepProps.parentX           = jsonObject["parentX"].toInt();
    stepProps.parentY           = jsonObject["parentY"].toInt();

    if (stepProps.editRoi) {
        QJsonArray jsonRoi = jsonObject["roiValue"].toArray();
        for(int i = 0; i < jsonRoi.size() && i < 4; i++) {
            stepProps.roiValue[i] = jsonRoi[i].toDouble();
        }
    }

    // convert datums json array to step datums list -------------------------------------
    QJsonArray datumsJson = jsonObject["datums"].toArray();

    for (int i = 0; i < datumsJson.size(); i++) {

        QJsonObject datumJson = datumsJson[i].toObject();
        StepDatum stepDatum = DatumProperties::datumFromJsonObject(datumJson);
        stepProps.datums.append(stepDatum);
    }

    return stepProps;
}

/*!
 * \brief StepProperties::fromJson
 * \param stepJson
 * \return
 */
StepProps StepProperties::fromJson(QString stepJson) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(stepJson.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();

    return fromJsonObject(jsonObject);
}

/*!
 * \brief StepProperties::listFromJson
 * \param stepsJson
 * \return
 */
QList<StepProps> StepProperties::listFromJson(QString stepsJson) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(stepsJson.toUtf8());
    QJsonArray jsonArray = jsonDoc.array();

    QList<StepProps> stepProps;

    for (int i = 0; i < jsonArray.size(); i++) {
        stepProps.append(StepProperties::fromJsonObject(jsonArray[i].toObject()));
    }

    return stepProps;
}

/*!
 * \brief StepProperties::fromStepObject
 * \param step
 * \param parentProps
 * \return
 */
StepProps StepProperties::fromStepObject(QAxObject *step, StepProps parentProps) {

    StepProps stepProps;

    stepProps.symName    = step->property("NameSym").toString();
    stepProps.name       = step->property("Name").toString();
    stepProps.type       = step->property("Type").toString();
    stepProps.trainable  = step->property("Trainable").toBool();
    stepProps.trained    = step->property("Trained").toBool();
    stepProps.path       = "";
    stepProps.parentPath = parentProps.path;
    stepProps.level      = parentProps.level + 1;

    if (parentProps.level > 0) {
        stepProps.path = (parentProps.level > 1 ? parentProps.path + "." : "") + stepProps.symName;
    }

    // set default values for extra properties
    stepProps.relativeToParent  = false;
    stepProps.useInputImage     = false;
    stepProps.useOutputImage    = false;
    stepProps.showGraphics      = false;
    stepProps.tryChildren       = false;
    stepProps.editRoi           = false;
    stepProps.rotatable         = false;
    stepProps.showParent        = false;
    stepProps.editGroup         = false;
    stepProps.allowDelete       = false;
    stepProps.childOnly         = false;
    stepProps.autoTrainParent   = false;

    StepProperties *exceptionDrain = new StepProperties();

    connect(step,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            exceptionDrain,
            SLOT(onStepException(int, const QString &, const QString &, const QString &))
            );

    // check the existence of ROI datum
    QAxObject *roi = step->querySubObject("Datum(\"ROI\")");
    if (roi != NULL && ! roi->isNull()) {

        QList<QVariant> roiVar = roi->property("Value").toList();
        if (roiVar.length() > 0) {
            stepProps.editRoi = true;
        }

        if (roiVar.length() >= 4) {
            stepProps.roiValue[2] = roiVar.at(2).toDouble();    // width
            stepProps.roiValue[3] = roiVar.at(3).toDouble();    // height
            stepProps.roiValue[0] = round(roiVar.at(0).toDouble() - roiVar.at(2).toDouble()/2.0);    // x
            stepProps.roiValue[1] = round(roiVar.at(1).toDouble() - roiVar.at(3).toDouble()/2.0);    // y
        }

        if (roiVar.length() > 4) {
            stepProps.rotatable = true;
            stepProps.angle = roiVar.at(4).toDouble();
        }

        delete roi;
    }

    // overwrite with Step Type Definitions
    if (stepTypes.contains(stepProps.type))
    {
        StepType stepType = stepTypes[stepProps.type];

        stepProps.trainable         = stepType.trainable;
        stepProps.relativeToParent  = stepType.relativeToParent;
        stepProps.useInputImage     = stepType.useInputImage;
        stepProps.useOutputImage    = stepType.useOutputImage;
        stepProps.showGraphics      = stepType.showGraphics;
        stepProps.tryChildren       = stepType.tryChildren;
        stepProps.editRoi           = stepType.editRoi;
        stepProps.showParent        = stepType.showParent;
        stepProps.editGroup         = stepType.editGroup;
        stepProps.allowDelete       = stepType.allowDelete;
        stepProps.childOnly         = stepType.childOnly;
        stepProps.autoTrainParent   = stepType.autoTrainParent;

        if (stepProps.relativeToParent == true) {
            stepProps.parentX = parentProps.roiValue[0];
            stepProps.parentY = parentProps.roiValue[1];
        }
    }

    // get the datum values -----------------------------------------------------------------
    // get the list of datum definitions for the specific step type
    if (g_stepDatums.contains(stepProps.type)) {

        QList<StepDatum> stepDatumsSpec = g_stepDatums[stepProps.type];

        QList<StepDatum> stepDatums;
        for (int i = 0; i < stepDatumsSpec.size(); ++i) {

            // fill the stepDatum structure from the step object
            StepDatum stepDatum = stepDatumsSpec[i];
            DatumProperties::fillFromStepObject(step, stepDatum);

            stepDatums.append(stepDatum);
        }
        stepProps.datums = stepDatums;
    }

    disconnect(step,
            SIGNAL(exception(int, const QString &, const QString &, const QString &)),
            exceptionDrain,
            SLOT(onStepException(int, const QString &, const QString &, const QString &))
            );

    delete exceptionDrain;

    return stepProps;
}
