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

#define latA LATDbits.LATD7
#define latB LATDbits.LATD6
#define latC LATDbits.LATD11
#define latD LATDbits.LATD5

void initMATRIX(){
  //Set 0utputs
  TRISE = 0xFF01;
  TRISF = 0xFFAF;
  TRISD = 0xF71F;

  LATE = 0;
  LATF = 0;
  LATD = 0;
  LATFbits.LATF4 = 1; //turn off output enable
}

void selectRow(uint8_t row){
  //LAT up and OE
  LATFbits.LATF6 = 1;
  LATFbits.LATF4 = 1;

  latA = (row & 0x08) >> 3;
  latB = (row & 0x04) >> 2;
  latC = (row & 0x02) >> 1;
  latD = (row & 0x01);
  //Send color information


  LATFbits.LATF6 = 1;
  LATFbits.LATF4 = 0;
}

void setup() {
  initMATRIX();
}

void loop() {
  LATEbits.LATE1 = 1;
  selectRow(1);
  delay(1);
  LATEbits.LATE1 = 0;
}