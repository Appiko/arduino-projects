/*************************************************
  This example is used when you want to simulate a
  button being held for certian amount of time
  before waking your Teensy from deepSleep sleep.

  This example uses the Bounce library to make sure
  the button is still pressed for 3 seconds after
  waking up from low power mode. If it released
  before the 3 seconds are up, go back to sleep.
 *************************************************/

#include <Snooze.h>
#include <Bounce.h>
#include <memorysaver.h>
#include <UTFT.h>
#include <ip.h>

#define GSM_WAKE_PIN 6
#define TEENSY_WAKE_PIN 11
#define CAGE_CLOSE_PIN 31


#define TINY_GSM_MODEM_SIM800
#define SERIAL_BUFFER_SIZE_RX 4000
#define SERIAL_BUFFER_SIZE_TX 2000
#define TINY_GSM_RX_BUFFER 2000

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#define SerialMon Serial
#define SerialAT Serial1


#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "‎portalnmms";
const char gprsUser[] = "";
const char gprsPass[] = "";

const char server[] = IP;
const int  port = 3600;

char image[30000];
int size = 0;


TinyGsm modem(SerialAT);

TinyGsmClient client(modem);
HttpClient http(client, server, port);

// Load drivers
SnoozeDigital digital;

// use bounce for pin 21, debounce of 5ms
Bounce button = Bounce(TEENSY_WAKE_PIN, 5);
SnoozeBlock config_teensy3x(digital);

void setup() {
  // Configure pin 2 for bounce library
  pinMode(TEENSY_WAKE_PIN, INPUT_PULLUP);
  digital.pinMode(TEENSY_WAKE_PIN, INPUT_PULLUP, FALLING);

  pinMode(GSM_WAKE_PIN, OUTPUT);
  digitalWrite(GSM_WAKE_PIN, LOW);

  pinMode(CAGE_CLOSE_PIN, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial2.begin(1000000);
  SerialAT.begin(115200);

  Serial.println("start...");
  delay(20);


}

void loop() {

SLEEP:

  button.update();
  digitalWrite(GSM_WAKE_PIN, LOW);
  Snooze.deepSleep( config_teensy3x );
  digitalWrite(LED_BUILTIN, HIGH);

  elapsedMillis timeout = 0;
  while (timeout < 6) button.update();

  // now check for 3 second button hold
  bool awake = holdMilli(90);

  if (!awake)
    goto SLEEP;

  elapsedMillis time = 0;
  int c = 0;
  char sizeArr[2];
  Serial2.read();

  digitalWrite(LED_BUILTIN, HIGH);

  while (1) {
    unsigned int t = time;
    delay(100);
    if (Serial2.available()) {
      Serial2.readBytes(sizeArr, 2);
      break;
    }
    if (t > 7000) {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("sleeping now :)");
      delay(5);
      goto SLEEP;
    }
  }


  size = sizeArr[0] + (256 * sizeArr[1]);
  int x, remaining;



  while (c < size) {
    unsigned int t = time;

    x = Serial2.available();
    remaining = size - c;
    if (x && (remaining >= 320)) {
      Serial2.readBytes(image + c, 320);
      c += 320;
    } else if (x) {
      Serial2.readBytes(image + c, remaining);
      c += remaining;
    }
    if (t > 5000) {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.print("Size:");
      Serial.println(size);
      delay(50);
      Serial.println("sleeping now :)");
      delay(5);
      goto SLEEP;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);




  if (onGSM()) {
    checkCageClose();
    if (sendImage()) {
      turnOffGSM();
      goto SLEEP;
    } else {
      Serial.println("PANIC: Image not sent");
      goto SLEEP;
    }

  } else {
    Serial.println("PANIC: No network");
    goto SLEEP;
  }

}

bool onGSM() {
  digitalWrite(GSM_WAKE_PIN, HIGH);
  modem.restart();
  Serial.println("Restart complete");
  //  modem.gprsConnect(apn, gprsUser, gprsPass);

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    return 0;
  }

  SerialMon.println(" OK");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    return 0;
  }
  SerialMon.println(" OK");
  return 1;
}


bool turnOffGSM() {
  digitalWrite(GSM_WAKE_PIN, HIGH);
  return 1;
}

bool checkCageClose() {
  int err = http.get("/cage-close");
  return err;
}

bool sendImage() {
  Serial.println("Sending image");
  SerialMon.print(F("Performing HTTP POST request... "));

  if (http.connect(server, port)) {
    http.println("POST /chunked/finally.png HTTP/1.1");
    http.print("Host: ");
    http.print(IP);
    http.println(":3600");
    http.println("User-Agent: Arduino/7.67.0");
    http.println("Accept: */*");
    http.println("Transfer-Encoding: chunked");
    http.println("Content-Type: application/x-www-form-urlencoded");
    http.println();
    int chunksize = 400;
    for (int i = 0; i < (size / chunksize); i++) {
      http.println(chunksize, HEX); //length in hex!!
      int x = http.write(&image[i * chunksize], chunksize);
      //      Serial.println(x);
      delay(5);
      if (x == 0) {
        return sendImage();
      }
      http.println();
      delay(300);

    }
    int left_out_bytes = size % chunksize;
    if (left_out_bytes) {
      http.println(left_out_bytes, HEX); //length in hex!!
      int x = http.write(&image[(size / chunksize) * chunksize], left_out_bytes);
      Serial.println(x);
      Serial.flush();
      if (x == 0) {
        return sendImage();
      }
      http.println();
      delay(200);
    }
    http.println("0");
    http.println();
  } else {
    SerialMon.printf("CRY");
    return (1);
  }
  SerialMon.println("end");

  int status = http.responseStatusCode();
  if (status != 200)
    return 0;
  return 1;
}


bool holdMilli(int milli) {
  while (button.duration() < milli) {
    button.update();
    if (button.read() != 0) {
      digitalWrite(LED_BUILTIN, LOW);
      return false;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
  return true;
}
