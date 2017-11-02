Objective		       	:	 Distance Measurement using VL6180X sensor with Arduino Pro Micro
=============
We want to use this distance measuring setup to measure the distance inside the enclosure of another product(length from top to bottom of enclosure). So whenever I make any changes inside the enclosure or on the PCB inside it we don't have to measure it again and again. Just close the enclosure and the change is displayed on an OLED display. 

Materials Required	:
====================
1.	Arduino Pro Micro (we used 5V, 16MHz configuration)
2.	VL6180X Time of Flight Micro-LIDAR distance sensor breakout board
3.	128x64 1.3" OLED graphical display with the SSD1306 driver with controller
4.	Wires or jumpers
5.	USB cable (USB to USB-micro)

Circuit and Wiring	:
=====================
![Circuit](http://fritzing.org/media/fritzing-repo/projects/d/distance_measurement/images/FritzingDistMeasure_bb.jpg)
Drawn in Fritzing.
Connect the sensor and OLED with Pro Micro as shown in the diagram above. 
Use USB cable for supply, or alternatively, you can use any external source of up to 12V at RAW pin
(which will be regulated to 5V or 3.3V resp.). Do not give an unregulated supply if you want to use the VCC pin.
 
Program				:
=====================
We have used Adafruit libraries: "Adafruit_GFX.h", "Adafruit_SSD1306.h", "Adafruit_VL6180X.h" and arduino library "Wire.h".
VL6180X library is for the distance sensor.
GFX and SSD1306 libraries are for the OLED display and Wire.h is used for I2C protocol communication via Arduino Pro Micro.
You can find the program (.ino file) "DistMeasure" here.
Make the connections, connect Arduino to your desktop. Select Board as "Arduino Leonardo", select the port and upload the code.
Now move your hand (or whatever object you want!) up and down over the distance sensor. You see the distance measured on the OLED! 

Observations :
==============
The VL6180X sensor shows output from a range of 5mm to 200mm (approx.). But the output was not very stable, so we tried to increase the READOUT__AVERAGING_SAMPLE_PERIOD{0x10A}  which according to the datasheet can be varied from 0 to 255. The default value is 48. We increased it to 100, 150 and finally 220, but no changes... the output is still flickering  "+/- 1mm".

Conclusion			:
==============
So this can be used at places where you want to measure small distances and it's not necessary to get a very very accurate measurement.
So we cannot use this setup for our original purpose, that is to measure the distance inside the enclosure as the final output keeps flickering.
