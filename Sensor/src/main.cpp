#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Arduino.h> 
#include <Wire.h>

#define Addr 0x40

SoftwareSerial virtualMonitor(2, 3); // RX | TX

#define HUMIDITY 0
#define TEMPERATURE 1

float lastHumidity = 0;

void setup() {
  Wire.begin();
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
  float* result = new float[2];
  unsigned int I2CData[2];
  
  Wire.beginTransmission(Addr);
  Wire.write(0xF5);
  Wire.endTransmission();
  delay(500);
  
  Wire.requestFrom(Addr, 2);

  if(Wire.available() == 2) {
    I2CData[0] = Wire.read();
    I2CData[1] = Wire.read();

    result[HUMIDITY] = calHumidity(I2CData[0], I2CData[1]);
    
    virtualMonitor.print("Humidity is:");
    virtualMonitor.print(result[HUMIDITY] );
    virtualMonitor.println(" %RH");
  }

  Wire.beginTransmission(Addr);
  Wire.write(0xF3);
  Wire.endTransmission();
  delay(500);
  
  Wire.requestFrom(Addr, 2);
  
  if(Wire.available() == 2) {
    I2CData[0] = Wire.read();    
    I2CData[1] = Wire.read();
    
    result[TEMPERATURE] = calTemperature(I2CData[0], I2CData[1]);
    
    virtualMonitor.print("Temperature is:");
    virtualMonitor.print(result[TEMPERATURE]);
    virtualMonitor.println(" C");
  }
  return result;
}

void sendToMain(float* sensorData) {
  // input format from sensor to main:
  // "humidity-temperature$"
  Serial.print(sensorData[HUMIDITY]);
  Serial.print("-");
  Serial.print(sensorData[TEMPERATURE]);
  Serial.print("$");
}

bool isChanged(float newHumidity){
  return abs(lastHumidity-newHumidity) > 0.05*lastHumidity;
}

void loop() {
  float* sensorData = extractDataForMain();

  if(isChanged(sensorData[HUMIDITY])){
    sendToMain(sensorData);
    lastHumidity = sensorData[HUMIDITY];
  }
  delay(500);
}
