
/* ARDUINO LED SLAVE 
   - drives the LEDS strands
   - knows all about the strands ( nr LEDs and PINs)
   - implementes their actions (transitions)
*/   

#include <Wire.h>
#include <EasyTransferI2C.h>
#include <FastLED.h>

//#define FASTLED_USE_PROGMEM 1   // NEW !
FASTLED_USING_NAMESPACE
#define COLOR_ORDER GRB
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define I2C_SLAVE_ADDRESS 9

#define NUM_STRIPS 5

/* 
 * Variables 
 * NUM_STRIPS - number of strips, used to 
 * - set other variables like (a) array with the lenghts of each strand, to loop over strands , keep configuration of each strand 
 * CRGB leds[num_strips][num] - two-dimensional array takes up a lot of space. 
 */

//CRGB leds[NUM_STRIPS][60];   // create two-dimensional array to change Strip


 
CRGB leds[774]; // one-dimensional array, better memory use.

/* methods to try ;
 *  fill_solid( leds, int numToFill,  const struct CRGB& color)  // operates on an array of LEDS rather; needs to operate on an array + an offset instead 
 *  fill_rainbow( leds, numtofill ) - fills a whole strip with rainbow colors
 *  Sigh, all methods opereate on an array of LEDs
 *  
 */

/* Another way to do this is : 36 of these arrays. Maybe can we have one array with pointers to point to these ? 
 *  How can we create an array of pointers ? in a for loop ? 
 *  for ( num strips ) { 
 *    ledRegister[1] = *create_array_with_leds(size) 
 *    }
 * CRGB redLeds[10];
   CRGB greenLeds[60];
   CRGB blueLeds[];
 */

byte nrLedsPerStrand[NUM_STRIPS]    = {13, 21, 46, 31, 12 }; 


//                           STRAND    S1  S2  S3  S4  S5  S6  S7  S8  S9 S10 S11  S12 S13 S14 S15 S16 S17 S18 S19 S20  S21 S22 S23 S24 S25 S26 S27 S28 S29 S30 S31 S32 S33 S34 S35 S36
//byte nrLedsPerStrand[NUM_STRIPS]    = {13, 21, 46, 31, 12, 10, 11, 14, 19, 17, 12,  5, 90, 75,  60, 28, 15, 7,  12, 27, 14, 36,  15, 19, 14, 12, 12, 14, 41, 14, 8,  6, 8,   9, 12, 15 };
//                              PIN    47  32  49   5  34  31  2    3   8  7   24   23  6  12   4   35  51  22  36  25  11  45   43  44  27  53  50  48  10  13  52  46 9    26 28, 29



EasyTransferI2C ET;


// ---- global variables to store data for the PATTERNS
boolean gSwitch               = true; // global variable to remember current ON/OFF status
byte    gstrand               = 0;    // global variable to point to current strand.
byte    gHue[NUM_STRIPS]      = {0, 0, 0, 0, 0 }; // , 0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0, 0, 0, 0, 0,0 };  // rotating "base color" used by many of the patterns, so each strip can have different gHue

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

/*       STRAND    S1  S2  S3  S4  S5  S6  S7  S8  S9 S10 S11  S12 S13 S14 S15 S16 S17 S18 S19 S20  S21 S22 S23 S24 S25 S26 S27 S28 S29 S30 S31 S32 S33 S34 S35 S36
 *          PIN    47  32  49   5  34  31  2    3   8  7   24   23  6  12   4   35  51  22  36  25  11  45   43  44  27  53  50  48  10  13  52  46 9    26 28, 29
 *       NR_LEDS   13, 21, 46, 31, 12, 10, 11, 14, 19, 17, 12,  5, 90, 75,  60, 28, 15, 7,  12, 27, 14, 36,  15, 19, 14, 12, 12, 14, 41, 14, 8,  6, 8,   9, 12, 15 };
*/

  // one array, multiple strands declaration 
  // tell FastLED there's 60 NEOPIXEL leds on pin 10, starting at index 0 in the led array
  //                       PIN      OFFSET NR_LEDS

  FastLED.addLeds<NEOPIXEL, 47>(leds, 0,   13);
  FastLED.addLeds<NEOPIXEL, 32>(leds,13, 21);
  FastLED.addLeds<NEOPIXEL, 49>(leds, 34, 46);
  FastLED.addLeds<NEOPIXEL,  5>(leds, 80, 31);


/*
  controllers[0] = &FastLED.addLeds<NEOPIXEL, 47>(leds, 0,   13).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<NEOPIXEL, 32>(leds, (0+13), 21).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<NEOPIXEL, 49>(leds, (0+13+21), 46).setCorrection(TypicalLEDStrip);
  controllers[3] = &FastLED.addLeds<NEOPIXEL,  5>(leds, (0+13+21+46), 31).setCorrection(TypicalLEDStrip);
  controllers[4] = &FastLED.addLeds<NEOPIXEL, 34>(leds, (0+13+21+46+31), 10).setCorrection(TypicalLEDStrip);
*/
  // old declaration
  /*
  controllers[0] = &FastLED.addLeds<WS2811, 47, GRB>(leds[0], nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<WS2811, 32, GRB>(leds[1], nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<WS2811, 49, GRB>(leds[2], nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  controllers[3] = &FastLED.addLeds<WS2811,  5, GRB>(leds[3], nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);
  controllers[4] = &FastLED.addLeds<WS2811, 34, GRB>(leds[4], nrLedsPerStrand[4]).setCorrection(TypicalLEDStrip);
*/
/*
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
*/


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

int getOffsetOfStrand (int * array) {
  int offset = 0;
  for (int i = 0; i < gstrand; i++) { 
    offset+=array [i];   
  }
  return offset; 
}



void loop() {
  // BEGINNING OF -loop-
  
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

  // Loop trough all strands and write the pattern.
  for (byte i = 0; i < NUM_STRIPS ; i++ ) {
    gstrand = i; // global index to point to current strand. used in the subroutines which do patterns.
    // the challenge is that we can't had this parameter over to the routine itself.
    gPatterns[ledConfig[i].pattern]();
    controllers[i]->showLeds(ledConfig[i].bright);
  }

   delay(3);
   // END OF -loop-
}

void receive(int numBytes) {
}


// ------------ FASTLed patterns --------
// fill_rainbow() ? 
// fade-to-black() ? 
// all methods operate on global variable [gstrand] to change pattern 


void rainbow() {
  gHue[gstrand]++;
  fill_rainbow( leds[gstrand], nrLedsPerStrand[gstrand], gHue[gstrand], 7);

  // rewrite rainbow: 
  // i have to supply a SLICE of the LED array or ? 
  fill_rainbow( leds, nrLedsPerStrand[gstrand], gHue[gstrand], 7); 

  
}

void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}

// change individual pixel a strand
void addGlitter( fract8 chanceOfGlitter) {
  // create random number between 0-255. this number is smaller than ChanceOfGlitter,
  // then create a random number from 0 to

  //int thisOffset = getOffset(nrLedsPerStrand); 

  if ( random8() < chanceOfGlitter) {
    // change STRAND X led Y 
    leds[gstrand][ random16(nrLedsPerStrand[gstrand]) ] += CRGB::White;
  }
}

// change individual pixel a strand
void fadeToBlack() {
  for (byte i = 0; i < nrLedsPerStrand[gstrand]; i++ ) {
    leds[gstrand][i].fadeToBlackBy( 64 );
  }
}

// change individual pixel a strand
void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 10);
  int pos = random16(nrLedsPerStrand[gstrand]);
  leds[gstrand][pos] += CHSV( gHue[gstrand] + random8(64), 200, 255);
}

// change individual pixel a strand
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


int numberOfALLLEDs (int * array, int len) {  
  int sum = 0;
  for (int i = 0 ; i < len ; i++) {
    sum += array [i] ;
  }
  return sum ;
}



