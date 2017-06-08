
/* ARDUINO LED SLAVE 
   - drives the LEDS strands
   - knows all about the strands ( nr LEDs and PINs)
   - implementes their actions (transitions)
*/   

#include <Wire.h>
#include <EasyTransferI2C.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE
#define COLOR_ORDER GRB
#define NUM_STRIPS  36
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define I2C_SLAVE_ADDRESS 9
CRGB leds[NUM_STRIPS][60];

//                           STRAND    S1  S2  S3  S4  S5  S6  S7  S8  S9 S10 S11  S12 S13 S14 S15 S16 S17 S18 S19 S20  S21 S22 S23 S24 S25 S26 S27 S28 S29 S30 S31 S32 S33 S34 S35 S36
byte nrLedsPerStrand[NUM_STRIPS]    = {13, 21, 46, 31, 12, 10, 11, 14, 19, 17, 12,  5, 90, 75,  60, 28, 15, 7,  12, 27, 14, 36,  15, 19, 14, 12, 12, 14, 41, 14, 8,  6, 8,   9, 12, 15 };
//                              PIN    47  32  49   5  34  31  2    3   8  7   24   23  6  12   4   35  51  22  36  25  11  45   43  44  27  53  50  48  10  13  52  46 9    26 28, 29




EasyTransferI2C ET;



// ---- global variables to store data for the PATTERNS
boolean gSwitch               = true; // global variable to remember current ON/OFF status
byte    gstrand               = 0;    // global variable to point to current strand.
byte    gHue[NUM_STRIPS]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0 };  // rotating "base color" used by many of the patterns, so each strip can have different gHue

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

  // -------- FASTLED strand setup with PINs --------S
//                                     S1  S2  S3  S4  S5  S6  S7  S8  S9 S10 S11  S12 S13 S14 S15 S16 S17 S18 S19 S20  S21 S22 S23 S24 S25 S26 S27 S28 S29 S30 S31 S32 S33 S34 S35 S36
byte nrLedsPerStrand[NUM_STRIPS]    = {13, 21, 46, 31, 12, 10, 11, 14, 19, 17, 12,  5, 90, 75,  60, 28, 15, 7,  12, 27, 14, 36,  15, 19, 14, 12, 12, 14, 41, 14, 8,  6, 8,   9, 12, 15 };
//                              PIN    47  32  49   5  34  31  2    3   8  7   24   23  6  12   4   35  51  22  36  25  11  45   43  44  27  53  50  48  10  13  52  46 9    26 28, 29
  //                                       PIN
  controllers[0] = &FastLED.addLeds<WS2811, 47, GRB>(leds[0], nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<WS2811, 32, GRB>(leds[1], nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<WS2811, 49, GRB>(leds[2], nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  controllers[3] = &FastLED.addLeds<WS2811,  5, GRB>(leds[3], nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);
  controllers[4] = &FastLED.addLeds<WS2811, 34, GRB>(leds[4], nrLedsPerStrand[4]).setCorrection(TypicalLEDStrip);
  
  controllers[5] = &FastLED.addLeds<WS2811, 31, GRB>(leds[5], nrLedsPerStrand[5]).setCorrection(TypicalLEDStrip);
  controllers[6] = &FastLED.addLeds<WS2811,  2, GRB>(leds[6], nrLedsPerStrand[6]).setCorrection(TypicalLEDStrip);
  controllers[7] = &FastLED.addLeds<WS2811,  3, GRB>(leds[7], nrLedsPerStrand[7]).setCorrection(TypicalLEDStrip);
  controllers[8] = &FastLED.addLeds<WS2811,  8, GRB>(leds[8], nrLedsPerStrand[8]).setCorrection(TypicalLEDStrip);
  controllers[9] = &FastLED.addLeds<WS2811,  7, GRB>(leds[9], nrLedsPerStrand[9]).setCorrection(TypicalLEDStrip);

  controllers[10] = &FastLED.addLeds<WS2811, 24, GRB>(leds[10], nrLedsPerStrand[10]).setCorrection(TypicalLEDStrip);
  controllers[11] = &FastLED.addLeds<WS2811, 23, GRB>(leds[11], nrLedsPerStrand[11]).setCorrection(TypicalLEDStrip);
  controllers[12] = &FastLED.addLeds<WS2811,  6, GRB>(leds[12], nrLedsPerStrand[12]).setCorrection(TypicalLEDStrip);
  controllers[13] = &FastLED.addLeds<WS2811, 12, GRB>(leds[13], nrLedsPerStrand[13]).setCorrection(TypicalLEDStrip);
  controllers[14] = &FastLED.addLeds<WS2811,  4, GRB>(leds[14], nrLedsPerStrand[14]).setCorrection(TypicalLEDStrip);

  controllers[15] = &FastLED.addLeds<WS2811, 35, GRB>(leds[15], nrLedsPerStrand[15]).setCorrection(TypicalLEDStrip);
  controllers[16] = &FastLED.addLeds<WS2811, 51, GRB>(leds[16], nrLedsPerStrand[16]).setCorrection(TypicalLEDStrip);
  controllers[17] = &FastLED.addLeds<WS2811, 22, GRB>(leds[17], nrLedsPerStrand[17]).setCorrection(TypicalLEDStrip);
  controllers[18] = &FastLED.addLeds<WS2811, 36, GRB>(leds[18], nrLedsPerStrand[18]).setCorrection(TypicalLEDStrip);
  controllers[19] = &FastLED.addLeds<WS2811, 25, GRB>(leds[19], nrLedsPerStrand[19]).setCorrection(TypicalLEDStrip);


  controllers[20] = &FastLED.addLeds<WS2811, 11, GRB>(leds[20], nrLedsPerStrand[20]).setCorrection(TypicalLEDStrip);
  controllers[21] = &FastLED.addLeds<WS2811, 45, GRB>(leds[21], nrLedsPerStrand[21]).setCorrection(TypicalLEDStrip);
  controllers[22] = &FastLED.addLeds<WS2811, 43, GRB>(leds[22], nrLedsPerStrand[22]).setCorrection(TypicalLEDStrip);
  controllers[23] = &FastLED.addLeds<WS2811, 44, GRB>(leds[23], nrLedsPerStrand[23]).setCorrection(TypicalLEDStrip);
  controllers[24] = &FastLED.addLeds<WS2811, 27, GRB>(leds[24], nrLedsPerStrand[24]).setCorrection(TypicalLEDStrip);

  
  controllers[25] = &FastLED.addLeds<WS2811, 53, GRB>(leds[25], nrLedsPerStrand[25]).setCorrection(TypicalLEDStrip);
  controllers[26] = &FastLED.addLeds<WS2811, 50, GRB>(leds[26], nrLedsPerStrand[26]).setCorrection(TypicalLEDStrip);
  controllers[27] = &FastLED.addLeds<WS2811, 48, GRB>(leds[27], nrLedsPerStrand[27]).setCorrection(TypicalLEDStrip);
  controllers[28] = &FastLED.addLeds<WS2811, 10, GRB>(leds[28], nrLedsPerStrand[28]).setCorrection(TypicalLEDStrip);
  controllers[29] = &FastLED.addLeds<WS2811, 13, GRB>(leds[29], nrLedsPerStrand[29]).setCorrection(TypicalLEDStrip);

  
  controllers[30] = &FastLED.addLeds<WS2811, 52, GRB>(leds[30], nrLedsPerStrand[30]).setCorrection(TypicalLEDStrip);
  controllers[31] = &FastLED.addLeds<WS2811, 46, GRB>(leds[31], nrLedsPerStrand[31]).setCorrection(TypicalLEDStrip);
  controllers[32] = &FastLED.addLeds<WS2811,  9, GRB>(leds[32], nrLedsPerStrand[32]).setCorrection(TypicalLEDStrip);
  controllers[33] = &FastLED.addLeds<WS2811, 26, GRB>(leds[33], nrLedsPerStrand[33]).setCorrection(TypicalLEDStrip);
  controllers[34] = &FastLED.addLeds<WS2811, 28, GRB>(leds[34], nrLedsPerStrand[34]).setCorrection(TypicalLEDStrip);

  controllers[35] = &FastLED.addLeds<WS2811, 29, GRB>(leds[35], nrLedsPerStrand[35]).setCorrection(TypicalLEDStrip);



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
     Serial.print(" Strip: "); Serial.print(mydata.ledStripToConfigure);
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
