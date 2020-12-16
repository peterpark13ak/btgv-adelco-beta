#include "mainwindow.h"
#include <QApplication>

#include <logger.h>

int main(int argc, char *argv[])
{
    // get application settings
    // ---------------------------------------------------------------------------
    QSettings *settings = new QSettings("C:/btgv-adelco/btgv-adelco.ini", QSettings::IniFormat);
    settings->beginGroup("Window");
    bool hideCursor = settings->value("hideCursor", true).toBool();
    bool fullScreen = settings->value("fullScreen", true).toBool();
    settings->endGroup();
    // ---------------------------------------------------------------------------

    setupLogging(settings);

    QApplication app(argc, argv);

    // prepare the main window
    // ---------------------------------------------------------------------------
    MainWindow w(settings);

    if (hideCursor) {
        app.setOverrideCursor( QCursor( Qt::BlankCursor ) );
    }

    if (fullScreen) {
        w.showFullScreen();
    }
    else {
        w.show();
    }
    // ---------------------------------------------------------------------------

    return app.exec();
}
