/*  ARDUINO UNO - SENDING / LCD
 *  - Infrared IR remote receiver on port 2 
 *  - SDA / SCL on freigesaegte ports 
 *  no GROUND connection
 *  - LCD on various other ports 
 */

 
#include <Wire.h>
#include <EasyTransferI2C.h>
#include <LiquidCrystal.h>
#include "MenuEntry.h"
#include "MenuLCD.h"
#include "MenuManager.h"
#include <IRremote.h>
#include <IRremoteInt.h>


/* 
 * This example shows how to connect an 16x2 display similar to this one: 
 * https://www.amazon.com/gp/product/B006OVYI1G/
 * 
 * Note: The pins are slightly different then the ones in the original example.
 * 
 */ 


const int LCDD7 = 7;
const int LCDD6 = 6;
const int LCDD5 = 5;
const int LCDD4 = 4;
const int LCDE  = 9;
const int LCDRS = 8;

bool configUpdated = false;

#define nrLEdStrands 4


// IR REMOTE CODE
#define IR_RECV_PIN    53

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

// EasyTransfer code 
EasyTransferI2C ET; 

struct SEND_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
<<<<<<< HEAD
  int     ledStripToConfigure;
  uint8_t toggleStrand;
  uint8_t pattern;
  uint8_t spd;
  uint8_t bright;
  uint8_t color;
  uint8_t blinks;
  uint8_t pause;
};

SEND_DATA_STRUCTURE ledStripConf;
=======
  int ledStripToConfigure;
  int toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
  int blinks;
  int pause;
} mydata; 


// structure to keep the config values for the state
/*
struct LED_CONFIG {
  int toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
  int blinks;
  int pause;
} ledConfig [nrLEdStrands]; 
*/


>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
#define I2C_SLAVE_ADDRESS 9  

//Now create the MenuLCD and MenuManager classes.
MenuLCD g_menuLCD( LCDRS, LCDE, LCDD4, LCDD5, LCDD6, LCDD7, 16, 2);

const bool doRootAction = true;
MenuManager g_menuManager( &g_menuLCD, doRootAction);  //pass  g_menuLCD object to g_menuManager with the & operator (pass the address)
                                                      // also pass boolean variable to configure the Menu if callback functions for 
                                                      // menu items with children are executed or not

unsigned int g_isDisplaying = false;

// Global variables to configure our LED strips, used by our Menu libraryr 

byte   g_toggleStrand = 1;        // variable to turn specific strand on/off
int    g_pattern = 3;            // MenuManager::DoIntInput only takes int but no uint8_t
int    g_speed   = 50;
int    g_bright  = 50;
int    g_color   = 50;

int  ga_pattern      [4] = { 1,2,3,4 }; 
int  ga_speed        [4] = { 50, 50, 50, 50}; 
int  ga_bright       [4] = {128, 128, 128, 128 }; 
int  ga_color        [4] = {128, 128, 128, 128 }; 
byte ga_toggleStrand [4] = {1, 1, 1, 1}; 

/*
 *    1. LED strip
 *    |     |----Choose Pattern
 *    |     |----Set Speed 
 *    |     |----Set Color
 *    |     |----Set Brightness
 *    |     |----Turn Strand ON
 *    |     |----Turn Strand OFF
 *    |              
 *    2. LED strip
 *    |     |----Choose Pattern
 *    |     |----Set Speed 
 *    |     |----Set Color
 *    |     |----Set Brightness
 *    |     |----Turn Strand ON
 *    |     |----Turn Strand OFF
 *    |              
 *    Global configuration 
 *    |     |---Set global brightness
 *    |     |---Set global color
 *    |
 *    |- Switch On/Off
 */



void configureLEDstripOne ( char * pMenuText, void * pUserData ) {
  // set the variable at address pUserData to value 0 (address gets handed over in menu with &variable)
  // dereference data type which is userData
   *(( int *) pUserData) = 0;
}
void configureLEDstripTwo ( char * pMenuText, void * pUserData ) {
   // dereference data type which is userData
   *(( int *) pUserData) = 1;
}
void configureLEDstripThree ( char * pMenuText, void * pUserData ) {
   *(( int *) pUserData) = 2;
}
void configureLEDstripAll ( char * pMenuText, void * pUserData ) {
   // dereference data type which is userData
   *(( int *) pUserData) = -1;
}

<<<<<<< HEAD

void SetPattern( char* pMenuText, void *pUserData ) {
  char *pLabel = (char*)"Choose Pattern ^";
  
=======
/*This callback uses the built-in Int Input routine in MenuManager.h to request input of a integer number from the 
  user.  Control will pass to the DoIntInput function until the user finishes.  the g_timerTime will be set to the 
  value the user selects.
*/


void SetPattern( char* pMenuText, void *pUserData ) {
  const char *pLabel = (char *)"Choose Pattern";

>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
  int iNumLabelLines = 1;
  int iMin   = 1;
  int iMax   = 10;
  int iStart = ga_pattern[mydata.ledStripToConfigure];
  Serial.println("Starting with pattern: ");
  Serial.println(iStart); 
                           
  int iStep  = 1;  //Each user input action (such as a push of button) will step this amount

<<<<<<< HEAD
                                                                              
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_pattern );

  Serial.print("Strand: "); 
  Serial.print(g_ledStripToConfigure); 
  Serial.println("  Pattern: "); 
  Serial.println(g_pattern); 

  ledStripConf.ledStripToConfigure = g_ledStripToConfigure;
  ledStripConf.pattern = g_pattern;
}
void SetSpeed ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char*)"Choose Speed ^";
=======
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &mydata.pattern );
}
void SetSpeed ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char *)"Choose Speed ^";
>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
  int iNumLabelLines = 1;
  int iMin   = 1;
  int iMax   = 255;
  int iStart = ga_speed[mydata.ledStripToConfigure];
  int iStep  = 5;
  
<<<<<<< HEAD
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_speed );
  ledStripConf.ledStripToConfigure = g_ledStripToConfigure;
  ledStripConf.spd = g_speed;
}
void SetBrightness ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char*)"Choose Brightness";
=======
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &mydata.spd );
  ga_speed[mydata.ledStripToConfigure] = mydata.spd;

}
void SetBrightness ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char *)"Choose Brightness";
>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = ga_bright[mydata.ledStripToConfigure];
  int iStep = 5;
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &mydata.bright);

<<<<<<< HEAD
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_bright);
  ledStripConf.ledStripToConfigure = g_ledStripToConfigure;
  ledStripConf.bright = g_bright;
} 
void SetColor ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char*)"Choose Color ^:";
=======
}
void SetColor ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char *)"Choose Color ^:";
>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = ga_color[mydata.ledStripToConfigure];

  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;

<<<<<<< HEAD
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_color);
  ledStripConf.ledStripToConfigure = g_color;
  ledStripConf.color = g_color;
=======
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &mydata.color);
>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
}


void setupMenus() {  
  g_menuLCD.MenuLCDSetup();  
  MenuEntry * p_menuEntryRoot;
<<<<<<< HEAD
  //Add root node
  //MenuEntry( char * menuText, void * userData/*=0*/, MENU_ACTION_CALLBACK_FUNC func);

//                                                                    VARIABLE-TO-CHANGE        FUNCTION_WHICH_MANIPULATES_VARIABLE 
//p_menuEntryRoot = new MenuEntry("1. LED Strand ",            (void *) (&g_ledStripToConfigure) , configureLEDstripOne);  
// handing over address of STRUCT member variable. does this work ? 
p_menuEntryRoot = new MenuEntry((char*)"1. LED Strand ",            (void *) (&ledStripConf.ledStripToConfigure) , configureLEDstripOne);  

    g_menuManager.addMenuRoot( p_menuEntryRoot );
    g_menuManager.addChild( new MenuEntry((char*)"Set Pattern",     (void *) (&g_pattern),  SetPattern) );
    g_menuManager.addChild( new MenuEntry((char*)"Set Speed",       (void *) (&g_speed),    SetSpeed ));
    g_menuManager.addChild( new MenuEntry((char*)"Set Brightness",  (void *) (&g_bright),   SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char*)"Set Color",       (void *) (&g_color),    SetColor ));
    
    //                                                                 pass address of variable and pass MenuEntry_BoolTrueCallbacFunc    
    g_menuManager.addChild( new MenuEntry((char*) "Turn Strip ON",  (void *) (&g_toggleStrand), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char*) "Turn Strip OFF", (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char*) "Reset strand",   (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char*)"Back",            (void *)   &g_menuManager,  MenuEntry_BackCallbackFunc) );

g_menuManager.SelectRoot();
g_menuManager.addSibling( new MenuEntry((char*)"2. LED Strand ",    (void *) (&g_ledStripToConfigure), configureLEDstripTwo ) );
g_menuManager.MenuDown();
    g_menuManager.addChild( new MenuEntry((char*)"Set Pattern",     (void *) (&g_pattern),      SetPattern) );
    g_menuManager.addChild( new MenuEntry((char*)"Set Speed",       (void *) (&g_speed),        SetSpeed ));
    g_menuManager.addChild( new MenuEntry((char*)"Set Brightness",  (void *) (&g_bright),       SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char*)"Set Color",       (void *) (&g_color),        SetColor ));
    g_menuManager.addChild( new MenuEntry((char*) "Turn Strip ON",  (void *) (&g_toggleStrand), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char*) "Turn Strip OFF", (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char*) "Reset strand",   (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char*) "Back",           (void *)  &g_menuManager,   MenuEntry_BackCallbackFunc) );

g_menuManager.SelectRoot();
g_menuManager.addSibling( new MenuEntry((char*)"Global configuration ",   (void *) (&g_ledStripToConfigure), configureLEDstripAll ) );
g_menuManager.MenuDown();
    g_menuManager.addChild( new MenuEntry((char*)"Reset all",             (void *) (&g_pattern),     SetPattern) );
    g_menuManager.addChild( new MenuEntry((char*)"Global brightness",     (void *) (&g_bright),      SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char*)"Global color",          (void *) (&g_color),       SetColor ));
    g_menuManager.addChild( new MenuEntry((char*)"Back",                  (void *)  &g_menuManager,  MenuEntry_BackCallbackFunc) );
=======
  p_menuEntryRoot = new MenuEntry((char *)"1. LED Strand ",  (void *) (&mydata.ledStripToConfigure), configureLEDstripOne);  
    g_menuManager.addMenuRoot( p_menuEntryRoot );
    g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",    (void *) (&mydata.pattern),  SetPattern ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Speed",      (void *) (&mydata.spd),  SetSpeed ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Brightness", (void *) (&mydata.bright),  SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Color",      (void *) (&mydata.color),  SetColor ));
    
 
    g_menuManager.addChild( new MenuEntry((char *)"Turn Strip ON",   (void *) (&mydata.toggleStrand), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF",  (void *) (&mydata.toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Reset strand",                               NULL, MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Back",            (void *)  &g_menuManager,  MenuEntry_BackCallbackFunc) );

g_menuManager.addSibling( new MenuEntry((char *)"2. LED Strand ",   (void *) (&mydata.ledStripToConfigure), configureLEDstripTwo ) );
g_menuManager.MenuDown();

   // g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",     (void *) (&mydata.pattern),  SetPattern) );
    g_menuManager.addChild( new MenuEntry((char *)"Set Speed",        (void *) (&mydata.spd),      SetSpeed ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Brightness",   (void *) (&mydata.bright),   SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Color",       (void *) (&mydata.color),    SetColor ));
    g_menuManager.addChild( new MenuEntry((char *) "Turn Strip ON",  NULL,   MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF", NULL,   MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Reset strand",   NULL,   MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Back",       (void *)  &g_menuManager,   MenuEntry_BackCallbackFunc) );


g_menuManager.addSibling( new MenuEntry((char *)"3. LED Strand ",     (void *) (&mydata.ledStripToConfigure) , configureLEDstripThree ) );
g_menuManager.MenuDown();
 // g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",      (void *) (&mydata.pattern),   SetPattern) );
  g_menuManager.addChild( new MenuEntry((char *)"Set Speed",                (void *) (&mydata.spd),       SetSpeed ));
  g_menuManager.addChild( new MenuEntry((char *)"Set Brightness",           (void *) (&mydata.bright),    SetBrightness ));
  g_menuManager.addChild( new MenuEntry((char *)"Set Color",                (void *) (&mydata.color),     SetColor ));
  g_menuManager.addChild( new MenuEntry((char *) "Turn Strip ON",  NULL,   MenuEntry_BoolTrueCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF", NULL,   MenuEntry_BoolFalseCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Reset strand",   NULL,   MenuEntry_BoolFalseCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Back",       (void *)  &g_menuManager,   MenuEntry_BackCallbackFunc) );


g_menuManager.addSibling( new MenuEntry((char *)"Global configuration ",   NULL ,     configureLEDstripAll ) );
g_menuManager.MenuDown();
    g_menuManager.addChild( new MenuEntry((char *)"Reset all",             NULL,      SetPattern) );
    g_menuManager.addChild( new MenuEntry((char *)"Global brightness",     NULL,      SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char *)"Global color",          NULL,      SetColor ));
    g_menuManager.addChild( new MenuEntry((char *)"Back",                  (void *)  &g_menuManager,  MenuEntry_BackCallbackFunc) );
>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600

//Get the selection state back to the root for startup and to add the last entry
 g_menuManager.SelectRoot();
 g_menuManager.DrawMenu();
 
}

void setup() {  
  delay(1000); // 3 second delay for recovery         
  Serial.begin(9600);
  Serial.println("READY");
  
  // IR remote setup 
  IRrecv irrecv(IR_RECV_PIN);
  irrecv.enableIRIn();
  
  setupMenus();
  
  //EasyTransfer setup 
  Wire.begin();
  ET.begin(details(ledStripConf), &Wire);
  randomSeed(analogRead(0));
  pinMode(13, OUTPUT);
}


void loop()  {

  // INPUT from INFRARED REMOTE 
  while (!irrecv.isIdle());
  
  if (irrecv.decode(&results)) {  
     configure_LCD_Menu_via_IR_remote(results.value);
     irrecv.resume();
   }

  // INPUT from LCD keypad 
  int readKey = analogRead(0);
   if (readKey < 790) {
    delay(100);
    readKey = analogRead(0);
   }
   
   configure_LCD_Menu_via_LEDScreenButtons(readKey); 

   if (configUpdated) { 
     Serial.print("Config updated - sending data.. ");
     Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
     Serial.print(" togle:"); Serial.print(mydata.toggleStrand);
     Serial.print(" pattern: "); Serial.print(mydata.pattern);
     Serial.print(" speed: "); Serial.print(mydata.spd);
     Serial.print(" brigh: "); Serial.print(mydata.bright);
     Serial.print(" color: "); Serial.print(mydata.color);
     Serial.println(" ");

    // ledConfig[mydata.ledStripToConfigure].pattern = mydata.pattern;

    
     
     i2c_send_data_structure();
     configUpdated = false;
   }
}

void i2c_send_data_structure(){
  
  // copy data between the two data structures ! 
  // 0) modify the ledConfig with the menu and pointers. 
  // does ledstrand to configure have to be global ? 
  ///1) i have to send the myData data structure
  // 2) before sending, i have to copy all datafrom ledConfig into myData strucutre structure 
  
  Serial.println("EasyTransfer SENDING");
<<<<<<< HEAD
   ledStripConf.blinks = 3;
   ledStripConf.pause = 2;
   ET.sendData(I2C_SLAVE_ADDRESS);
   
  //Just for fun, we will blink it out too
  for(int i = ledStripConf.blinks; i>0; i--){
    digitalWrite(13, HIGH);
    delay(ledStripConf.pause * 100);
    digitalWrite(13, LOW);
    delay(ledStripConf.pause * 100);
   }
 delay(10);
=======
  ET.sendData(I2C_SLAVE_ADDRESS);

 // mydata.ledStripToConfigure = g_ledStripToConfigure;
  mydata.toggleStrand = g_toggleStrand;
  mydata.pattern = g_pattern;
  mydata.spd = g_speed;
  mydata.bright = g_bright;
  mydata.color = g_color;
  mydata.blinks =3;
  mydata.pause =3;
  
  Serial.println("DONE");
  
>>>>>>> 95e685f3956400f96d11cb525cc7c82d2f6a3600
}

int evaluateKeyPadButton(int x) {
  // This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.

  int result = 0;
  if (x < 50) {
    result = 1; // right
  } else if (x < 195) {
    result = 2; // up
  } else if (x < 380) {
    result = 3; // down
  } else if (x < 790) {
    result = 4; // left
  }
  return result;
}

void configure_LCD_Menu_via_LEDScreenButtons(int rk) { 
  
  /*  The code below takes the input from the LCD keypad 
   *  and calls different MENU functions to execute the configured action.
   *  The configUpdated variable is used to only set to SEND the new data 
   *  strucutre via I2C if the config is updated. We don't want to always 
   *  send the same data, only new updated configurations...
   */
   
  int button = evaluateKeyPadButton(rk);
  
  switch( button ) {
    case 2:   // press MENU UP 
      g_menuManager.DoMenuAction( MENU_ACTION_UP );
      configUpdated = true;
      break;
      
    case 3:  // press MENU DOWN 
      g_menuManager.DoMenuAction( MENU_ACTION_DOWN );
      configUpdated = true;
      break;

    case 1: // press MENU RIGHT
      if( g_isDisplaying )  {
         g_isDisplaying = false;
         g_menuManager.DrawMenu();  
      } else {
         g_menuManager.DoMenuAction( MENU_ACTION_SELECT );
      }
      configUpdated = true;
      break;
      
    case 4: // LEFT << 
      g_menuManager.DoMenuAction( MENU_ACTION_BACK );
      configUpdated = true;
      break;
      
    default:
      break;
  }
}
  
void configure_LCD_Menu_via_IR_remote (long tmp) {
   
  switch (tmp) {

     case 0xE0E040BF : 
     // turnAllStrandsOnOff();  
      configUpdated = true;
      break;

   //     == STRAND 0 ==

    case 0xE0E01AE5 :
      Serial.println("Entering menu operation"); 
     // enterMenu(); 
      configUpdated = true;
      break; 
        
    case 0xE0E020DF : // BUTTON 1 
     // setNextPatternForStrand(0);
      configUpdated = true;
      break;

    case 0xE0E0A05F : // BUTTON 2 
      //turnIndStrandOnOff(0);
      configUpdated = true;
      break;

//     == STRAND 1 ==

    case 0xE0E010EF :  // BUTTON 4
    //  setNextPatternForStrand(1);
      configUpdated = true;
      break;

    case 0xE0E0906F :  // BUTTON 5
    //  turnIndStrandOnOff(1);
      configUpdated = true;
      break;

//     == STRAND 2 ==

    case 0xE0E030CF :  // BUTTON 7
    //  setNextPatternForStrand(2);
      configUpdated = true;
      break;

    case 0xE0E0B04F :  // BUTTON 8
    //  turnIndStrandOnOff(2);
      configUpdated = true;
      break;

//     == STRAND 3 ==

    case 0xE0E0C43B :  // BUTTON -
    //  setNextPatternForStrand(3);
      configUpdated = true;
      break;

   case 0xE0E08877 :  // BUTTON 0
    //  turnIndStrandOnOff(3);
      configUpdated = true;
      break;


    case 0xE0E0807F :  // SOURCE button
   //   fillStrandSolid();
      configUpdated = true;
      break;

    case 0xE0E0E01F :
      //Serial.println("Vol+");
     // gbrightnessUp();
      configUpdated = true;
      break;

   case 0xE0E0D02F :
      //Serial.println("Vol-");
      // gbrightnessDown();
      configUpdated = true;
      break;

/*
     case 0xE0E036C9 :
       Serial.println("Setting RED config");
       makeAllRed();
       configUpdated = true;
       break;
 */     

/*
    case 0xE0E046B9 : // RIGHT ARROW 
      Serial.println("Page all strands to next sketch");
      memset might not work for all numbers...
      memset(stripPatternConfig, gSketch++ , sizeof(stripPatternConfig));
      memset(gHue,0,sizeof(gHue));
      configUpdated = true;
      break;
*/

/*
   case 0xE0E0A659 : LEFT 
        Serial.println("Page all strands to previous sketch");
        memset might not work for all numbers...
        gSketch = (gSketch + 1) % ARRAY_SIZE( gPatterns);
        memset(stripPatternConfig, gSketch-- , sizeof(stripPatternConfig));
        memset(gHue,0,sizeof(gHue));
        configUpdated = true;
        break;
   */

/*
   case 0xE0E006F9 :
       // Cycle trough next favorite pattern
      for (byte i=0; i < NUM_STRIPS; i++){ 
         stripPatternConfig[i] =  favoriteConfigs[gFavPatternIndex][i];
      }
      gFavPatternIndex = (gFavPatternIndex + 1) % ARRAY_SIZE(favoriteConfigs);
      configUpdated = true;
      break;
*/

/*
   case 0xE0E012ED :
       Serial.println("Reset to default setting");
       memset(stripPatternConfig,0,sizeof(stripPatternConfig));
       memset(gHue,0,sizeof(gHue));
       configUpdated = true;
       break;
*/

     case 0xE0E006F9: // UP ARROW
      g_menuManager.DoMenuAction( MENU_ACTION_UP );
      configUpdated = true;
      break;

    case 0xE0E08679: // DOWN ARROW 
      g_menuManager.DoMenuAction( MENU_ACTION_DOWN );
      configUpdated = true;
      break;

    case 0xE0E0A659: // LEFT << 
      g_menuManager.DoMenuAction( MENU_ACTION_BACK );
      configUpdated = true;
      break;
      
    case 0xE0E046B9: // right >> 
      if( g_isDisplaying )  {
         g_isDisplaying = false;
         g_menuManager.DrawMenu();  
      } else {
         g_menuManager.DoMenuAction( MENU_ACTION_SELECT );
      }
      configUpdated = true;
      break;       
  }
}




