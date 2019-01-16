#include <Arduino.h>
#include "ledpixel.h"
#include <Adafruit_GFX.h>

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

void setup() {

}

void loop() {

}