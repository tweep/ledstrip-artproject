
# Ledstrip Art project 
This project aims to make an LED art installation of the Vasily Kadinsky "einige kreise" picture - see pictures below. 
The installation will be driven by 2 Arduino Mega 2560's ( one for the LEDs, one for a remote control ); each colored 
circle can change color independently.  

Here's the final installation - you fine some pictures if you scroll down, or look [here](./pics/)

![Final installation](https://github.com/tweep/ledstrip-artproject/blob/master/pics/rainbow_mode.jpg) 


## Notes 
Here are some good reads on colors and color theory: 

[The color wheel](https://blog.asmartbear.com/color-wheels.html) 
[Hue angle transitions](http://rileyjshaw.com/blog/hue-angle-transitions/)

I created a nice color transition in [JSfiddle](https://jsfiddle.net/user/vogelj/).


## Libraries /  Software / Dependencies
I'm using the FastLED library to drive my WS2812B 5V LED strips which have individually addressable LEDs. I bought them online. 
 
- [Arduino FastLED library](https://github.com/FastLED/FastLED)
- Infrared library ( currently used: : IRemote. I like to switch to the IRlib2 later. It has conflicts with the interrupts used by FastLED, 
  so I decided to use 2 Arduinos: one to deal with the remote and the LCD display, and one which drives the LEDs.
- [Easytransfer_I2C](has://github.com/madsci1016/Arduino-EasyTransfer) - to communicate between 2 Arduinos via Serial interface 


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


![Kadinsky picture](./pics/kadinsky.jpg) 

![Final installation](https://github.com/tweep/ledstrip-artproject/blob/master/pics/rainbow_mode.jpg) 

![Daylight](https://github.com/tweep/ledstrip-artproject/blob/master/pics/plywood_cut_4_layers.jpg) 

More pictures can be found [here](./pics/.jpg) 

