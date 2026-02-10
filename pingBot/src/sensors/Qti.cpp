#include "Qti.h"

// Constructor: Use an initialization list to set the pin
Qti::Qti(int pin) : m_pin(pin) {
    // No setup needed here yet, as pinMode changes dynamically in rcTime
}

// Destructor
Qti::~Qti() {
    // Nothing to clean up for a simple digital pin
}

// Read logic
long Qti::read() {
    pinMode(m_pin, OUTPUT);    
    digitalWrite(m_pin, HIGH); 
    delay(1);                
    pinMode(m_pin, INPUT);     
    digitalWrite(m_pin, LOW);  
    
    long startTime = micros();    
    while(digitalRead(m_pin)); // Wait for voltage to decay
    
    return micros() - startTime; 
}