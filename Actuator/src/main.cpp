#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>

String entry = "";
SoftwareSerial virtualMonitor(2, 3); // RX | TX
// SoftwareSerial BTSerial(4, 5);// RX | TX
#define M1 7
#define M2 6

#define HUMIDITY 0
#define TEMPERATURE 1
#define DUTY_CYCLE 2
// input format == humidity-temperature$

void setup() {
  Serial.begin(4800);
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

  virtualMonitor.print(data.substring(0, splitIndex[0]).toFloat());
  virtualMonitor.print(data.substring(splitIndex[0] + 1, splitIndex[1]).toFloat());
  int dutyCycle = data.substring(splitIndex[1] + 1, data.length()).toInt();
  virtualMonitor.print(dutyCycle);
  return dutyCycle;
}
int dutyCycle = 100;
void loop() {
  // put your main code here, to run repeatedly:
  char c = '\n';
  if (Serial.available()){
    c = Serial.read();
    entry += c;
    virtualMonitor.print("hi");
  }
  virtualMonitor.print("hello");
  virtualMonitor.print(c);
  if (c == '$'){
    virtualMonitor.print(c);
    virtualMonitor.print("MAIN: Received from MAIN: ");
    virtualMonitor.println(entry);
    dutyCycle = mainInputProccess(entry);

    entry = "";
  }

  digitalWrite(M1,HIGH);
  digitalWrite(M2,LOW);
  delay(10*dutyCycle);
  digitalWrite(M1,LOW);
  digitalWrite(M2,HIGH);
  delay(1000 - 10*dutyCycle);
}