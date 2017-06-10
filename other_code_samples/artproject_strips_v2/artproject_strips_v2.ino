
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <TimedAction.h>

//#define PIN2 7

//Note: All strips are on PIN 4,5,6 and 7 for the art project.


void blinkTwo(){
  rainbowTwo();
}


Adafruit_NeoPixel strip1  = Adafruit_NeoPixel(60, PIN4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2  = Adafruit_NeoPixel(60, PIN5, NEO_GRB + NEO_KHZ800);

uint16_t counter1 = 0;
uint16_t counter2 = 0;

TimedAction stripActionOne = TimedAction(3,rainbowOne);
TimedAction stripActionTwo = TimedAction(6,rainbowTwo);


void setup() {
  Serial.begin(9600);

  strip1.begin();
  strip1.show(); // Initialize all pixels to 'off'

  strip2.begin();
  strip2.show(); // Initialize all pixels to 'off'
 
 // colorWipe( strip1.Color(255, 0, 0), 5, strip1);   // light pixels red
 // colorWipe( strip2.Color(0, 0, 255), 5, strip2);   // light pixels blu
}

void loop() {
  
 stripActionOne.check();
 stripActionTwo.check();
}



void rainbowOne() {
  uint16_t pixelNr;

  for(pixelNr=0; pixelNr< strip1.numPixels(); pixelNr++) {
    strip1.setPixelColor(pixelNr, Wheel(((pixelNr * 256 / strip1.numPixels()) + counter1) & 255, strip1) );
  }
  strip1.show();
  
  counter1++;
  if (counter1 > 256 ) {
    counter1=0;
  }
}


void rainbowTwo() {
  uint16_t i;
    Serial.println("TWO") ;

    for(i=0; i< strip2.numPixels(); i++) {
         // strip1.setPixelColor(i, Wheel(((i * 256 / strip1.numPixels()) + counter1) & 255, strip1) );
    Serial.println(i) ;

          strip2.setPixelColor(i, Wheel(((i * 256 / strip2.numPixels()) + counter2) & 255,strip2));
            Serial.println("rainbowtwo") ;
    }
    strip2.show();
        Serial.println("TWO---") ;

   counter2++;
   if (counter2 > 256 ) {
    counter2=0;
  }

}


uint32_t Wheel(byte WheelPos,Adafruit_NeoPixel myStrip ) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return myStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return myStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  //WheelPos -= 170;
  WheelPos = WheelPos - 170;
  
  return myStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait, Adafruit_NeoPixel myStrip) {

  for(uint16_t i=0; i<myStrip.numPixels(); i++) {
    myStrip.setPixelColor(i, c);
    myStrip.show();
    delay(wait);
  }
}



