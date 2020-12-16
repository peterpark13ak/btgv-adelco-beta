#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include <QObject>
#include <QSettings>
#include <vscape/job.h>
#include <vscape/camera.h>
#include <vscape/reportconnection.h>
#include <vscape/setupmanager.h>
#include <vscape/datumproperties.h>
#include <vscape/stepproperties.h>
#include <batchconfiguration.h>

class JobManager : public QObject
{
    Q_OBJECT

public:
    explicit JobManager(QSettings *settings, QObject *parent = 0);
    ~JobManager();

    void start();
    void loadAVP(QString fileName);
    void saveAVP(QString fileName);
    void cleanUp();

    void editMode();
    void runMode();
    void acquireImage();
    void acquireStepImage(QString symName, bool useOutput, bool showGraphics);
    void acquireRootImage(bool showGraphics);
    int* getImageHSI(int x, int y);
    void cancelAcquireImage();
    bool acquireStatus();
    void getImage();

    void walkSteps();  // for testing
    void trainAll();
    void train(QString stepPath);
    void trainParent(QString stepPath);
    void tryout(QString stepPath);
    void tryOutSteps(QString stepPath);
    void deleteStep(QString stepPath);

    void setPath(QString path);
    QString getPath();
    void setAvp(QString avp);
    void setBatchConfiguration(BatchConfiguration *conf);

    void setAvpNumericParam(int inspectionId, QString code, int value);
    QVariant getAvpNumericParam(int inspectionId, QString code);
    QList<DatumType> getGlobalDatums(int inspectionId);
    QList<StepProps> *getConfigurableSteps(bool withUpdate = false);
    StepProps getStepProps(QString stepPath);
    void updateROI(StepProps step);
    void updateROIs(QList<StepProps> steps);
    void setDatumValue(QString stepPath, StepDatum stepDatum);
    void regenerate();
    void refresh();

signals:
    void ready();
    void trainReady();
    void stepImageReady();
    void trainImageAcquired1(QPixmap*);
    void trainImageAcquired2(QPixmap*);
    void trainImageAcquired3(QPixmap*);
    void trainImageAcquired4(QPixmap*);
    void newImage1(QPixmap*, bool);
    void newImage2(QPixmap*, bool);
    void newImage3(QPixmap*, bool);
    void newImage4(QPixmap*, bool);

    void newFailedImage(QPixmap*, int);

    void viewMessage(QString msg);
    void imageReady(bool);
    void newResult(bool, unsigned char);
    void newStatistics(QString statsJson);

public slots:
    void onTrainImageAcquired1(QPixmap*);
    void onTrainImageAcquired2(QPixmap*);
    void onTrainImageAcquired3(QPixmap*);
    void onTrainImageAcquired4(QPixmap*);
    void onNewImage1(QPixmap*,bool);
    void onNewImage2(QPixmap*,bool);
    void onNewImage3(QPixmap*,bool);
    void onNewImage4(QPixmap*,bool);
    void onNewResult(bool, unsigned char);
    void onNewFailedImage(QPixmap*, int providerId);

private slots:
    void onCameraReady();
    void onAvpReady(bool);

private:
    QSettings *m_settings;
    QString m_avpPath;
    QString m_avp;
    QString m_avpInProgress;
    QString m_cameraName;

    bool m_avpLoaded;
    bool m_loadInProgress;
    bool m_editMode;
    bool m_runMode;

    QMap<QString, DatumType> *m_globalDatums;
    void prepareDatums();
    void updateGlobalDatumValues(int inspectionId);

    QList<StepProps> *m_stepProperties;

    Job*                m_job;
    Camera*             m_camera;
    SetupManager*       m_setupManager[4];
    ReportConnection*   m_reportConnection[4];
    BatchConfiguration* m_batchConfiguration;
};

#endif // JOBMANAGER_H
