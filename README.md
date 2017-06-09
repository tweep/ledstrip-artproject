
# Ledstrip Art project 
This project aims to make an LED art installation of the Vasily Kadinsky "einige kreise" picture


![Kadinsky picture](https://github.com/tweep/ledstrip-artproject/blob/master/pics/kadinsky.jpg) 



Good reads on colors , perception and color theory : 

https://blog.asmartbear.com/color-wheels.html 

http://rileyjshaw.com/blog/hue-angle-transitions/


A color transition in JSfiddle: https://jsfiddle.net/user/vogelj/   


## Libraries / Dependencies 
- FastLED 
- Infrared library ( currently: IRemote, as IRlib2 has conflicts with interrupts) 

## Project status and files 
After realizing that there are some timing problems when using the FastLed library together with an IR-remote (the FastLed library turns all interrrupts off), I decided to use 2 Arduino boards for the project: 

 1.  1 x Arduino MEGA which controls the FastLED patterns and strands   
 * uses FastLED library to drive the strands 
 * EasyTransfer library to receive data via I2C from the Arduino Uno menu / UI and translate them into 
   visible patterns
 * code for patterns, transitions, and FastLED configuration 

 2.  1 x Arduino UNO to drive the User interface: 
 * LCD display with keypad 
 * Menu structure on LCD 
 * IR receiver to remote control the same menu with an IR remote 
 * EasyTransfer library to send data via I2C to the Arduino Mega 


## Code Stages 

### UNO_LCD_I2C_TX_example 
File UNO_LCD_I2C_TX_example.ino contains a fully working example for the Arduino Uno which 
  - shows a menu for 2 LED strands + some global configuration 
  - interfaces with an IR remote to use the menu   
  - interfaces with a keypad on the menu to choose functions 
  - sends the data to the Arduno Mega via I2C communication 

This example uses the following libraries: 
 - Wire.h
 - EasyTransferI2C.h
 - LiquidCrystal.h
 - MenuEntry.h"
 - MenuLCD.h"
 - MenuManager.h"
 - IRremote.h
 - IRremoteInt.h



## Todo: 

- allow to control brightness of strands individuall ( could be combined with the on/off possibility )    
- allow to switch individual strands on/off 

- Create solid patterns: red, green, blue
  Create solid transitions of full strand : red -> green -> blue -> ...
- add new transitons ( turning / rgb color circles ) 
- add beserker mode ( blinking in high frequency with different colors ) 

- store favorite configs and add possibility to cycle trough them. [ 0, 2, 1, 0 ] ... strips don't need to be all the same theme

- adjust speed of some patterns ? 

## Done : 
- reset function [ go back to start config ] 
- add possibility to cycle all strands trough all patterns together 
- switch all strands on / off 
- control brightness of all strands with single button  
- change the strand themes individually per strand 
