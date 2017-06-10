#include <IRremote.h>
#include <IRremoteInt.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define COLOR_ORDER GRB
#define NUM_STRIPS  4
#define RECV_PIN    2

// utility function to get the size of an array
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

CRGB leds[NUM_STRIPS][60];

byte gBrightness = 102;

boolean strandSwitch[NUM_STRIPS]       = {true, true, true, true}; // ON/OFF - keep memory of which strands are on / off.
byte nrLedsPerStrand[NUM_STRIPS]    = {60, 60,60, 60};
byte stripPatternConfig[NUM_STRIPS] = {0, 0,0, 0};    // start pattern config: all strands start with pattern 0,0,0,0
byte gHue[NUM_STRIPS]               = { 0, 0, 0, 0};   // rotating "base color" used by many of the patterns, so each strip can have different gHue
byte gPrevPattern[NUM_STRIPS]       = {0,  0, 0 , 0};   // previsous pattern before turning strand off.
byte favoriteConfigs[3][NUM_STRIPS] = { {0,0,0,0},{1,1,0,0},{1,1,3,3} };  // cycle trough a set of favorite configs.
 

byte gFavPatternIndex      = 0; // global index of current favorite sketch [ used to remember which pattern we are currently at]
byte gSketch               = 0; // global index of current sketch 
byte gstrand               = 0; // global variable to point to current strand. 
boolean gSwitch            = false; // global variable to remember current ON/OFF status

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  delay(1000); // 3 second delay for recovery
  Serial.begin(9600);
  irrecv.enableIRIn();
  Serial.println("Ready for commands");
                        //PIN
  FastLED.addLeds<WS2811, 4, GRB>(leds[0], nrLedsPerStrand[0]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 5, GRB>(leds[1], nrLedsPerStrand[1]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 6, GRB>(leds[2], nrLedsPerStrand[2]).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, 7, GRB>(leds[3], nrLedsPerStrand[3]).setCorrection(TypicalLEDStrip);

  for ( int j=0; j < NUM_STRIPS; j++ ) { 
    FastLED[j].showLeds(gBrightness); // set brightness of strip 
    FastLED.show();
  }
}


typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, fadeToBlack, colorAllRed };


void configure_stuff (long tmp) {

  switch (tmp) {

      /* Questions: 
          - when user pressess on/off on remote, what do we do ? how do we integretate these functions into 
            the current layout ? 
          - do i want to keep the shortcut remote functions : strand on/off | next pattern | prev pattern 
          - how do i encode Ir global brightness into the sketch ? 
      */

     case 0xE0E040BF : 
      turnAllStrandsOnOff();  
      break;

   //     == STRAND 0 ==

    case 0xE0E01AE5 :
      Serial.println("Entering menu operation"); 
    //  enterMenu(); 
      break; 
        
    case 0xE0E020DF : // BUTTON 1 
      setNextPatternForStrand(0);
      break;

    case 0xE0E0A05F : // BUTTON 2 
      turnIndStrandOnOff(0);
      break;

//     == STRAND 1 ==

    case 0xE0E010EF :  // BUTTON 4
      setNextPatternForStrand(1);
      break;

    case 0xE0E0906F :  // BUTTON 5
      turnIndStrandOnOff(1);
      break;

//     == STRAND 2 ==

    case 0xE0E030CF :  // BUTTON 7
      setNextPatternForStrand(2);
      break;

    case 0xE0E0B04F :  // BUTTON 8
      turnIndStrandOnOff(2);
      break;

//     == STRAND 3 ==

    case 0xE0E0C43B :  // BUTTON -
      setNextPatternForStrand(3);
      break;

   case 0xE0E08877 :  // BUTTON 0
      turnIndStrandOnOff(3);
      break;


    case 0xE0E0807F :  // SOURCE button
      fillStrandSolid();
      break;

    case 0xE0E0E01F :
      //Serial.println("Vol+");
      gbrightnessUp();
      break;

    case 0xE0E0D02F :
      //Serial.println("Vol-");
      gbrightnessDown();
      break;

     case 0xE0E036C9 :
     // Serial.println("Setting RED config");
      makeAllRed();
      break;

     case 0xE0E046B9 :
     // Serial.println("Page all strands to next sketch");
      // memset might not work for all numbers...
      memset(stripPatternConfig, gSketch++ , sizeof(stripPatternConfig));
      memset(gHue,0,sizeof(gHue));
      break;

     case 0xE0E0A659 :
      //Serial.println("Page all strands to previous sketch");
      // memset might not work for all numbers...
      gSketch = (gSketch + 1) % ARRAY_SIZE( gPatterns);
      memset(stripPatternConfig, gSketch-- , sizeof(stripPatternConfig));
      memset(gHue,0,sizeof(gHue));
      break;

     case 0xE0E006F9 :
       // Cycle trough next favorite pattern
       for (byte i=0; i < NUM_STRIPS; i++){ 
         stripPatternConfig[i] =  favoriteConfigs[gFavPatternIndex][i];
       }
      gFavPatternIndex = (gFavPatternIndex + 1) % ARRAY_SIZE(favoriteConfigs);
      break;

     case 0xE0E012ED :
     // Serial.println("Reset to default setting");
      memset(stripPatternConfig,0,sizeof(stripPatternConfig));
      memset(gHue,0,sizeof(gHue));
      break;

      
  }
}


void loop() {

  while (!irrecv.isIdle());
  
  if (irrecv.decode(&results)) {  
    configure_stuff(results.value);   // calls functions depending on input and sets global variables
    irrecv.resume();
  }

  for (byte i = 0; i < NUM_STRIPS ; i++ ) { 
    gstrand = i; 
    gPatterns[stripPatternConfig[i]]();
  }
 // testStrand() ;


    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    static uint8_t lenghr = 6; // length of patter. small=better

 // FillLEDsFromPaletteColors(startIndex,lenghr);
 //FastLED.show();

  if ( gSwitch == true ) {
     FastLED.show();
  }
  //FastLED.delay(40);
  delay(10);
}




void turnAllStrandsOnOff() { 
  gSwitch = !gSwitch;
  if (gSwitch == false ) { 
    Serial.println("Turning all strands OFF"); 
    for (byte i = 0; i< NUM_STRIPS; i++ ) { 
       FastLED[i].clearLeds( nrLedsPerStrand[i]);
       memset(strandSwitch,0,sizeof(strandSwitch)); // set all array values to 0
       // FastLED[i].showColor(CRGB::Black, nrLedsPerStrand[i], 0);
    }
 }else{
  Serial.println("Turning all stands ON");
 }
}

void turnAllStrandsOnOffx() { 
//  gSwitch = !gSwitch;
//  if (gSwitch == false ) { 
    for (byte strand = 0; strand< NUM_STRIPS; strand++ ) { 
        if (gSwitch == false ) { 
           gPrevPattern[strand] = stripPatternConfig[strand];
           stripPatternConfig[strand] = 6;
        }else{
           stripPatternConfig[strand] = gPrevPattern[strand];
        }
       //Serial.println("OFF");
       //FastLED[i].clearLeds( nrLedsPerStrand[i]);
       //memset(strandSwitch,0,sizeof(strandSwitch)); // set all array values to 0
       // FastLED[i].showColor(CRGB::Black, nrLedsPerStrand[i], 0);
   }
   gSwitch = !gSwitch;

}

void turnIndStrandOnOff(byte strand) { 
 
 boolean status = strandSwitch[strand];
 
  if ( status == true) { 
     // If the strand is ON, turn it off.
     gPrevPattern[strand] = stripPatternConfig[strand]; // Store current pattern of strand

     Serial.print("Turning strand OFF:"); 
     Serial.println(strand);
     Serial.print("Previous pattern:");  
     Serial.println(gPrevPattern[strand]);
   
    // set new pattern [OFF pattern] as Nr.6 for strip = fadeToBlack
    stripPatternConfig[strand] = 6;
    strandSwitch[strand] = false;
  }else {
    
    Serial.print("Turing strand ON:");
    Serial.println(strand);
    Serial.print("Recovering + setting previous pattern:");  
    Serial.println(gPrevPattern[strand]);

    stripPatternConfig[strand] = gPrevPattern[strand];
    strandSwitch[strand] = true;
  }
}

void dimStrand(byte strand) {
  // reduce the brightness of a specific strand.
  
  //FastLED[0].getBrightness()
  //FastLED[0].setBrightness()
  
  //leds[strand].getBrightnessSettingh
  //leds[strand].dimBrightness(brightness)
  //leds[strand][i].setIndividualLedBrightness(brightness);
}



void testStrand() {
    fill_solid(leds[0], nrLedsPerStrand[0], CRGB::Purple);
    fill_solid(leds[1], nrLedsPerStrand[1], CRGB::Red);

   // fill_gradient_RGB(leds[i], 0, CRGB::Red, nrLedsPerStrand[i], CRGB::Green);
   // fill_gradient_RGB(leds[i], 0, CRGB::Blue, nrLedsPerStrand[i], CRGB::Red);
  // fill_gradient_RGB(leds[i], nrLedsPerStrand[i], CRGB::Red,CRGB::Green);
     // fade_video(leds[i], nrLedsPerStrand[i], 20);

  for (byte i = 0; i < 100; i++ ) { 
       delay(300);
       fade_video(leds[0], nrLedsPerStrand[0], 20); // dims the strand; u have to call show() after it
       FastLED.show();

  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex, uint8_t spd) {
    uint8_t brightness = 75;
    
    for( int i = 0; i < 60; i++) {
        //leds[0][i] = ColorFromPalette( ForestColors_p, colorIndex, brightness, LINEARBLEND);
         //  leds[0][i] = ColorFromPalette( ForestColors_p, colorIndex, brightness, NOBLEND);
           leds[0][i] = ColorFromPalette( OceanColors_p, colorIndex, brightness, LINEARBLEND);

        colorIndex += spd;
    }
}

void gbrightnessUp() {
  gBrightness += 17;
  Serial.print("Global brightness UP:");
  Serial.println(gBrightness);
  gBrightness += 17;
  FastLED.setBrightness(  gBrightness );
}
void gbrightnessDown() {
  gBrightness -= 17;
  Serial.print("Global brightness DOWN:");
  Serial.println(gBrightness);
  FastLED.setBrightness( gBrightness );
}


/// FAST LED ROUTIES


void setNextPatternForStrand(byte strand) {
  Serial.print("Changing pattern for strand:");
  Serial.println(strand);
  // Just change our stripPatternConfig array for this strand : { 0, x, 0, 0} 
  // Incremembt the pattern by +1, if we go higher than the number of patterns, start from 0 again.
  byte currentPatternOfStrand = (stripPatternConfig[strand]  + 1) % ARRAY_SIZE(gPatterns); 
  Serial.print("New pattern:"); Serial.println(currentPatternOfStrand); 
  stripPatternConfig[strand] = currentPatternOfStrand;
}

void makeAllRed() {
  // Makes sure that gPattern nr.7 (colorAllRed) is called 
  stripPatternConfig[0]=7;
  stripPatternConfig[1]=7;
  stripPatternConfig[2]=7;
  stripPatternConfig[3]=7;
}

void rainbow() {
  gHue[gstrand]++;
  fill_rainbow( leds[gstrand], nrLedsPerStrand[gstrand], gHue[gstrand], 7);
}
void rainbowWithGlitter() {
  rainbow();
  addGlitter(80);
}
void fadeToBlack() { 
  for (byte i =0; i < nrLedsPerStrand[gstrand]; i++ ){
    leds[gstrand][i].fadeToBlackBy( 64 );
  }
}
void addGlitter( fract8 chanceOfGlitter) {
  // create random number between 0-255. this number is smaller than ChanceOfGlitter,
  // then create a random number from 0 to 
  if ( random8() < chanceOfGlitter) {
    leds[gstrand][ random16(nrLedsPerStrand[gstrand]) ] += CRGB::White;
  }
}
void confetti(){
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds[gstrand], nrLedsPerStrand[gstrand], 10);
  int pos = random16(nrLedsPerStrand[gstrand]);
  leds[gstrand][pos] += CHSV( gHue[gstrand] + random8(64), 200, 255);
}
void sinelon(){
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
void colorAllRed(){
 // Serial.println("colorAllRed");
  fill_solid(leds[gstrand], nrLedsPerStrand[gstrand], CRGB::Red);
}

void fillStrandSolid() {
 Serial.println("Turning Strand RED");
 // executed when pressing "SRC" button. does this work actually ? 
 // because makeAllRed() does not work .. 
 
 for (byte i = 0; i < NUM_STRIPS; i++ ) { 
    fill_solid(leds[0], nrLedsPerStrand[0], CRGB::Red);
 }
}

