#ifndef BATCHCONFIGURATION_H
#define BATCHCONFIGURATION_H

#include <QObject>

class BatchConfiguration : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name                 READ getName                WRITE setName                NOTIFY nameChanged)
    Q_PROPERTY(QString avpFileName          READ getAvpFileName         WRITE setAvpFileName         NOTIFY avpFileNameChanged)
//    Q_PROPERTY(quint16 rejectValveCycles    READ getRejectValveCycles   WRITE setRejectValveCycles   NOTIFY rejectValveCyclesChanged)
//    Q_PROPERTY(quint32 rejectValveOn        READ getRejectValveOn       WRITE setRejectValveOn       NOTIFY rejectValveOnChanged)
//    Q_PROPERTY(quint32 rejectValveDuration  READ getRejectValveDuration WRITE setRejectValveDuration NOTIFY rejectValveDurationChanged)
    Q_PROPERTY(quint16 noOfProducts         READ getNoOfProducts        WRITE setNoOfProducts        NOTIFY noOfProductsChanged)

    Q_PROPERTY(quint16 cameraToReject           READ getCameraToReject          WRITE setCameraToReject         NOTIFY cameraToRejectChanged)
    Q_PROPERTY(quint16 trigger1Offset           READ getTrigger1Offset          WRITE setTrigger1Offset         NOTIFY trigger1OffsetChanged)
    Q_PROPERTY(quint16 trigger2Offset           READ getTrigger2Offset          WRITE setTrigger2Offset         NOTIFY trigger2OffsetChanged)
    Q_PROPERTY(quint16 reject1Offset            READ getReject1Offset           WRITE setReject1Offset          NOTIFY reject1OffsetChanged)
    Q_PROPERTY(quint16 rejectDistance           READ getRejectDistance          WRITE setRejectDistance         NOTIFY rejectDistanceChanged)
    Q_PROPERTY(quint16 rejectDuration           READ getRejectDuration          WRITE setRejectDuration         NOTIFY rejectDurationChanged)


public:
    explicit BatchConfiguration(QObject *parent = 0);
    BatchConfiguration(BatchConfiguration *config, QObject *parent);

    QString getName()                { return name; }
    QString getAvpFileName()         { return avpFileName; }
//    quint16 getRejectValveCycles()   { return rejectValveCycles; }
//    quint32 getRejectValveOn()       { return rejectValveOn; }
//    quint32 getRejectValveDuration() { return rejectValveDuration; }
    quint16 getNoOfProducts()        { return 4;  } // noOfProducts;

    quint16 getCameraToReject()         { return cameraToReject; }
    quint16 getTrigger1Offset()         { return trigger1Offset; }
    quint16 getTrigger2Offset()         { return trigger2Offset; }
    quint16 getReject1Offset()          { return reject1Offset; }
    quint16 getRejectDistance()         { return rejectDistance; }
    quint16 getRejectDuration()         { return rejectDuration; }

    void setName(QString value);
    void setAvpFileName(QString value);
//    void setRejectValveCycles(quint16 value);
//    void setRejectValveOn(quint32 value);
//    void setRejectValveDuration(quint32 value);
    void setNoOfProducts(quint16 value);

    void setCameraToReject(quint16 value);
    void setTrigger1Offset(quint16 value);
    void setTrigger2Offset(quint16 value);
    void setReject1Offset(quint16 value);
    void setRejectDistance(quint16 value);
    void setRejectDuration(quint16 value);

private:
    QString  name;
    QString  avpFileName;

//    quint16  rejectValveCycles;
//    quint32  rejectValveOn;
//    quint32  rejectValveDuration;
    quint16  noOfProducts;

    quint16 cameraToReject;
    quint16 trigger1Offset;
    quint16 trigger2Offset;
    quint16 reject1Offset;
    quint16 rejectDistance;
    quint16 rejectDuration;


signals:
    void nameChanged();
    void avpFileNameChanged();

//    void rejectValveCyclesChanged();
//    void rejectValveOnChanged();
//    void rejectValveDurationChanged();
    void noOfProductsChanged();

    void cameraToRejectChanged();
    void trigger1OffsetChanged();
    void trigger2OffsetChanged();
    void reject1OffsetChanged();
    void rejectDistanceChanged();
    void rejectDurationChanged();

public slots:

};

#endif // BATCHCONFIGURATION_H
