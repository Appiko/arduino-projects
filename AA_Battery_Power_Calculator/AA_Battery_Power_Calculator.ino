/*
 * *******************AA BATTERY POWER CALCULATOR***************************
 * 
 */

#include <ADC.h>
#include <SD.h>
#include <SPI.h>
 
int batRead = A9;                    // select the input pin for the AA battery
int voltageValue = 0;                // variable to store the value coming from the Battery
float val = 0;                       // for calculating Battery voltage

// Teensy 3.5 & 3.6 on-board: BUILTIN_SDCARD
const int chipSelect = BUILTIN_SDCARD;

ADC *adc = new ADC(); // adc object

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(batRead, INPUT);
  
  Serial.begin(9600);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
               ///ADC///
  adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
  adc->setAveraging(16); // set number of averages
  adc->setResolution(16); // set bits of resolution
  
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
  // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed
  
  //adc->startContinuous(batRead, ADC_0);
  
  delay(500);  
}



void loop() 
{
  
  // make a string for assembling the data to log:
  String dataString = "";
  
  //read Analog pin 0 for battery voltage level and append to the string:
  //Battery Voltage calculation
  voltageValue = analogRead(batRead);
  
  //val = voltageValue;
  val = (3.3 * voltageValue)/adc->getMaxValue(ADC_0);               
  Serial.print("\n Battery Voltage (in V): ");    //show value on Serial Monitor
  Serial.println(val); 
  Serial.println(adc->getMaxValue(ADC_0));
  Serial.println(voltageValue); 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  
  dataString += String(val);
  dataString += ","; 
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 

  //delay of 1 min.
  delay(6000);  
}
