# Instrument Cluster
This is an arduino code to control the Rx8 instrument cluster using the CAN bus.
This code is based on the code described here:
https://www.chamberofunderstanding.co.uk/2018/07/12/rx8-project-part-13-canbus-3-labview/

## Fuel Gauge
(This is a work in progress)
The RX8 uses a saddle shaped fuel tank to allow it to clear the driveshaft and so it has two senders denoted as ‘main’ and ‘sub’.

![](https://www.chamberofunderstanding.co.uk/wp-content/uploads/2017/12/RX8-Cluster-Pinout.jpg)

 - Each of these  has a resistance value of between about 325Ω at the bottom and 10Ω at the top. 
 - Putting 100Ω a resistor between both 2R-2P and 2R-2T gives a value of about 1/3 of a tank and 
 - 9Ω (adding a 10Ω on along side the existing 100Ω) across these connections gives a full tank reading (actually a little above full)
 - It takes a long time to move up the scale, so be patient.
