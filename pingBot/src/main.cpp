#include <Arduino.h>
#include "sensors/QtiController.h"

void setup() {
  Serial.begin(9600);
}

QtiController controller;

void loop() {
  Serial.println(controller.requestState());
  delay(200);
}