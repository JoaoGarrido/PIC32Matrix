#include <Arduino.h>
#include "stdint.h"

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

/*typedef union{ //R3 R2 R1 R0 | G3 G2 G1 G0 | B3 B2 B1 B0 | - - - -
     uint16_t p16;

    struct {
        bool R3 : 1;
        bool R2 : 1;
        bool R1 : 1;
        bool R0 : 1;
        bool G3 : 1;
        bool G2 : 1;
        bool G1 : 1;
        bool G0 : 1;
        bool B3 : 1;
        bool B2 : 1;
        bool B1 : 1;
        bool B0 : 1;
        //undefined
        bool U3 : 1;
        bool U2 : 1;
        bool U1 : 1;
        bool U0 : 1;
    }

    struct{
        uint8_t R : 4;
        uint8_t G : 4;
        uint8_t B : 4;
        //uint8_t U : 4;
    }
} pixel444;*/

class ledmatrix
{
private:
    uint16_t matrixBuffer[32][64]; //R3 R2 R1 R0 | G3 G2 G1 G0 | B3 B2 B1 B0 | - - - -
    void latch(uint8_t propagationTime);
    void colorInformation(uint8_t width, uint8_t sector, uint8_t imageIteraction);
    void selectSector(uint8_t sector);
public:
    void matrixUpdate();
    void matrixInit();
    ledmatrix(/* args */);
    ~ledmatrix();
};

ledmatrix::ledmatrix(/* args */)
{
}

ledmatrix::~ledmatrix()
{
}

void ledmatrix::matrixUpdate(){
    static uint8_t iteraction = 0;
    for(uint8_t sector = 0; sector < 16; sector++){
        selectSector(sector);
        colorInformation(64,sector,iteraction);
        latch();
        iteraction = (iteraction + 1) % 4; //0->1->2->3->0...
    }
}

void ledmatrix::matrixInit(){
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
void ledmatrix::latch(uint8_t propagationTime){
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
void ledmatrix::colorInformation(uint8_t width, uint8_t sector, uint8_t imageIteraction){
  for(uint8_t col = 0; col < width; col++){
    //sector 1
    latR1 = (matrixBuffer[sector][col] >> (15 - imageIteration)) & 0x01;   //matrixBuffer[sector][col][15 - imageIteraction];
    latG1 = (matrixBuffer[sector][col] >> (11 - imageIteration)) & 0x01;
    latB1 = (matrixBuffer[sector][col] >> (7 - imageIteration)) & 0x01;
    //sector 2
    latR2 = (matrixBuffer[sector+16][col] >> (15 - imageIteration)) & 0x01;
    latG2 = (matrixBuffer[sector+16][col] >> (11 - imageIteration)) & 0x01;
    latB2 = (matrixBuffer[sector+16][col] >> (7 - imageIteration)) & 0x01;
    //registers to leds
    latCLK = 1;
    latCLK = 0; 
  }
}