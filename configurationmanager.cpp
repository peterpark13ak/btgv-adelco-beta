#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include "configurationmanager.h"

/**
 * @brief ConfigurationManager::ConfigurationManager
 * @param settings
 * @param parent
 */
ConfigurationManager::ConfigurationManager(QSettings *settings, QObject *parent) : QObject(parent)
{
    m_settings = settings;
    m_dbFname = settings->value("database", "btgv-adelco.db").toString();

    m_settings->beginGroup("Vscape");
    m_avpPath = settings->value("avpPath", "C:/btgv-adelco/avp").toString();
    m_settings->endGroup();

    m_readOnlyConfigurations = settings->value("readOnlyConfig", "").toStringList();

    // check if Database exists
    QFile *dbFile = new QFile(m_dbFname);
    if (! dbFile->exists() ) {
        qDebug() << "Creating configuration database: " + m_dbFname;
        createDatabase();
    }
    delete dbFile;

    openDatabase();
}

/**
 * @brief ConfigurationManager::~ConfigurationManager
 */
ConfigurationManager::~ConfigurationManager() {
    closeDatabase();
}

/*!
 * \brief ConfigurationManager::storeConfigurationId
 * \param id
 */
void ConfigurationManager::storeConfigurationId(QString id) {
    m_settings->setValue("configurationID", id);
}

/**
 * @brief ConfigurationManager::closeDatabase
 */
void ConfigurationManager::closeDatabase() {
    this->cfgDb.close();
}

/**
 * @brief ConfigurationManager::openDatabase
 * @return
 */
bool ConfigurationManager::openDatabase() {
    this->cfgDb = QSqlDatabase::addDatabase("QSQLITE");
    this->cfgDb.setDatabaseName(m_dbFname);
    return this->cfgDb.open();
}

/**
 * @brief ConfigurationManager::createDatabase
 */
void ConfigurationManager::createDatabase() {
    if ( ! openDatabase() ) {
        qCritical() << "Cannot open configuration database " + m_dbFname;
        return;
    }

    QSqlQuery query;

    if ( ! query.exec("create table batchconfig("
               "name varchar(50) PRIMARY KEY ASC,"
               "timecreated varchar(20),"
               "cameratoreject int,"
               "trigger1offset int,"
               "trigger2offset int,"
               "reject1offset int,"
               "rejectdistance int,"
               "rejectduration int,"
               "avp varchar(200))") ) {
        qCritical() << "Cannot create batch configuration table. " + query.lastError().databaseText();
        this->cfgDb.close();
        return;
    }
    if ( ! query.exec("create index bcidx on batchconfig (timecreated)") ) {
        qCritical() << "Cannot create batch configuration timestamp index. " + query.lastError().databaseText();
        this->cfgDb.close();
        return;
    }
    if ( ! query.exec("insert into batchconfig "
               "(name, timecreated, cameratoreject, trigger1offset, trigger2offset, reject1offset, rejectdistance, rejectduration, avp)"
               " values "
               "('SAMPLE', datetime('now'), 0, 0, 0, 0, 0, 100, 'sample.avp')") ) {
        qCritical() << "Cannot create batch configuration table. " + query.lastError().databaseText();
        this->cfgDb.close();
        return;
    }

    this->cfgDb.close();
}

/*!
 * \brief ConfigurationManager::getReadOnlyConfigurations
 * \return
 */
QStringList ConfigurationManager::getReadOnlyConfigurations() {
    return m_readOnlyConfigurations;
}

/**
 * @brief ConfigurationManager::getConfigurationNames
 * @return
 */
QStringList ConfigurationManager::getConfigurationNames() {

    QSqlQuery query;
    if ( ! query.exec("select name from batchconfig order by name") ) {
        qCritical() << "Cannot retrieve batch configurations list. " + query.lastError().databaseText();
        return QStringList();
    }

    QStringList names;
    while (query.next()) {
        names.append(query.value(0).toString());
    }

    return names;
}

/**
 * @brief ConfigurationManager::getConfigurationByID
 * @param id
 * @param config
 */
void ConfigurationManager::getConfigurationById(QString id, BatchConfiguration *config) {

    QSqlQuery query;
    if ( ! query.exec("select avp, cameratoreject, trigger1offset, trigger2offset, reject1offset, rejectdistance, rejectduration from batchconfig where name = '" + id + "'") ) {
        qCritical() << "Cannot retrieve batch configuration with name " + id + ". " + query.lastError().databaseText();
        return;
    }

    if (query.next()) {
        config->setName(id);
        config->setAvpFileName(query.value(0).toString());

        config->setCameraToReject(query.value(1).toInt());
        config->setTrigger1Offset(query.value(2).toInt());
        config->setTrigger2Offset(query.value(3).toInt());
        config->setReject1Offset( query.value(4).toInt());
        config->setRejectDistance(query.value(5).toInt());
        config->setRejectDuration(query.value(6).toInt());
    }

    return;
}

/**
 * @brief ConfigurationManager::nameExists
 * @param name
 * @return
 */
bool ConfigurationManager::nameExists(QString name) {
    QSqlQuery query;
    if ( ! query.exec("select count(*) from batchconfig where name = '" + name + "'") ) {
        qCritical() << "Cannot check existence of name " + name + ". " + query.lastError().databaseText();
        return false;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;
    }

    return false;
}

/*!
 * \brief ConfigurationManager::getAvpById
 * \param name
 * \return
 */
QString ConfigurationManager::getAvpById(QString name) {
    QSqlQuery query;
    if ( ! query.exec("select avp from batchconfig where name = '" + name + "'") ) {
        qCritical() << "Cannot get avp from config id " + name + ". " + query.lastError().databaseText();
        return "";
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return "";
}

/**
 * @brief ConfigurationManager::saveConfiguration
 * @param config
 */
void ConfigurationManager::saveConfiguration(BatchConfiguration *config) {
    QSqlQuery query;
    query.prepare("insert into batchconfig (name, avp, cameratoreject, trigger1offset, trigger2offset, reject1offset, rejectdistance, rejectduration) "
                  "values (:name, :avp, :cameratoreject, :trigger1offset, :trigger2offset, :reject1offset, :rejectdistance, :rejectduration)");
    query.bindValue(":name",                config->getName());
    query.bindValue(":avp",                 config->getAvpFileName());

//    query.bindValue(":rejectvalvecycles",   config->getRejectValveCycles());
//    query.bindValue(":rejectvalveon",       config->getRejectValveOn());
//    query.bindValue(":rejectvalveduration", config->getRejectValveDuration());
//    query.bindValue(":noofproducts",        config->getNoOfProducts());

    query.bindValue(":cameratoreject", config->getCameraToReject());
    query.bindValue(":trigger1offset", config->getTrigger1Offset());
    query.bindValue(":trigger2offset", config->getTrigger2Offset());
    query.bindValue(":reject1offset" , config->getReject1Offset());
    query.bindValue(":rejectdistance", config->getRejectDistance());
    query.bindValue(":rejectduration", config->getRejectDuration());

    if ( ! query.exec() ) {
        qCritical() << "Cannot save configuration " + config->getName() + ". " + query.lastError().databaseText();
        return;
    }
}

/**
 * @brief ConfigurationManager::updateConfiguration
 * @param config
 */
void ConfigurationManager::updateConfiguration(BatchConfiguration *config) {
    QSqlQuery query;
    query.prepare("update batchconfig set cameratoreject = :cameratoreject, trigger1offset = :trigger1offset, trigger2offset = :trigger2offset, "
                  " reject1offset = :reject1offset, rejectdistance = :rejectdistance, rejectduration = :rejectduration, "
                  " avp = :avp "
                  "where name = :name ");
    query.bindValue(":name",                config->getName());
    query.bindValue(":avp",                 config->getAvpFileName());

//    query.bindValue(":rejectvalvecycles",   config->getRejectValveCycles());
//    query.bindValue(":rejectvalveon",       config->getRejectValveOn());
//    query.bindValue(":rejectvalveduration", config->getRejectValveDuration());
//    query.bindValue(":noofproducts",        config->getNoOfProducts());

    query.bindValue(":cameratoreject", config->getCameraToReject());
    query.bindValue(":trigger1offset", config->getTrigger1Offset());
    query.bindValue(":trigger2offset", config->getTrigger2Offset());
    query.bindValue(":reject1offset" , config->getReject1Offset());
    query.bindValue(":rejectdistance", config->getRejectDistance());
    query.bindValue(":rejectduration", config->getRejectDuration());

    if ( ! query.exec() ) {
        qCritical() << "Cannot update configuration " + config->getName() + ". " + query.lastError().databaseText();
        return;
    }

    qDebug() << query.lastQuery();
}

/**
 * @brief ConfigurationManager::copyAvp
 * @param source
 * @param destination
 */
void ConfigurationManager::copyAvp(QString source, QString destination) {
   QString sourceFname = m_avpPath + "/" + source;
   QString destinationFname = m_avpPath + "/" + destination;

   if (! QFile::copy(sourceFname, destinationFname) ) {
       qCritical() << "Cannot copy avp " + sourceFname + " to " + destinationFname;
       return;
   }
}

/**
 * @brief ConfigurationManager::deleteConfiguration
 * @param name
 */
void ConfigurationManager::deleteConfiguration(QString name) {

    QSqlQuery query;
    if ( ! query.exec("delete from batchconfig where name = '" + name + "'") ) {
        qCritical() << "Cannot delete configuration " + name + ". " + query.lastError().databaseText();
        return;
    }

    QString sourceFname = m_avpPath + "/" + name + ".avp";
    if (! QFile::remove(sourceFname)) {
        qCritical() << "Cannot delete avp file " + sourceFname;
        return;
    }

    return;
}
