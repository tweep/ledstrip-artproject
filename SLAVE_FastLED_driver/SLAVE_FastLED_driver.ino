
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
byte    stripPatternConfig[NUM_STRIPS] = {0, 0, 0, 0};   // start pattern config: all strands start with pattern 0,0,0,0
byte gPrevPattern[NUM_STRIPS]          = {0,  0, 0 , 0};   // previsous pattern before turning strand off.


// TODO Master: have longer sleep time in setup()
// send initial configuration to SLAVE - this should be done in MASTER.setup()

struct RECEIVE_DATA_STRUCTURE {
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int global;
  int ledStripToConfigure;
  bool toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
};
RECEIVE_DATA_STRUCTURE mydata;

typedef struct {    // Structure to hold the configuration of each strand
  bool toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
  int blinks;
  int pause;
} struct_ledconf;
struct_ledconf ledConfig[NUM_STRIPS];



void setup() {
  Serial.begin(9600);

  // -------- FASTLED strand setup --------

  FastLED.addLeds<WS2811, 4, GRB>(leds[0], nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 5, GRB>(leds[1], nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 6, GRB>(leds[2], nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 7, GRB>(leds[3], nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);

  for ( int j = 0; j < NUM_STRIPS; j++ ) {
    FastLED[j].showLeds(128); // set initial brightness of strip
    FastLED.show();
  }

  // -------- EasyTransfer --------

  Wire.begin(I2C_SLAVE_ADDRESS);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Wire);
  Wire.onReceive(receive);   //define handler function on receiving data
}


typedef void (*SimplePatternList[])();
// array with list of patters which serve as functions. problem is we can't hand arguments over so we use global variables
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, fadeToBlack, colorStripRed };



void loop() {
  
  if (ET.receiveData()) {
    // Store the retrieved configuration for each strand
    Serial.println("Data");
    int strip = mydata.ledStripToConfigure;
    
    if ( mydata.global > 0 ) {
      Serial.println("Handling global configuration");

      switch( mydata.global ) {
    
        case 1:   // Turn strand on/off  
        turnAllStrandsOnOff(mydata.toggleStrand);
        break;

        case 2:   // brightness
        setGlobalBrightness(mydata.bright);
        break;
        
      }

          
     

     // setGlobalPattern(mydata.pattern);


     


    } else {
      // configure a specific strand only.
      ledConfig[strip].toggleStrand = mydata.toggleStrand;
      ledConfig[strip].pattern      = mydata.pattern;
      ledConfig[strip].spd          = mydata.spd;
      ledConfig[strip].bright       = mydata.bright;
      ledConfig[strip].color        = mydata.color;
    }
    if(mydata.toggleStrand == false){
       // set pattern fadeToDark for this strand 
    } 
    
    Serial.println("Retrieving data...");
    Serial.print("Strip: "); Serial.print(mydata.ledStripToConfigure);

    Serial.print("global: "); Serial.print(mydata.global);
    Serial.print(" togle:"); Serial.print(mydata.toggleStrand);
    Serial.print(" theme: "); Serial.print(mydata.pattern);
    Serial.print(" speed: "); Serial.print(mydata.spd);
    Serial.print(" brigh: "); Serial.print(mydata.bright);
    Serial.print(" color: "); Serial.print(mydata.color);
    Serial.println(" ");
    // routine which configures the specific strip: speed, pattern, brightness, on/off
    // routine which configures ALL strips: off/on, global brightness, ...
  }


  for (byte i = 0; i < NUM_STRIPS ; i++ ) {
    gstrand = i; // global index to point to current strand. used in the subroutines which do patterns.
    // the challenge is that we can't had this parameter over to the routine itself.
    // gPatterns[stripPatternConfig[i]]();
    gPatterns[ledConfig[i].pattern]();
  }

  if ( gSwitch == true ) {  // turn ALL strands on/off. how to read this out of the mydata which is sent ?
    //                          mydata[0].globalSwitch = 0
    //                          mydata[1].globalSwitch = 1   // only use strand 0 for global config values ?
 // if (ledConfig[0].globalSwitch == true ){
    FastLED.show();
  }
  // check if value for strand changed and turn indv stand on/off
  // check if value for starnd changed and dim strand
  delay(10);
}

void receive(int numBytes) {
  }



void setGlobalPattern( int pat) {
  for (byte i = 0; i < NUM_STRIPS ; i++ ) {
    ledConfig[i].pattern = pat;
  }
}

void setGlobalBrightness(int gbr) {
  FastLED.setBrightness( gbr );
}


void turnAllStrandsOnOff(int toggle) {
  if (toggle == 0 ) {
    Serial.println("Turning all strands OFF");
    for (byte i = 0; i < NUM_STRIPS; i++ ) {
      gPrevPattern[i] = ledConfig[i].pattern;
      FastLED[i].clearLeds( nrLedsPerStrand[i]);
      ledConfig[i].pattern = 6;
      //memset(strandSwitch, 0, sizeof(strandSwitch)); // set all array values to 0
      // FastLED[i].showColor(CRGB::Black, nrLedsPerStrand[i], 0);
    }
  } else {
    Serial.println("Turning all stands ON to previous pattern saved");
    for (byte i = 0; i < NUM_STRIPS; i++ ) {
      ledConfig[i].pattern = gPrevPattern[i];
    }
  }
}

void turnIndStrandOnOff(byte strand) {

  int status = ledConfig[strand].toggleStrand;

  if ( status == 1) {
    // If the strand is ON, turn it off.
    gPrevPattern[strand] = stripPatternConfig[strand]; // Store current pattern of strand

    Serial.print("Turning strand OFF:");
    Serial.println(strand);
    Serial.print("Previous pattern:");
    Serial.println(gPrevPattern[strand]);

    // set new pattern [OFF pattern] as Nr.6 for strip = fadeToBlack
    stripPatternConfig[strand] = 6;
    ledConfig[strand].toggleStrand = true;
  } else {

    Serial.print("Turing strand ON:");
    Serial.println(strand);
    Serial.print("Recovering + setting previous pattern:");
    Serial.println(gPrevPattern[strand]);

    stripPatternConfig[strand] = gPrevPattern[strand];
    ledConfig[strand].toggleStrand = true;
  }
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
