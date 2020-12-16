#ifndef SYSTEMCONF_H
#define SYSTEMCONF_H

#include <qglobal.h>

#include <QObject>

class SystemConfiguration : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint16 noAnswerThreshold        READ getNoAnswerThreshold       WRITE setNoAnswerThreshold      NOTIFY noAnswerThresholdChanged)
    Q_PROPERTY(quint16 failedThreshold          READ getFailedThreshold         WRITE setFailedThreshold        NOTIFY failedThresholdChanged)

    Q_PROPERTY(bool    continueOnError          READ getContinueOnError         WRITE setContinueOnError        NOTIFY continueOnErrorChanged)

public:
    explicit SystemConfiguration(QObject *parent = 0);

    quint16 getNoAnswerThreshold()      { return noAnswerThreshold; }
    quint16 getFailedThreshold()        { return failedThreshold; }

    bool getContinueOnError()           { return continueOnError; }

    void setNoAnswerThreshold(quint16 value);
    void setFailedThreshold(quint16 value);

    void setContinueOnError(bool value);

private:
    quint16 noAnswerThreshold;
    quint16 failedThreshold;

    bool continueOnError;

signals:
    void noAnswerThresholdChanged();
    void failedThresholdChanged();

    void continueOnErrorChanged();

public slots:

};

#endif // SYSTEMCONF_H
