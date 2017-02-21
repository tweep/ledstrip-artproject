
// ARDUINO MEGA - control LEDs

#include <Wire.h>
#include <EasyTransferI2C.h>

//create object
EasyTransferI2C ET; 

struct RECEIVE_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  uint8_t g_ledStripToConfigure;
  uint8_t g_toggleStrand;
  uint8_t g_theme;
  uint8_t g_speed;
  uint8_t g_bright;
  uint8_t g_color;
  uint8_t blinks;
  uint8_t pause;
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
  Serial.println("Mega is Ready");
}

void loop() {
  //check and see if a data packet has come in. 
  if(ET.receiveData()){
    
    Serial.println("Retrieving data...");
    Serial.print("Config updated - sending data.. ");
    Serial.print("Strip: ");Serial.print(mydata.g_ledStripToConfigure); 
    Serial.print(" togle:"); Serial.print(mydata.g_toggleStrand);
    Serial.print(" theme: "); Serial.print(mydata.g_theme);
    Serial.print(" speed: "); Serial.print(mydata.g_speed);
    Serial.print(" brigh: "); Serial.print(mydata.g_bright);
    Serial.print(" color: "); Serial.print(mydata.g_color);
    Serial.println(" ");
    
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out. 
    for(int i = mydata.g_ledStripToConfigure; i>0; i--){
      digitalWrite(13, HIGH);
      delay(mydata.pause * 100);
      digitalWrite(13, LOW);
      delay(mydata.pause * 100);
    }
  }
}

void receive(int numBytes) {}
