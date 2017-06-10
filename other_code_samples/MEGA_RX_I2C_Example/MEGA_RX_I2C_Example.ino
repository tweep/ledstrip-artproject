
// ARDUINO LED MEGA - control LEDs

#include <Wire.h>
#include <EasyTransferI2C.h>

//create object
EasyTransferI2C ET; 

struct RECEIVE_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int ledStripToConfigure;
  int toggleStrand;
  int pattern;
  int spd;
  int bright;
  int color;
  int blinks;
  int pause;
};
RECEIVE_DATA_STRUCTURE mydata;

//define slave i2c address
#define I2C_SLAVE_ADDRESS 9

void setup(){
  Serial.begin(9600);

  Wire.begin(I2C_SLAVE_ADDRESS);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 
  ET.begin(details(mydata), &Wire);
  //define handler function on receiving data
  Wire.onReceive(receive);
  
  pinMode(13, OUTPUT);
  Serial.println("LED-Mega is Ready");
}

void loop() {
  //check and see if a data packet has come in. 

  
  if(ET.receiveData()){
    
    Serial.println("Retrieving data...");
    Serial.print("Strip: ");Serial.print(mydata.ledStripToConfigure); 
    Serial.print(" togle:"); Serial.print(mydata.toggleStrand);
    Serial.print(" theme: "); Serial.print(mydata.pattern);
    Serial.print(" speed: "); Serial.print(mydata.spd);
    Serial.print(" brigh: "); Serial.print(mydata.bright);
    Serial.print(" color: "); Serial.print(mydata.color);
    Serial.println(" ");
    
  }
}

void receive(int numBytes) {}
