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
bool gSwitch = false;
bool gConf   = false;
#define nrLEDStrands 4


// IR REMOTE CODE
#define IR_RECV_PIN    53

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

EasyTransferI2C ET; 

struct SEND_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  bool global;
  int ledStripToConfigure;
  bool toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
} mydata; 

typedef struct{
  bool toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
  int blinks;
  int pause;
} struct_ledconf;
struct_ledconf ledConfig[nrLEDStrands];

struct GLOBAL_CONF {
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  bool global;
  int ledStripToConfigure;
  bool toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
} global_conf; 



#define I2C_SLAVE_ADDRESS 9  

//Now create the MenuLCD and MenuManager classes.
MenuLCD g_menuLCD( LCDRS, LCDE, LCDD4, LCDD5, LCDD6, LCDD7, 16, 2);

const bool doRootAction = true;
MenuManager g_menuManager( &g_menuLCD, doRootAction);  //pass  g_menuLCD object to g_menuManager with the & operator (pass the address)
                                                      // also pass boolean variable to configure the Menu if callback functions for 
                                                      // menu items with children are executed or not

unsigned int g_isDisplaying = false;


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
void configureLEDstripFour ( char * pMenuText, void * pUserData ) {
   *(( int *) pUserData) = 3;
}
void configureLEDstripAll ( char * pMenuText, void * pUserData ) {
   // dereference data type which is userData
   *(( int *) pUserData) = -1;
}

/*This callback uses the built-in Int Input routine in MenuManager.h to request input of a integer number from the 
  user.  Control will pass to the DoIntInput function until the user finishes.  the g_timerTime will be set to the 
  value the user selects.
*/


void SetPattern( char* pMenuText, void *pUserData ) {
  const char *pLabel = (char *)"Choose Pattern";

  int iNumLabelLines = 1;
  int iMin   = 1;
  int iMax   = 10;
  // previously stored value 
  int iStart = ledConfig[mydata.ledStripToConfigure].pattern;  // global       
  int iStep  = 1;  //Each user input action (such as a push of button) will step this amount
   g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &ledConfig[mydata.ledStripToConfigure].pattern );    
}
void SetSpeed ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char *)"Choose Speed ^";
  int iNumLabelLines = 1;
  int iMin   = 1;
  int iMax   = 255;
  int iStart = ledConfig[mydata.ledStripToConfigure].spd; 
  int iStep  = 5;
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines,&ledConfig[mydata.ledStripToConfigure].spd );
}
void SetBrightness ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char *)"Choose Brightness";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = ledConfig[mydata.ledStripToConfigure].bright;
  int iStep = 5;
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &ledConfig[mydata.ledStripToConfigure].bright);

}
void SetColor ( char* pMenuText, void *pUserData ) {
  char *pLabel = (char *)"Choose Color ^:";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = ledConfig[mydata.ledStripToConfigure].color;

  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;

  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &ledConfig[mydata.ledStripToConfigure].color);
}
void setupMenus() {  
  g_menuLCD.MenuLCDSetup();  
  MenuEntry * p_menuEntryRoot;
  p_menuEntryRoot = new MenuEntry((char *)"1. LED Strand ",  (void *) (&mydata.ledStripToConfigure), configureLEDstripOne);  
    g_menuManager.addMenuRoot( p_menuEntryRoot );
    g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",    NULL,  SetPattern ));    
    g_menuManager.addChild( new MenuEntry((char *)"Set Speed",      NULL,  SetSpeed ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Brightness", NULL,  SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Color",      NULL,  SetColor ));
    
 
    g_menuManager.addChild( new MenuEntry((char *)"Turn Strip ON",    NULL,   MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF",  NULL,  MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Reset strand",    NULL, MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Back",            (void *)  &g_menuManager,  MenuEntry_BackCallbackFunc) );

g_menuManager.addSibling( new MenuEntry((char *)"2. LED Strand ",   (void *) (&mydata.ledStripToConfigure), configureLEDstripTwo ) );
g_menuManager.MenuDown();

    g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",      NULL,  SetPattern) );
    g_menuManager.addChild( new MenuEntry((char *)"Set Speed",        NULL,      SetSpeed ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Brightness",   NULL,   SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char *)"Set Color",        NULL,    SetColor ));
    g_menuManager.addChild( new MenuEntry((char *) "Turn Strip ON",   NULL,   MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF",  NULL,   MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Reset strand",    NULL,   MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry((char *) "Back",       (void *)  &g_menuManager,   MenuEntry_BackCallbackFunc) );


g_menuManager.addSibling( new MenuEntry((char *)"3. LED Strand ",     (void *) (&mydata.ledStripToConfigure) , configureLEDstripThree ) );
g_menuManager.MenuDown();
  g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",             NULL,   SetPattern) );
  g_menuManager.addChild( new MenuEntry((char *)"Set Speed",               NULL,   SetSpeed ));
  g_menuManager.addChild( new MenuEntry((char *)"Set Brightness",          NULL,   SetBrightness ));
  g_menuManager.addChild( new MenuEntry((char *)"Set Color",               NULL,   SetColor ));
  g_menuManager.addChild( new MenuEntry((char *) "Turn Strip ON",          NULL,   MenuEntry_BoolTrueCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF",         NULL,   MenuEntry_BoolFalseCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Reset strand",           NULL,   MenuEntry_BoolFalseCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Back",                   (void *)  &g_menuManager,   MenuEntry_BackCallbackFunc) );


g_menuManager.addSibling( new MenuEntry((char *)"4. LED Strand ",     (void *) (&mydata.ledStripToConfigure) , configureLEDstripFour ) );
g_menuManager.MenuDown();
  g_menuManager.addChild( new MenuEntry((char *)"Set Pattern",             NULL,   SetPattern) );
  g_menuManager.addChild( new MenuEntry((char *)"Set Speed",               NULL,   SetSpeed ));
  g_menuManager.addChild( new MenuEntry((char *)"Set Brightness",          NULL,   SetBrightness ));
  g_menuManager.addChild( new MenuEntry((char *)"Set Color",               NULL,   SetColor ));
  g_menuManager.addChild( new MenuEntry((char *) "Turn Strip ON",          NULL,   MenuEntry_BoolTrueCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Turn Strip OFF",         NULL,   MenuEntry_BoolFalseCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Reset strand",           NULL,   MenuEntry_BoolFalseCallbackFunc ) );
  g_menuManager.addChild( new MenuEntry((char *) "Back",                   (void *)  &g_menuManager,   MenuEntry_BackCallbackFunc) );


g_menuManager.addSibling( new MenuEntry((char *)"Global config",    (void *) (&mydata.ledStripToConfigure) ,     configureLEDstripAll ) );
g_menuManager.MenuDown();
    g_menuManager.addChild( new MenuEntry((char *)"Reset all",             NULL,      SetPattern) );
    g_menuManager.addChild( new MenuEntry((char *)"Global brightness",     NULL,      SetBrightness ));
    g_menuManager.addChild( new MenuEntry((char *)"Global color",          NULL,      SetColor ));
    g_menuManager.addChild( new MenuEntry((char *)"Back",                  (void *)  &g_menuManager,  MenuEntry_BackCallbackFunc) );

//Get the selection state back to the root for startup and to add the last entry
 g_menuManager.SelectRoot();
 g_menuManager.DrawMenu();
 
}

void setup() {  
  delay(1000); // 3 second delay for recovery         
  Serial.begin(9600);
  Serial.println("READY");
  
  IRrecv irrecv(IR_RECV_PIN);
  irrecv.enableIRIn();
  
  setupMenus();
  
  //EasyTransfer setup 
  Wire.begin();
  ET.begin(details(mydata), &Wire);
  randomSeed(analogRead(0));

  configureStrands();
  i2c_send_init_config();
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
     i2c_send_data_structure();
     configUpdated = false;
   }
}


void i2c_send_init_config(){
   Serial.println("Sending initial configuration to SLAVE LED driver");
   
   // send DEFAULT GLOBAL CONFIG 
     mydata.global              = true;
     mydata.ledStripToConfigure =  0;     
     mydata.toggleStrand        =  true;
     mydata.pattern             =  1;
     mydata.spd                 =  1;
     mydata.bright              =  1;
     mydata.color               =  1;
     ET.sendData(I2C_SLAVE_ADDRESS);
     delay(100);
     
   for (byte i=0; i < nrLEDStrands; i++){ 
     Serial.print("Strand:");
     Serial.println(i);
     mydata.ledStripToConfigure = i; 
     mydata.global              = false;   
     mydata.toggleStrand        = ledConfig[i].toggleStrand;
     mydata.pattern             = ledConfig[i].pattern;
     mydata.spd                 = ledConfig[i].spd;
     mydata.bright              = ledConfig[i].bright;
     mydata.color               = ledConfig[i].color;
     ET.sendData(I2C_SLAVE_ADDRESS);
     delay(100);
   }
}

void i2c_send_data_structure(){
  
  if ( gConf  == true ) { 
   // if somewhere in the code we set / modified the confic which applies to ALL strands
   mydata.global       = gConf; 
   mydata.ledStripToConfigure = 0;
   mydata.toggleStrand = global_conf.toggleStrand;
   mydata.pattern      = global_conf.pattern;
   mydata.spd          = global_conf.spd;
   mydata.bright       = global_conf.bright;
   mydata.color        = global_conf.color;
   Serial.print("Globbal config updated - sending data.. ");
   Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
   Serial.print(" global: ");Serial.print(mydata.global); 
   Serial.print(" togle:"); Serial.print(ledConfig[mydata.ledStripToConfigure].toggleStrand);
   Serial.print(" pattern: "); Serial.print(ledConfig[mydata.ledStripToConfigure].pattern);
   Serial.print(" speed: "); Serial.print(ledConfig[mydata.ledStripToConfigure].spd);
   Serial.print(" brigh: "); Serial.print(ledConfig[mydata.ledStripToConfigure].bright);
   Serial.print(" color: "); Serial.print(ledConfig[mydata.ledStripToConfigure].color);
   Serial.println(" ");
   gConf = false;
     
  }else {
   mydata.toggleStrand = ledConfig[mydata.ledStripToConfigure].toggleStrand;
   mydata.pattern      = ledConfig[mydata.ledStripToConfigure].pattern;
   mydata.spd          = ledConfig[mydata.ledStripToConfigure].spd;
   mydata.bright       = ledConfig[mydata.ledStripToConfigure].bright;
   mydata.color        = ledConfig[mydata.ledStripToConfigure].color;

  
   Serial.print("Strand specific config updated - sending data.. ");
   Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
   Serial.print(" global: ");Serial.print(mydata.global); 
   Serial.print(" togle:"); Serial.print(ledConfig[mydata.ledStripToConfigure].toggleStrand);
   Serial.print(" pattern: "); Serial.print(ledConfig[mydata.ledStripToConfigure].pattern);
   Serial.print(" speed: "); Serial.print(ledConfig[mydata.ledStripToConfigure].spd);
   Serial.print(" brigh: "); Serial.print(ledConfig[mydata.ledStripToConfigure].bright);
   Serial.print(" color: "); Serial.print(ledConfig[mydata.ledStripToConfigure].color);
   Serial.println(" ");
  }
   ET.sendData(I2C_SLAVE_ADDRESS);
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
      turnAllStrandsOnOff();  
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

    case 0xE0E016E9: // middle button  
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

void configureStrands(){
  // configuration of all strips - config is sent to SLAVE 
  Serial.println("Configuring default stands");
  ledConfig[0].pattern = 1;
  ledConfig[0].spd     = 66;
  ledConfig[0].bright  = 50;
  
  ledConfig[1].pattern = 2;
  ledConfig[1].spd     = 66;
  ledConfig[1].bright  = 100;

  ledConfig[2].pattern = 3;
  ledConfig[2].spd     = 66;
  ledConfig[2].bright  = 150;

  ledConfig[3].pattern = 4;
  ledConfig[3].spd     = 66;
  ledConfig[3].bright  = 200;

}

void turnAllStrandsOnOff(){
  gSwitch = !gSwitch; 
    gConf = true; 
  if (gSwitch == true ) {
    Serial.println("ALL ON");
    global_conf.global = true;
    global_conf.toggleStrand = gSwitch;
    global_conf.ledStripToConfigure = 0; 
    global_conf.pattern = 0;
    global_conf.spd = 0;
    global_conf.bright = 0;
    global_conf.color = 0;
    
  }else {
    Serial.println("ALL OFF");  
    global_conf.global = true;
    global_conf.toggleStrand = gSwitch;
    global_conf.ledStripToConfigure = 0; 
    global_conf.pattern = 0;
    global_conf.spd = 0;
    global_conf.bright = 0;
    global_conf.color = 0;
  }
}



