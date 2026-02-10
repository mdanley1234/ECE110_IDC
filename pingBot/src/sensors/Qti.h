#ifndef QTI_H
#define QTI_H
#include <Arduino.h>

class Qti {
public:
    // Constructor
    Qti(int pin);

    // Destructor
    ~Qti();

    // Method to read the sensor value
    long read();

private:
    int m_pin; // Pin number for the QTI sensor
};

#endif // QTI_H