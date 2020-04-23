//www.elegoo.com
//2016.12.12

/************************
Exercise the using
the L293D chip
************************/

#define ENABLE 5
//need to tell the chip which way the current is flowing.
//We're using 'direction A'
#define DIRECTIONA 3
#define DIRECTIONB 4

//positions, (as indicated on the gauge)
int empty=100;
int half=150;
int full=250;
 
void setup() {
  //---set pin direction
  pinMode(ENABLE,OUTPUT);
  pinMode(DIRECTIONA,OUTPUT);
  pinMode(DIRECTIONB,OUTPUT);
  digitalWrite(DIRECTIONA,HIGH); //one way
  digitalWrite(DIRECTIONB,LOW);
}

void loop() {
  analogWrite(ENABLE,half);
  delay(2000);
}
   
