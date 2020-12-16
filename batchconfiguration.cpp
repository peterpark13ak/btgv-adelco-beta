#include "batchconfiguration.h"

/*!
 * \brief BatchConfiguration::BatchConfiguration
 * \param parent
 */
BatchConfiguration::BatchConfiguration(QObject *parent) : QObject(parent)
{

}

/*!
 * \brief BatchConfiguration::BatchConfiguration
 * \param config
 * \param parent
 */
BatchConfiguration::BatchConfiguration(BatchConfiguration *config, QObject *parent) : QObject(parent) {
    name = config->name;
    avpFileName = config->avpFileName;
//    rejectValveCycles = config->rejectValveCycles;
//    rejectValveOn = config->rejectValveOn;
//    rejectValveDuration = config->rejectValveDuration;

    cameraToReject = config->cameraToReject;
    trigger1Offset = config->trigger1Offset;
    trigger2Offset = config->trigger2Offset;
    reject1Offset  = config->reject1Offset ;
    rejectDistance = config->rejectDistance;
    rejectDuration = config->rejectDuration;

    noOfProducts = config->noOfProducts;
}

/*!
 * \brief BatchConfiguration::setName
 * \param value
 */
void BatchConfiguration::setName(QString value) {
    if (QString::compare(value, name) != 0) {
        name = value;
        emit nameChanged();
    }
}

/*!
 * \brief BatchConfiguration::setAvpFileName
 * \param value
 */
void BatchConfiguration::setAvpFileName(QString value) {
    if (QString::compare(value, avpFileName, Qt::CaseInsensitive) != 0) {
        avpFileName = value;
        emit avpFileNameChanged();
    }
}

// /*!
// * \brief BatchConfiguration::setRejectValveCycles
// * \param value
// */
//void BatchConfiguration::setRejectValveCycles(quint16 value) {
//    if (value != rejectValveCycles) {
//        rejectValveCycles = value;
//        emit rejectValveCyclesChanged();
//    }
//}

// /*!
// * \brief BatchConfiguration::setRejectValveOn
// * \param value
// */
//void BatchConfiguration::setRejectValveOn(quint32 value) {
//    if (value != rejectValveOn) {
//        rejectValveOn = value;
//        emit rejectValveOnChanged();
//    }
//}

// /*!
// * \brief BatchConfiguration::setRejectValveDuration
// * \param value
// */
//void BatchConfiguration::setRejectValveDuration(quint32 value) {
//    if (value != rejectValveDuration) {
//        rejectValveDuration = value;
//        emit rejectValveDurationChanged();
//    }
//}

/*!
 * \brief BatchConfiguration::setNoOfProducts
 * \param value
 */
void BatchConfiguration::setNoOfProducts(quint16 value) {
    if (value != noOfProducts) {
        noOfProducts = value;
        emit noOfProductsChanged();
    }
}






/*!
 * \brief BatchConfiguration::setCameraToReject
 * \param value
 */
void BatchConfiguration::setCameraToReject(quint16 value) {
    if (value != cameraToReject) {
        cameraToReject = value;
        emit cameraToRejectChanged();
    }
}

/*!
 * \brief BatchConfiguration::setTrigger1Offset
 * \param value
 */
void BatchConfiguration::setTrigger1Offset(quint16 value) {
    if (value != trigger1Offset) {
        trigger1Offset = value;
        emit trigger1OffsetChanged();
    }
}

/*!
 * \brief BatchConfiguration::setTrigger2Offset
 * \param value
 */
void BatchConfiguration::setTrigger2Offset(quint16 value) {
    if (value != trigger2Offset) {
        trigger2Offset = value;
        emit trigger2OffsetChanged();
    }
}

/*!
 * \brief BatchConfiguration::setReject1Offset
 * \param value
 */
void BatchConfiguration::setReject1Offset(quint16 value) {
    if (value != reject1Offset) {
        reject1Offset = value;
        emit reject1OffsetChanged();
    }
}

/*!
 * \brief BatchConfiguration::setRejectDistance
 * \param value
 */
void BatchConfiguration::setRejectDistance(quint16 value) {
    if (value != rejectDistance) {
        rejectDistance = value;
        emit rejectDistanceChanged();
    }
}

/*!
 * \brief BatchConfiguration::setRejectDuration
 * \param value
 */
void BatchConfiguration::setRejectDuration(quint16 value) {
    if (value != rejectDuration) {
        rejectDuration = value;
        emit rejectDurationChanged();
    }
}
