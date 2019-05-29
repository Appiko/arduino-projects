#include "config.h"


AdafruitIO_Feed *light_log_1 = io.feed("long-dist-rssi ");
//AdafruitIO_Feed *light_log_2 = io.feed("light-intensity-log.sensebe-rx-log-2");
void setup() {
  // put your setup code here, to run once:
//  Serial2.setRX(13);
//  Serial2.begin(1000000);
//  while(!Serial2);
//  Serial2.setTimeout(1);

//  Serial.setRX(3);
  Serial.begin(1000000);
  while(!Serial);
  Serial.setTimeout(1);
  
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    delay(500);
  }
}
String light_sense_val1 = "";
//String light_sense_val2 = "";

void loop() {
  // put your main code here, to run repeatedly:
  io.run();
  if(Serial.available() > 1)
  {
    light_sense_val1 = Serial.readString();
    if(light_sense_val1 != "")
    {
      light_log_1 -> save(light_sense_val1.toInt());
    }
    
  }
//  if(Serial2.available() > 1)
//  {
//    light_sense_val2 = Serial2.readString();
//    if(light_sense_val2 != "")
//    {
//      light_log_2 -> save(light_sense_val2.toInt());
//    }
//    
//  }

}
