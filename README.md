
# Ledstrip Art project 
This project aims to make an LED art installation of the Vasily Kadinsky "einige kreise" picture


Good reads on colors , perception and color theory : 

https://blog.asmartbear.com/color-wheels.html 

http://rileyjshaw.com/blog/hue-angle-transitions/


A color transition in JSfiddle: https://jsfiddle.net/user/vogelj/   


## Libraries / Dependencies 
- FastLED 
- Infrared library ( currently: IRemote, as IRlib2 has conflicts with interrupts) 

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
