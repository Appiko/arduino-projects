#include "config.h"
#include <Servo.h>

#define CYCLE_FREQ_MS 10

#define CYCLES_10S (5 * 1000)/CYCLE_FREQ_MS

#define CYCLES_100MS 100/CYCLE_FREQ_MS

#define SERVO_MOTOR_PIN 2

#define TRIGG_1 13

#define TRIGG_2 12

#define TRIGG1_ERR1 0

#define TRIGG1_ERR2 1

#define TRIGG2_ERR1 2

#define TRIGG2_ERR2 3

Servo motor;

AdafruitIO_Feed *trig_log = io.feed("sensebe-radio-trig-log ");


void setup() {

  pinMode(TRIGG_1, INPUT); 
  pinMode(TRIGG_2, INPUT); 
  // put your setup code here, to run once:
  motor.attach(2);
  motor.write(0);
  
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    delay(500);
  }
}

static int err_cnt = 0;


void loop() {

  io.run();

  // put your main code here, to run repeatedly:
  motor.write(90);
  delay(1500);
  for(int d_ms = 0; d_ms < CYCLES_100MS; d_ms++)
  {
    if(digitalRead(TRIGG_1) == 1)
    {
      trig_log->save(TRIGG1_ERR1);
    }
//    delay(CYCLE_FREQ_MS);
    if(digitalRead(TRIGG_2) == 1)
    {
      trig_log->save(TRIGG2_ERR1);
    }
    delay(CYCLE_FREQ_MS);
  }
  motor.write(0);
  delay(3500);
  
  for(int d_ms = 0; d_ms < CYCLES_10S; d_ms++){
    if(digitalRead(TRIGG_1) == 0)
    {
      trig_log->save(TRIGG1_ERR2);
    }
//    delay(CYCLE_FREQ_MS);
    if(digitalRead(TRIGG_2) == 0)
    {
      trig_log->save(TRIGG2_ERR2);
    }
    delay(CYCLE_FREQ_MS);
  }


  

}
