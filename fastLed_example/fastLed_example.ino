

#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6

// reserve memory block [array] 
CRGB leds[NUM_LEDS];


void setup() {
   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

}

void loop() {        
  
  for(int dot = 0; dot < NUM_LEDS; dot++) { 

            // set LED to blue
            leds[dot] = CRGB::Blue;
            // show the led 
            FastLED.show(); // shows the new BLUE led and the (n-1) led which we put to black.
            
            // clear this led for the next time around the loop
            leds[dot] = CRGB::Black;
            delay(5);
        }
}
