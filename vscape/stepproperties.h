#ifndef STEPPROPERTIES_H
#define STEPPROPERTIES_H

#include <QObject>
#include <QMap>
#include <QRect>
#include <QPoint>
#include <QAxObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <vscape/datumproperties.h>

// StepTypes Map ---------------------------------------------------------------------------
#include <map>

typedef struct StepType {
    QString type;
    bool relativeToParent;
    bool useInputImage;
    bool useOutputImage;
    bool showGraphics;
    bool tryChildren;
    bool trainable;
    bool editRoi;
    bool showParent;
    bool editGroup;
    bool allowDelete;
    bool childOnly;
    bool autoTrainParent;
} StepType;

typedef struct StepProps {
    QString symName;
    QString name;
    QString type;
    QString path;
    QString parentPath;
    int     level;
    bool    relativeToParent;
    bool    useInputImage;
    bool    useOutputImage;
    bool    showGraphics;
    bool    tryChildren;
    bool    trainable;
    bool    trained;
    bool    editRoi;
    bool    showParent;
    bool    editGroup;
    bool    allowDelete;
    bool    childOnly;
    bool    autoTrainParent;
    int     roiValue[4];
    bool    rotatable;
    double  angle;
    int     parentX;
    int     parentY;
    QList<StepDatum> datums;
} StepProps;

static QMap<QString, StepType> stepTypes = {
    //                                                    rel    input  output graph  try ch train  roi    parent group  delete child  train-parent
    //                                                    -----  -----  -----  -----  ------ -----  -----  ------ -----  ------ ------ ----------
    {"Step.1PinFind.1",    StepType{ "Step.1PinFind.1",   false, false, false, false, false, true,  false, false, false, false, true,  false } }, // one point locator tool - train but no need to edit ROI
    {"Step.2PinFind.1",    StepType{ "Step.2PinFind.1",   false, false, false, false, false, true,  false, false, false, false, false, false } }, // two point locator tool - train but no need to edit ROI
    {"Step.Find.1",        StepType{ "Step.Find.1",       false, false, false, false, false, false, true,  false, true,  false, false, true  } }, // search region for find-template - do not need training
    {"Step.FindSetup.1",   StepType{ "Step.FindSetup.1",  false, false, false, false, false, false, true,  true,  false, false, true,  false } }, // find template - show with parent
    {"Step.FlawTool.1",    StepType{ "Step.FlawTool.1",   true,  true,  false, false, false, false, true,  false, false, true,  false, false } }, // child of the RectWarp - edit in group with siblings
    {"Step.RectWarp.1",    StepType{ "Step.RectWarp.1",   false, false, false, false, false, false, true,  false, true,  false, false, false } }, // crop a region of the image - edit with children ROIs
    {"Step.GainOffset.1",  StepType{ "Step.GainOffset.1", false, false, false, false, false, false, false, false, false, false, false, false } }, // just hide this step
    {"Step.BlobFilter.1",  StepType{ "Step.BlobFilter.1", false, false, false, false, false, false, false, false, false, false, false, false } }, // just show
    {"Step.ThreshColor.1", StepType{ "Step.ThresColor.1", false, false, false, true,  true,  false, true,  false, false, false, false, false } }, // show output buffer
};

// -----------------------------------------------------------------------------------------

class StepProperties : public QObject
{
    Q_OBJECT
public:
    explicit StepProperties(QObject *parent = 0);

    ~StepProperties();

    static QJsonObject asJson(StepProps stepProps);
    static StepProps fromStepObject(QAxObject *step, StepProps parentProps);
    static StepProps fromJsonObject(QJsonObject stepJsonObject);
    static StepProps fromJson(QString stepJson);
    static QList<StepProps> listFromJson(QString stepsJson);

signals:

private slots:
    void onStepException(int, const QString &, const QString &, const QString &);

public slots:

private:
};

#endif // STEPPROPERTIES_H
