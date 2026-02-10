#ifndef QTI_CONTROLLER_H
#define QTI_CONTROLLER_H

#include "Qti.h"
#include "Config.h"

class QtiController {
public:
    /**
     * Constructor: Initializes the three QTI sensors using 
     * the pins defined in our RobotConfig namespace.
     */
    QtiController();

    // Request read state [0-7]
    int requestState();

private:
    // Instantiating the 3 QTI objects
    Qti m_sensorLeft;
    Qti m_sensorMiddle;
    Qti m_sensorRight;
};

#endif // QTI_CONTROLLER_H