#include <Servo.h>
Servo motor;
void setup() {
  // put your setup code here, to run once:
  motor.attach(2);
  motor.write(0);
  delay(500);
}

void loop() {

	int slowness = 60;
	int angle = 95;
  // put your main code here, to run repeatedly:
  for(int i = 0; i < angle; i += 15){
		motor.write(i);
		delay(slowness);
	}

	delay(1000);

	for(int i = angle; i > 0; i -= 15){
		motor.write(i);
		delay(slowness);
	}

  delay(1000);

}
