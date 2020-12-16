#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <uibackend.h>
#include <uicontroller.h>
#include <plcconnection.h>
#include <configurationmanager.h>
#include <vscape/jobmanager.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSettings*, QWidget *parent = 0);
    ~MainWindow();

private:
    UIController            *m_controller;
    UIBackend               *m_uiBackend;
    PLCConnection           *m_plcConnection;
    ConfigurationManager    *m_configurationManager;
    JobManager              *m_jobManager;

    TrainImageProvider      *m_trainImageProvider[4];
    BufferImageProvider     *m_bufferImageProvider[5];
};

#endif // MAINWINDOW_H
