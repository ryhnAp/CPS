#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>


String entry = "";
SoftwareSerial virtualMonitor(2, 3); // RX | TX
#define M1 7
#define M2 6

#define HUMIDITY 0
#define TEMPERATURE 1
#define DUTY_CYCLE 2
// input format == humidity-temperature$

void setup() {
  Serial.begin(9600);
  virtualMonitor.begin(9600);
  pinMode(M1,OUTPUT) ; 
  pinMode(M2,OUTPUT) ;
}

int mainInputProccess(const String &data)
{
  int splitIndex[2] = {0, 0};
  int j = 0;
  for (unsigned int i = 0; i < data.length(); i++)
    if (data[i] == '-'){
      splitIndex[j] = i;
      j++;
    }
  virtualMonitor.print("Humidity: ");
  virtualMonitor.print(data.substring(0, splitIndex[0]).toFloat());
  virtualMonitor.print("Temperature");
  virtualMonitor.print(data.substring(splitIndex[0] + 1, splitIndex[1]).toFloat());
  int dutyCycle = data.substring(splitIndex[1] + 1, data.length()).toInt();
  virtualMonitor.print(dutyCycle);
  return dutyCycle;
}
int dutyCycle = 100;
void loop() {
  char c = '\n';
  if (Serial.available()){
    c = Serial.read();
    entry += c;
    virtualMonitor.println(c);
  }

  if (c == '$'){
    virtualMonitor.print("ACTUATOR: Received from MAIN: ");
    virtualMonitor.println(entry);
    dutyCycle = mainInputProccess(entry);
    entry = "";
  }

  // digitalWrite(M1,HIGH);
  // digitalWrite(M2,LOW);
  // delay(10*dutyCycle);
  // digitalWrite(M1,LOW);
  // digitalWrite(M2,HIGH);
  // delay(1000 - 10*dutyCycle);
}