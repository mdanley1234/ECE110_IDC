#include "QtiController.h"

// Initialize each sensor with its specific pin from the namespace
QtiController::QtiController()
    : m_sensorLeft(RobotConfig::QTI::QTI_PIN_0),
      m_sensorMiddle(RobotConfig::QTI::QTI_PIN_1),
      m_sensorRight(RobotConfig::QTI::QTI_PIN_2)
{
}

int QtiController::requestState() {
    int state = (m_sensorLeft.read() < RobotConfig::QTI::QTI_THRESH_0) * 4;
    state += ( m_sensorMiddle.read() < RobotConfig::QTI::QTI_THRESH_1) * 2;
    state += (m_sensorRight.read() < RobotConfig::QTI::QTI_THRESH_2);
    return state;
}

