#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#define HUMIDITY 0
#define TEMPERATURE 1
float info[2];// input format == humidity-temperature$

#define ANALOG_WRITE_INTERVAL 255
SoftwareSerial virtualMonitor(2, 3); // RX | TX

int outputPin = 6;

void setup()
{
  pinMode(outputPin, OUTPUT);
  Serial.begin(9600);
  virtualMonitor.begin(9600); // debug
}

char getInput()
{
  if (Serial.available())
  {
    char c = Serial.read(); // gets one byte from serial buffer
    return c;
  }
  return ((char)0);
}

float *sensorInputProccess(const String &data)
{
  int splitIndex = 0;
  for (unsigned int i = 0; i < data.length(); i++)
    if (data[i] == '-')
      splitIndex = i;

  info[HUMIDITY] = data.substring(0, splitIndex).toFloat();
  info[TEMPERATURE] = data.substring(splitIndex + 1, data.length()).toFloat();

  return info;
}

void sendDataToActuator(float* sensorData)
{
  if (info[HUMIDITY] < 10)
    analogWrite(outputPin, ANALOG_WRITE_INTERVAL*(25/100));
  else if (info[HUMIDITY] >= 10 && info[HUMIDITY] < 20)
    analogWrite(outputPin, ANALOG_WRITE_INTERVAL*(20/100));
  else if (info[HUMIDITY] >= 20 && info[HUMIDITY] < 30)
    if (info[TEMPERATURE] > 25)
      analogWrite(outputPin, ANALOG_WRITE_INTERVAL*(10/100));
    else
      analogWrite(outputPin, 0); 
  else
    analogWrite(outputPin, 0);
}

String entry = "";

void loop()
{
  char c = '\n';
  if (Serial.available()){
    c = Serial.read();
    entry += c;
  }
  
  if (c == '$'){
    virtualMonitor.print(c);
    virtualMonitor.print("MAIN: Received from sensor: ");
    virtualMonitor.println(entry);
    float *sensorData = sensorInputProccess(entry);
    sendDataToActuator(sensorData);
    virtualMonitor.print("MAIN: SEND to actuator");
    entry = "";
  }
}
