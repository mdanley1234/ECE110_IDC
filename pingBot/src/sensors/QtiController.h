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

    // Destructor
    ~QtiController();

    /**
     * Reads all sensors and returns them in an array or struct.
     * For now, we'll just print them or provide individual getters.
     */
    void updateSensors();
    
    long getLeft()   const { return m_leftValue; }
    long getMiddle() const { return m_middleValue; }
    long getRight()  const { return m_rightValue; }

private:
    // Instantiating the 3 QTI objects
    Qti m_sensorLeft;
    Qti m_sensorMiddle;
    Qti m_sensorRight;

    // Variables to store the latest readings
    long m_leftValue;
    long m_middleValue;
    long m_rightValue;
};

#endif // QTI_CONTROLLER_H