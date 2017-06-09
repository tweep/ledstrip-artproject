
#include "LedStripColorTransitions.h"
#include "TimedAction.h"
#include "SpectrumBand.h" // Data type definition for the Spectrum band 

#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7
#define BRIGHTNESS 62

/* Todo: Add new function which 
 *  - runs on a selected spectrum of our color ( like between 100 - 150 ) for the whole strip 
 *  - changes the pixel colors indviduall similar to rainbowCycle()
 *  - something like : spectrumCycle                   pix1   pix2     pix3    pix4
 *  - stretch the spectrum out over the strand : 
 *                                     loop-1          blue - violet - purple  red 
 *                                     loop-2          red    blue     violet  purple 
 *                                     loop-3          purple red      blue    violet 
 *                                     loop-4          violet purple   red     blue 
 *                                     loop-1          blue   violet   purple  red 
 *                                     
 *                                     Todo: 
 *                                     - normalize the spectrum set by user over the number of pixels ( similar to RainbowCycle ) 
 *                                     - for every loop, ?
 *  - now move the colors by 1 pixel : 
  */
LedStripColorTransitions a1 = LedStripColorTransitions(60,4,BRIGHTNESS,100,150);  // problem: function breaks if start + width is > 255. fix this.
LedStripColorTransitions a2 = LedStripColorTransitions(60,PIN5,BRIGHTNESS); 
LedStripColorTransitions a3 = LedStripColorTransitions(60,PIN6,BRIGHTNESS);

SpectrumBand sb = { 1,99 } ;
LedStripColorTransitions a4 = LedStripColorTransitions(60, PIN7, BRIGHTNESS, sb);


/*
TimedAction stripActionOne = TimedAction(10,&one);
TimedAction stripActionTwo = TimedAction(20,&two);
TimedAction stripActionThree = TimedAction(60,&three);
TimedAction stripActionFour = TimedAction(160,&four);
*/
void setup() {
  Serial.begin(9600);
  Serial.println("in setup()"); 
  a1.init();
  a2.init();
  a3.init();
  a4.init();
  
  a1.lightUp(255,0,0);
  a2.lightUp(255,0,255);
  a3.lightUp(255,255,0);
  a2.lightUp(255,0,0);



}

void loop() {
    Serial.println("loop()"); 

     a1.rgbBand();
     a2.rainbowCycle();
     a3.rainbow();

     // Set the spectrum start and end during runtime ( not during initialization)
     //a3.setSpectrumStart(10);
     //a3.setSpectrumEnd(90);


     //a4.setSpectrumStart(10);
     //a4.setSpectrumEnd(100);
     a4.percentToRGB();

}



