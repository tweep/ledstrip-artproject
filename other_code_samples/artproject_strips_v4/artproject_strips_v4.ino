
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <TimedAction.h>

#define PIN4 4
#define PIN5 5
#define PIN6 6

//Note: All strips are on PIN 4,5,6 and 7 for the art project.


Adafruit_NeoPixel strip1  = Adafruit_NeoPixel(60, PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2  = Adafruit_NeoPixel(60, PIN5, NEO_GRB + NEO_KHZ800);

TimedAction stripActionOne = TimedAction(20,&rainbowOne);
TimedAction stripActionTwo = TimedAction(60,&rainbowTwo);
  
uint16_t counter1 = 0;
uint16_t counter2 = 0;


void setup() {

  Serial.begin(9600);

  strip1.begin();
  strip1.show(); // Initialize all pixels to 'off'
  strip1.setBrightness(255);
  
  strip2.begin();
  strip2.show(); 
  strip2.setBrightness(255);

  colorWipe( strip1.Color(255, 0, 0), strip1);   // light pixels red
  colorWipe( strip2.Color(0, 0, 255), strip2);   // light pixels blu
}

void loop() {
 stripActionOne.check();
 stripActionTwo.check();
}




void rainbowOne() {

  for(int pixelNr=0; pixelNr< strip1.numPixels(); pixelNr++) {
    strip1.setPixelColor(pixelNr, Wheel(((pixelNr * 256 / strip1.numPixels()) + counter1) & 255) );
  }
  strip1.show();
  counter1++;
  if (counter1 > 256 ) {
    counter1=0;
  }
}


void rainbowTwo() {
    for(int i=0; i< strip2.numPixels(); i++) {
     // strip2.setPixelColor(i, Wheel((   (i * 256 / strip2.numPixels()) + counter2)   & 255)     );

      strip2.setPixelColor(i, Wheel((   (i * 256 / 200) + counter2)   & 255)     );
    }
    strip2.show();
    counter2++;
   if (counter2 > 256 ) {
    counter2=0;
  }
}


uint32_t Wheel(byte WheelPos) {
  
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos = WheelPos - 170;
  
  return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// Fill the dots one after the other with a color
// PASSING an object AS A REFERENCE &myStrip 

void colorWipe(uint32_t c, Adafruit_NeoPixel &myStrip) {

  for(uint16_t i=0; i<myStrip.numPixels(); i++) {
    myStrip.setPixelColor(i, c);
    myStrip.show();
    delay(3);
  }
}



