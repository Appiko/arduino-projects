###Objective			:	#Distance Measurement using VL6180X sensor and Arduino Pro Micro

###Materials Required	:

1.	Arduino Pro Micro (we used 5V, 16MHz cofiguration)
2.	VL6180X Time of Flight Micro-LIDAR distance sensor breakout board
3.	128x64 1.3" OLED graphical display with SSD1306 driver with controller
4.	Wires or jumpers
5.	USB cable (USB to USB-micro)

Circuit and Wiring	:
=====================
![Circuit](http://fritzing.org/media/fritzing-repo/projects/d/distance_measurement/images/FritzingDistMeasure_bb.jpg)
Drawn in Fritzing.
Connect the sensor and OLED with Pro Micro as shown in the diagram. 
Use USB cable for supply, or alternatively, you can use any external source of upto 12V at RAW pin
(which will be regulated to 5V or 3.3V resp.). Do not give an unregulated supply if you want to use the VCC pin.
 
Program				:
=====================
We have used Adafruit libraries: "Adafruit_GFX.h", "Adafruit_SSD1306.h", "Adafruit_VL6180X.h" and arduino library "Wire.h".
VL6180X library is for the distance sensor.
GFX and SSD1306 libraries are for the OLED display and Wire.h is used for I2C protocol communication via Arduino Pro Micro.

Conclusion			:
=====================
