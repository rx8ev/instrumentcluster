# Instrument Cluster
This is an arduino code to control the Rx8 instrument cluster using the CAN bus.
This code is based on the code described here:
https://www.chamberofunderstanding.co.uk/2018/07/12/rx8-project-part-13-canbus-3-labview/

## Fuel Gauge
The RX8 uses a saddle shaped fuel tank to allow it to clear the driveshaft and so it has two senders denoted as ‘main’ and ‘sub’. The following pinout shows their locations.

![](https://www.chamberofunderstanding.co.uk/wp-content/uploads/2017/12/RX8-Cluster-Pinout.jpg)

 - Each of these  has a resistance value of between about 325Ω at the bottom and 10Ω at the top. 
 - Putting 100Ω a resistor between both 2R-2P and 2R-2T gives a value of about 1/3 of a tank and 
 
Pins 3, 4 and 5 control the fuel gauge using a PWM, through a DC motor driver, for example, L293D (one side of it, at least).

 - Pins 2R & 2P connect in series to pin 3 (out 1) on the chip. 2T connects to pin 3 (out 2)
 - Pin 3 connects to pin 7 (in 2)
 - Pin 4 connects to pin 2 (in 1)
 - Pin 5 connects to pin 1 (enable)

A PWM duty cycle of 0->100 indicates an empty tank
A PWM duty cycle of 255 indicates a full tank
