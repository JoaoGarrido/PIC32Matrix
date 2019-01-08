#include <Arduino.h>
//#include <Adafruit_GFX.h>

//PORT E
#define R1 RE7
#define G1 RE6 
#define B1 RE5
#define R2 RE4
#define G2 RE3 
#define B2 RE2 
#define CLK RE1
//PORT F
#define OEng RF4 //ng -> Inverted Output
#define LAT RF6
//PORT D
#define A RD7
#define B RD6
#define C RD11
#define D RD5

//Modify Output
#define latA LATDbits.LATD7
#define latB LATDbits.LATD6
#define latC LATDbits.LATD11
#define latD LATDbits.LATD5
#define latLAT LATFbits.LATF6
#define lat_OE LATFbits.LATF4 //_ inverted Output
#define latR1 LATEbits.LATE7 
#define latG1 LATEbits.LATE6 
#define latB1 LATEbits.LATE5
#define latR2 LATEbits.LATE4
#define latG2 LATEbits.LATE3
#define latB2 LATEbits.LATE2
#define latCLK LATEbits.LATE1

#define nLedsLine 192

void initMATRIX(){
  //Set 0utputs
  TRISE = 0xFF01;
  TRISF = 0xFFAF;
  TRISD = 0xF71F;

  LATE = 0;
  LATF = 0;
  LATD = 0;
  lat_OE = 1; //turn off output enable
}

void latch(){
  latLAT = 1;
  latLAT = 0;
  lat_OE = 0;
  delay(1); //NEEDS DELAY for propagation! 
  lat_OE = 1;
}

void selectRow(uint8_t row){
  latA = (row & 0x01);
  latB = (row & 0x02) >> 1;
  latC = (row & 0x04) >> 2;
  latD = (row & 0x08) >> 3;

  //Send color information
  for(int a = 0; a < nLedsLine; a++){
    latCLK = 1;
    latCLK = 0;
  }
  latch();

}

void setup() {
  initMATRIX();
  latR1 = 1;
  latG1 = 1;
  latB1 = 1;
  latR2 = 1;
  latG2 = 1;
  latB2 = 1;
}

void loop() {
  static int i = -1;
  //delay(1);
  if(i < 15) i++;
  else i = 0;
  selectRow(i);
  //lat_OE = 0;
}