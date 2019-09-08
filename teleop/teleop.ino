#include <AccelStepper.h>
#include <Servo.h>

// -------------------------------------------------------------
const int NUM_STEPPERS = 2;
AccelStepper steppers[] = {
  AccelStepper(AccelStepper::DRIVER, 2 /* CLK+ */, 3 /* CW+ */),
  AccelStepper(AccelStepper::DRIVER, 4 /* CLK+ */, 5 /* CW+ */)
};

const int FACTOR = 16;
const int FULL_PULSES_PER_ROT = 200;
const int PULSES_PER_ROT = FULL_PULSES_PER_ROT * FACTOR / 2;
const int ACCELERATION = PULSES_PER_ROT * 5;
const int MAX_SPEED = PULSES_PER_ROT * 8;
const int HOMING_SPEED = PULSES_PER_ROT * 8;
const int MINIMUM_PULSE_WIDTH = 100;
const int HOMING_START_POSITION = 0;

bool constant_speed = false;
bool spraying = false;
char previous_cmd = '.';

// -------------------------------------------------------------
const int SPRAY_SERVO_PIN = 12;
const int SERVO_SPRAY_POSITION = 90;
const int SERVO_RELAX_POSITION = 180;
Servo spray_servo;

// -------------------------------------------------------------
const int LED_BOARD_PIN = 13;

// -------------------------------------------------------------

long seq_test2[][2] = {
  {0, 0},
  {-23000, 23000},
  {5600, 9300},
  {-14000, 38000},
  {21000, 15000},
  {0, 0}
};

long seq_test[][2] = {
 {0, 0},
 {-10000, 10000},
 {-1000, 20000},
 {12000, 8000},
 {0, 0}
};

// -------------------------------------------------------------

void setupSteppers() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {    
    steppers[i].setMaxSpeed(MAX_SPEED);
    steppers[i].setMinPulseWidth(MINIMUM_PULSE_WIDTH);
    steppers[i].setAcceleration(ACCELERATION);
  
    // homing reset
    steppers[i].setCurrentPosition(HOMING_START_POSITION);
  
    // TODO
    steppers[i].setSpeed(0);

    spray_servo.attach(SPRAY_SERVO_PIN);
    spray_servo.write(SERVO_RELAX_POSITION);

    pinMode(LED_BOARD_PIN, OUTPUT);
  }
}

void runAllSteppers(bool constant_speed = true) {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    if (constant_speed) {
      steppers[i].runSpeed(); 
    } else {
      steppers[i].run();
    }
  }
}

bool running() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    if (steppers[i].isRunning()) {
      return true;
    }
  }
  return false;
}

void stopAllSteppers() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
     steppers[i].setSpeed(0);
     steppers[i].runSpeed();
     steppers[i].moveTo(steppers[i].currentPosition());
     steppers[i].move(0);
  }
  ledStandby();
}

void homeAllSteppers() {
  ledRolling();
  for (int i = 0; i < NUM_STEPPERS; ++i) {
     if (steppers[i].currentPosition() == HOMING_START_POSITION) {
      continue;
     }
     steppers[i].moveTo(HOMING_START_POSITION);
  }
}

void printSpeeds() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    Serial.println("Motor " + String(i) + " speed: " + String(steppers[i].speed()));
  }
}

void printPositions() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    Serial.println("Motor " + String(i) + " position: " + String(steppers[i].currentPosition()));
  }
}

void printGoals() {
  for (int i = 0; i < NUM_STEPPERS; ++i) {
    Serial.println("Motor " + String(i) + " goal: " + String(steppers[i].targetPosition()));
  }
}

bool charInputIs(char c) {
  //delay(1);
  if (Serial.available()) {
    return (Serial.read() == c);
  }
  return false;
}

bool stopRequested() {
  return charInputIs('0');
}

bool sprayToggleRequested() {
  return charInputIs('x');
}

// Returns false if stopped before reaching goal.
bool moveToPosition(long position_0, long position_1) {
  ledRolling();
  steppers[0].moveTo(position_0);
  //steppers[0].setSpeed(HOMING_SPEED);
  steppers[1].moveTo(position_1);
  //steppers[1].setSpeed(HOMING_SPEED);
  while (steppers[0].currentPosition() != position_0
      && steppers[1].currentPosition() != position_1) {
   if (stopRequested()) {
    Serial.println("Stop requested!");
    previous_cmd = 's';
    stopAllSteppers();
    return false;
   }
   if (sprayToggleRequested()) {
    toggleSprayer();
   }
   runAllSteppers(false);
  }
  stopAllSteppers();
  return true;
}

void runSequence(long positions[][2], int num_positions) {
  Serial.println("Starting sequence");
  for (int i = 0; i < num_positions; ++i) {
    Serial.println("Moving to " + String(positions[i][0]) + " " + String(positions[i][1]));
    if (!moveToPosition(positions[i][0], positions[i][1])) {
      break;
    }
  }
  ledStandby();
}

void runTestSequence() {
  // #hackerman
  runSequence(seq_test, sizeof(seq_test)/sizeof(seq_test[0]));
}

void toggleSprayer() {
  spraying = !spraying;
  if (spraying) {
    Serial.println("Spraying!");
    spray_servo.write(SERVO_SPRAY_POSITION);
  } else {
    Serial.println("Not spraying!");
    spray_servo.write(SERVO_RELAX_POSITION);
  }
}

void ledStandby() {
  //Serial.println("LEDs set to Standby");
  digitalWrite(LED_BOARD_PIN, LOW);
}

void ledRolling() {
  //Serial.println("LEDs set to Rolling");
  digitalWrite(LED_BOARD_PIN, HIGH);
}

void setup() {
  setupSteppers();
  Serial.begin(9600);
  Serial.println("initialized - use PUTTY for smooth control!");
}

void loop() {
  if (previous_cmd == 'r') {
    runSequence(seq_test, sizeof(seq_test)/sizeof(seq_test[0]));
  }
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case '0': stopAllSteppers(); break;
      case 'h': stopAllSteppers(); homeAllSteppers(); break;
      case 'p': printPositions(); break;
      case 'g': printGoals(); break;
      case 'x': toggleSprayer(); break;
      case 'r': runTestSequence(); break;
      // big steps
      case 'W': steppers[0].setSpeed(steppers[0].speed() - 100 * FACTOR); ledRolling(); break;
      case 'S': steppers[0].setSpeed(steppers[0].speed() + 100 * FACTOR); ledRolling(); break;
      case 'O': steppers[1].setSpeed(steppers[1].speed() + 100 * FACTOR); ledRolling(); break;
      case 'L': steppers[1].setSpeed(steppers[1].speed() - 100 * FACTOR); ledRolling(); break;
      // small steps
      case 'w': steppers[0].setSpeed(steppers[0].speed() - 10 * FACTOR); ledRolling(); break;
      case 's': steppers[0].setSpeed(steppers[0].speed() + 10 * FACTOR); ledRolling(); break;
      case 'o': steppers[1].setSpeed(steppers[1].speed() + 10 * FACTOR); ledRolling(); break;
      case 'l': steppers[1].setSpeed(steppers[1].speed() - 10 * FACTOR); ledRolling(); break;
      default: break;
    }
    previous_cmd = cmd;
    bool homing = (cmd == 'h' || previous_cmd == 'h');
    if (cmd != 'p') {
      constant_speed = !homing;
      printSpeeds();
    }
  }
  runAllSteppers(constant_speed);
  if (running() || spraying) {
    ledRolling();
  } else {
    ledStandby();
  }
}
