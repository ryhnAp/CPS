#include <SoftwareSerial.h>

struct SensorData {
  int humidity;
  int cTemp;
};

SoftwareSerial virtualMonitor(2, 3); // RX | TX

String msg = "";
int pwmOutputPin = 6;

void setup() {
  pinMode(pwmOutputPin, OUTPUT); 
  Serial.begin(9600);
  virtualMonitor.begin(9600);
}

char pollSerial() {
  if (Serial.available()) {
    char c = Serial.read(); // gets one byte from serial buffer
    return c;
  }

  return ((char) 0);
}

struct SensorData msgToSensorData(const String& msg) {
  int value = 0;
  
  int cIndex = 0;
  
  for (int i = 1; i < msg.length(); i++)
    if (msg[i] == 'C')
      cIndex = i;

  struct SensorData sensorData;
  sensorData.humidity = msg.substring(1, cIndex).toInt();
  sensorData.cTemp = msg.substring(cIndex + 1, msg.length() - 1).toInt();
  
  return sensorData;
}

int convertPwmDutyCycle(int duty_cycle) {
  int maxPwmValue = 255;
  return (duty_cycle * maxPwmValue) / 100;
}

void generatePwmSignal(struct SensorData &sensorData) {
  if (sensorData.humidity < 10) {
    analogWrite(pwmOutputPin, convertPwmDutyCycle(25));
  } 
  else if(sensorData.humidity >= 10 && sensorData.humidity < 20){
    analogWrite(pwmOutputPin, convertPwmDutyCycle(20));//15 cc/min ??? 
  }
  else if (sensorData.humidity >= 20 && sensorData.humidity < 30) {
     if (sensorData.cTemp > 25) {
        analogWrite(pwmOutputPin, convertPwmDutyCycle(10));
     } 
     else {
        analogWrite(pwmOutputPin, 0);
     }
  } 
  else {
    analogWrite(pwmOutputPin, 0);
  }
}

void loop() {
    char c = pollSerial();
    
    if (c)
      msg += c;
    
    if (c == '!') {
      virtualMonitor.print("MASTER: Received: ");
      virtualMonitor.println(msg);
      struct SensorData sensorData = msgToSensorData(msg);
      generatePwmSignal(sensorData);
      msg = "";
    }
}
