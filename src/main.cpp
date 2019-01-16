#include "ledpixel.h"

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

Ledmatrix led;
//Adafruit_GFX ad;

void setup() {
  led.drawPixelRGB444(3, 3, 0x0FE1); 
  led.drawPixelRGB444(3, 4, 0x0F00); 
  led.drawPixelRGB444(3, 5, 0x00E0); 
  led.drawPixelRGB444(3, 6, 0x0001); 
  led.drawPixelRGB444(15, 21, 0x0FF0); 
  led.drawPixelRGB444(15, 17, 0x00F0);

  led.drawPixelRGB444(15, 16, 0x0001);
  led.drawPixelRGB444(15, 17, 0x0003);
  led.drawPixelRGB444(15, 18, 0x0007);
  led.drawPixelRGB444(15, 19, 0x000F);

}

void loop() {
  led.matrixUpdate();
  
}