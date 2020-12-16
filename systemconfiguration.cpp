#include <systemconfiguration.h>

/*!
 * \brief SystemConfiguration::SystemConfiguration
 * \param parent
 */
SystemConfiguration::SystemConfiguration(QObject *parent) : QObject(parent)
{
}

/*!
 * \brief SystemConfiguration::setNoAnswerThreshold
 * \param value
 */
void SystemConfiguration::setNoAnswerThreshold(quint16 value) {
    if (value != noAnswerThreshold) {
        noAnswerThreshold = value;
        emit noAnswerThresholdChanged();
    }
}

/*!
 * \brief SystemConfiguration::setFailedThreshold
 * \param value
 */
void SystemConfiguration::setFailedThreshold(quint16 value) {
    if (value != failedThreshold) {
        failedThreshold = value;
        emit failedThresholdChanged();
    }
}

/*!
 * \brief SystemConfiguration::setContinueOnError
 * \param value
 */
void SystemConfiguration::setContinueOnError(bool value) {
    if (value != continueOnError) {
        continueOnError = value;
        emit continueOnErrorChanged();
    }
}
