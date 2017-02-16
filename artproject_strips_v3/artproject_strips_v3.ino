
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <TimedAction.h>

//Note: All strips are on PIN 4,5,6 and 7 for the art project.



Adafruit_NeoPixel strip1  = Adafruit_NeoPixel(60, PIN4, NEO_GRB + NEO_KHZ800);

uint16_t counter1 = 0;



void rainbowCycle() {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip1.numPixels(); i++) {
      strip1.setPixelColor(i, Wheel(((i * 256 / strip1.numPixels()) + j) & 255));
    }
    strip1.show();
    delay(2);
  }
}



TimedAction stripActionOne = TimedAction(3, rainbowCycle);


void setup() {
  Serial.begin(9600);

  strip1.begin();
  strip1.show(); // Initialize all pixels to 'off'
 // colorWipe( strip1.Color(255, 0, 0), 3, strip1);   // light pixels red
}

void loop() {
  stripActionOne.check();
  //rainbowCycle();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}





// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait, Adafruit_NeoPixel myStrip) {

  for (uint16_t i = 0; i < myStrip.numPixels(); i++) {
    myStrip.setPixelColor(i, c);
    myStrip.show();
    delay(wait);
  }
}



