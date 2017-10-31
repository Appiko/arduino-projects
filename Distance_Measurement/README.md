Objective		       	:	 Distance Measurement using VL6180X sensor with Arduino Pro Micro
=============
We want to use this distance measuring setup to measure the distance inside the enclosure of another product(length from top to bottom of enclosure). So whenever me make any changes inside the enclosure or on the PCB inside it we don't have to measure it again and again. Just close the enclosure and the change is displayed on an OLED display. 

Materials Required	:
====================
1.	Arduino Pro Micro (we used 5V, 16MHz cofiguration)
2.	VL6180X Time of Flight Micro-LIDAR distance sensor breakout board
3.	128x64 1.3" OLED graphical display with SSD1306 driver with controller
4.	Wires or jumpers
5.	USB cable (USB to USB-micro)

Circuit and Wiring	:
=====================
![Circuit](http://fritzing.org/media/fritzing-repo/projects/d/distance_measurement/images/FritzingDistMeasure_bb.jpg)
Drawn in Fritzing.
Connect the sensor and OLED with Pro Micro as shown in the diagram above. 
Use USB cable for supply, or alternatively, you can use any external source of upto 12V at RAW pin
(which will be regulated to 5V or 3.3V resp.). Do not give an unregulated supply if you want to use the VCC pin.
 
Program				:
=====================
We have used Adafruit libraries: "Adafruit_GFX.h", "Adafruit_SSD1306.h", "Adafruit_VL6180X.h" and arduino library "Wire.h".
VL6180X library is for the distance sensor.
GFX and SSD1306 libraries are for the OLED display and Wire.h is used for I2C protocol communication via Arduino Pro Micro.
The program is as following:
```c
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
```
Make the connections, connect arduino to your desktop. Select Board as "Arduino Leonardo", select the port and upload the code.
Now move your hand (or whatever object you want!) up and down over the distance sensor. You see the distance measured on the OLED! 

Observations :
==============
The VL6180X sensor shows output from a range of 5mm to 200mm (approx.). But the output was not very stable, so we tried to increase the READOUT__AVERAGING_SAMPLE_PERIOD{0x10A}  which according to the datasheet can be varied from 0 to 255. The default value is 48. We increased it to 100, 150 and finally 220, but no changes... the output is still flickering  "+/- 1mm".

Conclusion			:
==============
So this can be used at places where you want to measure small distances and it's not necessary to get a very very accurate measurement.
So we cannot use this setup for our original purpose, that is to measure the distance inside the enclosure as the final output keeps flickering.
