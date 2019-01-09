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

#define nLedsLine 64

void initMATRIX(){
  //Set 0utputs
  TRISE = 0xFF01;
  TRISF = 0xFFAF;
  TRISD = 0xF71F;

  LATE = 0;
  LATF = 0;
  LATD = 0;
  //lat_OE = 1; //turn off output enable -> MAYBE NOT NEEDED - TEST!!
}

/* latch
Description:  writes on the LAT and OE registers to indicate that the color information data for that sector is over.
              After that turns off the LEDs, waits for the signal propagation to happen and turns ON the LEDs again
*/
void latch(uint8_t propagationTime){
  latLAT = 1;
  latLAT = 0;
  lat_OE = 0;
  delay(propagationTime); //NEEDS DELAY for propagation! 
  lat_OE = 1;
}

/* colorInformation
Description: writes on the registers the color information present on the buffer lineary
Variables:
  width: the matrix max Width 
  row: the sector that will be written
    values: [0;15]
  imageIteraction:  There is no PWM control-> you can only send 3 bits of color each time the image is drawn(1 bit per R, G and B)
                    Solution: Manually PWM the Matrix -> Draw the image fast enough that you see the color that you want
                    Using 12 bit color deph(RGB 4/4/4) we draw 4 images to PWM the matrix-> This variable contains the iteraction that will be drawn this time 
    value: [0;3]
*/
void colorInformation(uint8_t width, uint8_t sector, uint8_t imageIteraction){
  for(uint8_t col = 0; col < width; col++){
    //sector 1
    latR1 = matrixBuffer[sector][col][15 - imageIteraction];
    latG1 = matrixBuffer[sector][col][11 - imageIteraction];
    latB1 = matrixBuffer[sector][col][7 - imageIteraction];
    //sector 2
    latR2 = matrixBuffer[sector+16][col][15 - imageIteraction];
    latG2 = matrixBuffer[sector+16][col][11 - imageIteraction];
    latB2 = matrixBuffer[sector+16][col][7 - imageIteraction];
    //registers to leds
    latCLK = 1;
    latCLK = 0; 
  }
}

/* selectSector
Description: writes on the registers the sector information
Variables:
  sector: a sector aggregates 2 rows: row and row+16
    values: [0;15]
*/
void selectSector(uint8_t sector){
  latA = (sector & 0x01); //LSB
  latB = (sector & 0x02) >> 1;
  latC = (sector & 0x04) >> 2;
  latD = (sector & 0x08) >> 3; //MSB
}

void colorDebug(){
  //Basic color testing
  for(int a = 0; a < nLedsLine; a++){
    if(a == 0){
      latR1 = 1;
      latG1 = 0;
      latB1 = 0;

      latR2 = 0;
      latG2 = 0;
      latB2 = 1;
    }
    else if(a == 1){
      latR1 = 1;
      latG1 = 1;
      latB1 = 1;
      latR2 = 0;
      latG2 = 1;
      latB2 = 0;
    }
    else if(!((a + 1) % 8) ){
      latR1 = 1;
      latG1 = 1;
      latB1 = 1;
      latR2 = 1;
      latG2 = 1;
      latB2 = 1;
    }
    else{
      LATE = 0;
    }
  
    latCLK = 1;
    latCLK = 0;
  }
}

void displayUpdate(){
  static uint8_t iteraction = 0;
  for(uint8_t sector = 0; sector < 16; sector++){
    selectSector(sector);
    colorInformation(64,sector,iteraction);
    latch();
    iteraction = (iteraction + 1) % 4; //0->1->2->3->0...
  }
} 

void setup() {
  initMATRIX();
}

void loop() {
  displayUpdate();
}