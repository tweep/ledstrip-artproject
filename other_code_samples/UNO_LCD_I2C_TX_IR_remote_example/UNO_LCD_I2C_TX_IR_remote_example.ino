
/*  ARDUINO UNO - SENDING / LCD
 *  - IR on port 2 
 *  -SDA / SCL on freigesaegte ports 
 *  no GROUND connection
 *  all works except I2C 
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


int button = 0 ; 
int readKey = 0; 
const int LCDD7 = 7;
const int LCDD6 = 6;
const int LCDD5 = 5;
const int LCDD4 = 4;
const int LCDE  = 9;
const int LCDRS = 8;

bool configUpdated = false;

// IR REMOTE CODE
#define IR_RECV_PIN    2

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

// EasyTransfer code 
EasyTransferI2C ET; 

struct SEND_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  uint8_t g_ledStripToConfigure;
  uint8_t g_toggleStrand;
  uint8_t g_theme;
  uint8_t g_speed;
  uint8_t g_bright;
  uint8_t g_color;
  uint8_t blinks;
  uint8_t pause;
};

SEND_DATA_STRUCTURE mydata;
#define I2C_SLAVE_ADDRESS 9  

//Now create the MenuLCD and MenuManager classes.
MenuLCD g_menuLCD( LCDRS, LCDE, LCDD4, LCDD5, LCDD6, LCDD7, 16, 2);

const bool doRootAction = true;
MenuManager g_menuManager( &g_menuLCD, doRootAction);  //pass  g_menuLCD object to g_menuManager with the & operator (pass the address)
                                                      // also pass boolean variable to configure the Menu if callback functions for 
                                                      // menu items with children are executed or not

unsigned int g_isDisplaying = false;


int  strandOneSpeed       = 100;
int  strandOneBrightness  =  60;
int  strandOneColor       = 123;

// Global variables to configure our LED strips

uint8_t g_ledStripToConfigure = 0; // the current strand to configure. set in root menu
byte    g_toggleStrand = 1;        // variable to turn specific strand on/off
int     g_theme;                   // MenuManager::DoIntInput only takes int but no uint8_t
int     g_speed;
int     g_bright;
int     g_color; 

byte arrow_up[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};


/*
 *    1. LED strip
 *    |     |----Choose Theme
 *    |     |----Set Speed 
 *    |     |----Set Color
 *    |     |----Set Brightness
 *    |     |----Turn Strand ON
 *    |     |----Turn Strand OFF
 *    |              
 *    2. LED strip
 *    |     |----Choose Theme
 *    |     |----Set Speed 
 *    |     |----Set Color
 *    |     |----Set Brightness
 *    |     |----Turn Strand ON
 *    |     |----Turn Strand OFF
 *    |              

 */


int evaluateButton(int x) {
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

/*
 * Methods to set the number of the LED strip which we are currently configuring
 */

void configureLEDstripOne ( char * pMenuText, void * pUserData ) {
  // set the variable at address pUserData to value 0 (address gets handed over in menu with &variable)
   *((unsigned int *) pUserData) = 0;
}
void configureLEDstripTwo ( char * pMenuText, void * pUserData ) {
   *((unsigned int *) pUserData) = 1;
}


 
//This callback uses the built-in Int Input routine in MenuManager.h to request input of a integer number from the 
//user.  Control will pass to the DoIntInput function until the user finishes.  the g_timerTime will be set to the 
//value the user selects.

void setTheme( char* pMenuText, void *pUserData ) {
  char *pLabel = "Choose Theme ^";
  
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 10;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 1;
                                                                                // hand over int variable for strand 1 
                                                                                // instead, we like to 
                                                                                // - have the strand number + manipulate on the them for the specific strand.
                                                                                // 1) setTheme should take additional parameter for strand 
                                                                                // 2) g_theme should be the array of the strand ?
                                                                                //   ..... &strandOneConfig 
  int g_previous_value = g_theme;                                                        
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_theme );
  if ( g_previous_value != g_theme ) { 
    Serial.println("Value changed");
  }
  
  Serial.print("Strand: "); 
  Serial.print(g_ledStripToConfigure); 
  Serial.println("  Theme: "); 
  Serial.println(g_theme); 

 // ledStripConfig.setTheme(1,g_theme); 
}
void SetSpeed ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Choose Theme ^";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  int g_previous_value = g_speed; 
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_speed );
  // check if the variable has been changed. copy variable value, then compare by value. 
  if ( g_previous_value != g_speed ) { 
    Serial.println("Value changed");
  }
}
void SetBrightness ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Brightness ↑";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  int g_previous_value = g_bright; 
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_bright);
  // check if the variable has been changed. copy variable value, then compare by value. 
  if ( g_previous_value != g_bright ) { 
    Serial.println("Value changed");
  }
} 
void SetColor ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Choose Color ^:";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;

  int g_previous_value = g_color; 
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_color);
  // check if the variable has been changed. copy variable value, then compare by value. 
  if ( g_previous_value != g_color ) { 
    Serial.println("Value changed");
  }
}
void configure_stuff_with_ir_remote (long tmp) {
  configUpdated = true;
   
  switch (tmp) {


     case 0xE0E040BF : 
     // turnAllStrandsOnOff();  
      break;

   //     == STRAND 0 ==

    case 0xE0E01AE5 :
      Serial.println("Entering menu operation"); 
     // enterMenu(); 
      break; 
        
    case 0xE0E020DF : // BUTTON 1 
     // setNextPatternForStrand(0);
      break;

    case 0xE0E0A05F : // BUTTON 2 
      //turnIndStrandOnOff(0);
      break;

//     == STRAND 1 ==

    case 0xE0E010EF :  // BUTTON 4
    //  setNextPatternForStrand(1);
      break;

    case 0xE0E0906F :  // BUTTON 5
    //  turnIndStrandOnOff(1);
      break;

//     == STRAND 2 ==

    case 0xE0E030CF :  // BUTTON 7
    //  setNextPatternForStrand(2);
      break;

    case 0xE0E0B04F :  // BUTTON 8
    //  turnIndStrandOnOff(2);
      break;

//     == STRAND 3 ==

    case 0xE0E0C43B :  // BUTTON -
    //  setNextPatternForStrand(3);
      break;

   case 0xE0E08877 :  // BUTTON 0
    //  turnIndStrandOnOff(3);
      break;


    case 0xE0E0807F :  // SOURCE button
   //   fillStrandSolid();
      break;

    case 0xE0E0E01F :
      //Serial.println("Vol+");
    //  gbrightnessUp();
      break;

    case 0xE0E0D02F :
      //Serial.println("Vol-");
     // gbrightnessDown();
      break;

     case 0xE0E036C9 :
     // Serial.println("Setting RED config");
    //  makeAllRed();
      break;

  //   case 0xE0E046B9 : // RIGHT ARROW 
     // Serial.println("Page all strands to next sketch");
      // memset might not work for all numbers...
  //    memset(stripPatternConfig, gSketch++ , sizeof(stripPatternConfig));
   //   memset(gHue,0,sizeof(gHue));
    //  break;

  //   case 0xE0E0A659 : LEFT 
      //Serial.println("Page all strands to previous sketch");
      // memset might not work for all numbers...
  //    gSketch = (gSketch + 1) % ARRAY_SIZE( gPatterns);
   //   memset(stripPatternConfig, gSketch-- , sizeof(stripPatternConfig));
   //   memset(gHue,0,sizeof(gHue));
   //   break;

  //   case 0xE0E006F9 :
       // Cycle trough next favorite pattern
  //     for (byte i=0; i < NUM_STRIPS; i++){ 
  //       stripPatternConfig[i] =  favoriteConfigs[gFavPatternIndex][i];
  //     }
  //    gFavPatternIndex = (gFavPatternIndex + 1) % ARRAY_SIZE(favoriteConfigs);
      break;

     case 0xE0E012ED :
     // Serial.println("Reset to default setting");
   //   memset(stripPatternConfig,0,sizeof(stripPatternConfig));
   //   memset(gHue,0,sizeof(gHue));
      break;

     case 0xE0E006F9: // UP ARROW
      g_menuManager.DoMenuAction( MENU_ACTION_UP );
      break;

    case 0xE0E08679: // DOWN ARROW 
      g_menuManager.DoMenuAction( MENU_ACTION_DOWN );
      break;

    case 0xE0E0A659: // LEFT << 
      g_menuManager.DoMenuAction( MENU_ACTION_BACK );
      break;
      
    case 0xE0E046B9: // right >> 
      if( g_isDisplaying )  {
         g_isDisplaying = false;
         g_menuManager.DrawMenu();  
      } else {
         g_menuManager.DoMenuAction( MENU_ACTION_SELECT );
      }
      break;       
  }
}

void setupMenus() {  
  g_menuLCD.MenuLCDSetup();  
  //Add nodes via a depth-first traversal order
  MenuEntry * p_menuEntryRoot;
  //Add root node
  //MenuEntry( char * menuText, void * userData/*=0*/, MENU_ACTION_CALLBACK_FUNC func);


//                                                                       VARIABLE-TO-CHANGE   FUNCTION_WHICH_MANIPULATES_VARIABLE 
//    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *)   (&toggleStrand),     fuckYesCallbackFunction ) );



p_menuEntryRoot = new MenuEntry("1. LED Strand ",            (void *) (&g_ledStripToConfigure) , configureLEDstripOne);  
    g_menuManager.addMenuRoot( p_menuEntryRoot );
    g_menuManager.addChild( new MenuEntry("Set Theme",       (void *) (&g_theme),  setTheme) );
    g_menuManager.addChild( new MenuEntry("Set Speed",       (void *) (&g_speed),  SetSpeed ));
    g_menuManager.addChild( new MenuEntry("Set Brightness",  (void *) (&g_bright), SetBrightness ));
    g_menuManager.addChild( new MenuEntry("Set Color",       (void *) (&g_color),  SetColor ));
    
    //                                                                 pass address of variable and pass MenuEntry_BoolTrueCallbacFunc    
    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *) (&g_toggleStrand), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Turn Strip OFF", (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Reset strand",   (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    //                                                               pass address of menuManager obj + callback function to call data 
    //                                                               can we pass the ledConfigManager here ? 
    //                                                               &ledConfig       setTheme
    g_menuManager.addChild( new MenuEntry("Back",            (void *) &g_menuManager, MenuEntry_BackCallbackFunc) );

g_menuManager.SelectRoot();
g_menuManager.addSibling( new MenuEntry("2. LED Strand ",    (void *) (&g_ledStripToConfigure), configureLEDstripTwo ) );
g_menuManager.MenuDown();
    g_menuManager.addChild( new MenuEntry("Set Theme",       (void *) (&g_theme),        setTheme) );
    g_menuManager.addChild( new MenuEntry("Set Speed",       (void *) (&g_speed),        SetSpeed ));
    g_menuManager.addChild( new MenuEntry("Set Brightness",  (void *) (&g_bright),       SetBrightness ));
    g_menuManager.addChild( new MenuEntry("Set Color",       (void *) (&g_color),        SetColor ));
    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *) (&g_toggleStrand), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Turn Strip OFF", (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Reset strand",   (void *) (&g_toggleStrand), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry("Back",            (void *) &g_menuManager,    MenuEntry_BackCallbackFunc) );

g_menuManager.SelectRoot();
g_menuManager.addSibling( new MenuEntry("Global configuration ",   (void *) (&g_ledStripToConfigure), configureLEDstripTwo ) );
g_menuManager.MenuDown();
    g_menuManager.addChild( new MenuEntry("Reset all strands", (void *) (&g_theme),     setTheme) );
    g_menuManager.addChild( new MenuEntry("Global brightness", (void *) (&g_bright),    SetBrightness ));
    g_menuManager.addChild( new MenuEntry("Global color",      (void *) (&g_color),     SetColor ));
    g_menuManager.addChild( new MenuEntry("Reset all strands", (void *) (&g_theme),     setTheme) );
    g_menuManager.addChild( new MenuEntry("Back",              (void *) &g_menuManager, MenuEntry_BackCallbackFunc) );

//Get the selection state back to the root for startup and to add the last entry
 g_menuManager.SelectRoot();
 g_menuManager.DrawMenu();
 
 //g_menuLCD.getLCD()->createChar( 0, arrow_up );

}

void fuckYesCallbackFunction ( char * pMenuText, void * pUserData ) {
  // dereference data type which is userData
  *((unsigned int *) pUserData) = true;
}

void setup() {  
  delay(3000); // 3 second delay for recovery         
  Serial.begin(9600);

  // IR remote setup 
  IRrecv irrecv(IR_RECV_PIN);
  irrecv.enableIRIn();
  decode_results results;
  setupMenus();
  
  //EasyTransfer setup 
  Wire.begin();
  ET.begin(details(mydata), &Wire);
  randomSeed(analogRead(0));
  pinMode(13, OUTPUT);
}

void loop()  {

  // INPUT from INFRARED REMOTE 
  while (!irrecv.isIdle());
  
  if (irrecv.decode(&results)) {  
    configure_stuff_with_ir_remote(results.value);
    irrecv.resume();
  }

  // INPUT from LCD keypad 
  readKey = analogRead(0);
   if (readKey < 790) {
    delay(100);
    readKey = analogRead(0);
   }
   configureMenuViaLEDScreenButtons( readKey); 

   if (configUpdated) { 
     Serial.print("Config updated - sending data.. ");
     Serial.print("Strip: ");Serial.print(g_ledStripToConfigure); 
     Serial.print(" togle:"); Serial.print(g_toggleStrand);
     Serial.print(" theme: "); Serial.print(g_theme);
     Serial.print(" speed: "); Serial.print(g_speed);
     Serial.print(" brigh: "); Serial.print(g_bright);
     Serial.print(" color: "); Serial.print(g_color);
     Serial.println(" ");
     i2c_send_data_structure();
     configUpdated = false;
   }
}

void i2c_send_data_structure(){
  
  Serial.println("EasyTransfer SENDING");
   mydata.blinks = 3;
   mydata.pause = 2;
   ET.sendData(I2C_SLAVE_ADDRESS);
   
  //Just for fun, we will blink it out too
  for(int i = mydata.blinks; i>0; i--){
    digitalWrite(13, HIGH);
    delay(mydata.pause * 100);
    digitalWrite(13, LOW);
    delay(mydata.pause * 100);
   }
 delay(10);
}
void configureMenuViaLEDScreenButtons(int rk) { 
  button = evaluateButton(readKey);
  
  switch( button ) {
    case 2: 
      g_menuManager.DoMenuAction( MENU_ACTION_UP );
      configUpdated = true;
      Serial.println("UP");
      break;
      
    case 3: 
      g_menuManager.DoMenuAction( MENU_ACTION_DOWN );
      break;
      configUpdated = true;

    case 1: // right >> 
    
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
  






