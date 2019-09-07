#include <AccelStepper.h>

const int num_steppers = 2;
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

void setup() {
  for (int i = 0; i < num_steppers; ++i) {
    steppers[i].setPinsInverted(false, false, true);
    steppers[i].setEnablePin(8);
    steppers[i].enableOutputs();
    steppers[i].setMaxSpeed(MAX_SPEED);
    steppers[i].setMinPulseWidth(MIN_PULSE_WIDTH);
    steppers[i].setAcceleration(ACCELERATION);
  
    // homing reset
    steppers[i].setCurrentPosition(HOMING_START_POSITION);
  
    // TODO
    steppers[i].setSpeed(TEST_SPEED);
    steppers[i].moveTo(PULSES_PER_ROT);
  }
}

void loop() {  
  if (!steppers[0].run()) {
    steppers[0].moveTo(-steppers[0].targetPosition());
  }
  if (!steppers[1].run()) {
    steppers[1].moveTo(-steppers[1].targetPosition());
  }
}
