/*
 * *******************AA BATTERY LIFE CALCULATOR***************************
 * 
 */
int batPin = A0;                     // select the input pin for the AA battery
int voltageValue = 0;                // variable to store the value coming from the Battery
float val = 0;                       // for calculating Battery voltage
unsigned long time;

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  voltageValue = analogRead(batPin);              //read the value of the AA Battery

  val = (4.88 * voltageValue)/1000;               //Battery Voltage calculation
  
  Serial.print("\n Battery Voltage (in V): ");    // show value on Serial Monitor
  Serial.println(val); 
  delay(4000);                                    //delay for 5 seconds

  //Serial.print("Time: ");
  //time = millis();
  //Serial.println(time);    //prints time since program started
  //delay(1000);
}
