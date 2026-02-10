#include "Drivetrain.h"

Drivetrain::Drivetrain() {
    servoLeft.attach(12);   
    servoRight.attach(11);  
}


// --- Movement Methods ---

// 1. STOP
void Drivetrain::stop() {
    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1500);
}

// 2. FORWARD
// Standard servos: Left > 1500, Right < 1500 (due to mirrored mounting)
void Drivetrain::forward() {
    servoLeft.writeMicroseconds(1600);   // Moderate speed forward
    servoRight.writeMicroseconds(1400);  // Balanced speed forward
}

// 3. PIVOT TURNS (Spin in place)
// Used when you completely lose the line (e.g., "Not on line" state)
void Drivetrain::turnLeft() {
    servoLeft.writeMicroseconds(1400);   // Left wheel backwards
    servoRight.writeMicroseconds(1400);  // Right wheel forward
}

void Drivetrain::turnRight() {
    servoLeft.writeMicroseconds(1600);   // Left wheel forward
    servoRight.writeMicroseconds(1600);  // Right wheel backwards
}

// 4. VEER TURNS (Gentle corrections)
// Used for normal line following (e.g., "Left sensor sees line")
void Drivetrain::veerLeft() {
    servoLeft.writeMicroseconds(1500);   // Stop left wheel
    servoRight.writeMicroseconds(1400);  // Keep right wheel moving forward
}

void Drivetrain::veerRight() {
    servoLeft.writeMicroseconds(1600);   // Keep left wheel moving forward
    servoRight.writeMicroseconds(1500);  // Stop right wheel
}