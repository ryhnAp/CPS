#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <AltSoftSerial.h>
#include <Wire.h>


#define HUMIDITY 0
#define TEMPERATURE 1
float info[2];// input format == humidity-temperature$


#define RXPIN 8
#define TXPIN 9
AltSoftSerial BTSerial(RXPIN, TXPIN);


#define ANALOG_WRITE_INTERVAL 255
SoftwareSerial virtualMonitor(2, 3); // RX | TX
int outputPin = 6;

void setup()
{
  pinMode(outputPin, OUTPUT);
  Serial.begin(9600);
  virtualMonitor.begin(9600); // debug
  BTSerial.begin(9600);
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);
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

void sendDataToActuator(int dutyCycle){
  BTSerial.print(info[HUMIDITY]);
  BTSerial.print("-");
  BTSerial.print(info[TEMPERATURE]);
  BTSerial.print("-");
  BTSerial.print(dutyCycle);
  BTSerial.print("$");
  virtualMonitor.println("MAIN: sent to actuator!");
}

void sendDataToActuator(float* sensorData)
{
  if (info[HUMIDITY] < 10)
    sendDataToActuator(25);
  else if (info[HUMIDITY] >= 10 && info[HUMIDITY] < 20)
    sendDataToActuator(20);
  else if (info[HUMIDITY] >= 20 && info[HUMIDITY] < 30)
    if (info[TEMPERATURE] > 25)
      sendDataToActuator(10);
    else
      sendDataToActuator(0);
  else
      sendDataToActuator(0);
}

String entry = "";

void loop()
{
  char c = '\n';
  if (Serial.available()){
    c = Serial.read();
    entry += c;
    virtualMonitor.print(c);
  }
  if (c == '$'){
    virtualMonitor.print("MAIN: Received from sensor: ");
    virtualMonitor.println(entry);
    float *sensorData = sensorInputProccess(entry);
    sendDataToActuator(sensorData);
    entry = "";
  }
}
