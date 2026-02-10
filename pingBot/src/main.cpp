#include <Arduino.h>
#include "sensors/Qti.h"

#define QTI_0 47
#define QTI_1 51
#define QTI_2 52

Qti sensor0(QTI_0);
Qti sensor1(QTI_1);
Qti sensor2(QTI_2);

void setup() {
  Serial.begin(9600);
}

void loop() {
  long value0 = sensor0.read();
  long value1 = sensor1.read();
  long value2 = sensor2.read();

  // Print data
  Serial.print("QTI Discharge Times: ");
  Serial.print(value0);
  Serial.print(" . ");
  Serial.print(value1);
  Serial.print(" | ");
  Serial.println(value2);

  delay(200);
}