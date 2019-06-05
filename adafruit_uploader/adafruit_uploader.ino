#include "config.h"
#include "adafruit_uploader_config.h"

AdafruitIO_Feed *logger = io.feed(FEED_NAME);

void setup() {

  // start the serial connection
  Serial.begin(BAUD_RATE);

  // wait for serial monitor to open
  while(! Serial);
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {

  io.run();
  
  if(Serial.available() > 0) {

    String data = Serial.readString();
    Serial.println(data);
    logger -> save(data);
  
  }

  delay(300);

}
