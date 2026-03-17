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

// Time limiter before moving on from first stage
#define FIRST_STAGE_WAIT 20000

// Xbee Polling Interval
#define XBEE_POLL_INTERVAL 250  // Time (ms) between Xbee polls
int lastPollTime = 0;           // Time tracker

// Object Setup
Servo servoLeft;
Servo servoRight;

// Data trackers
int hash = 0;                            // Tracks current hash
int objectPos = 0;                       // Tracks registered object position
int scores[5] = { -1, -1, -1, -1, -1 };  // Tracks scores for other teams

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
  while (Serial2.available()) Serial2.read();  // Clear Xbee

  // Setup the LCD
  Serial3.begin(9600);
  delay(100);
  Serial.write(12);  // clear display
  delay(10);
  Serial.write(22);  // no cursor no blink
  delay(10);
  Serial.write(18);  // backlight off
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
    pollXbee();
  }
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

      while (millis() < FIRST_STAGE_WAIT)
        ;

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
        scores[3] = objectPos;
      } else {
        setERGB(1, 0, 0);
        delay(250);
        sendXbee(ASCII_0);
        scores[3] = 0;
      }
      delay(250);
      setERGB(0, 0, 0);
      while (!checkScores()) {
        pollXbee();
        delay(250);
      }
      setERGB(0, 0, 0);
      int points = updateLCD();

      // Victory / Loss Code
      if (points < 10) {
        performLoss();
      } else {
        performVictory();
      }

      while (true)
        ;

      break;
  }
}

// Victory
void performVictory() {
  lightshow_run();
}

// Loss
void performLoss() {
  imperial_march();
}

// Check that score data is recieved from all bots
bool checkScores() {
  for (int i = 0; i < 5; i++) {
    if (scores[i] == -1) {
      return false;
    }
  }
  return true;
}

// Update LCD from scores[] array
int updateLCD() {
  int x = 0;
  for (int i = 0; i < 5; i++) {
    Serial3.print(scores[i]);
    Serial3.print(' ');
    x += scores[i];
  }
  Serial3.print('\n');
  Serial3.print("SUM ");
  Serial3.print(x);

  return x;
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
  left = constrain(left, -100, 100);
  right = constrain(right, -100, 100);

  int leftMicros = map(left, -100, 100, 1300, 1700);
  int rightMicros = map(right, -100, 100, 1700, 1300);  // Inverted: mirrored servo

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

// Listens for data from the Xbee buffer, stores in score[], and updates ERGB LED
void pollXbee() {
  if (!Serial2.available()) {
    setERGB(0, 0, 0);
    return;
  }

  char data = Serial2.read();
  bool valid = (data >= 65 && data <= 95);

  setERGB(valid, valid, valid);
  if (valid) updateScores(data);
}

// Decode and store scores from other groups
void updateScores(int data) {
  data -= 65;
  int group = data / 6;
  int score = data % 6;
  scores[group] = score;
}

// Wrapper class for Xbee transmit function and updates external LED
int sendXbee(char data) {
  setERGB(0, 0, 1);  // Set external RGB LED blue
  delay(250);
  setERGB(0, 0, 0);  // Turn off external RGB LED
  Serial2.print(data);
}

// Derock's Import Code

static int RGB_OFF[] = { 0, 0, 0 };

#define RGB_RED_PIN 45
#define RGB_GREEN_PIN 46
#define RGB_BLUE_PIN 44

/**
 * Sets the CS-Bot RGB
 */
void rgb_set(int *rgb) {
  analogWrite(RGB_RED_PIN, 255 - rgb[0]);
  analogWrite(RGB_GREEN_PIN, 255 - rgb[1]);
  analogWrite(RGB_BLUE_PIN, 255 - rgb[2]);
}


#define EXT_RGB_R_PIN A1
#define EXT_RGB_G_PIN A0
#define EXT_RGB_B_PIN A2


/**
 * Set the RGB to a [r, g, b] value
 * where r,g,b \in [0, 255]
 */
void ext_rgb_set(int (&rgb)[3]) {
  analogWrite(E_RED_PIN, rgb[0]);
  analogWrite(E_GREEN_PIN, rgb[1]);
  analogWrite(E_BLUE_PIN, rgb[2]);
}

#define LIGHT_SHOW_COUNT 97

struct LightShowEntry {
  bool is_external_led;
  int led_rgb[3];
  int delay;
};

LightShowEntry LIGHT_SHOW[LIGHT_SHOW_COUNT] = {
  { .is_external_led = true, .led_rgb = { 186, 113, 139 }, .delay = 144 },
  { .is_external_led = false, .led_rgb = { 0, 255, 0 }, .delay = 124 },
  { .is_external_led = true, .led_rgb = { 249, 164, 216 }, .delay = 94 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 123 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 148 },
  { .is_external_led = false, .led_rgb = { 255, 0, 255 }, .delay = 56 },
  { .is_external_led = true, .led_rgb = { 147, 12, 66 }, .delay = 79 },
  { .is_external_led = true, .led_rgb = { 141, 75, 134 }, .delay = 122 },
  { .is_external_led = true, .led_rgb = { 175, 71, 128 }, .delay = 76 },
  { .is_external_led = false, .led_rgb = { 0, 255, 255 }, .delay = 53 },
  { .is_external_led = true, .led_rgb = { 240, 80, 82 }, .delay = 69 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 70 },
  { .is_external_led = true, .led_rgb = { 172, 44, 13 }, .delay = 117 },
  { .is_external_led = true, .led_rgb = { 27, 186, 60 }, .delay = 142 },
  { .is_external_led = true, .led_rgb = { 144, 198, 170 }, .delay = 95 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 107 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 115 },
  { .is_external_led = true, .led_rgb = { 141, 93, 26 }, .delay = 91 },
  { .is_external_led = true, .led_rgb = { 60, 230, 31 }, .delay = 60 },
  { .is_external_led = true, .led_rgb = { 217, 42, 243 }, .delay = 99 },
  { .is_external_led = false, .led_rgb = { 0, 255, 0 }, .delay = 62 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 124 },
  { .is_external_led = true, .led_rgb = { 53, 225, 83 }, .delay = 73 },
  { .is_external_led = true, .led_rgb = { 99, 70, 112 }, .delay = 55 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 65 },
  { .is_external_led = true, .led_rgb = { 14, 84, 2 }, .delay = 144 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 69 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 117 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 145 },
  { .is_external_led = true, .led_rgb = { 14, 166, 40 }, .delay = 50 },
  { .is_external_led = true, .led_rgb = { 81, 212, 32 }, .delay = 68 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 89 },
  { .is_external_led = false, .led_rgb = { 0, 255, 0 }, .delay = 71 },
  { .is_external_led = true, .led_rgb = { 26, 245, 24 }, .delay = 116 },
  { .is_external_led = true, .led_rgb = { 140, 154, 52 }, .delay = 121 },
  { .is_external_led = true, .led_rgb = { 80, 145, 172 }, .delay = 131 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 112 },
  { .is_external_led = false, .led_rgb = { 0, 255, 255 }, .delay = 123 },
  { .is_external_led = true, .led_rgb = { 35, 183, 214 }, .delay = 113 },
  { .is_external_led = true, .led_rgb = { 131, 78, 150 }, .delay = 87 },
  { .is_external_led = true, .led_rgb = { 174, 177, 88 }, .delay = 132 },
  { .is_external_led = false, .led_rgb = { 0, 255, 255 }, .delay = 102 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 133 },
  { .is_external_led = true, .led_rgb = { 33, 14, 138 }, .delay = 107 },
  { .is_external_led = false, .led_rgb = { 0, 0, 0 }, .delay = 146 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 147 },
  { .is_external_led = false, .led_rgb = { 255, 255, 255 }, .delay = 147 },
  { .is_external_led = true, .led_rgb = { 69, 238, 103 }, .delay = 115 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 146 },
  { .is_external_led = false, .led_rgb = { 255, 0, 255 }, .delay = 61 },
  { .is_external_led = false, .led_rgb = { 0, 255, 255 }, .delay = 62 },
  { .is_external_led = true, .led_rgb = { 45, 11, 34 }, .delay = 88 },
  { .is_external_led = true, .led_rgb = { 239, 157, 185 }, .delay = 61 },
  { .is_external_led = true, .led_rgb = { 239, 59, 42 }, .delay = 147 },
  { .is_external_led = false, .led_rgb = { 0, 255, 255 }, .delay = 139 },
  { .is_external_led = false, .led_rgb = { 255, 0, 255 }, .delay = 116 },
  { .is_external_led = false, .led_rgb = { 255, 0, 255 }, .delay = 140 },
  { .is_external_led = false, .led_rgb = { 0, 0, 0 }, .delay = 95 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 101 },
  { .is_external_led = true, .led_rgb = { 99, 8, 72 }, .delay = 74 },
  { .is_external_led = true, .led_rgb = { 247, 162, 199 }, .delay = 132 },
  { .is_external_led = false, .led_rgb = { 255, 0, 255 }, .delay = 73 },
  { .is_external_led = true, .led_rgb = { 9, 78, 105 }, .delay = 137 },
  { .is_external_led = true, .led_rgb = { 245, 135, 248 }, .delay = 95 },
  { .is_external_led = true, .led_rgb = { 27, 170, 91 }, .delay = 106 },
  { .is_external_led = true, .led_rgb = { 216, 27, 23 }, .delay = 76 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 111 },
  { .is_external_led = false, .led_rgb = { 0, 0, 255 }, .delay = 56 },
  { .is_external_led = true, .led_rgb = { 174, 152, 23 }, .delay = 137 },
  { .is_external_led = true, .led_rgb = { 193, 112, 20 }, .delay = 65 },
  { .is_external_led = true, .led_rgb = { 112, 182, 81 }, .delay = 127 },
  { .is_external_led = true, .led_rgb = { 32, 130, 130 }, .delay = 121 },
  { .is_external_led = true, .led_rgb = { 206, 225, 165 }, .delay = 110 },
  { .is_external_led = true, .led_rgb = { 46, 46, 126 }, .delay = 66 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 96 },
  { .is_external_led = true, .led_rgb = { 27, 137, 227 }, .delay = 148 },
  { .is_external_led = true, .led_rgb = { 74, 71, 11 }, .delay = 94 },
  { .is_external_led = true, .led_rgb = { 81, 168, 133 }, .delay = 73 },
  { .is_external_led = true, .led_rgb = { 201, 213, 146 }, .delay = 57 },
  { .is_external_led = false, .led_rgb = { 255, 0, 255 }, .delay = 69 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 89 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 118 },
  { .is_external_led = false, .led_rgb = { 0, 0, 0 }, .delay = 122 },
  { .is_external_led = true, .led_rgb = { 168, 124, 175 }, .delay = 140 },
  { .is_external_led = true, .led_rgb = { 78, 223, 142 }, .delay = 76 },
  { .is_external_led = true, .led_rgb = { 81, 150, 135 }, .delay = 134 },
  { .is_external_led = false, .led_rgb = { 0, 255, 0 }, .delay = 124 },
  { .is_external_led = false, .led_rgb = { 255, 255, 255 }, .delay = 131 },
  { .is_external_led = true, .led_rgb = { 109, 194, 16 }, .delay = 144 },
  { .is_external_led = true, .led_rgb = { 41, 113, 84 }, .delay = 78 },
  { .is_external_led = false, .led_rgb = { 0, 255, 0 }, .delay = 136 },
  { .is_external_led = true, .led_rgb = { 133, 12, 54 }, .delay = 116 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 129 },
  { .is_external_led = true, .led_rgb = { 9, 15, 9 }, .delay = 62 },
  { .is_external_led = true, .led_rgb = { 183, 2, 144 }, .delay = 139 },
  { .is_external_led = false, .led_rgb = { 255, 255, 0 }, .delay = 110 },
  { .is_external_led = false, .led_rgb = { 255, 0, 0 }, .delay = 104 },
};

void lightshow_run() {
  for (int i = 0; i < LIGHT_SHOW_COUNT; i++) {
    LightShowEntry entry = LIGHT_SHOW[i];

    if (entry.is_external_led) {
      ext_rgb_set(entry.led_rgb);
    } else {
      rgb_set(entry.led_rgb);
    }

    delay(entry.delay);
  }

  rgb_set(RGB_OFF);
  ext_rgb_set(RGB_OFF);
}

// Dylan's Imported Code
const int songLen = 22;

int durs[songLen] = { 212, 212, 212, 211, 210, 210,  //one line per measure
                      212, 211, 210, 210, 213,
                      212, 212, 212, 211, 210, 210,
                      212, 211, 210, 210, 213 };

int octs[songLen] = { 215, 215, 215, 215, 216, 216,
                      215, 215, 215, 216, 215,
                      216, 216, 216, 216, 216, 216,
                      215, 215, 215, 216, 215 };

int notes[songLen] = { 230, 230, 230, 226, 232, 221,
                       230, 226, 232, 221, 230,
                       225, 225, 225, 226, 232, 221,
                       229, 226, 232, 221, 230 };

void imperial_march() {  //function to play the song
  for (long k = 0; k < songLen; k++) {
    Serial3.write(durs[k]);
    Serial3.write(octs[k]);
    Serial3.write(notes[k]);
    int len = 214 - durs[k];
    float del = 2000 / pow(2, len);
    delay(int(del * 1.1));
  }
}
