 
#include <LiquidCrystal.h>
#include "MenuEntry.h"
#include "MenuLCD.h"
#include "MenuManager.h"

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

/*
 * 
   MenuConfig->setBrightness(255);
   MenuConfig->setTheme(4);
   MenuConfig->setSpeed(100);
   MenuConfig->setColor(55);

   MenuConfig->getPatternForStrand(1); 
   MenuConfig->getNextPatternForStrand(1);
   MenuConfig->getPreviousPatternForStrand(1);

   MenuConfig->setPatternForStrand(1, 100); 
   MenuConfig->toggleStrand(1);

turnAllStrandsOnOff
turnIndStrandOnOff
dimStrand
fillStrandSolid
gbrightnessUp
gbrightnessDown

rainbow()
rainbowWithGlitter() 


uint8_t nrLedsPerStrand[] = {60, 60, 60, 60};
uint8_t stripPatternConfig[] = { 1, 1, 2, 2 };

boolean strandSwitch[NUM_STRIPS]    = {true, true, true, true}; // ON/OFF - keep memory of which strands are on / off.
byte nrLedsPerStrand[NUM_STRIPS]    = {60, 60,60, 60};
byte stripPatternConfig[NUM_STRIPS] = {0, 0,0, 0};    // start pattern config: all strands start with pattern 0,0,0,0
byte gHue[NUM_STRIPS]               = { 0, 0, 0, 0};   // rotating "base color" used by many of the patterns, so each strip can have different gHue
byte gPrevPattern[NUM_STRIPS]       = {0,  0, 0 , 0};   // previsous pattern before turning strand off.
byte favoriteConfigs[3][NUM_STRIPS] = { {0,0,0,0},{1,1,0,0},{1,1,3,3} };  // cycle trough a set of favorite configs.


   
*/

//Get the selection state back to the root for startup and to add the last entry
 g_menuManager.SelectRoot();
 g_menuManager.DrawMenu();
 
 //g_menuLCD.getLCD()->createChar( 0, arrow_up );

}


void fuckYesCallbackFunction ( char * pMenuText, void * pUserData ) {
  // dereference data type which is userData
  *((unsigned int *) pUserData) = true;
}



void setup() 
{           
  Serial.begin(9600);
  Serial.print("Ready.");
  setupMenus();
}



void loop()  {
   Serial.println("READY");
   readKey = analogRead(0);
   if (readKey < 790) {
    delay(100);
    readKey = analogRead(0);
   }
   button = evaluateButton(readKey);
    Serial.print("button:");

    Serial.println(button);
  
  switch( button ) {
    
    case 2: 
      g_menuManager.DoMenuAction( MENU_ACTION_UP );
      break;
      
    case 3: 
      g_menuManager.DoMenuAction( MENU_ACTION_DOWN );
      break;
      
    case 1: // right >> 
    
      if( g_isDisplaying )  {
         g_isDisplaying = false;
         g_menuManager.DrawMenu();  
      } else {
         g_menuManager.DoMenuAction( MENU_ACTION_SELECT );
      }
      break;
      
    case 4: // LEFT << 
      g_menuManager.DoMenuAction( MENU_ACTION_BACK );
      break;
      
    default:
      break;
  }

Serial.print("Strip: ");Serial.print(g_ledStripToConfigure); 
Serial.print(" togle:"); Serial.print(g_toggleStrand);
Serial.print(" theme: "); Serial.print(g_theme);
Serial.print(" speed: "); Serial.print(g_speed);
Serial.print(" brigh: "); Serial.print(g_bright);
Serial.print(" color: "); Serial.print(g_color);

Serial.println(" ");


}


// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
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
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_theme );
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
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_speed );
}


void SetBrightness ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Brightness ↑";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_bright);
}

  
void SetColor ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Choose Color ^:";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &g_color);
}


