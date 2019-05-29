#include <Servo.h>
Servo motor;
void setup() {
  // put your setup code here, to run once:
  motor.attach(2);
  motor.write(0);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  motor.write(90);
  delay(1500);
  motor.write(0);
  delay(3500);

}
