
#include "ArtProject.h"
#include "TimedAction.h"

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
ArtProject a1 = ArtProject(60,4,BRIGHTNESS,210,45);  // problem: function breaks if start + width is > 255. fix this.
ArtProject a2 = ArtProject(60,PIN5,BRIGHTNESS); 
ArtProject a3 = ArtProject(60,PIN6,BRIGHTNESS);
ArtProject a4 = ArtProject(60,PIN7,BRIGHTNESS);

/*
TimedAction stripActionOne = TimedAction(10,&one);
TimedAction stripActionTwo = TimedAction(20,&two);
TimedAction stripActionThree = TimedAction(60,&three);
TimedAction stripActionFour = TimedAction(160,&four);
*/
void setup() {
  Serial.begin(9600);
  Serial.println("in setup()"); 
}

void loop() {
    Serial.println("loop()"); 

     a1.rgbBand();
     a2.rainbowCycle();
     a3.rainbow();

}



