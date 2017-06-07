
// ARDUINO LED SLAVE - drives the LEDS and knows all about transitions

#include <Wire.h>
#include <EasyTransferI2C.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE
#define COLOR_ORDER GRB
#define NUM_STRIPS  4
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define I2C_SLAVE_ADDRESS 9
CRGB leds[NUM_STRIPS][60];
byte nrLedsPerStrand[NUM_STRIPS]    = {60, 60, 60, 60};

EasyTransferI2C ET;



// ---- global variables to store data for the PATTERNS
boolean gSwitch               = true; // global variable to remember current ON/OFF status
byte    gstrand               = 0;    // global variable to point to current strand.
byte    gHue[NUM_STRIPS]      = {0, 0, 0, 0};  // rotating "base color" used by many of the patterns, so each strip can have different gHue

// TODO Master: have longer sleep timer in setup()
// sleep timer - to turn LED off after 60 min.

struct RECEIVE_DATA_STRUCTURE {
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int ledStripToConfigure;
  int pattern;
  int spd;
  int bright;
  int color;
} mydata;

typedef struct {    // Structure to hold a copy of the configuration of each strand
  int pattern;
  int spd;
  int bright;
  int color;
} struct_ledconf;
struct_ledconf ledConfig[NUM_STRIPS];

CLEDController *controllers[NUM_STRIPS];


void setup() {
  Serial.begin(115200);

  // -------- FASTLED strand setup --------

  controllers[0] = &FastLED.addLeds<WS2811, 4, GRB>(leds[0], nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<WS2811, 5, GRB>(leds[1], nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<WS2811, 6, GRB>(leds[2], nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  controllers[3] = &FastLED.addLeds<WS2811, 7, GRB>(leds[3], nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);

  for ( int j = 0; j < NUM_STRIPS; j++ ) {
    FastLED[j].showLeds(128); // set initial brightness of strip
    FastLED.show();
  }

  // -------- EasyTransfer --------

  Wire.begin(I2C_SLAVE_ADDRESS);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Wire);
  Wire.onReceive(receive);   //define handler function on receiving data
  Serial.println("Ready to receive data");
}


typedef void (*SimplePatternList[])();
// array with list of patters which serve as functions. problem is we can't hand arguments over so we use global variables
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, fadeToBlack, colorStripRed };



void loop() {
  
  if (ET.receiveData()) {
      int strip = mydata.ledStripToConfigure;
      ledConfig[strip].pattern      = mydata.pattern;
      ledConfig[strip].spd          = mydata.spd;
      ledConfig[strip].bright       = mydata.bright;
      ledConfig[strip].color        = mydata.color;

     controllers[strip]->showLeds(ledConfig[strip].bright);
    
     Serial.println("Retrieving data...");
     Serial.print("Strip: "); Serial.print(mydata.ledStripToConfigure);
     Serial.print(" pattern: "); Serial.print(mydata.pattern);
     Serial.print(" speed: "); Serial.print(mydata.spd);
     Serial.print(" brigh: "); Serial.print(mydata.bright);
     Serial.print(" color: "); Serial.print(mydata.color);
     Serial.println(" ");

  }


  for (byte i = 0; i < NUM_STRIPS ; i++ ) {
    gstrand = i; // global index to point to current strand. used in the subroutines which do patterns.
    // the challenge is that we can't had this parameter over to the routine itself.
    gPatterns[ledConfig[i].pattern]();
    controllers[i]->showLeds(ledConfig[i].bright);
  }

   delay(3);
}

void receive(int numBytes) {
  }


// ------------ FASTLed patterns --------

void rainbow() {
  gHue[gstrand]++;
  fill_rainbow( leds[gstrand], nrLedsPerStrand[gstrand], gHue[gstrand], 7);
}
void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}
void addGlitter( fract8 chanceOfGlitter) {
  // create random number between 0-255. this number is smaller than ChanceOfGlitter,
  // then create a random number from 0 to
  if ( random8() < chanceOfGlitter) {
    leds[gstrand][ random16(nrLedsPerStrand[gstrand]) ] += CRGB::White;
  }
}
void fadeToBlack() {
  for (byte i = 0; i < nrLedsPerStrand[gstrand]; i++ ) {
    leds[gstrand][i].fadeToBlackBy( 64 );
  }
}
void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 10);
  int pos = random16(nrLedsPerStrand[gstrand]);
  leds[gstrand][pos] += CHSV( gHue[gstrand] + random8(64), 200, 255);
}
void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 20);
  int pos = beatsin16(13, 0, nrLedsPerStrand[gstrand]);
  leds[gstrand][pos] += CHSV( gHue[gstrand], 255, 192);
}
void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for (byte i = 0; i < nrLedsPerStrand[gstrand]; i++) { //9948
    leds[gstrand][i] = ColorFromPalette(palette, gHue[gstrand] + (i * 2), beat - gHue[gstrand] + (i * 10));
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
void colorStripRed() {
  // Serial.println("colorStripRed");
  fill_solid(leds[gstrand], nrLedsPerStrand[gstrand], CRGB::Red);
}
