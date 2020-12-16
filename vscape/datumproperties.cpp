#include "datumproperties.h"
#include <QDebug>

QMap<QString, QList<StepDatum>> g_stepDatums = {
       // TYPE                           LABEL                           TYPE       SCALE MIN         MAX VALUE READONLY  CHILD CHILDTYPE
    { "Step.FlawTool.1", {
        {"Thr",                          "Edge Threshold",               integerType,   1,  3,        255,   0,   false, false, ""},
        {"Val:MinEdgeCnt:MaxEdgeCnt",    "Edge Tolerance %",             tolPerc,       1,  3,        255,  25,   false, false, ""},
        {"MinEdgeCnt",                   "Min Edge Count",               integerType,   1,  0,    1000000,   0,   true,  false, ""},
        {"MaxEdgeCnt",                   "Max Edge Count",               integerType,   1,  0,    1000000,   0,   true,  false, ""},
        {"Val",                          "Edge Count",                   integerType,   1,  0,    1000000,   0,   true,  false, ""}
    }},
    { "Step.RectWarp.1", {
        {"Thr",                          "Child Edge Threshold",         integerType,   1,  3,        255,   0,   false, true,  "Step.FlawTool.1"},
        {"Val:MinEdgeCnt:MaxEdgeCnt",    "Child Edge Tolerance %",       tolPerc,       1,  3,        255,  25,   false, true,  "Step.FlawTool.1"}
    }},
    { "Step.ThreshColor.1", {
        {"Hue Lower Threshold",          "Hue Lower Threshold",          integerType,   1,  0,        255,   0,   false, false, ""},
        {"Hue Upper Threshold",          "Hue Upper Threshold",          integerType,   1,  0,        255,   0,   false, false, ""},
        {"Invert Thresholded Hue",       "Invert Thresholded Hue",       booleanType,   1,  0,          1,   0,   false, false, ""},
        {"Saturation Lower Threshold",   "Saturation Lower Threshold",   integerType,   1,  0,        255,   0,   false, false, ""},
        {"Saturation Upper Threshold",   "Saturation Upper Threshold",   integerType,   1,  0,        255,   0,   false, false, ""},
        {"Invert Thresholded Saturation","Invert Thresholded Saturation",booleanType,   1,  0,          1,   0,   false, false, ""},
        {"Intensity Lower Threshold",    "Intensity Lower Threshold",    integerType,   1,  0,        255,   0,   false, false, ""},
        {"Intensity Upper Threshold",    "Intensity Upper Threshold",    integerType,   1,  0,        255,   0,   false, false, ""},
        {"Invert Thresholded Intensity", "Invert Thresholded Intensity", booleanType,   1,  0,          1,   0,   false, false, ""},
        {"InvertFinal",                  "Invert Output Image",          booleanType,   1,  0,          1,   0,   false, false, ""}
    }},
    { "Step.Blob.1", {
        {"MinBlob",                      "Minimum Blob Size",            integerType,   1,  0, 2000000000,   0,   false, false, ""},
        {"MaxBlob",                      "Maximum Blob Size",            integerType,   1,  0, 2000000000,   0,   false, false, ""},
        {"FilterMinParts",               "Apply MinBlob to parts",       booleanType,   1,  0,          1,   0,   false, false, ""},
        {"FilterMaxParts",               "Apply MaxBlob to parts",       booleanType,   1,  0,          1,   0,   false, false, ""},
        {"FilterArea",                   "Filter Blobs by Area",         booleanType,   1,  0,          1,   0,   false, false, ""},
        {"FilterTouchROI",               "Filter Blobs that touch ROI",  booleanType,   1,  0,          1,   0,   false, false, ""},
        {"DiscardChildren",              "Discard Children Blobs",       booleanType,   1,  0,          1,   0,   false, false, ""},
        {"MinNumBlobs",                  "Minimum Number of Blobs",      integerType,   1,  0, 2000000000,   0,   false, false, ""},
        {"MinNumBlobs",                  "Maximum Number of Blobs",      integerType,   1,  0, 2000000000,   0,   false, false, ""}
    }},
    { "Step.BlobFilter.1", {
        {"Area",                         "Area",                         integerType,   1,  0, 2000000000,   0,   true,  false, ""},
        {"MinFeat",                      "Minimum of Feature",           integerType,   1,  0, 2000000000,   0,   false, false, ""},
        {"MaxFeat",                      "Maximum of Feature",           integerType,   1,  0, 2000000000,   0,   false, false, ""}
    }}
};

/*!
 * \brief DatumProperties::DatumProperties
 * \param typesMap
 * \param parent
 */
DatumProperties::DatumProperties(QMap<QString, DatumType>*typesMap, QObject *parent) : QObject(parent)
{
    Q_UNUSED(typesMap);
}

/*!
 * \brief DatumProperties::~DatumProperties
 */
DatumProperties::~DatumProperties() {

}

/*!
 * \brief DatumProperties::getProperties
 * \return
 */
DatumType DatumProperties::getProperties() {
    return m_datumType;
}

/*!
 * \brief DatumProperties::getIntValue
 * \return
 */
int DatumProperties::getIntValue() {
    return m_intValue;
}

/*!
 * \brief DatumProperties::getRoiValue
 * \return
 */
QRect DatumProperties::getRoiValue() {
    return m_roiValue;
}

/*!
 * \brief DatumProperties::getPointValue
 * \return
 */
QPoint DatumProperties::getPointValue() {
    return m_pointValue;
}

/*!
 * \brief DatumProperties::asJson
 * \param datumType
 * \return
 */
QJsonObject DatumProperties::asJson(DatumType datumType) {

    QJsonObject props = {
        { "name",               datumType.name },
        { "label",              datumType.label },
        { "type",               datumType.type },
        { "scale",              datumType.scale },
        { "minValue",           datumType.minValue },
        { "maxValue",           datumType.maxValue },
        { "showGraphics",       datumType.showGraphics }
    };

    QJsonArray arrayValue;

    switch (datumType.type) {
        case doubleType:
            props.insert("value", QJsonValue( datumType.doubleValue * datumType.scale ) );
            break;
        case integerType:
            props.insert("value", QJsonValue( datumType.intValue * datumType.scale ) );
            break;
        case booleanType:
            props.insert("value", QJsonValue( datumType.booleanValue ) );
            break;
        case roiType:
            arrayValue = {
                datumType.roiValue.x(),
                datumType.roiValue.y(),
                datumType.roiValue.width(),
                datumType.roiValue.height()
            };
            props.insert("value", arrayValue );
            break;
        case pointType:
            arrayValue = {
                datumType.pointValue.x(),
                datumType.pointValue.y()
            };
        props.insert("value", arrayValue);
            break;
        default:
            break;
    }

    return props;
}

/*!
 * \brief DatumProperties::asJson
 * \param stepDatum
 * \return
 */
QJsonObject DatumProperties::asJson(StepDatum stepDatum) {

    QJsonObject props = {
        { "symName",        stepDatum.symName },
        { "label",          stepDatum.label },
        { "type",           stepDatum.type },
        { "scale",          stepDatum.scale },
        { "minValue",       stepDatum.minValue },
        { "maxValue",       stepDatum.maxValue },
        { "value",          stepDatum.value },
        { "readOnly",       stepDatum.readOnly },
        { "childDatum",     stepDatum.childDatum },
        { "childType",      stepDatum.childType }
    };

    return props;
}

/*!
 * \brief DatumProperties::fromJsonObject
 * \param datumJsonObject
 * \return
 */
DatumType DatumProperties::fromJsonObject(QJsonObject datumJsonObject) {
    QJsonArray values;

    DatumType datum;
    datum.name = datumJsonObject["name"].toString();
    datum.label = datumJsonObject["label"].toString();
    datum.type = (DatumValueType)datumJsonObject["type"].toInt();
    datum.scale = datumJsonObject["scale"].toInt();
    datum.minValue = datumJsonObject["minValue"].toInt();
    datum.maxValue = datumJsonObject["maxValue"].toInt();
    datum.showGraphics = datumJsonObject["showGraphics"].toBool();

    switch (datum.type) {
    case doubleType:
        datum.doubleValue = datumJsonObject["value"].toDouble();
        break;
    case integerType:
        datum.doubleValue = datumJsonObject["value"].toInt();
        break;
    case booleanType:
        datum.booleanValue = datumJsonObject["value"].toBool();
        break;
    case pointType:
        values = datumJsonObject["value"].toArray();
        if (values.size() >= 2) {
            datum.pointValue = QPoint(values[0].toInt(), values[1].toInt());
        }
        break;
    case roiType:
        values = datumJsonObject["value"].toArray();
        if (values.size() >= 4) {
            datum.roiValue = QRect(
                    values[0].toInt(),
                    values[1].toInt(),
                    values[2].toInt(),
                    values[3].toInt()
            );
        }
        break;
    default:
        break;
    }

    return datum;
}

/*!
 * \brief DatumProperties::datumFromJsonObject
 * \param datumJsonObject
 * \return
 */
StepDatum DatumProperties::datumFromJsonObject(QJsonObject datumJsonObject) {

    StepDatum datum;
    datum.symName    = datumJsonObject["symName"].toString();
    datum.label      = datumJsonObject["label"].toString();
    datum.type       = (DatumValueType)datumJsonObject["type"].toInt();
    datum.scale      = datumJsonObject["scale"].toInt();
    datum.minValue   = datumJsonObject["minValue"].toInt();
    datum.maxValue   = datumJsonObject["maxValue"].toInt();
    datum.readOnly   = datumJsonObject["readOnly"].toBool();
    datum.childDatum = datumJsonObject["childDatum"].toBool();
    datum.childType  = datumJsonObject["childType"].toString();

    switch(datum.type) {
    case booleanType:
        datum.value = datumJsonObject["value"].toBool() ? 1 : 0;
        break;
    default:
        datum.value = datumJsonObject["value"].toInt();
    }

    return datum;
}

/*!
 * \brief DatumProperties::datumFromJson
 * \param datumJsonObject
 * \return
 */
StepDatum DatumProperties::datumFromJson(QString datumJson) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(datumJson.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();

    return datumFromJsonObject(jsonObject);
}

/*!
 * \brief DatumProperties::onStepException
 * \param source
 * \param desc
 * \param help
 */
void DatumProperties::onStepException(int, const QString &source, const QString &desc, const QString &help) {
    Q_UNUSED(source)
    Q_UNUSED(desc)
    Q_UNUSED(help)
}

/*!
 * \brief DatumProperties::fromStep
 * \param symName
 * \return
 */
void DatumProperties::fillFromStepObject(QAxObject *step, StepDatum &stepDatum) {

    QAxObject *targetStep = NULL;

    if (stepDatum.type == tolPerc) {
        return;
    }

    // if we reference a child step datum, get the child step
    if (stepDatum.childDatum) {
        targetStep = step->querySubObject(QString("Find(\"" + stepDatum.childType + "\", 0)").toLatin1().data());
    }
    else {
        targetStep = step;
    }

    if (targetStep != NULL && ! targetStep->isNull()) {


//        qDebug() << "getting datums from " << targetStep->property("NameSym").toString() << " - datums count: " << targetStep->querySubObject("Datums")->property("Count").toInt();

//        int count = targetStep->querySubObject("Datums")->property("Count").toInt();
//        QAxObject* datums = targetStep->querySubObject("Datums");
//        for (int i = 0; i < count; i ++) {
//            qDebug() << datums->querySubObject("Item([in] VARIANT varIndex)", i + 1)->property("NameSym").toString();
//        }


        QString cmd = "Datum(\"" + stepDatum.symName + "\")";
        QAxObject *datum = targetStep->querySubObject(cmd.toLatin1().data());

        if (datum != NULL && ! datum->isNull()) {
            stepDatum.value = datum->property("Value").toInt();
            delete datum;

//            qDebug() << "Datum value" << stepDatum.symName << stepDatum.value;
        }

        if (stepDatum.childDatum) {
            delete targetStep;
        }
    }
}
