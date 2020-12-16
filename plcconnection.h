#ifndef PLCCONNECTION_H
#define PLCCONNECTION_H

#include <QObject>
#include <QtNetwork>
#include <QSettings>

#include <systemstatus.h>
#include <systemconfiguration.h>
#include <batchconfiguration.h>

class PLCConnection: public QObject
{
    Q_OBJECT

signals:
    void plcConnected();
    void plcDisconnected();
    void plcStatus(SystemStatus *status);

    void started();
    void stopped();
    void newSampleTrigger(int seq);
    void viewMessage(QString msg);

public:
    explicit PLCConnection(QSettings *settings, QObject *parent = 0);

    void resetCounters(int);
    bool isConnected();

    void start();
    void stop();
    void startTest(int test);
    void stopTest(int test);
    void setManualMode(bool manualMode);
    void setContinueOnError(bool mode);
    void setStrobeAlwaysOn(bool mode);
    void getPLCStatus();
    void sendSystemConfiguration(SystemConfiguration*);
    void saveSystemConfiguration(SystemConfiguration*);
    SystemConfiguration *getSystemConfiguration();
    void resetErrors();
    void sendBatchConfiguration(BatchConfiguration *batchConf);

    int getCyclePulses();
    int getTriggerOffset();

public slots:
    void plcConnect();
    void onNewResult(bool, unsigned char);

private slots:
    void connected();
    void disconnected();
    void connectionError(QAbstractSocket::SocketError);
    void newData();

private:
    QSettings *m_settings;

    QString m_serverAddress;
    int m_serverPort;
    int m_pollingPeriod;
    int m_cyclePulses;
    int m_triggerOffset;

    quint8 m_sampleSeq;
    bool m_running;
    QTcpSocket *m_plcSocket;
    bool m_connected;
    SystemStatus *m_systemStatus;

    // System configuration variables
    // -------------------------------------------------------------
    SystemConfiguration *m_systemConfiguration;
    uint m_noAnswerThreshold;
    uint m_failedThreshold;
    bool m_continueOnError;

    bool m_systemConfigAppliedToPLC;
    // -------------------------------------------------------------

    void updateLocalSystemConfiguration(SystemConfiguration *);
    void sendAnswer(unsigned char seq);
    void sendAck(unsigned char seq);
    void sendSimpleCommand(unsigned char command);
    void sendCommand(unsigned char command, unsigned char arg);

    void fillInSystemStatus(QDataStream &in, SystemStatus &status);
    void readPermanentCounters(QDataStream &in, SystemStatus &status);
    void readGlobalCounters(QDataStream &in, SystemStatus &status);
    void readBatchCounters(QDataStream &in, SystemStatus &status);
    void readDailyCounters(QDataStream &in, SystemStatus &status);
};

#endif // PLCCONNECTION_H
