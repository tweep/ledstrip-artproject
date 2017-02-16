 
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
//MenuManager g_menuManager( &g_menuLCD);  //pass  g_menuLCD object to g_menuManager with the & operator (pass the address)

const bool doRootAction = false;
MenuManager g_menuManager( &g_menuLCD, doRootAction);  //pass  g_menuLCD object to g_menuManager with the & operator (pass the address)


unsigned int g_isDisplaying = false;

// LED CONFIG STRAND 1
byte strandOneToggle      = 1;
int  strandOneTheme       = 1;
int  strandOneSpeed       = 100;
int  strandOneBrightness  =  60;
int  strandOneColor       = 123;

int g_strandNumber = 0; 


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

 
void setupMenus()
{  
  g_menuLCD.MenuLCDSetup();  
  //Add nodes via a depth-first traversal order
  MenuEntry * p_menuEntryRoot;
  //Add root node
  //MenuEntry( char * menuText, void * userData/*=0*/, MENU_ACTION_CALLBACK_FUNC func);


//                                                                       VARIABLE-TO-CHANGE   FUNCTION_WHICH_MANIPULATES_VARIABLE 
//    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *)   (&strandOneToggle),     fuckYesCallbackFunction ) );

  p_menuEntryRoot = new MenuEntry("1. LED Strand ", NULL , setStrandOne);
  
    g_menuManager.addMenuRoot( p_menuEntryRoot );

    //Use the built-in BOOL setting callbacks from MenuEntry.h: MenuEntry_Bool*CallbackFunc
    g_menuManager.addChild( new MenuEntry("Set Theme",  (void *) (&g_strandNumber), setStrandOne) );
    g_menuManager.addChild( new MenuEntry("Set Speed", NULL, SetSpeed ));
    g_menuManager.addChild( new MenuEntry("Set Brightness", NULL, SetBrightness ));
    g_menuManager.addChild( new MenuEntry("Set Color", NULL, SetColor ));
    //Use the built-in BOOL setting callbacks from MenuEntry.h: MenuEntry_Bool*CallbackFunc 
    //                                                                 pass address of variable and pass MenuEntry_BoolTrueCallbacFunc 
    
    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *) (&strandOneToggle), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Turn Strip OFF", (void *) (&strandOneToggle), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Reset strand", (void *) (&strandOneToggle), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry("Back", (void *) &g_menuManager, MenuEntry_BackCallbackFunc) );

    g_menuManager.SelectRoot();
    g_menuManager.addSibling( new MenuEntry("2. LED Strand ",  (void *) (&g_strandNumber), setStrandTwo ) );
    g_menuManager.MenuDown();
    
    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *) (&strandOneToggle), MenuEntry_BoolTrueCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry( "Turn Strip OFF", (void *) (&strandOneToggle), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry("Set Theme",  (void *) (&g_strandNumber), setStrandTwo) );
    g_menuManager.addChild( new MenuEntry("Set Speed", NULL, SetSpeed ));
    g_menuManager.addChild( new MenuEntry("Set Brightness", NULL, SetBrightness ));
    g_menuManager.addChild( new MenuEntry("Set Color", NULL, SetColor ));
    g_menuManager.addChild( new MenuEntry("Back", (void *) &g_menuManager, MenuEntry_BackCallbackFunc) );


    g_menuManager.SelectRoot();
    g_menuManager.addSibling( new MenuEntry("3. LED Strand", NULL, NULL ) );
    g_menuManager.MenuDown();

//                                                                       VARIABLE-TO-CHANGE   FUNCTION_WHICH_MANIPULATES_VARIABLE 
//    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *)   (&strandOneToggle),     fuckYesCallbackFunction ) );

    
    g_menuManager.addChild( new MenuEntry( "Turn Strip ON",  (void *) (&strandOneToggle), fuckYesCallbackFunction ) );
    g_menuManager.addChild( new MenuEntry( "Turn Strip OFF", (void *) (&strandOneToggle), MenuEntry_BoolFalseCallbackFunc ) );
    g_menuManager.addChild( new MenuEntry("Set Theme", NULL, SetTheme) );
    g_menuManager.addChild( new MenuEntry("Set Speed", NULL, SetSpeed ));
    g_menuManager.addChild( new MenuEntry("Set Brightness", NULL, SetBrightness ));
    g_menuManager.addChild( new MenuEntry("Set Color", NULL, SetColor ));
    g_menuManager.addChild( new MenuEntry("Back", (void *) &g_menuManager, MenuEntry_BackCallbackFunc) );

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



void loop() 
{
  //The example shows using bytes on the serial port to move the menu. You can hook up your buttons or other controls.
 // char incomingByte = 0;
 // if (Serial.available() > 0) 
 // {
 //   incomingByte = Serial.read();
 // }

   readKey = analogRead(0);
   if (readKey < 790) {
    delay(100);
    readKey = analogRead(0);
   }
   button = evaluateButton(readKey);
 
  // Serial.print("Button:");
  // Serial.println(button);
  
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


//Serial.print("1. Strand Theme: ");
//Serial.println(strandOneTheme);

Serial.print("STRAND: ");
Serial.println(g_strandNumber); 

  /*
   *  option to use the variables and manipulate them ...`
   *  in here
   */
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



//This callback uses the built-in Int Input routine in MenuManager.h to request input of a integer number from the 
//user.  Control will pass to the DoIntInput function until the user finishes.  the g_timerTime will be set to the 
//value the user selects.

void SetTheme( char* pMenuText, void *pUserData ) {
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
                                                                                // 2) strandOneTheme should be the array of the strand ?
                                                                                //   ..... &strandOneConfig 
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &strandOneTheme );
}




void setStrandOne ( char * pMenuText, void * pUserData ) {
  Serial.println("STRAND ONE !!!!");
  *((unsigned int *) pUserData) = 12;
  g_strandNumber = 12; 
}

void setStrandTwo ( char * pMenuText, void * pUserData ) {
   Serial.println("STRAND TWO !!!!");
   *((unsigned int *) pUserData) = 1;
   g_strandNumber = 1; 

}



void SetSpeed ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Choose Theme ^";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &strandOneSpeed );
}


void SetBrightness ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Brightness ↑";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &strandOneBrightness);
}

  
void SetColor ( char* pMenuText, void *pUserData ) {
  char *pLabel = "Choose Color ^:";
  int iNumLabelLines = 1;
  int iMin = 1;
  int iMax = 255;
  int iStart = 1;
  //Each user input action (such as a push of button) will step this amount
  int iStep = 5;
  
  g_menuManager.DoIntInput( iMin, iMax, iStart, iStep, &pLabel, iNumLabelLines, &strandOneColor);
}


