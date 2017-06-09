

/* 
 *  Missing : 
 *  - list of patterns
 *  - routine which configures each strand with a specific pattern 
 *     int stripPattern[4] = { 1, 1, 2, 2 };
 *     
 *    OLD: rainbow();   ===   gPatterns[gCurrentPatternNumber]();
 *    
 *    NEW: rainbow(1)   ===>   fill_rainbow( leds[1], nrLedsPerStrand[1], gHue, 7);

        int stripPatternConfig[4] = { 1, 1, 2, 2 };
 */

#include <IRremote.h>
#include <IRremoteInt.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define COLOR_ORDER GRB
#define NUM_STRIPS 4
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_STRIPS][60];

byte BRIGHTNESS = 96;
uint8_t RECV_PIN = 2;

uint8_t nrLedsPerStrand[] = {60, 60, 60, 60};
uint8_t stripPatternConfig[] = { 1, 1, 2, 2 };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

uint8_t gCurrentStrand = 0; // index of the current strand
uint8_t gstrand = 0; 

IRrecv irrecv(RECV_PIN);
decode_results results;


void setup() {
  delay(1000); // 3 second delay for recovery
  Serial.begin(9600);
  irrecv.enableIRIn();
  
  FastLED.addLeds<WS2811, 4, GRB>(leds[0], nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 5, GRB>(leds[1], nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 6, GRB>(leds[2], nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 7, GRB>(leds[3], nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);


  for ( int j=0; j < NUM_STRIPS; j++ ) { 
    FastLED[j].showLeds(BRIGHTNESS); // set brightness of strip 
    FastLED.show();
  }
}


typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };


void configure_stuff (long tmp) {

  switch (tmp) {
    case 0xE0E020DF :
      gstrand = 1; 
      nextPattern();
      break;

    case 0xE0E0A05 :
      Serial.println("dim");
      break;

    case 0xE0E0E01F :
      Serial.println("Vol+");
      gbrightnessUp();
      break;

    case 0xE0E0D02F :
      Serial.println("Vol-");
      gbrightnessDown();
      break;

  //  case 0xE0E0E01F :
      // change pattern for specific strip
     // changeStripPattern(0);
    //  break;
      
  //  case 0xE0E0E01F :
    //  changeStripPattern(1);
   //   break;
      
  }
}






void loop() {

  while (!irrecv.isIdle());  // if not idle, wait till complete

  if (irrecv.decode(&results)) {
    configure_stuff(results.value);
    irrecv.resume(); // Set up IR to receive next value.
  }

  for (uint8_t i = 0; i < NUM_STRIPS ; i++ ) { 
    gstrand = i; 
    gPatterns[gCurrentPatternNumber]();
  }
  FastLED.show();
  //FastLED.delay(40);

  EVERY_N_MILLISECONDS( 10 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  delay(100);
}







void nextStrand() {
  Serial.println("nextStrand");
  gCurrentStrand = (gCurrentStrand + 1) % NUM_STRIPS;
  // blink strand 
  FastLED[gCurrentStrand].showLeds(0);
  delay(50);
  FastLED[gCurrentStrand].showLeds(BRIGHTNESS);
}


void gbrightnessUp() {
  BRIGHTNESS += 25;
  Serial.println(BRIGHTNESS);
  for ( int j=0; j < NUM_STRIPS; j++ ) { 
    FastLED[j].showLeds(BRIGHTNESS); 
  } 
}

void gbrightnessDown() {
  BRIGHTNESS -= 25;
  Serial.println(BRIGHTNESS);
  for ( int j=0; j < NUM_STRIPS; j++ ) { 
    FastLED[j].showLeds(BRIGHTNESS);
  } 
}


/*
void changeStripPattern(int stripNr)
{
   // need method to know which pattern we are on. Need array ! 
   int stripPattern[4] = { 1, 1, 1, 1 };

   // initial configuration of which strips show which pattern ? 
   int stripPattern[4] = { 1, 1, 2, 2 };
   
   for ( int j = 0; j < NUM_STRIPS; j++ ) { 
    int patternForStrip = stripPattern[j];

    rainbow();   ===   gPatterns[gCurrentPatternNumber]();

    
    rainbow ==  fill_rainbow( leds, NUM_LEDS, gHue, 7);

    fill_rainbow( leds,  nrLedsPerStrand[stripNr], gHue, 7 ) 

    --> NOTE: record HUE for each strand separately 
    
    strip[j] ( gPatterns[patternForStrip](); 
   }

  
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);

  gPatterns[gCurrentPatternNumber]();
  gPatterns[rainbow]();
  gPatterns[fire]();

  FastLED[stripNr].showPattern(getCurrentPatternForStrip[stripNr]);
  
  FastLED[stripNr].showLeds(BRIGHTNESS);
}

*/


/// FAST LED ROUTIES

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  Serial.println("nextPattern");
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  gPatterns[gCurrentPatternNumber]();

}

void rainbow()
{
  fill_rainbow( leds[gstrand], nrLedsPerStrand[gstrand], gHue, 7);
}

void rainbowWithGlitter()
{
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[gstrand][ random16(nrLedsPerStrand[gstrand]) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 10);
  int pos = random16(nrLedsPerStrand[gstrand]);
  leds[gstrand][pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 20);
  int pos = beatsin16(13, 0, nrLedsPerStrand[gstrand]);
  leds[gstrand][pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < nrLedsPerStrand[gstrand]; i++) { //9948
    leds[gstrand][i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[gstrand][beatsin16(i + 7, 0, nrLedsPerStrand[gstrand])] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

