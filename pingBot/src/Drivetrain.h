#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include <Servo.h>
#include "Config.h"       // Assuming your PIN numbers are defined here
#include <Arduino.h>      // Required for servo and pin constants



class Drivetrain {
private:
    Servo servoLeft;
    Servo servoRight;

public:
    Drivetrain();
    
    void stop();
    void forward();
    void turnLeft();
    void turnRight();
    
    // ADD THESE TWO LINES:
    void veerLeft();
    void veerRight();
};