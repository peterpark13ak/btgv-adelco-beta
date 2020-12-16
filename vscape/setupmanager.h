#ifndef SETUPMANAGER_H
#define SETUPMANAGER_H

#include <QObject>
#include <QAxObject>
#include <QAxWidget>
#include <QTimer>

#include <vscape/job.h>

// Use once with MSVS
// #import "C:\Microscan\Vscape\702_17\Dll\BufMgr.ocx"
// #import "C:\Microscan\Vscape\702_17\Dll\Setupmgr.ocx"

class SetupManager : public QObject
{
    Q_OBJECT

public:
    explicit SetupManager(int inspectionId = 0, QObject *parent = 0);
    ~SetupManager();

    void connectJob(QAxObject*);
    void disconnectJob();
    void acquireImage();
    void acquireRootImage(bool showGraphics = false);
    void acquireStepImage(QString path, bool useOutput = false, bool showGraphics = false);
    void acquireStepImage(QAxObject *step, bool useOutput = false, bool showGraphics = false);
    void selectStep(QAxObject *step);
    void tryoutStep(QAxObject *step);
    void tryOut(QAxObject *step);
    void cancelAcquireImage();
    bool acquireStatus();
    void getImage(bool showGraphics = false);
    int* getImageHSI(int x, int y);
    void regenerate();
    void refresh();

signals:
    void trainImageAcquired(QPixmap*);

private slots:
    void onSetupManagerException(int, const QString &, const QString &, const QString &);
    void onAcquireStarted();
    void onAcquireDone();
    void onAcquireCheck();

private:
    QAxWidget*  m_setupManager;
    QAxObject*  m_systemStep;
    QTimer*     m_acquireTimer;
    int         m_inspectionId;
    QAxObject   *insp[5];
};

#endif // SETUPMANAGER_H
