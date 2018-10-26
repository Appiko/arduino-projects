#include <TimeLib.h>
#include <Wire.h>

#include <SD.h>
#include <SPI.h>

#define WAVE1_DUR_BY2 1
#define CONST_MAX 2475
#define CONST_MIN 2400
#define DA0 A21
#define DA1 A22

typedef enum dac_val_status_t  {
  LOW_LIMIT,
  IN_LIMIT,
  HIGH_LIMIT,
};

File logfile;

const int led = 13;

const int chipSelect = BUILTIN_SDCARD;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Serial1.setRX(0);
  Serial1.begin(1000000);
  while(!Serial1);
  Serial1.setTimeout(1);
  if (!SD.begin(chipSelect)) {
    return;
  }
  else
  {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
  logfile = SD.open("FPVES.CSV", FILE_WRITE);
  if(logfile)
  {
    logfile.close();
  }
  else
  {
    logfile = SD.open("FPVES.CSV", FILE_WRITE);
    if(logfile)
    {
      logfile.println("False Positives\n");
      logfile.close();
    }
    else
    {
      Serial.println("FILE NOT FOUND..!!");
    }  
  }
  

  pinMode(DA0, OUTPUT);
  pinMode(DA1, OUTPUT);

}

dac_val_status_t dac0_val_status = LOW_LIMIT;
int dac0_val = 0;
int dac0_step = 1;
bool dac0_rise_fall_flag = true;

dac_val_status_t dac1_val_status = LOW_LIMIT;
int dac1_val = CONST_MIN;
int dac1_step = 15;
bool dac1_rise_fall_flag = true;


void loop() {
  // put your main code here, to run repeatedly:
 switch(dac0_val_status)
 {
  case LOW_LIMIT : 
      dac0_rise_fall_flag = true;
      dac0_val += dac0_step;
      dac0_val_status = IN_LIMIT;
    break;
  case IN_LIMIT :
      if(dac0_val > CONST_MAX)
      {
        dac0_val_status = HIGH_LIMIT;
      }
      else if(dac0_val < CONST_MIN)
      {
        dac0_val_status = LOW_LIMIT;
      }
      if(dac0_rise_fall_flag == true)
      {
        dac0_val += dac0_step;
      }
      else
      {
        dac0_val -= dac0_step;
      }
    break;
  case HIGH_LIMIT :
      dac0_rise_fall_flag = false;
      dac0_val -= dac0_step;
      dac0_val_status = IN_LIMIT;
    break; 
 }
 analogWriteResolution(12);
 analogWrite(DA0, dac0_val);

 switch(dac1_val_status)
 {
  case LOW_LIMIT : 
      dac1_rise_fall_flag = true;
      dac1_val += dac1_step;
      dac1_val_status = IN_LIMIT;
    break;
  case IN_LIMIT :
      if(dac1_val > CONST_MAX)
      {
        dac1_val_status = HIGH_LIMIT;
      }
      else if(dac1_val < CONST_MIN)
      {
        dac1_val_status = LOW_LIMIT;
      }
      if(dac1_rise_fall_flag == true)
      {
        dac1_val += dac1_step;
      }
      else
      {
        dac1_val -= dac1_step;
      }
    break;
  case HIGH_LIMIT :
      dac1_rise_fall_flag = false;
      dac1_val -= dac0_step;
      dac1_val_status = IN_LIMIT;
    break; 
 }
 analogWriteResolution(12);
 analogWrite(DA1, dac1_val);
 if(Serial1.available() > 0)
 {
    String dataString;
    dataString  = Serial1.readString();
    logfile = SD.open("FPVES.CSV", FILE_WRITE);
    if(logfile)
    {
      logfile.println(dataString);
      logfile.close();
    }
 }
 delay(50);

}
