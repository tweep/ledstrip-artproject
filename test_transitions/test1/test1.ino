

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

#define initial_pattern  2

int globalCounter = 0; 
byte stepper = 1; 

#define NUM_STRIPS 3

 
CRGB leds[180]; // one-dimensional array, better memory use.

byte nrLedsPerStrand[NUM_STRIPS]    = { 60, 60, 60 }; 

EasyTransferI2C ET;


// ---- global variables to store data for the PATTERNS
boolean gSwitch               = true; // global variable to remember current ON/OFF status
byte    gstrand               = 0;    // global variable to point to current strand.
byte    gHue[NUM_STRIPS]      = {0, 0, 0 }; 

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

  controllers[0] = &FastLED.addLeds<WS2812B, 53>(leds, getOffsetOfStrand(0), nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  controllers[1] = &FastLED.addLeds<WS2812B, 51>(leds, getOffsetOfStrand(1), nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  controllers[2] = &FastLED.addLeds<WS2812B, 49>(leds, getOffsetOfStrand(2), nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);


  for ( int j = 0; j < NUM_STRIPS; j++ ) {
    Serial.print("Strip"); 
    Serial.println(j); 
    FastLED[j].showLeds(128); // set initial brightness of strip
    FastLED.show();
    gstrand=j;
   // colorStripBlack();
   // colorStripBlue();

    //delay(400);

    // set very initial config 
      ledConfig[j].pattern      = initial_pattern;
      ledConfig[j].spd          = 0;
      ledConfig[j].bright       = 256;
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


SimplePatternList gPatterns = { rainbow, 
                                rainbowWithGlitter, 
                                breathing_led_heartbeat, 
                                sinelon, 
                                juggle, 
                                bpm, 
                                fadeToBlack, 
                                colorStripRed,
                                fadeByColor };



int getOffsetOfStrand (int strand) {
  int offset = 0;
  for (int i = 0; i < strand; i++) { 
    offset+= nrLedsPerStrand[i];   
  }
  return offset; 
}

void loop() {
  // BEGINNING OF -loop-

if (0){ 
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
}
  // Loop trough all strands and write the pattern.
  for (byte i = 0; i < NUM_STRIPS ; i++ ) {
    gstrand = i; 
    // global index to point to current strand. gstrand (global Var!!) is used in the subroutines which do patterns. DONT change this !
    // the challenge is that we can't had this parameter over to the routine itself.
    gPatterns[ledConfig[i].pattern]();
    controllers[i]->showLeds(ledConfig[i].bright);
  }
   // END OF -loop-
}

void receive(int numBytes) {
}

void rainbow() {
  // use global variable strand
  gHue[gstrand]++;

 //fill_rainbow( struct CRGB * pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue = 5);

 //  fill_rainbow(leds , nrLedsPerStrand[gstrand], gHue[gstrand], 7);

 fill_rainbow(&leds[getOffsetOfStrand(gstrand)] , nrLedsPerStrand[gstrand], gHue[gstrand], 7);

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
    // dim an existing color by 25 % ( 64 / 255 ) , eventually fading to full black. 
    leds[offset+i].fadeToBlackBy( 64 );
  }
}


// light random pixels in a similar color, slowly iterating trough the rainbow colors

void confetti_rainbow() {
  // random colored speckles that blink in and fade smoothly
  int offset = getOffsetOfStrand(gstrand);

  // dim an existing color by 10 % ( 
  fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 10);

  int pos = random16(nrLedsPerStrand[gstrand]);
  leds[offset+pos] += CHSV( gHue[gstrand] + random8(64), 200, 255); // random number from 0-255
  gHue[gstrand]++;
}


// light a pixel in a random color, dim it slowly. 
void confetti_random_colors() {
  // random colored speckles that blink in and fade smoothly
  int offset = getOffsetOfStrand(gstrand);

  // does this fade all pixels to black ? 
  fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 10);

  int pos = random16(nrLedsPerStrand[gstrand]); // random number from [0 - pixels in the strand ] to choose random pixel

  // create an CHSV object (this is used to represent a color in HSV color space)
  // this gets automatically converted to RGB space
  // += is adding color to the pixel ( color addtion ) 

  gHue[gstrand]+=random8(64);  // add a number from 0-64 to the gHue for the strand 
  leds[offset+pos] += CHSV( gHue[gstrand], 200, 255); // set a random pixel to the color  

  /* another way to do this is to just add random8(255) to it. 
   *  this could be a new parameter; 
   *  byte color_added = random8(PARAMETER_SET_BY_USER) 
   *    leds[offset+pos] += CHSV( gHue[gstrand] + color_added, 200, 255); // set a random pixel to the color  
   *    
   *    Another parameter could be that the user can set the gHUE for the strand - so he can have a confetti in red, green, blue - whatever color he chooses. 
   *    like this - todo: add function to remote so user can the HUE of a selected strand. 
   *      //gHue[gstrand] = 50; // yellow
          //gHue[gstrand] = 171; // blue confetti 
          // gHue[gstrand] = 213; // pink confetti 

   */

}



void confetti_parameters_can_be_added() {
  int offset = getOffsetOfStrand(gstrand);

  // does this fade all pixels to black ? 
  fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 10);

  int pos = random16(nrLedsPerStrand[gstrand]); // random number from [0 - pixels in the strand ] to choose random pixel

  // create an CHSV object (this is used to represent a color in HSV color space)
  // this gets automatically converted to RGB space
  // += is adding color to the pixel ( color addtion ) 

  gHue[gstrand]=100;
  //gHue[gstrand] = 50; // yellow
  //gHue[gstrand] = 171; // blue confetti 
  // gHue[gstrand] = 213; // pink confetti 
                                          // the RANDOM(64) DEFINES how much we will deviate from the base color and reach into the next one. 
                                          // if gHue[strand] == 0 and rand(32) than we have green confetti 
                                          // if gHue[strand] == 0 and rand(128) than we have green and pink confetti, as the HUE ranges from 0-128 
                                          // if gHUE[strand] = 100 and rand(128) then we have confetti in the 100-228 HUE colors (blue and pink ) 

  leds[offset+pos] += CHSV( gHue[gstrand] +random8(64), 200, 255); // set a random pixel to the color  

 // gHue[gstrand]++;
}


void breathing_led_heartbeat() {
  int offset = getOffsetOfStrand(gstrand);


  float val = (exp(sin(millis()/(500.0)*PI)) - 0.36787944)*108.0;
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


void breathing_led_heartbeat_rainbow_single_color() {
  int offset = getOffsetOfStrand(gstrand);


  float val = (exp(sin(millis()/(2000.0)*PI)) - 0.36787944)*108.0;
  Serial.print("gstrand: ");
  Serial.print(gstrand);
  Serial.print("  val : ");
  Serial.print(val);
  Serial.print("ghue :");
  Serial.println(gHue[gstrand]);

  ledConfig[gstrand].bright=val;
  fill_solid(&leds[offset], nrLedsPerStrand[gstrand],  CHSV( gHue[gstrand] , 128, 255));

 // change the gHue slowly for each strand, randomly. 
  if(random8(2)<1){
    gHue[gstrand]++;
  }
}

void breathing_led_heartbeat_rainbow() {
  int offset = getOffsetOfStrand(gstrand);

  float factor = 1;
  if ( gstrand == 1 ){
    factor = 1.3;
  }
   if ( gstrand == 2 ){
    factor = 1.6;
  }

  float val = (exp(sin(millis()/(factor*2000.0)*PI)) - 0.36787944)*108.0;
  Serial.print("gstrand: ");
  Serial.print(gstrand);
  Serial.print("  val : ");
  Serial.println(val);
  ledConfig[gstrand].bright=val;

  // add color animator [ some pix are red, some are gree, some are blue ]  ( otherwise it will all be just one color ) 
  fill_rainbow(&leds[getOffsetOfStrand(gstrand)] , nrLedsPerStrand[gstrand], gHue[gstrand], 3);


 // fill_solid(&leds[offset], nrLedsPerStrand[gstrand], CRGB::Red);

 // gHue[gstrand]++;
  
}


// change individual pixel a strand
void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  int offset = getOffsetOfStrand(gstrand);
  fadeToBlackBy( &leds[offset], nrLedsPerStrand[gstrand], 20);  
  int pos = beatsin16(13, 0, nrLedsPerStrand[gstrand]);
  leds[offset+pos] += CHSV( gHue[gstrand], 255, 192);
}


void fadeByColor() {
  int offset = getOffsetOfStrand(gstrand);
  fill_solid(&leds[offset], nrLedsPerStrand[gstrand], CRGB::Blue);
  
  fadeUsingColor( &leds[offset], nrLedsPerStrand[gstrand], CRGB( 200, 100, 50));

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

void colorStripBlue() {
  int offset = getOffsetOfStrand(gstrand);
  fill_solid(&leds[offset], nrLedsPerStrand[gstrand], CRGB::Blue);
}
    
void colorStripBlack() {
  int offset = getOffsetOfStrand(gstrand);
  fill_solid(&leds[offset], nrLedsPerStrand[gstrand], CRGB::Black);
}


