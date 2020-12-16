#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QObject>
#include <plcconnection.h>
#include <configurationmanager.h>
#include <trainimageprovider.h>
#include <vscape/jobmanager.h>

class UIController : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(SystemConfiguration *systemConfiguration READ getSystemConfiguration)
    Q_INVOKABLE void requestPLCStatus();
    Q_INVOKABLE void requestPLCStart();
    Q_INVOKABLE void requestPLCStop();
    Q_INVOKABLE void resetCounters(int);

    Q_INVOKABLE void startTest(int);
    Q_INVOKABLE void stopTest(int);
    Q_INVOKABLE void setManualMode(bool);
    Q_INVOKABLE void resetErrors();

    Q_INVOKABLE void saveSystemConfiguration(SystemConfiguration *);

    Q_INVOKABLE QStringList getConfigurationNames();
    Q_INVOKABLE QStringList getReadOnlyConfigurations();
    Q_INVOKABLE QString getActiveConfigurationId();
    Q_INVOKABLE BatchConfiguration *getConfigurationById(QString);
    Q_INVOKABLE void useBatchConfiguration(BatchConfiguration*, bool loadAvp = false);
    Q_INVOKABLE void updateConfiguration(BatchConfiguration*);
    Q_INVOKABLE void saveConfiguration(BatchConfiguration*);
    Q_INVOKABLE bool configurationNameExists(QString);
    Q_INVOKABLE void copyAvp(QString, QString);
    Q_INVOKABLE void deleteConfiguration(QString);

    Q_INVOKABLE bool isJobReady();
    Q_INVOKABLE void loadAvp(QString code, bool editMode = true);
    Q_INVOKABLE void saveAvp(QString code);
    Q_INVOKABLE void setAvpNumericParam(int inspectionId, QString code, int value);
    Q_INVOKABLE QString getGlobalDatums(int inspectionId);
    Q_INVOKABLE QString getConfigurableSteps(bool withUpdate = false);
    Q_INVOKABLE QString getStep(QString path);
    Q_INVOKABLE void setDatumValue(QString stepPath, QString datumJson);
    Q_INVOKABLE void setEditMode();
    Q_INVOKABLE void setRunMode();

    Q_INVOKABLE void acquireImage();
    Q_INVOKABLE void acquireStepImage(QString symName, bool useOutput = false, bool showGraphics = false);    // TODO: check compliance with step path - change uses
    Q_INVOKABLE void acquireRootImage(bool showGraphics = false);
    Q_INVOKABLE QString getImageHSI(int x, int y);
    Q_INVOKABLE void cancelAcquireImage();
    Q_INVOKABLE bool acquireStatus();
    Q_INVOKABLE void updateROI(QString stepModel);
    Q_INVOKABLE void updateROIs(QString stepsModel);
    Q_INVOKABLE void trainAll();
    Q_INVOKABLE void train(QString stepPath);
    Q_INVOKABLE void trainParent(QString stepPath);
    Q_INVOKABLE void tryout(QString stepPath, QString requestor);
    Q_INVOKABLE void tryOutSteps(QString startStepModel, QString requestor);
    Q_INVOKABLE void deleteStep(QString stepPath);
    Q_INVOKABLE void regenerate(QString requestor);

    Q_INVOKABLE void shutDown();

    explicit UIController(
            QString configId,
            PLCConnection*,
            ConfigurationManager*,
            JobManager*,
            TrainImageProvider**,
            QObject *parent = 0
            );

    void initialize();

    SystemConfiguration *getSystemConfiguration();
    void setActiveConfigurationId(QString);

signals:
    void plcConnect();
    void plcDisconnect();
    void plcStatus(SystemStatus*);
    void plcStarted();
    void plcStopped();
    void plcErrorStatus(bool);
    void plcWarningStatus(bool);

    void jobReady(bool);
    void trainImageReady();
    void stepImageReady();
    void imageReady(bool);
    void newStatistics(QString statsJson);
    void managerReady(QString requestor);       // this signal is emitted at the end of long asynch operations of setup-manager

    void newMessage(QString);
    void clearFailedImages();

public slots:

private slots:
    void onJobReady();
    void onTrainReady();
    void onPlcStatus(SystemStatus*);

private:
    bool m_ready;
    bool m_plcRunning;
    bool m_plcError;
    bool m_plcWarning;
    bool m_jobReady;

    int m_cyclePulses;
    int m_triggerOffset;

    QString m_activeConfigurationId;
    BatchConfiguration m_config;

    PLCConnection           *m_plcConnection;
    ConfigurationManager    *m_configurationManager;
    JobManager              *m_jobManager;
    TrainImageProvider      *m_trainImageProvider[4];

    void convertDegressToPulses(BatchConfiguration *);
};

#endif // UICONTROLLER_H
