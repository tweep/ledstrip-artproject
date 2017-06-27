
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
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define I2C_SLAVE_ADDRESS 9

#define NUM_STRIPS 36

 
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


/*       STRAND    S0  S1  S2  S3  S4   5   6   7   8  9   10   11  12 13  14   15  16  17  18  19  20  21   22  23  24  25  26  27  28  29  30  31 32   33 34  35
 *       NR_LEDS   13, 21, 46, 31, 12, 10, 11, 14, 19, 17, 12,  5, 90, 75,  60, 28, 15, 7,  12, 27, 14, 36,  15, 19, 14, 12, 12, 14, 41, 14, 8,  6, 8,    9, 12, 15 
 *          PIN    47  32  49   5  34  31   2   3   8  7   24   23  6  12   4   35  51  22  36  25  11  45   43  44  27  53  50  48  10  13  52  46 9    26 28, 29

*/

                   //        STRAND    S0  S1  S2  S3  S4 -  5   6   7   8   9  -  10  11  12  13  14 -  15  16  17  18  19 - 20  21  22  23  24  - 25  26  27  28  29 - 30  31  32  33  34  35
byte nrLedsPerStrand[NUM_STRIPS]    = {13, 21, 46, 31, 12,  10, 11, 14, 19, 17,    12,  5, 90, 75, 60,   28, 15,  7, 12, 27,  14, 36, 15, 19, 14,   12, 12, 14, 41, 14,   8,  6,  8,  9, 12, 15 };
//byte stripPin[NUM_STRIPS]           = {47, 32, 49,  5, 34,  31,  2,  3,  8,  7,    24, 23,  6, 12,  4,   35, 51,  22,36, 25,  11, 45, 43, 44, 27,   53, 50, 48, 10, 13,  52, 46,  9, 26, 28, 29 };



EasyTransferI2C ET;


// ---- global variables to store data for the PATTERNS
boolean gSwitch               = true; // global variable to remember current ON/OFF status
byte    gstrand               = 0;    // global variable to point to current strand.
byte    gHue[NUM_STRIPS]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0 };  // rotating "base color" used by many of the patterns, so each strip can have different gHue

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

  controllers[0] = &FastLED.addLeds<WS2812B, 47>(leds, getOffsetOfStrand(0), nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<WS2812B, 32>(leds, getOffsetOfStrand(1), nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<WS2812B, 49>(leds, getOffsetOfStrand(2), nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  controllers[3] = &FastLED.addLeds<WS2812B,  5>(leds, getOffsetOfStrand(3), nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);
  controllers[4] = &FastLED.addLeds<WS2812B, 34>(leds, getOffsetOfStrand(4), nrLedsPerStrand[4]).setCorrection(TypicalLEDStrip);

  controllers[5] = &FastLED.addLeds<WS2812B, 31, GRB>(leds,getOffsetOfStrand(5),nrLedsPerStrand[5]).setCorrection(TypicalLEDStrip);
  controllers[6] = &FastLED.addLeds<WS2812B,  2, GRB>(leds,getOffsetOfStrand(6),nrLedsPerStrand[6]).setCorrection(TypicalLEDStrip);
  controllers[7] = &FastLED.addLeds<WS2812B,  3, GRB>(leds,getOffsetOfStrand(7),nrLedsPerStrand[7]).setCorrection(TypicalLEDStrip);
  controllers[8] = &FastLED.addLeds<WS2812B,  8, GRB>(leds,getOffsetOfStrand(8),nrLedsPerStrand[8]).setCorrection(TypicalLEDStrip);
  controllers[9] = &FastLED.addLeds<WS2812B,  7, GRB>(leds,getOffsetOfStrand(9),nrLedsPerStrand[9]).setCorrection(TypicalLEDStrip);

  controllers[10] = &FastLED.addLeds<WS2812B, 24, GRB>(leds,getOffsetOfStrand(10),nrLedsPerStrand[10]).setCorrection(TypicalLEDStrip);
  controllers[11] = &FastLED.addLeds<WS2812B, 23, GRB>(leds,getOffsetOfStrand(11),nrLedsPerStrand[11]).setCorrection(TypicalLEDStrip);
  controllers[12] = &FastLED.addLeds<WS2812B,  6, GRB>(leds,getOffsetOfStrand(12),nrLedsPerStrand[12]).setCorrection(TypicalLEDStrip);
  controllers[13] = &FastLED.addLeds<WS2812B, 12, GRB>(leds,getOffsetOfStrand(13),nrLedsPerStrand[13]).setCorrection(TypicalLEDStrip);
  controllers[14] = &FastLED.addLeds<WS2812B,  4, GRB>(leds,getOffsetOfStrand(14),nrLedsPerStrand[14]).setCorrection(TypicalLEDStrip);

  controllers[15] = &FastLED.addLeds<WS2812B, 35, GRB>(leds,getOffsetOfStrand(15),nrLedsPerStrand[15]).setCorrection(TypicalLEDStrip);
  controllers[16] = &FastLED.addLeds<WS2812B, 51, GRB>(leds,getOffsetOfStrand(16),nrLedsPerStrand[16]).setCorrection(TypicalLEDStrip);
  controllers[17] = &FastLED.addLeds<WS2812B, 22, GRB>(leds,getOffsetOfStrand(17),nrLedsPerStrand[17]).setCorrection(TypicalLEDStrip);
  controllers[18] = &FastLED.addLeds<WS2812B, 36, GRB>(leds,getOffsetOfStrand(18),nrLedsPerStrand[18]).setCorrection(TypicalLEDStrip);
  controllers[19] = &FastLED.addLeds<WS2812B, 25, GRB>(leds,getOffsetOfStrand(19),nrLedsPerStrand[19]).setCorrection(TypicalLEDStrip);


  controllers[20] = &FastLED.addLeds<WS2812B, 11, GRB>(leds,getOffsetOfStrand(20),nrLedsPerStrand[20]).setCorrection(TypicalLEDStrip);
  controllers[21] = &FastLED.addLeds<WS2812B, 45, GRB>(leds,getOffsetOfStrand(21),nrLedsPerStrand[21]).setCorrection(TypicalLEDStrip);
  controllers[22] = &FastLED.addLeds<WS2812B, 43, GRB>(leds,getOffsetOfStrand(22),nrLedsPerStrand[22]).setCorrection(TypicalLEDStrip);
  controllers[23] = &FastLED.addLeds<WS2812B, 44, GRB>(leds,getOffsetOfStrand(23),nrLedsPerStrand[23]).setCorrection(TypicalLEDStrip);
  controllers[24] = &FastLED.addLeds<WS2812B, 27, GRB>(leds,getOffsetOfStrand(24),nrLedsPerStrand[24]).setCorrection(TypicalLEDStrip);

  
  controllers[25] = &FastLED.addLeds<WS2812B, 53, GRB>(leds,getOffsetOfStrand(25),nrLedsPerStrand[25]).setCorrection(TypicalLEDStrip);
  controllers[26] = &FastLED.addLeds<WS2812B, 50, GRB>(leds,getOffsetOfStrand(26),nrLedsPerStrand[26]).setCorrection(TypicalLEDStrip);
  controllers[27] = &FastLED.addLeds<WS2812B, 48, GRB>(leds,getOffsetOfStrand(27),nrLedsPerStrand[27]).setCorrection(TypicalLEDStrip);
  controllers[28] = &FastLED.addLeds<WS2812B, 10, GRB>(leds,getOffsetOfStrand(28),nrLedsPerStrand[28]).setCorrection(TypicalLEDStrip);
  controllers[29] = &FastLED.addLeds<WS2812B, 13, GRB>(leds,getOffsetOfStrand(29),nrLedsPerStrand[29]).setCorrection(TypicalLEDStrip);
  
  controllers[30] = &FastLED.addLeds<WS2812B, 52, GRB>(leds,getOffsetOfStrand(30),nrLedsPerStrand[30]).setCorrection(TypicalLEDStrip);
  controllers[31] = &FastLED.addLeds<WS2812B, 46, GRB>(leds,getOffsetOfStrand(31),nrLedsPerStrand[31]).setCorrection(TypicalLEDStrip);
  controllers[32] = &FastLED.addLeds<WS2812B,  9, GRB>(leds,getOffsetOfStrand(32),nrLedsPerStrand[32]).setCorrection(TypicalLEDStrip);
  controllers[33] = &FastLED.addLeds<WS2812B, 26, GRB>(leds,getOffsetOfStrand(33),nrLedsPerStrand[33]).setCorrection(TypicalLEDStrip);
  controllers[34] = &FastLED.addLeds<WS2812B, 28, GRB>(leds,getOffsetOfStrand(34),nrLedsPerStrand[34]).setCorrection(TypicalLEDStrip);

  controllers[35] = &FastLED.addLeds<WS2812B, 29, GRB>(leds,getOffsetOfStrand(35),nrLedsPerStrand[35]).setCorrection(TypicalLEDStrip);


  for ( int j = 0; j < NUM_STRIPS; j++ ) {
    Serial.print("Strip"); 
    Serial.println(j); 
    FastLED[j].showLeds(128); // set initial brightness of strip
    FastLED.show();
    gstrand=j;
    colorStripBlack();
    delay(50);

    // set very initial config 
      ledConfig[j].pattern      = 6; 
      ledConfig[j].spd          = 0;
      ledConfig[j].bright       = 0;
      ledConfig[j].color        = 0;

    
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
SimplePatternList gPatterns = { //breathing_led_heartbeat, 
                               rainbow, 
                                rainbowWithGlitter, 
                                confetti, 
                                sinelon, 
                                juggle,
                                bpm, 
                                fadeToBlack,
                                colorStripRed };

int gNrPatterns = ARRAY_SIZE(gPatterns);


int getOffsetOfStrand (int strand) {
  int offset = 0;
  for (int i = 0; i < strand; i++) { 
    offset+= nrLedsPerStrand[i];   
  }
  return offset; 
}



void loop() {
  // BEGINNING OF -loop-
  
  if (ET.receiveData()) {
      int strip = mydata.ledStripToConfigure;

      // if we retrieve GLOBAL data we apply it to all strands 
      if (strip == -1) { 
           for (byte i = 0; i < NUM_STRIPS ; i++ ) {
             ledConfig[i].pattern      = ( mydata.pattern % gNrPatterns);
             ledConfig[i].spd          = mydata.spd;
             ledConfig[i].bright       = mydata.bright;
             ledConfig[i].color        = mydata.color;
             controllers[i]->showLeds(ledConfig[i].bright);
           }
      }else { 
        ledConfig[strip].pattern      = ( mydata.pattern % gNrPatterns);

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
  }

  // Loop trough all strands and write/apply the values of the ledConfig to each strand.
  for (byte i = 0; i < NUM_STRIPS ; i++ ) {
    gstrand = i; // global index to point to current strand. gstrand (global Var!!) is used in the subroutines which do patterns. 
    // DONT change this !
    // the challenge is that we can't had this parameter over to the routine itself.
    gPatterns[ledConfig[i].pattern]();
    controllers[i]->showLeds(ledConfig[i].bright);
  }
 //  delay(3);
   // END OF -loop-
}

void receive(int numBytes) {
}

void breathing_led_heartbeat() {
  int offset = getOffsetOfStrand(gstrand);

  /* uploaded 26-06-2017
    todo:
      - only light up a certain nu ber of circles at the same time ( like 2,3,4 ) 
      - randomly switch to a new circle once the currenlty light cirlce is off
      - ( need a factor for each strand to define the speed of light on / light off 
      - not all cirlces adaapot the new (led breathing pattern - why ? ) some stay on the previous pattern.
      - refactor how global pattersn are sent. 
  */

  float val = (exp(sin(millis()/(gstrand*200.0)*PI)) - 0.36787944)*108.0;
  Serial.print("gstrand: ");
  Serial.print(gstrand);
  Serial.print("  val : ");
  Serial.print(val);
  Serial.print(" ghue :");
  Serial.println(gHue[gstrand]);

  // this can be done faster using the method in fill_rainbow() or fill_solid 
 // for (byte x=0; x< nrLedsPerStrand[gstrand]; x++){
  //   leds[offset+x] = CHSV( gHue[gstrand] , 128, 255); // random number from 0-255
// }
  ledConfig[gstrand].bright=val;

 fill_solid(&leds[offset], nrLedsPerStrand[gstrand],  CHSV( gHue[gstrand] , 255, 255));

 // only change the color once we 'breathed out'  
  if(val < 0.1 ){
    gHue[gstrand] = gHue[gstrand]+random8(0,25);
    //gHue[gstrand] = gHue[gstrand]+5;
     // gHue[gstrand] = random8();

  }
}




void rainbow() {
  // use global variable strand
  gHue[gstrand]++;

 //fill_rainbow( struct CRGB * pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue = 5);

 //  fill_rainbow(leds , nrLedsPerStrand[gstrand], gHue[gstrand], 7);

  fill_rainbow(&leds[getOffsetOfStrand(gstrand)] , nrLedsPerStrand[gstrand], gHue[gstrand], 3);

 // fill_rainbow(leds, getOffsetOfStrand(gstrand) , nrLedsPerStrand[gstrand], gHue[gstrand], 7);

}

void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}

// change individual pixel a strand
void addGlitter( fract8 chanceOfGlitter) {
  // create random number between 0-255. this number is smaller than ChanceOfGlitter,
  // then create a random number from 0 to

  if ( random8() < chanceOfGlitter) {

   int glitterPixel = random16(nrLedsPerStrand[gstrand]) + getOffsetOfStrand(gstrand);
   leds[glitterPixel] += CRGB::White;
  }
}

void fadeToBlack() {
  int offset = getOffsetOfStrand(gstrand);
  
  for (byte i = 0; i < nrLedsPerStrand[gstrand]; i++ ) {
    leds[offset+i].fadeToBlackBy( 64 );
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  int offset = getOffsetOfStrand(gstrand);
//  fadeToBlackBy( leds,offset, nrLedsPerStrand[gstrand], 10);
  fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 20);  

  int pos = random16(nrLedsPerStrand[gstrand]);
  leds[offset+pos] += CHSV( gHue[gstrand] + random8(64), 200, 255);
}

// change individual pixel a strand
void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  int offset = getOffsetOfStrand(gstrand);
    fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 20);  

  //fadeToBlackBy( leds,offset, nrLedsPerStrand[gstrand], 20);
  int pos = beatsin16(13, 0, nrLedsPerStrand[gstrand]);
  leds[offset+pos] += CHSV( gHue[gstrand], 255, 192);
}


void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  int offset = getOffsetOfStrand(gstrand);

  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for (byte i = 0; i < nrLedsPerStrand[gstrand]; i++) { //9948
    leds[offset+i] = ColorFromPalette(palette, gHue[gstrand] + (i * 2), beat - gHue[gstrand] + (i * 10));
  }
}


void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  int offset = getOffsetOfStrand(gstrand);
  fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 20);  

 // fadeToBlackBy( leds,offset, nrLedsPerStrand[gstrand], 20);
 
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
   leds[offset + beatsin16(i + 7, 0, nrLedsPerStrand[gstrand])] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


void colorStripRed() {
  int offset = getOffsetOfStrand(gstrand);
  fill_solid(&leds[offset], nrLedsPerStrand[gstrand], CRGB::Red);
}
    
void colorStripBlack() {
  int offset = getOffsetOfStrand(gstrand);
    fill_solid(&leds[offset], nrLedsPerStrand[gstrand], CRGB::Black);
}



