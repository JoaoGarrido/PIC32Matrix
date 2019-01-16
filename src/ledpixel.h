#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
//#define CHANNELMIX444(r,g,b) r>>12|g>>8|b>>4

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


class Ledmatrix : public Adafruit_GFX{
public:
    Ledmatrix();
    ~Ledmatrix();
    void matrixInit();
    void matrixUpdate();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);
    void drawPixelRGB888(int16_t x, int16_t y, uint32_t color);
    void drawPixelRGB444(int16_t x, int16_t y, uint16_t color);
    void bufferFill(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);
    
 private:
    uint16_t matrixBuffer[32][64]; //R3 R2 R1 R0 | G3 G2 G1 G0 | B3 B2 B1 B0 | - - - -
    void latch(uint8_t propagationTime);
    void colorInformation(uint8_t width, uint8_t sector, uint8_t imageIteration);
    void selectSector(uint8_t sector);
};

Ledmatrix::Ledmatrix():Adafruit_GFX(64, 32)
{
    matrixInit();
}

Ledmatrix::~Ledmatrix()
{
}


void Ledmatrix::matrixInit(){
    //Set 0utputs
    TRISE = 0xFF01;
    TRISF = 0xFFAF;
    TRISD = 0xF71F;

    LATE = 0;
    LATF = 0;
    LATD = 0;
    matrixBuffer[32][64] = {0};
    //lat_OE = 1; //turn off output enable -> MAYBE NOT NEEDED - TEST!!
}

void Ledmatrix::matrixUpdate(){
    static uint8_t iteration = 0;
    for(uint8_t sector = 0; sector < 16; sector++){
        selectSector(sector);
        colorInformation(64,sector,iteration);
        latch(2 * (8>>iteration));
    }
    iteration = (iteration + 1) % 4; //0->1->2->3->0...
}

/* bufferFill
Description: writes a pixel with 12bit color depth(RGB444) on the buffer
Variables:
    x: x position
    y: y position
    r: 4 bit color red
    g: 4 bit color green
    b: 4 bit color
*/
void Ledmatrix::bufferFill(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b){
    uint16_t auxColor = (r << 12) | (g << 8) | (b << 4);
    matrixBuffer[x][y] = auxColor;
}

void Ledmatrix::drawPixel(int16_t x, int16_t y, uint16_t color){
    drawPixelRGB565(y, x, color);
}

void Ledmatrix::drawPixelRGB565(int16_t x, int16_t y, uint16_t color){
    //RGB565 to RGB444 
    uint8_t r =  color >> 12;        // RRRRrggggggbbbbb
    uint8_t g = (color >>  7) & 0xF; // rrrrrGGGGggbbbbb
    uint8_t b = (color >>  1) & 0xF; // rrrrrggggggBBBBb
    bufferFill(x, y, r, g, b);
}

void Ledmatrix::drawPixelRGB888(int16_t x, int16_t y, uint32_t color){
    //RGB888 to RGB444 
    //color565_444(color);
    //bufferFill(x, y, r, g, b);
}

void Ledmatrix::drawPixelRGB444(int16_t x, int16_t y, uint16_t color){
    uint8_t r = (color >> 8) & 0x0F;
    uint8_t g = (color >> 4) & 0x0F;
    uint8_t b = color & 0x0F;
    bufferFill(x, y, r, g, b);
}


/* selectSector
Description: writes on the registers the sector information
Variables:
  sector: a sector aggregates 2 rows: row and row+16
    values: [0;15]
*/
void Ledmatrix::selectSector(uint8_t sector){
    latA = (sector & 0x01); //LSB
    latB = (sector & 0x02) >> 1;
    latC = (sector & 0x04) >> 2;
    latD = (sector & 0x08) >> 3; //MSB
}

/* latch
Description:  writes on the LAT and OE registers to indicate that the color information data for that sector is over.
              After that turns off the LEDs, waits for the signal propagation to happen and turns ON the LEDs again
*/
void Ledmatrix::latch(uint8_t propagationTime){
    latLAT = 1;
    latLAT = 0;
    lat_OE = 0;
    delayMicroseconds(propagationTime); //NEEDS DELAY for propagation! 
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
                    Using 12 bit color deph(RGB 4/4/4) we draw 4 images to PWM the matrix-> This variable contains the iteration that will be drawn this time 
    value: [0;3]
*/
void Ledmatrix::colorInformation(uint8_t width, uint8_t sector, uint8_t imageIteration){
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