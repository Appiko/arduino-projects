#include "config.h"
#include "adafruit_uploader_config.h"
#include <Timer.h>

AdafruitIO_Feed *logger = io.feed(FEED_NAME);
Timer t;

void setup() {

  // start the serial connection
  Serial.begin(BAUD_RATE);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
	t.every(1*60*60*1000, sayAlive, 0);
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

	logger -> save("Resetting");

	pinMode(3, FUNCTION_3); // Change Rx to GPIO3
	attachInterrupt(digitalPinToInterrupt(3), logTrigger, FALLING);
}

void loop() {
  io.run();
	t.update();
}

ICACHE_RAM_ATTR
void logTrigger() {
	String message = "triggered";
	Serial.println(message);
	logger -> save(message);
}

void sayAlive(void* context) {
	logger -> save("Alive");
}