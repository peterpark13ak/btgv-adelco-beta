#ifndef DATUMPROPERTIES_H
#define DATUMPROPERTIES_H

#include <QObject>
#include <QMap>
#include <QRect>
#include <QPoint>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAxObject>

enum DatumValueType {
    doubleType = 0,
    integerType = 1,
    pointType = 2,
    roiType = 3,
    tolPerc = 4,             // tolerance percentage % up/down of a reference (typical value)
    booleanType = 5
};

typedef struct DatumType {
    QString         name;
    QString         label;
    DatumValueType  type;
    int             scale;
    int             minValue;
    int             maxValue;
    bool            showGraphics;
    int             intValue;
    double          doubleValue;
    QRect           roiValue;
    QPoint          pointValue;
    bool            booleanValue;
} DatumType;

typedef struct StepDatum {
    QString         symName;
    QString         label;
    DatumValueType  type;
    int             scale;
    int             minValue;
    int             maxValue;
    int             value;
    bool            readOnly;
    bool            childDatum;
    QString         childType;
} DatumUse;

//
// The following map holds a list of datum specifications (datum types) for specific step types
//
extern QMap<QString, QList<StepDatum>> g_stepDatums;

class DatumProperties : public QObject
{
    Q_OBJECT
public:
    explicit DatumProperties(QMap<QString, DatumType>*, QObject *parent = 0);
    ~DatumProperties();

    DatumType getProperties();
    int getIntValue();
    QRect getRoiValue();
    QPoint getPointValue();

    static QJsonObject asJson(DatumType datumType);
    static QJsonObject asJson(StepDatum stepDatum);
    static DatumType fromJsonObject(QJsonObject datumJsonObject);
    static StepDatum datumFromJson(QString datumJsonObject);
    static StepDatum datumFromJsonObject(QJsonObject datumJsonObject);
    static void fillFromStepObject(QAxObject*, StepDatum&);

private slots:
    void onStepException(int, const QString &, const QString &, const QString &);

private:
    DatumType   m_datumType;
    int         m_intValue;
    QRect       m_roiValue;
    QPoint      m_pointValue;
};

#endif // DATUMPROPERTIES_H
