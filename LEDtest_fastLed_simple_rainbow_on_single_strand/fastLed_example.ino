

#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 52

// reserve memory block [array] 
CRGB leds[NUM_LEDS];


void setup() {
   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

}

void loop() {        

    rainbowCycle(10);
/*
  for(int dot = 0; dot < NUM_LEDS; dot++) { 

            // set LED to blue
            leds[dot] = CRGB::Blue;
            // show the led 
            FastLED.show(); // shows the new BLUE led and the (n-1) led which we put to black.
            
            // clear this led for the next time around the loop
            leds[dot] = CRGB::Black;
            delay(5);
        }
        */
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
