#include <Servo.h>

// ECE 110 - IDC Code for Team 3 (Ping)

// Pin Config
#define QTI_PIN_0 47  // Left Qti
#define QTI_PIN_1 51  // Middle Qti
#define QTI_PIN_2 52  // Right Qti

#define SERVO_PIN_0 11  // Left Servo
#define SERVO_PIN_1 12  // Right Servo

#define RED_PIN 45    // Red LED pin
#define GREEN_PIN 46  // Green LED pin
#define BLUE_PIN 44   // Blue LED pin

#define E_RED_PIN 9     // External Red LED pin
#define E_GREEN_PIN 10  // External Green LED pin
#define E_BLUE_PIN 8    // External Blue LED pin

#define PING_PIN 50  // Ping sensor pin

// QTI Threshold Config
#define QTI_THRESH_0 500  // Left Qti
#define QTI_THRESH_1 300  // Middle Qti
#define QTI_THRESH_2 400  // Right Qti

// Ping Threshold Config (cm)
#define PING_THRESH 20

// ASCII Number for Team 3 (Ping)
#define ASCII_0 83

// Xbee Polling Interval
#define XBEE_POLL_INTERVAL 250 // Time (ms) between Xbee polls
int lastPollTime = 0; // Time tracker

// Object Setup
Servo servoLeft;
Servo servoRight;

// Data trackers
int hash = 0; // Tracks current hash
int objectPos = 0;  // Tracks registered object position
int scores[5]; // Tracks scores for other teams

// SYSTEM STATE DECLARATIONS

// Drivetrain states
enum class DriveState : int {
  HASH,        // Mapped to STOP command
  VEER_LEFT,   // n/a
  ERR,         // Mapped to STOP command
  TURN_LEFT,   // n/a
  VEER_RIGHT,  // n/a
  FORWARD,     // n/a
  TURN_RIGHT,  // n/a
  JUMP         // Mapped to FORWARD command
};

void setup() {
  // Xbee setup
  Serial2.begin(9600);
  delay(500);
  while (Serial2.available()) Serial2.read(); // Clear Xbee

  // Setup the LCD
  Serial3.begin(9600);
  delay(100);
  Serial.write(12); // clear display
  delay(10);
  Serial.write(22); // no cursor no blink
  delay(10);
  Serial.write(18); // backlight off
  delay(10);

  // Setup servos
  servoLeft.attach(11);
  servoRight.attach(12);

  // Setup LED
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  setRGB(0, 0, 0);

  // Setup External LED
  pinMode(E_RED_PIN, OUTPUT);
  pinMode(E_GREEN_PIN, OUTPUT);
  pinMode(E_BLUE_PIN, OUTPUT);
  setERGB(0, 0, 0);
}

void loop() {
  // Poll sensors and update system states
  int qtiState = getQtiState();
  DriveState driveState = static_cast<DriveState>(qtiState);
  setDriveMode(driveState);

  // If HASH state is detected, run Hash
  if (driveState == DriveState::HASH) { runHash(); }

  // Poll Xbee every XBEE_POLL_INTERVAL for new data
  if (millis() > lastPollTime + XBEE_POLL_INTERVAL) {
    lastPollTime = millis();
    int data = pollXbee();
    if (data == 0) {
      setERGB(0, 0, 0);  // Turn off external RGB LED if no new data
    } 
    else {
      decodeStore(data);
    }
  }
}

// Decode and store scores from other groups
void decodeStore(int data) {
  data -= 65;
  int group = data / 6;
  int score = data % 6;
  scores[group] = score;
}

// Hash code
void runHash() {
  hash++;

  // Hash cases for robot
  switch (hash) {
    case 1:               // First hash mark
      setRGB(1, 0, 1);    // Set RGB to Purple
      updateObjectPos();  // Check for object with ping sensor (updates external LED)
      delay(500);
      setRGB(0, 0, 0);   // Turn off RGB LED
      setERGB(0, 0, 0);  // Turn off external RGB LED
      setDriveMode(DriveState::FORWARD);
      delay(250);
      break;
    case 2:               // Second hash mark
      setRGB(0, 1, 0);    // Set RGB to Green
      updateObjectPos();  // Check for object with ping sensor (updates external LED)
      delay(500);
      setRGB(0, 0, 0);   // Turn off RGB LED
      setERGB(0, 0, 0);  // Turn off external RGB LED
      setDriveMode(DriveState::FORWARD);
      delay(250);
      break;
    case 3:               // Third hash mark
      setRGB(0, 0, 1);    // Set RGB to Blue
      updateObjectPos();  // Check for object with ping sensor (updates external LED)
      delay(500);
      setRGB(0, 0, 0);   // Turn off RGB LED
      setERGB(0, 0, 0);  // Turn off external RGB LED
      setDriveMode(DriveState::FORWARD);
      delay(250);
      break;
    case 4:               // Fourth hash mark
      setRGB(1, 0, 0);    // Set RGB to Red
      updateObjectPos();  // Check for object with ping sensor (updates external LED)
      delay(500);
      setRGB(0, 0, 0);   // Turn off RGB LED
      setERGB(0, 0, 0);  // Turn off external RGB LED
      setDriveMode(DriveState::FORWARD);
      delay(250);
      break;
    case 5:               // Fifth hash mark
      setRGB(1, 1, 1);    // Set RGB to White
      updateObjectPos();  // Check for object with ping sensor (updates external LED)
      delay(500);
      setRGB(0, 0, 0);   // Turn off RGB LED
      setERGB(0, 0, 0);  // Turn off external RGB LED
      setDriveMode(DriveState::FORWARD);
      delay(250);
      break;
    case 6:  // Jump across white gap
      delay(250);
      setDriveMode(DriveState::TURN_RIGHT);
      delay(500);
      break;
    case 7:  // Ignore first parking spot
      delay(500);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 8:  // Ignore second parking spot
      delay(500);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 9:  // Ignore third parking spot
      delay(500);
      setDriveMode(DriveState::FORWARD);
      delay(150);
      break;
    case 10:  // Park bot in fourth parking spot

      // If object is present, transmit objectPos score, otherwise transmit 0
      if (pingObject()) {
            setERGB(0, 1, 0);
            delay(250);
                  sendXbee(ASCII_0 + objectPos);
      }
      else {
        setERGB(1, 0, 0);
        delay(250);
        sendXbee(ASCII_0);
      }
      delay(250);
      setERGB(0,0,0);
      while (true)
        ;
      break;
  }
}

// LED set code
void setRGB(bool R, bool G, bool B) {
  digitalWrite(RED_PIN, !R);
  digitalWrite(GREEN_PIN, !G);
  digitalWrite(BLUE_PIN, !B);
}

// External LED set code
void setERGB(bool R, bool G, bool B) {
  digitalWrite(E_RED_PIN, R);
  digitalWrite(E_GREEN_PIN, G);
  digitalWrite(E_BLUE_PIN, B);
}

// Compile QTI State (Returns int 0-7)
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

// Maps -100 to 100 speed value to microseconds for each wheel.
// Left wheel:  100 = 1700µs (forward), -100 = 1300µs (backward)
// Right wheel: 100 = 1300µs (forward), -100 = 1700µs (backward) [mirrored]
void setWheelSpeed(int left, int right) {
  left  = constrain(left,  -100, 100);
  right = constrain(right, -100, 100);

  int leftMicros  = map(left,   -100, 100, 1300, 1700);
  int rightMicros = map(right,  -100, 100, 1700, 1300);  // Inverted: mirrored servo

  servoLeft.writeMicroseconds(leftMicros);
  servoRight.writeMicroseconds(rightMicros);
}

// Set drivetrain mode (movement commands)
void setDriveMode(DriveState state) {
  switch (state) {

    // Drive forward
    case DriveState::JUMP:
    case DriveState::FORWARD:
      setWheelSpeed(100, 100);
      break;

    // Veer to the left
    case DriveState::VEER_LEFT:
      setWheelSpeed(25, 100);
      break;

    // Turn left
    case DriveState::TURN_LEFT:
      setWheelSpeed(-100, 100);
      break;

    // Veer to the right
    case DriveState::VEER_RIGHT:
      setWheelSpeed(100, 25);
      break;

    // Turn right
    case DriveState::TURN_RIGHT:
      setWheelSpeed(100, -100);
      break;

    // Stop
    case DriveState::HASH:
    case DriveState::ERR:
    default:
      setWheelSpeed(0, 0);
      break;
  }
}

// Updates objectPos and external LED using pingObject method
void updateObjectPos() {
  if (pingObject()) {
    objectPos = hash;
    setERGB(0, 1, 0);
  } else {
    setERGB(1, 0, 0);
  }
}

// Returns true if object is detected
bool pingObject() {
  pinMode(PING_PIN, OUTPUT);
  digitalWrite(PING_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PING_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(PING_PIN, LOW);
  pinMode(PING_PIN, INPUT);
  long duration = pulseIn(PING_PIN, HIGH);
  long cm = duration / 29 / 2;
  return cm < PING_THRESH;
}

// Returns true if data is read from the Xbee buffer
bool pollXbee() {
  if (Serial2.available()) {
    setERGB(1, 1, 1);  // Set external RGB LED white if data is read
    return Serial2.read();
  } else {
    return 0;
  }
}

// Wrapper class for Xbee transmit function and updates external LED
int sendXbee(char data) {
  setERGB(0, 0, 1);  // Set external RGB LED blue
  delay(250);
  setERGB(0, 0, 0);  // Turn off external RGB LED
  Serial2.print(data);
}