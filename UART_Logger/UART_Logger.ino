#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

#include <SD.h>
#include <SPI.h>

File logfile1, logfile2, logfile3, logfile;

const int led = 13;

const int chipSelect = BUILTIN_SDCARD;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Serial1.setRX(0);
  Serial1.begin(1000000);
  while(!Serial1);
  Serial1.setTimeout(1);

  Serial2.setRX(9);
  Serial2.begin(1000000);
  while(!Serial2);
  Serial2.setTimeout(1);

  Serial3.setRX(7);
  Serial3.begin(1000000);
  while(!Serial3);
  Serial3.setTimeout(1);

//  Serial.begin(1000000);
//  while (!Serial) ; // Needed for Leonardo only
//  Serial.println("Welcome..!!");
  if (!SD.begin(chipSelect)) {
//    Serial.println("Card failed, or not present");
    // don't do anything more:
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
  logfile1 = SD.open("SPN1.csv");
  if(logfile1)
  {
//    Serial.println("File 1 already exists..!!");
    logfile1.close();
  }
  else
  {
    logfile1 = SD.open("SPN1.csv", FILE_WRITE);
    if(logfile1)
    {
//      Serial.println("File Created..!!!");
      logfile1.println("light_reading,time_date \n");
      logfile1.close();
    }
    else
    {
//      Serial.println("FILE NOT FOUND..!!");
    }  
  }
  
  logfile2 = SD.open("SPN2.csv");
  if(logfile2)
  {
//    Serial.println("File 2 already exists..!!");
    logfile2.close();
  }
  else
  {
    logfile2 = SD.open("SPN2.csv", FILE_WRITE);
    if(logfile2)
    {
//      Serial.println("File Created..!!!");
      logfile2.println("light_reading,time_date \n");
      logfile2.close();
    }
    else
    {
//      Serial.println("FILE NOT FOUND..!!");
    }  
  }
  
  logfile3 = SD.open("SPN3.csv");
  if(logfile3)
  {
//    Serial.println("File 3 already exists..!!");
    logfile3.close();
  }
  else
  {
    logfile1 = SD.open("SPN3.csv", FILE_WRITE);
    if(logfile3)
    {
//      Serial.println("File Created..!!!");
      logfile3.println("light_reading,time_date \n");
      logfile3.close();
    }
    else
    {
//      Serial.println("FILE NOT FOUND..!!");
    }  
  }


  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  int curr_time = 1532960100 + 19800;
  adjustTime(curr_time);
}
String board1, board2, board3;
void loop() {
  // put your main code here, to run repeatedly:

  if(Serial1.available() > 0)
  {
    board1 = Serial1.readString();
    if(board1 != "")
    {
      String dataString = String(board1 + "," + digitalClockDisplay());
      logfile1 = SD.open("SPN1.csv", FILE_WRITE);
      if(logfile1)
      {
        logfile1.println(dataString);
        logfile1.close();
      }
    }
  }


  if(Serial2.available() > 0)
  {
    board2 = Serial2.readString();
    if(board2 != "")
    {
      String dataString = String(board2 + "," + digitalClockDisplay());
      logfile2 = SD.open("SPN2.csv", FILE_WRITE);
      if(logfile2)
      {
        logfile2.println(dataString);
        logfile2.close();
      }
    }
  }


  if(Serial3.available() > 0)
  {
    board3 = Serial3.readString();
    if(board3 != "")
    {
      String dataString = String(board3 + "," + digitalClockDisplay());
      logfile3 = SD.open("SPN3.csv", FILE_WRITE);
      if(logfile3)
      {
        logfile3.println(dataString);
        logfile3.close();
      }
    }
  }
}


String digitalClockDisplay(){
  // digital clock display of the time
  String retString;
  retString = String( String(hour()) +   printDigits(minute()) +   printDigits(second())
  + " " + String(day()) + " "+String(month())+ "");
//  Serial.println(); 
  return retString;
}

String printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  String retString = ":";
  if(digits < 10)
    retString = String(retString + "0" + String(digits));
  else
    retString = String(retString + String(digits));
  return retString;
}

