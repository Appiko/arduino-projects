/***<<<<<***Distance Measurement using VL6180X (Time of Flight Micro-LIDAR Distance Sensor) Breakout with Arduino Pro Micro***>>>>>
 *                  and showing the output on a 128x64 1.3" OLED graphical display
 *VL6180X and SSD1306 (OLED Driver IC) use I2C protocol under <Wire.h> library                  
 *Select Board "Arduino Leonardo" for Arduino Pro Micro 
 *if your Pro Micro works on 5V, 16MHz directly connnect with Sensor and OLED.
 */
#include <Wire.h>
#include <Adafruit_GFX.h>                                     //Adafruit library for OLED
#include <Adafruit_SSD1306.h>                                 //OLED Driver   
#include <Adafruit_VL6180X.h>                                 //Distance Sensor

Adafruit_SSD1306 display = Adafruit_SSD1306();                //creating objects    

Adafruit_VL6180X vl = Adafruit_VL6180X();

#define OLED_RESET 4

int sum = 0, x=0, avg=0, reads[10] = {0};

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(115200);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);                  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  delay(1000);
  for(int i = 0; i<10; i++)
  {   reads[i] = 0;
  }
  Serial.println("Adafruit VL6180x test!");                   //detect sensor connected or not
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Sensor found!");

}
  

void loop() 
{
  int range = vl.readRange();                               //read value from sensor
  int status = vl.readRangeStatus();                        //read status register of sensor 
                                              
    if (status == VL6180X_ERROR_NONE) {
    sum = sum - reads[x];                                   //floating average calculation for smooothing the analog readings
    reads[x] = vl.readRange();
    sum += reads[x];
    x += 1;
    if(x>=10)
    { x=0;
    }
    avg = sum/10;
    display.clearDisplay();
    display.setTextSize(1);                                 //showing text on OLED
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Range Sensor");
    display.print("\n");
    
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.print(avg); 
    display.print("mm");
    display.display();
  }
  else{
      display.display();                                    //keep the screen shut off when getting no reading from the sensor
      display.clearDisplay();
      return;
  }
}
