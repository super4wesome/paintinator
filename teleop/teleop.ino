#include <AccelStepper.h>

const int NUM_STEPPERS = 2;
AccelStepper steppers[] = {
  AccelStepper(AccelStepper::DRIVER, 3 /* CW+ */, 2 /* CLK+ */),
  AccelStepper(AccelStepper::DRIVER, 5 /* CW+ */, 4 /* CLK+ */)
};

const int FACTOR = 16;
const int FULL_PULSES_PER_ROT = 200;
const int PULSES_PER_ROT = FULL_PULSES_PER_ROT * FACTOR / 2;
const int ACCELERATION = PULSES_PER_ROT * 2;
const int MAX_SPEED = PULSES_PER_ROT * 8;
const int TEST_SPEED = PULSES_PER_ROT * 4;
const int MIN_PULSE_WIDTH = 100;
const int HOMING_START_POSITION = 0;

void setupSteppers() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {    
    steppers[i].setMaxSpeed(MAX_SPEED);
    steppers[i].setMinPulseWidth(MIN_PULSE_WIDTH);
    steppers[i].setAcceleration(ACCELERATION);
  
    // homing reset
    steppers[i].setCurrentPosition(HOMING_START_POSITION);
  
    // TODO
    steppers[i].setSpeed(0);
  }
}

void runAllSteppers() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    steppers[i].runSpeed();
  }
}

void stopAllSteppers() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    steppers[i].stop();
  }
}

void printSpeeds() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    Serial.println("Motor " + String(i) + " speed: " + String(steppers[i].speed()));
  }
}

void setup() {
  setupSteppers();
  Serial.begin(9600);
  Serial.println("initialized");
}

void loop() {  
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case '0': stopAllSteppers(); break;
      case 'w': steppers[0].setSpeed(steppers[0].speed() + 100 * FACTOR); break;
      case 's': steppers[0].setSpeed(steppers[0].speed() - 100 * FACTOR); break;
      case 'o': steppers[1].setSpeed(steppers[1].speed() + 100 * FACTOR); break;
      case 'l': steppers[1].setSpeed(steppers[1].speed() - 100 * FACTOR); break;
      default: break;
    }
    printSpeeds();
  }
  runAllSteppers();
}
