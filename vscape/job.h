#ifndef JOB_H
#define JOB_H

#include <QObject>
#include <QAxObject>
#include <QPixmap>
#include <QtWinExtras/QtWin>
#include <vscape/stepproperties.h>

enum walkCommand {
    scanForSteps = 1,
    trainSteps   = 2
};

class Job : public QObject
{
    Q_OBJECT
public:
    explicit Job(QObject *parent = 0);
    ~Job();

    void load(QString avpFileName);
    void removeAllJobs();
    QAxObject *getJobStep();
    QAxObject *getSystemStep();

    void walkStep(QAxObject*, QList<StepProps> *, StepProps parentProps, QString path = "", int level = 0, walkCommand cmd = scanForSteps);
    void walkJob(QList<StepProps> *, walkCommand);

    void setAvpNumericParameter(int inspectionId, QString stepSym, QString datumSym, QVariant value);
    QVariant getAvpNumericParameter(int inspectionId, QString stepSym, QString datumSym);
    void updateROI(StepProps stepProps);
    void setDatumValue(QAxObject *step, StepDatum stepDatum);
    void setDatumValue(QString stepPath, StepDatum stepDatum);
    void setDatumRange(QAxObject *step, QString refDatum, QString minValueDatum, QString maxValueDatum, int perc);
    void train(QString stepPath);
    void trainParent(QString stepPath);
    void deleteStep(QAxObject *step);
    void regenerate();

    static QPixmap fromBufferDm(QAxObject *, bool withGraphics = false);
    static QRect getStepROI(QAxObject *step);
    static QString getStepFullPath(QAxObject *root, QString stepName);
signals:

public slots:

private slots:
    void onJobException(int, const QString &, const QString &, const QString &);

private:
    static QString getFullPath(QAxObject *step, QString startPath);

    QAxObject *m_jobStep;
};

#endif // JOB_H
