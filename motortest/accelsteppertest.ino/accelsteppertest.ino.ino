#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 5, 2);
const int enPin = 8;  // EN+

void setup() {
  
  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);

  stepper.setMaxSpeed(800);
  //stepper.setMinPulseWidth(50);
  // put your setup code here, to run once:
  stepper.setSpeed(400);
  stepper.setAcceleration(100);

}

void loop() {
  //stepper.runSpeed();
  stepper.move(200);
  stepper.runToPosition();
  stepper.move(-200);
  stepper.runToPosition();
}
