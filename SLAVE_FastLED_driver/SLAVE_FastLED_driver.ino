
// ARDUINO LED SLAVE - drives the LEDS and knows all about transitions

#include <Wire.h>
#include <EasyTransferI2C.h>

EasyTransferI2C ET; 

struct RECEIVE_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int ledStripToConfigure;
  int toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
};
RECEIVE_DATA_STRUCTURE mydata;

#define I2C_SLAVE_ADDRESS 9

void setup(){
  Serial.begin(9600);

  Wire.begin(I2C_SLAVE_ADDRESS);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 
  ET.begin(details(mydata), &Wire);
  //define handler function on receiving data
  Wire.onReceive(receive);
  Serial.println("Slave is to read data and to drive LEDs");
}

void loop() {
  if(ET.receiveData()){
    
    Serial.println("Retrieving data...");
    Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
    Serial.print(" togle:"); Serial.print(mydata.toggleStrand);
    Serial.print(" theme: "); Serial.print(mydata.pattern);
    Serial.print(" speed: "); Serial.print(mydata.spd);
    Serial.print(" brigh: "); Serial.print(mydata.bright);
    Serial.print(" color: "); Serial.print(mydata.color);
    Serial.println(" ");
    // routine which configures the specific strip: speed, pattern, brightness, on/off
    // routine which configures ALL strips: off/on, global brightness, ... 
  }
}

void receive(int numBytes) {
  Serial.println("DATA !");
  }
