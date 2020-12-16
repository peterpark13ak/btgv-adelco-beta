#ifndef LOGGER_H
#define LOGGER_H

#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <iostream>

// define enumeration type for log levels
typedef enum LogLevel {
    LogFatal    = 0,
    LogCritical = 1,
    LogWarning  = 2,
    LogInfo     = 3,
    LogDebug    = 4
} LogLevel;

FILE *logFile;
LogLevel logLevel;

/*!
 * \brief logMessageOutput
 * \param type
 * \param context
 * \param msg
 */
void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // create time-stamp
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    // prepare message
    QString contextMsg = msg;
    QString contextFile = context.file     == NULL ? "" : QString::fromStdString(context.file);
    QString contextLine = context.file     == NULL ? "" : QString::number(context.line);
    QString contextFunc = context.function == NULL ? "" : QString::fromStdString(context.function);

    if (! contextFile.isEmpty()) {
        contextMsg += " (" + contextFile + ":" + contextLine + "), " + contextFunc;
    }

    // check log level
    switch (type) {
    case QtDebugMsg:
        if (logLevel < LogDebug) break;
        fprintf(logFile, "%s DEBUG: %s\n", timeStamp.toLocal8Bit().data(), contextMsg.toLocal8Bit().data());
        break;
    case QtInfoMsg:
        if (logLevel < LogInfo) break;
        fprintf(logFile, "%s INFO: %s\n", timeStamp.toLocal8Bit().data(), contextMsg.toLocal8Bit().data());
        break;
    case QtWarningMsg:
        if (logLevel < LogWarning) break;
        fprintf(logFile, "%s WARN: %s\n", timeStamp.toLocal8Bit().data(), contextMsg.toLocal8Bit().data());
        break;
    case QtCriticalMsg:
        if (logLevel < LogCritical) break;
        fprintf(logFile, "%s CRITICAL: %s\n", timeStamp.toLocal8Bit().data(), contextMsg.toLocal8Bit().data());
        break;
    case QtFatalMsg:
        fprintf(logFile, "%s FATAL: %s\n", timeStamp.toLocal8Bit().data(), contextMsg.toLocal8Bit().data());
        abort();
    default:
        std::cerr << "Invalid message type (level)\n";
    }
    fflush(logFile);
}

/*!
 * \brief setupLogging
 * \param settings
 */
void setupLogging(QSettings *settings) {

    QString dateStr = QDateTime::currentDateTime().toString("yy-MM-dd");

    // Read logging configuration -----------------------
    settings->beginGroup("Logging");

    bool logEnabled     = settings->value("logEnabled",  "true").toBool();
    bool appendToLog    = settings->value("appendToLog", "false").toBool();
    QString logPath     = settings->value("logPath",     "c:/btgv-adelco").toString();
    QString logLevelStr = settings->value("logLevel",    "Info").toString();

    if (QString::compare(logLevelStr, "Fatal") == 0)
        logLevel = LogFatal;
    else if (QString::compare(logLevelStr, "Critical") == 0)
        logLevel = LogCritical;
    else if (QString::compare(logLevelStr, "Warning") == 0)
        logLevel = LogWarning;
    else if (QString::compare(logLevelStr, "Info") == 0)
        logLevel = LogInfo;
    else
        logLevel = LogDebug;

    settings->endGroup();
    // --------------------------------------------------

    if (logEnabled) {
        QString logFileName = logPath + "/" + dateStr + "-bltgv.log";

        logFile = fopen (logFileName.toLatin1().data(), appendToLog ? "a+" : "w+");
        if (logFile != NULL) {
            qInstallMessageHandler(logMessageOutput);
        } else {
            std::cerr << "Cannot open log file " << logFileName.toLatin1().data() << "\n";
            std::cerr.flush();
        }
    }

    qInfo() << "\n"
       "---------------------\n"
       "BLISTER-TAGV Starting\n"
       "---------------------\n"
       ;
}

#endif // LOGGER_H
