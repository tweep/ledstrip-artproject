/*  ARDUINO UNO - SENDING / LCD
 *  - Infrared IR remote receiver on port 2 
 *  - SDA / SCL on freigesaegte ports 
 *  no GROUND connection
 *  - LCD on various other ports 
 *  
 *  This sketch is running on an Arduino MEGA. It takes input from an LCD keypad or an infrared remote. 
 *  It sends the desired configuration to the connected other Arduino. 
 *  Problem: At the moment, we use a "global" parameter to send global configs, like brightness or turn on/off. 
 *  the logic is than translated at the FASTLED Slave side. 
 *  we could also implement the logic in here (LCD MENU) and then just send it over ? 
 *  IF LCD doees logic: 
 *  - know what pattern "go black" is 
 *  - set information for all connected strands over. transmit time + reaction time 
 *    vs. send one information over, then code takes it from there...
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


#define NUM_STRIPS 36
#define gNrPatterns  7   // 0 to 7 - change this if you add new patterns. not ideal really. just do ++ and use type byte and check on slave if it's out of aray 
#define blackPattern 6

bool configUpdated = false;
bool gSwitch       = false;
bool gConf         = false;
byte gPattern      = 0;
int  gBright       = 128;     // start brightness

// these values will be applied to ALL strands at startup
byte initialPattern    = 0;
byte initialSpeed      = 60;
byte initialBrightness = 200;


// IR REMOTE CODE
#define IR_RECV_PIN    53

byte gPrevPattern[NUM_STRIPS];


IRrecv irrecv(IR_RECV_PIN);
decode_results results;

EasyTransferI2C ET; 

struct SEND_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int ledStripToConfigure;
  int pattern;
  int spd;
  int bright;
  int color;
} mydata; 

typedef struct{
  int pattern;
  int spd;
  int bright;
  int color;
} struct_ledconf;
struct_ledconf ledConfig[NUM_STRIPS];


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


/* LED MENU configuration for different strips */ 

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
   *(( int *) pUserData) = 99;
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
  Serial.begin(115200);
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
  /*
  int readKey = analogRead(0);
   if (readKey < 790) {
    delay(100);
    readKey = analogRead(0);
   }
   
   configure_LCD_Menu_via_LEDScreenButtons(readKey); 
*/
   if (configUpdated) { 
     i2c_send_data_structure();
     configUpdated = false;
   }
}


void i2c_send_init_config(){
   Serial.println("Sending initial configuration to SLAVE LED driver");
   
     
   /* Loop trough all strands and send the inital configuration to the   
    *  other Arduino
    */
    
   for (byte i=0; i < NUM_STRIPS; i++){ 
     Serial.print("INIT Strand: ");
     Serial.println(i);
     mydata.ledStripToConfigure = i; 
     mydata.pattern             = initialPattern;
     mydata.spd                 = initialSpeed;
     mydata.bright              = initialBrightness;
     mydata.color               = 0;
     ET.sendData(I2C_SLAVE_ADDRESS);
     delay(15);
   }
}

void i2c_send_data_structure(){
  
  if ( gConf  == true ) { 
    gConf = !gConf;  // flag if we choose a global config parameter or not.
                     // a global config param will be sent to ALL strands

      for (byte i=0; i < NUM_STRIPS; i++){ 
        mydata.pattern = ledConfig[i].pattern;
        mydata.bright =  ledConfig[i].bright;
        mydata.ledStripToConfigure = i;
        for(byte j=0;j<3;j++){
          ET.sendData(I2C_SLAVE_ADDRESS);
          delay(15);
        }
      //  Serial.print("Sending data for all strands...");
   //     Serial.println(i);
     //   delay(30);
      }
      Serial.print("Globbal config updated - sending data.. ");
      Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
      Serial.print(" pattern: "); Serial.print(mydata.pattern);
      Serial.print(" speed: "); Serial.print(mydata.spd);
      Serial.print(" brigh: "); Serial.print(mydata.bright);
      Serial.print(" color: "); Serial.print(mydata.color);
      Serial.println(" ");


  }else {
   mydata.pattern      = ledConfig[mydata.ledStripToConfigure].pattern;
   mydata.spd          = ledConfig[mydata.ledStripToConfigure].spd;
   mydata.bright       = ledConfig[mydata.ledStripToConfigure].bright;
   mydata.color        = ledConfig[mydata.ledStripToConfigure].color;

  
   Serial.print("Strand specific config updated - sending data.. ");
   Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
   Serial.print(" pattern: "); Serial.print(ledConfig[mydata.ledStripToConfigure].pattern);
   Serial.print(" speed: "); Serial.print(ledConfig[mydata.ledStripToConfigure].spd);
   Serial.print(" brigh: "); Serial.print(ledConfig[mydata.ledStripToConfigure].bright);
   Serial.print(" color: "); Serial.print(ledConfig[mydata.ledStripToConfigure].color);
   Serial.println(" ");
   ET.sendData(I2C_SLAVE_ADDRESS);

  }
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
  Serial.print("Retrieved HEX:");
  Serial.println(tmp,HEX);
  
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
      setNextPatternForStrand(0);
      configUpdated = true;
      break;

    case 0xE0E0A05F : // BUTTON 2
      setNextPatternForStrand(1);
      configUpdated = true;
      break;

    case 0xE0E0609F : // BUTTON 3
      setNextPatternForStrand(2);
      configUpdated = true;
      break;

    case 0xE0E010EF :  // BUTTON 4
      setNextPatternForStrand(3);
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
      gbrightnessUp();
      configUpdated = true;
      break;

   case 0xE0E0D02F :
      //Serial.println("Vol-");
      gbrightnessDown();
      configUpdated = true;
      break;

    case 0xE0E048B7 :
      //Ch+ : Set all strands to next pattern
      setNextGlobalPattern();
      configUpdated = true;
      break;

    case 0xE0E008F7 :
      //Ch- : Set all strands to previous pattern
      setPrevGlobalPattern();
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
  delay(10);
}

void configureStrands(){
  // configuration of all strips - config is sent to SLAVE 
  Serial.println("Setting the default config for all strands... ");
  for (byte i=0; i < NUM_STRIPS; i++){ 
    ledConfig[i].pattern             = initialPattern;
    ledConfig[i].spd                 = initialSpeed;
    ledConfig[i].bright              = initialBrightness;
    ledConfig[i].color               =  0;

  }
}

void turnAllStrandsOnOff(){
  

  for (byte i=0; i < NUM_STRIPS; i++){ 
    if ( gSwitch == true ) { 
      Serial.println("Turn all strands back to previous pattern / ON ");
      ledConfig[i].pattern = gPrevPattern[i];
    }else{ 
      Serial.println("Turn all strands back to blackend pattern / OFF ");
      gPrevPattern[i] = ledConfig[i].pattern; // save previous pattern for when we turn it back on again.
      ledConfig[i].pattern = blackPattern; // pattern 6 is the "BLACK PATTERN" 
    }
  }
  gSwitch = !gSwitch; 
  gConf = true; 
}

void gbrightnessUp(){
   gConf = true; 
   gBright = (byte) (gBright + 25);
   for (byte i=0; i < NUM_STRIPS; i++){ 
    ledConfig[i].bright = gBright;
   }

}

void gbrightnessDown(){
   gConf = true;  // flag to indicate that we send config for ALL strands.
   gBright = (byte) (gBright - 25);
   for (byte i=0; i < NUM_STRIPS; i++){ 
    ledConfig[i].bright = gBright;
   }
}

void setNextPatternForStrand(int strand){
  mydata.ledStripToConfigure = strand;
  
  int pat = ledConfig[mydata.ledStripToConfigure].pattern;
  pat++;
  ledConfig[mydata.ledStripToConfigure].pattern = (pat % gNrPatterns);
  }

void setNextGlobalPattern(){
  // Rotate all strands to the same next global pattern
  gConf = true;  // flag to indicate that we send config for ALL strands.

  for (byte i=0; i < NUM_STRIPS; i++){ 
    ledConfig[i].pattern = gPattern;
  }
  gPattern++;
  gPattern = ( gPattern % gNrPatterns);
}

void setPrevGlobalPattern(){
  gConf = true;  // flag to indicate that we send config for ALL strands.

  for (byte i=0; i < NUM_STRIPS; i++){ 
    ledConfig[i].pattern = gPattern;
  }
  gPattern--;
  gPattern = ( gPattern % gNrPatterns);
}


/*
 * some patterns can take additonal parameters. color. speed. which ones ? 
 * cycle trough a list of nice, predefined patterns : strand 1 = 1, strand 2 = 3, strand 3 = 10; .. 
 */
