#include <Servo.h>

// Pin Config
#define QTI_PIN_0 47  // Left Qti
#define QTI_PIN_1 51  // Middle Qti
#define QTI_PIN_2 52  // Right Qti

#define SERVO_PIN_0 11  // Left Servo
#define SERVO_PIN_1 12  // Right Servo

#define RED_PIN 45    // Red LED pin
#define GREEN_PIN 46  // Green LED pin
#define BLUE_PIN 44   // Blue LED pin

// QTI Threshold Config
#define QTI_THRESH_0 600  // Left Qti
#define QTI_THRESH_1 300  // Middle Qti
#define QTI_THRESH_2 400  // Right Qti

// Object Setup
Servo servoLeft;
Servo servoRight;

// Hash Setup
int hash = 0;

// SYSTEM STATE DECLARATIONS

// Drivetrain states (DriveState != DriveMode)
enum class DriveState : int {
  HASH,        // Mapped to STOP command
  VEER_LEFT,   // n/a
  ERR,         // Mapped to STOP command
  TURN_LEFT,   // n/a
  VEER_RIGHT,  // n/a
  FORWARD,     // n/a
  TURN_RIGHT,  // n/a
  JUMP         // Mapped to VEER_RIGHT
};

void setup() {
  // Setup servos
  servoLeft.attach(11);
  servoRight.attach(12);

  // Setup LEDs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  setRGB(0, 0, 0);
}

void loop() {
  // Poll sensors and update system states
  int qtiState = getQtiState();
  DriveState driveState = static_cast<DriveState>(qtiState);
  setDriveMode(driveState);

  // If HASH state is detected, run Hash
  if (driveState == DriveState::HASH) { runHash(); }
}

// Hash code
void runHash() {
  hash++;

  // Set corresponding LED
  switch (hash) {
    case 1:
      setRGB(1, 0, 1);
      delay(500);
      setRGB(0, 0, 0);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 2:
      setRGB(0, 1, 0);
      delay(500);
      setRGB(0, 0, 0);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 3:
      setRGB(0, 0, 1);
      delay(500);
      setRGB(0, 0, 0);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 4:
      setRGB(1, 0, 0);
      delay(500);
      setRGB(0, 0, 0);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 5:
      setRGB(1, 1, 1);
      delay(500);
      setRGB(0, 0, 0);
      while (true);  // Code stop command
      break;
  }
}

// LED set code
void setRGB(bool R, bool G, bool B) {
  digitalWrite(RED_PIN, R);
  digitalWrite(GREEN_PIN, G);
  digitalWrite(BLUE_PIN, B);
}

// Compile QTI State
int getQtiState() {
  int state = readQti(QTI_PIN_0, QTI_THRESH_0) * 4;
  state += readQti(QTI_PIN_1, QTI_THRESH_1) * 2;
  state += readQti(QTI_PIN_2, QTI_THRESH_2);
  return state;
}

// Returns 1 if below thresh (white) or 0 if above thresh (black)
int readQti(int pin, int thresh) {
  // Set pin to OUTPUT and write HIGH
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(1);

  // Set pin to INPUT and write LOW
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);

  // Wait for discharge and check time against thresh
  long startTime = micros();
  while (digitalRead(pin))
    ;  // Wait for voltage to decay
  int dischargeTime = ((micros() - startTime) < thresh);
  return dischargeTime;
}

// Set drivetrain mode (movement commands)
void setDriveMode(DriveState state) {
  switch (state) {

    // Drive forward
    case DriveState::FORWARD:
      servoLeft.writeMicroseconds(1700);   // Counter-Clockwise
      servoRight.writeMicroseconds(1300);  // Clockwise
      break;

    // Veer to the left
    case DriveState::VEER_LEFT:
      servoLeft.writeMicroseconds(1550);   // Slow Left
      servoRight.writeMicroseconds(1300);  // Fast Right
      break;

    // Turn left
    case DriveState::TURN_LEFT:
      servoLeft.writeMicroseconds(1300);   // Backwards
      servoRight.writeMicroseconds(1300);  // Forwards
      break;

    // Veer to the right
    case DriveState::JUMP:
    case DriveState::VEER_RIGHT:
      servoLeft.writeMicroseconds(1700);   // Fast Left
      servoRight.writeMicroseconds(1450);  // Slow Right
      break;

    // Turn right
    case DriveState::TURN_RIGHT:
      servoLeft.writeMicroseconds(1700);   // Forwards
      servoRight.writeMicroseconds(1700);  // Backwards
      break;

    // Stop
    case DriveState::HASH:
    case DriveState::ERR:
    default:
      servoLeft.writeMicroseconds(1500);
      servoRight.writeMicroseconds(1500);
      break;
  }
}