
/* Example on using an HX 1838 IR remote 
 *  receiver with new IR lib
 *  HX1838 VS1838 NEC Infrared IR Wireless Remote Control Sensor Module For Arduino
 *  
 *  Amazon 7.00 $ 
 *  https://www.amazon.com/gp/product/B019I4MYSE 
 */

/* hashDecode.ino Example sketch for IRLib2
 * Instead of decoding using a standard encoding scheme
 * (e.g. Sony, NEC, RC5), the code is hashed to a 32-bit value.
 * This should produce a unique 32-bit number however that number 
 * cannot be used to retransmit the same code. This is just a quick 
 * and dirty way to detect a unique code for controlling a device 
 * when you don't really care what protocol or values are being sent.
 */
//First will create a decoder that handles only NEC, Sony and the hash
// decoder. If it is not NEC or Sony that it will return a 32 bit hash.
#include <IRLibDecodeBase.h> 
#include <IRLib_P01_NEC.h>   
#include <IRLib_P02_Sony.h>  
#include <IRLib_HashRaw.h>  //Must be last protocol
#include <IRLibCombo.h>     // After all protocols, include this
// All of the above automatically creates a universal decoder
// class called "IRdecode" containing only the protocols you want.
// Now declare an instance of that decoder.
IRdecode myDecoder;

// Include a receiver either this or IRLibRecvPCI or IRLibRecvLoop
#include <IRLibRecv.h> 
IRrecv myReceiver(2);    //create a receiver on pin number 2

#include "FastLED.h"


FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    7
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    64
#define FRAMES_PER_SECOND  120 

CRGB leds[NUM_LEDS];

byte BRIGHTNESS = 96; 




void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}




void configure_stuff (int tmp) {
      
       switch (tmp) {
         case 0xE0E020DF :
           nextPattern();
           break;
           
         case 0xE0E0A05F :
           Serial.println("dim");
           break;

         case 0xE0E0E01F :
           Serial.println("Vol+");
           brightnessUp();
           break;
           
         case 0xE0E0D02F :
           Serial.println("Vol-");
           brightnessDown();
           break;

       }

}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
 


void loop() {
  
  if(myReceiver.getResults()) {
    myDecoder.decode();
    
   if(myDecoder.protocolNum==UNKNOWN) {
      Serial.print(F("Unknown protocol. Hash value is: 0x"));
      Serial.println(myDecoder.value,HEX);
    } else {
      myDecoder.dumpResults(false);
    };

    
    configure_stuff(myDecoder.value); 
    Serial.println(myDecoder.value,HEX);
    myReceiver.enableIRIn(); 
  }
     gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  //FastLED.delay(1000/FRAMES_PER_SECOND); 
  FastLED.delay(40); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 10 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  delay(100);
}



void brightnessUp() { 
  BRIGHTNESS+=25; 
  FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();  

}

void brightnessDown(){
  BRIGHTNESS-=25; 
  Serial.println(BRIGHTNESS);
  FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();  

}


/// FAST LED ROUTIES 

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  Serial.println("nextPattern");
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

