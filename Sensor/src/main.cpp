#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Arduino.h> 
#include <Wire.h>

// SHT25 I2C address is 0x40(64)
#define Addr 0x40

SoftwareSerial virtualMonitor(2, 3); // RX | TX

#define HUMIDITY 0
#define TEMPERATURE 1
float info[2];

float lastHumidity = 0;

void setup() {
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);
  virtualMonitor.begin(9600);
}


float calHumidity(unsigned int I2CInput1, unsigned int I2CInput2){
  return (((I2CInput1 << 8 | I2CInput2) * 125.0) / 65536.0) - 6;
}

float calTemperature(unsigned int I2CInput1, unsigned int I2CInput2){
  return (((I2CInput1 << 8 | I2CInput2) * 175.72) / 65536.0) - 46.85;
}

float* extractDataForMain() {
  float result[2];
  unsigned int I2CData[2];
  
  // Each of the following 3 lines will do
  // one of the following actions, respectively:
  // 1. Start I2C transmission
  // 2. Send humidity measurement command, NO HOLD master
  // 3. Stop I2C transmission
  
  Wire.beginTransmission(Addr);
  Wire.write(0xF5);
  Wire.endTransmission();
  delay(500);
  
  // Request 2 bytes of data
  Wire.requestFrom(Addr, 2);
  
  // Read 2 bytes of data in the following format:
  // [humidity msB, humidity lsB]
  if(Wire.available() == 2) {
    I2CData[0] = Wire.read();
    I2CData[1] = Wire.read();
    // Convert the data
    result[HUMIDITY] = calHumidity(I2CData[0], I2CData[1]);
    // Output data to Serial virtualMonitor
    virtualMonitor.print("Humidity is:");
    virtualMonitor.print(result[HUMIDITY] );
    virtualMonitor.println(" %RH");
  }

  // Each of the following 3 lines will do
  // one of the following actions, respectively:
  // 1. Start I2C transmission
  // 2. Send temperature measurement command, NO HOLD master
  // 3. Stop I2C transmission
  
  Wire.beginTransmission(Addr);
  Wire.write(0xF3);
  Wire.endTransmission();
  delay(500);
  
  // Request 2 bytes of data  
  Wire.requestFrom(Addr, 2);
  
  // Read 2 bytes of data in the following format:
  // [temp msb, temp lsb]
  if(Wire.available() == 2) {
    I2CData[0] = Wire.read();    
    I2CData[1] = Wire.read();
    
    // Convert the data
    result[TEMPERATURE] = calTemperature(I2CData[0], I2CData[1]);
    
    // Output data to Serial virtualMonitor
    virtualMonitor.print("Temperature is:");
    virtualMonitor.print(result[TEMPERATURE]);
    virtualMonitor.println(" C");
  }
  return result;
}

void sendToMain(float* sensorData) {
  // input format from sensor to main:
  // "humidity-temperature$"
  Serial.print(info[HUMIDITY]);
  Serial.print("-");
  Serial.print(info[TEMPERATURE]);
  Serial.print("$");
}

bool isChanged(float newHumidity){
  return abs(lastHumidity-newHumidity) < 0.05*lastHumidity;
}

void loop() {
  float* sensorData = extractDataForMain();
  if(isChanged(sensorData[HUMIDITY])){
    sendToMain(sensorData);
    lastHumidity = sensorData[HUMIDITY];
  }
  delay(1000);
}
