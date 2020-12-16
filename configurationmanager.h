#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSettings>
#include <batchconfiguration.h>

class ConfigurationManager: public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QStringList getConfigurationNames();
    Q_INVOKABLE QStringList getReadOnlyConfigurations();
    Q_INVOKABLE void getConfigurationById(QString, BatchConfiguration*);
    Q_INVOKABLE void saveConfiguration(BatchConfiguration*);
    Q_INVOKABLE void updateConfiguration(BatchConfiguration*);
    Q_INVOKABLE bool nameExists(QString);
    Q_INVOKABLE void copyAvp(QString, QString);
    Q_INVOKABLE void deleteConfiguration(QString);
    Q_INVOKABLE QString getAvpById(QString);

    ConfigurationManager(QSettings *settings, QObject *parent=0);
    ~ConfigurationManager();

    void storeConfigurationId(QString);

private:
    QSettings *m_settings;
    QString m_dbFname;
    QString m_avpPath;
    QSqlDatabase cfgDb;
    QStringList m_readOnlyConfigurations;

    void createDatabase();
    bool openDatabase();
    void closeDatabase();
};

#endif // CONFIGURATIONMANAGER_H
