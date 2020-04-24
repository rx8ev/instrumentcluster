/*
 * Taken from: https://www.chamberofunderstanding.co.uk/2017/12/02/rx8-project-part-6-canbus-2/
 */

#include <Arduino.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>


#define COMMAND 0xFE
#define CLEAR 0x01
#define LINE0 0x80
#define LINE1 0xC0

#define CANint 2
#define LED2 8
#define LED3 7

#define NOP __asm__ ("nop\n\t")

// Variables for StatusMIL
bool checkEngineMIL;
bool checkEngineBL;
byte engTemp;
byte odo;
bool oilPressure;
bool lowWaterMIL;
bool batChargeMIL;
bool oilPressureMIL;

// Variables for DSC
bool dscOff;
bool absMIL;
bool brakeFailMIL;
bool etcActiveBL;
bool etcDisabled;

//READING THE RX8 PEDAL. THis has 2 sensors, 1 and 2.
const int pedal_1_pin = 0; // analog pin connected to sensor 1
const int pedal_2_pin = 1; // analog pin connected to sensor 2

const int fuelGauge1_pin = 2;
const int fuelGauge2_pin = 3;

int pedal1ReadingsCurve[] = {200, 550};
int pedal2ReadingsCurve[] = {350, 550};

MCP_CAN CAN0(10); // Set CS to pin 10

void setup()
{

    Serial.begin(9600);
    Serial.println("Init…");
    Serial.println("Setup pins");
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(CANint, INPUT);
    pinMode(fuelGauge1_pin, OUTPUT);
    pinMode(fuelGauge2_pin, OUTPUT);

    //Serial.println("Enable pullups");
    digitalWrite(LED2, LOW);
    //Serial.println("CAN init:");

    if (CAN0.begin(CAN_500KBPS) == CAN_OK)
    {
        //Serial.println("OK!");
    }
    else
    {
        //Serial.println("fail :-(");
        while (1)
        {
            //Serial.print("Zzz… ");
            delay(1000);
        }
     }

Serial.println("Good to go!");
}

byte stmp[8]       = {0, 0, 0, 0, 0, 0, 0, 0};                         // Always Off Array
byte otmp[8]       = {255,255,255,255,255,255,255,255};                // Always On Array

byte statusPCM[16] = {125,0,0,0,156,0,0,0};                            // Write to 201
byte statusMIL[8]  = {140,0,0,0,0,0,0,0};                              // Write to 420
byte statusDSC[8]  = {0,0,0,0,0,0,0,0};                                // Write to 212
byte statusPS[8]   = {0,0,0,0,0,0,0,0};                                // Write to 300

byte statusEPS1[8] = {0x00,0x00,0xFF,0xFF,0x00,0x32,0x06,0x81};        // Write to 200 0x00 00 FF FF 00 32 06 81
byte statusEPS2[8] = {0x89,0x89,0x89,0x19,0x34,0x1F,0xC8,0xFF};        // Write to 202 0x89 89 89 19 34 1F C8 FF

byte statusECU1[8] = {0x02,0x2D,0x02,0x2D,0x02,0x2A,0x06,0x81};        // Write to 215 - Unknown
byte statusECU2[8] = {0x0F,0x00,0xFF,0xFF,0x02,0x2D,0x06,0x81};        // Write to 231 - Unknown
byte statusECU3[8] = {0x04,0x00,0x28,0x00,0x02,0x37,0x06,0x81};        // Write to 240 - Unknown
byte statusECU4[8] = {0x00,0x00,0xCF,0x87,0x7F,0x83,0x00,0x00};        // Write to 250 - Unknown


/*

215 02 2D 02 2D 02 2A 06 81 // Some ECU status

231 0F 00 FF FF 02 2D 06 81 // Some ECU status

240 04 00 28 00 02 37 06 81 // Some ECU status

250 00 00 CF 87 7F 83 00 00 // Some ECU status

*/


void updateMIL()
{
    statusMIL[0] = engTemp;
    statusMIL[1] = odo;
    statusMIL[4] = oilPressure;

  if (checkEngineMIL == 1)
  {
    statusMIL[5] = statusMIL[5] | 0b01000000;
  }
  else
  {
    statusMIL[5] = statusMIL[5] & 0b10111111;
  }

    if (checkEngineBL == 1)
  {
    statusMIL[5] = statusMIL[5] | 0b10000000;
  }
  else
  {
    statusMIL[5] = statusMIL[5] & 0b01111111;
  }

   if (lowWaterMIL == 1)
  {
    statusMIL[6] = statusMIL[6] | 0b00000010;
  }
  else
  {
    statusMIL[6] = statusMIL[6] & 0b11111101;
  }

     if (batChargeMIL == 1)
  {
    statusMIL[6] = statusMIL[6] | 0b01000000;
  }
  else
  {
    statusMIL[6] = statusMIL[6] & 0b10111111;
  }

       if (oilPressureMIL == 1)
  {
    statusMIL[6] = statusMIL[6] | 0b10000000;
  }
  else
  {
    statusMIL[6] = statusMIL[6] & 0b01111111;
  }
}

void updatePCM(byte engRPM, byte vehicleSpeed)
{

    //The 2nd byte for each reading (1 and 4) are for
    //greater definition. THe first byte is what makes the gauges active

    statusPCM[0] = engRPM;
    statusPCM[1] = 0; // what should this be?
  
    statusPCM[4] = vehicleSpeed;
    statusPCM[5] = byte(135); //WHy is this 135???
}

void updatePS(bool on){
  if (on == true){
    statusPS[0] = 128;
  }else{
    statusPS[0] = 0;
  };
  Serial.println(statusPS[0]);
}

void updateDSC()
{
  if (dscOff == 1)
  {
    statusDSC[3] = statusDSC[3] | 0b00000100;
  }
  else
  {
    statusDSC[3] = statusDSC[3] & 0b01111011;
  }

    if (absMIL == 1)
  {
    statusDSC[4] = statusDSC[4] | 0b00001000;
  }
  else
  {
    statusDSC[4] = statusDSC[4] & 0b11110111;
  }

      if (brakeFailMIL == 1)
  {
    statusDSC[4] = statusDSC[4] | 0b01000000;
  }
  else
  {
    statusDSC[4] = statusDSC[4] & 0b10111111;
  }

     if (etcActiveBL == 1)
  {
     statusDSC[5] = statusDSC[5] | 0b00100000;
  }
  else
  {
    statusDSC[5] = statusDSC[5] & 0b11011111;
  }

    if (etcDisabled == 1)
  {
    statusDSC[5] = statusDSC[5] | 0b00010000;
  }
  else
  {
    statusDSC[5] = statusDSC[5] & 0b11101111;
  }


}

byte getSpeedMsgValue(byte speed){
  /*
   *  Create a value to send to the CAN bus for vehicle speed.
   *
   *  Pass in the required speed, returns the CAN code for that speed
   *
   *  Calculation is: 0.63*(Speed)+38.5
   *
   *  For the sake of simplicity, this calculation returns an int.
   *  TODO: Is this a sensible solution?
   */
  //float multiplier=0.63;
  float multiplier=0.621371; // this is the kph calculation. 
  float constant=38.5;
  float speedCode;

  speedCode = (multiplier * speed) + constant;
 // Serial.print("Vehicle Speed: ");
 // Serial.println(speedCode);

  return byte(speedCode);
}

byte getRPMMsgValue(int RPM){
  /*
  * Create the correct CAN code for the given RMP
  */
  float multiplier=65;

  byte RPMCode=byte(RPM/multiplier);
 //Serial.print("RPM: ");
 // Serial.println(RPMCode);

  return RPMCode;
}

int convertAccReadingToRPM(int reading){
  /*
   * Convert a reading coming from the rx-8 accelerator pedal to an rpm value
   * This amounts to transposing a value in the domain [350 -> 550] to a value
   * in the range [0 -> 9000]
   */

   //Find the domain by subtracting the min from the max of the domain list
   int domainMin = pedal1ReadingsCurve[0];
   int domainMax = pedal1ReadingsCurve[1];

   int domain = domainMax - domainMin;

   int rangeMax = 9000;
   int rangeMin = 0;

   int range = rangeMax - rangeMin;

   long a = reading - domainMin;
   long b = a*range;
   long c = b / domain;
   long d = c + rangeMin;

   long rpm = ((a * range) / domain) + rangeMin;

   Serial.println(rpm);
   return int(rpm);

}

void loop()
{

    //THe RX-8 pedal produces 2 readings when pressed. This stores both readings.
    //The ECU compares the 2 readings to make sure the pedal's working properly.
    //For testing purposes, we are just using 1 of the 2 readings (Reading 1)
    int pedalReading1 = analogRead(pedal_1_pin);
    Serial.println(pedalReading1);
    int pedalReading2 = analogRead(pedal_2_pin);


    // StatusMIL
    engTemp         = 145;
    odo             = 0;
    oilPressure     = 1;    // Either 0 (fault) or >=1 (Ok)
    checkEngineMIL  = 0;
    checkEngineBL   = 0;
    lowWaterMIL     = 0;
    batChargeMIL    = 0;
    oilPressureMIL  = 0;

    updateMIL();
    CAN0.sendMsgBuf(0x420, 0, 8, statusMIL);
    delay(10);

    // StatusPCM
    int rpm              = convertAccReadingToRPM(pedalReading1);  //calculate the rpm to send to the can
    byte engRPM          = getRPMMsgValue(rpm);    // RPM  Value*67 gives 8500 RPM Reading Redline is 127
    byte vehicleSpeed    = getSpeedMsgValue(100);    // Speed  Value=0.63*(Speed)+38.5. Max speed is 188 before gauge stops reporting
    
    updatePCM(engRPM, vehicleSpeed);

    CAN0.sendMsgBuf(0x201, 0, 8, statusPCM); //CAN0.sendMsgBuf(CAN_ID, Data Type (normally 0), length of data, Data

    delay(10);

    // StatusDSC
    dscOff          = 0;
    absMIL          = 0;
    brakeFailMIL    = 0;
    etcActiveBL     = 0;    // Only works with dscOff set
    etcDisabled     = 0;    // Only works with dscOff set

    updateDSC();
    CAN0.sendMsgBuf(0x212, 0, 8, statusDSC);
    delay(10);

    updatePS(false);
    CAN0.sendMsgBuf(0x300, 0, 8, statusPS);
    delay(10);

    analogWrite(fuelGauge1_pin, 0);
    analogWrite(fuelGauge2_pin, 0);
/*
    CAN0.sendMsgBuf(0x202, 0, 8, statusEPS2);
    delay(10);


    CAN0.sendMsgBuf(0x215, 0, 8, statusECU1);
    delay(10);

    CAN0.sendMsgBuf(0x231, 0, 8, statusECU2);
    delay(10);

    CAN0.sendMsgBuf(0x240, 0, 8, statusECU3);
    delay(10);
    CAN0.sendMsgBuf(0x250, 0, 8, statusECU4);
    delay(10);

*/

 }
