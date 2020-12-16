QT     += core qml quick gui axcontainer sql winextras
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = btgv-adelco
TEMPLATE = app

VERSION=1.2.0.7 # major.minor.patch.build
VERSION_PE_HEADER=1.0
QMAKE_TARGET_DESCRIPTION=BLISTER MULTI TAG VISUAL VERIFICATION
QMAKE_TARGET_COPYRIGHT=nikos.lykouropoulos@gmail.com

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        uibackend.cpp \
        uicontroller.cpp \
        plcconnection.cpp \
        systemstatus.cpp \
        batchconfiguration.cpp \
        configurationmanager.cpp \
        bufferimageprovider.cpp \
        trainimageprovider.cpp \
        systemconfiguration.cpp \
        systemmanager.cpp \
        vscape/jobmanager.cpp \
        vscape/setupmanager.cpp \
        vscape/camera.cpp \
        vscape/datumproperties.cpp \
        vscape/stepproperties.cpp \
        vscape/job.cpp \
        vscape/reportconnection.cpp

HEADERS  += \
        mainwindow.h \
        logger.h \
        uibackend.h \
        uicontroller.h \
        plcconnection.h \
        systemstatus.h \
        batchconfiguration.h \
        configurationmanager.h \
        bufferimageprovider.h \
        trainimageprovider.h \
        systemconfiguration.h \
        systemmanager.h \
        vscape/jobmanager.h \
        vscape/setupmanager.h \
        vscape/camera.h \
        vscape/datumproperties.h \
        vscape/stepproperties.h \
        vscape/job.h \
        vscape/reportconnection.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

OTHER_FILES += \
    qml/MainWindow.qml \
    qml/MainPanelView.qml \
    qml/TopMenu.qml \
    qml/CounterList.qml \
    qml/IndicatorLed.qml \
    qml/ConfigListView.qml \
    qml/BatchConfigurationView.qml \
    qml/SystemConfigurationView.qml \
    qml/SystemTestView.qml \
    qml/ResultsView.qml \
    qml/AvpEditView.qml \
    qml/StepDatumsView.qml \
    qml/StepEditView.qml \
    qml/StepListView.qml \
    qml/StepPlacementView.qml \
    qml/ImageInspect.qml \
    qml/MessageView.qml \
    qml/ErrorsView.qml \
    qml/NumberInputField.qml \
    qml/NumberInputSpinner.qml \
    qml/BooleanInput.qml \
    qml/NumericKeypadView.qml \
    qml/StringKeypadView.qml \
    qml/FormHeader.qml \
    qml/KeyButton.qml \
    qml/ToggleDelayButton.qml \
    qml/TouchTabViewStyle.qml \
    qml/BlackButtonBackground.qml \
    qml/BlackButtonStyle.qml \
    qml/BlackSwitchStyle.qml \
    qml/BusyScreen.qml \
    qml/DelayedExecution.qml \
    qml/CloseDialog.qml \
    gml.qrc

DISTFILES += \
    qml/CloseDialog.qml
