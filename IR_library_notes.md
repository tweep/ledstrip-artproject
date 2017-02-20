# Notes on IR Libraries 

There are serveral libraries out there which support IR receviers ( and the sending of IR signals). The first library was written by Ken Shirriff. 


## IRremote.h and IRremoteInt.h 
Written by Ken Shiriff. Latest version: 2.2.1 
Available here: https://github.com/shirriff/Arduino-IRremote
Works ok, but seems to not handle interrupts - the IR singal gets only read 
every now-and-than.  
Conflicts: No conflicts with FastLED libary.

## IRlib 
Writteb by Cyborg5. 
Available here : https://github.com/cyborg5/IRLib/
Conflicts: 

## IRlib2  
Written by Cyborg.
Available here: https://github.com/cyborg5/IRLib2
Conflicts: This library has problems when used with the FastLED library - when calling the delay function ( which sets the frame rate ) - the IR remote codes are not read correctly anymore: 

     FastLED.delay(40) 


## Infrared4Arduino
Written by bengtmartensson
Available here : https://github.com/bengtmartensson/Infrared4Arduino/
http://www.harctoolbox.org/Infrared4Arduino.html
Conflicts: ? 
Notes: Unsure if this lib works with interrupts or not.  
