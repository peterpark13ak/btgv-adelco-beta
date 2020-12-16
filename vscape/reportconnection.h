#ifndef REPORTCONNECTION_H
#define REPORTCONNECTION_H

#include <QObject>
#include <QAxObject>
#include <QPixmap>
#include <QSettings>
#include <QDateTime>
#include <vscape/job.h>
#include <objbase.h>

class ReportConnection : public QObject
{
    Q_OBJECT
public:
    explicit ReportConnection(QSettings *settings, int reportId, QObject *parent = 0);
    bool connectReport(QAxObject *device, QAxObject* system, int inspectionIndex, uint noOfProducts);
    void disconnectReport();
    void addImageToReport(QAxObject *system);

signals:
    void newImage(QPixmap*, bool success);
    void newFailedImage(QPixmap*, int providerId);
    void newResult(bool result, unsigned char resultMask);
    void newStatistics(QString statsJson);

public slots:
private slots:
    void onNewReport(IDispatch*, bool);
    void onReportConnectionException(int, const QString &, const QString &, const QString &);

private:
    QAxObject*     m_reportConnection;
    bool           m_connected;
    int            m_overruns;
    QString        m_imagePath;
    bool           m_imageSave;
    bool           m_saveOnlyFailed;
    int            m_reportId;

    QList<QString> m_prodStepNames;
    QList<QString> m_prodStepPaths;
    uint           n_noOfProducts;

    bool updateStats(QAxObject *stats, QJsonArray &results);
    unsigned char updateResults(QAxObject *results, QJsonArray &values);
};

#endif // REPORTCONNECTION_H
