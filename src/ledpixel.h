#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

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

#define MATRIX_MAX_HEIGHT 32
#define MATRIX_MAX_WIDTH 128 


class Ledmatrix : public Adafruit_GFX{
    public:
        Ledmatrix(uint8_t width, uint8_t height, uint8_t brightness);
        ~Ledmatrix();
        void matrixInit(uint8_t width, uint8_t height, uint8_t brightness);
        void matrixUpdate();
        void drawPixel(int16_t x, int16_t y, uint16_t color);
        void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);
        void drawPixelRGB888(int16_t x, int16_t y, uint32_t color);
        void drawPixelRGB444(int16_t x, int16_t y, uint16_t color);
        void bufferFill(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);
        void clearDisplay(void);

    private:
        //uint16_t **matrixBuffer; //R3 R2 R1 R0 | G3 G2 G1 G0 | B3 B2 B1 B0 | - - - -
        //uint16_t *matrixBuffer;
        uint16_t matrixBuffer[MATRIX_MAX_HEIGHT][MATRIX_MAX_WIDTH] = {{0}}; //R3 R2 R1 R0 | G3 G2 G1 G0 | B3 B2 B1 B0 | - - - -
        uint8_t error[32] = {0};
        uint8_t _height, _width, _brightness;

        void latch(uint8_t show_time);
        void colorInformation(uint8_t sector, uint8_t imageIteration);
        void selectSector(uint8_t sector);
};

Ledmatrix::Ledmatrix(uint8_t width, uint8_t height, uint8_t brightness):Adafruit_GFX(width, height){
    matrixInit(width, height, brightness);
}

Ledmatrix::~Ledmatrix()
{
}

/* matrixInit
Description: sets outputs and saves height and width
Variables:
    height: matrix height
    width: matrix width
    brightness: matrix brightness(related to show_time)
Notes:
    Tried to allocate the buffer dynamically but couldn't access array
    (maybe some problem with dynamic memory allocation in the PIC32 library?)
*/
void Ledmatrix::matrixInit(uint8_t width, uint8_t height, uint8_t brightness){
    //Set 0utputs
    TRISE = 0xFF01;
    TRISF = 0xFFAF;
    TRISD = 0xF71F;
    //Write outputs
    LATE = 0;
    LATF = 0;
    LATD = 0;
    //Some checks
    if(height != MATRIX_MAX_HEIGHT) height = 32;
    if(width != 64 && width != 128) width = 64;
    if(brightness > 255) brightness = 255;
    if(brightness <= 0) brightness = 1;
    //object variables
    _height = height;
    _width  = width; 
    _brightness = brightness;
    
    //Testing dynamic buffer allocation
    /*//Didn't work
    //dynamic buffer allocation C -> malloc
    matrixBuffer = (uint16_t **) malloc(height * sizeof(uint16_t *));
    if(matrixBuffer == NULL) exit;
    for(int i = 0; i < height; i++){
        matrixBuffer[i] = (uint16_t *) malloc(width * sizeof(uint16_t));
        if(matrixBuffer[i] == NULL) exit;
    }
    memset(matrixBuffer, 0, height * width * sizeof(uint16_t) );
    */

    //Didn't work
    /*//buffer allocation C++ -> new
    matrixBuffer = new uint16_t* [height];
    for(int i = 0; i < height; i++){
        matrixBuffer[i] = new uint16_t[width];
        memset(matrixBuffer[i], 0, width * sizeof(uint16_t)); 
    }
    */
}

/* matrixUpdate
Description: refresh the matrix
            for each sector{
                writes on the ABCD outputs the sector
                writes the color information for each sector
                
            } 
Variables:
    height: matrix height
    width: matrix width
Notes:
    Tried to allocate the buffer dynamically but couldn't access array
    (maybe some problem with dynamic memory allocation in the PIC32 library?)
*/
void Ledmatrix::matrixUpdate(){
    static uint8_t iteration = 0;
    for(uint8_t sector = 0; sector < _height/2; sector++){
        selectSector(sector);
        while(iteration < 4){
            colorInformation(sector,iteration);
            latch(10*(_brightness/255)*(8>>iteration));
            iteration++;
        }
        iteration = 0;
    }
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
    if(y < _height && x < _width)   matrixBuffer[y][x] = auxColor;
}


/*drawPixel
Description: method belonging to Adafruit_GFX that is overwritten so that our function writes to our buffer
Variables:
    x: x position
    y: y position
    color : rrrr rggg gggb bbbb 16bit 565
*/

void Ledmatrix::drawPixel(int16_t x, int16_t y, uint16_t color){
    drawPixelRGB565(x, y, color);
}

/*drawPixelRGB565
Description: slices the 565 variable color to extract the 444 rgb channel values
Variables:
    x: x position
    y: y position
    color : rrrr rggg gggb bbbb 16bit
*/

void Ledmatrix::drawPixelRGB565(int16_t x, int16_t y, uint16_t color){
    //RGB565 to RGB444 
    uint8_t r =  color >> 12;        // RRRRrggggggbbbbb
    uint8_t g = (color >>  7) & 0xF; // rrrrrGGGGggbbbbb
    uint8_t b = (color >>  1) & 0xF; // rrrrrggggggBBBBb
    bufferFill(x, y, r, g, b);
}

/*drawPixelRGB444
Description: slices the 444 variable color to extract the 4bit channel values
Variables:
    x: x position
    y: y position
    color : 0000 rrrr gggg bbbb 16bit
*/

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
void Ledmatrix::latch(uint8_t show_time){
    latLAT = 1;
    latLAT = 0;
    lat_OE = 0;
    delayMicroseconds(show_time);
    lat_OE = 1;
}

/* colorInformation
Description: writes on the registers the color information present on the buffer lineary
Variables:
  row: the sector that will be written
    values: [0;15]
  imageIteraction:  There is no PWM control-> you can only send 3 bits of color each time the image is drawn(1 bit per R, G and B)
                    Solution: Manually PWM the Matrix -> Draw the image fast enough that you see the color that you want
                    Using 12 bit color deph(RGB 4/4/4) we draw 4 images to PWM the matrix-> This variable contains the iteration that will be drawn this time 
    value: [0;3]
*/
void Ledmatrix::colorInformation(uint8_t sector, uint8_t imageIteration){
    for(uint8_t x = 0; x < _width; x++){
        //sector 1
        latR1 = (matrixBuffer[sector][x] >> (15 - imageIteration)) & 0x01;  
        latG1 = (matrixBuffer[sector][x] >> (11 - imageIteration)) & 0x01;
        latB1 = (matrixBuffer[sector][x] >> (7 - imageIteration)) & 0x01;
        //sector 2
        latR2 = (matrixBuffer[sector+16][x] >> (15 - imageIteration)) & 0x01;
        latG2 = (matrixBuffer[sector+16][x] >> (11 - imageIteration)) & 0x01;
        latB2 = (matrixBuffer[sector+16][x] >> (7 - imageIteration)) & 0x01;
        //registers to leds
        latCLK = 1;
        latCLK = 0; 
    }

}
/*clearDisplay
Description: clears display by setting every uint16_t in the matrix buffer to 0
*/
void Ledmatrix::clearDisplay(void) {
    memset(matrixBuffer, 0, sizeof(matrixBuffer));
}