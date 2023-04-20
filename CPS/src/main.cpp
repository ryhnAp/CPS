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
SoftwareSerial virtualMonitor(7, 6); // RX | TX
int outputPin = 6;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  pinMode(outputPin, OUTPUT);
  Serial.begin(9600);
  virtualMonitor.begin(9600); // debug
  BTSerial.begin(9600);
  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("MAIN");
}

float *sensorInputProccess(const String &data)
{
  int splitIndex = 0;
  for (unsigned int i = 0; i < data.length(); i++)
    if (data[i] == '-')
      splitIndex = i;

  info[HUMIDITY] = data.substring(0, splitIndex).toFloat();
  lcd.setCursor(0, 1);
  lcd.println(info[HUMIDITY]);
  info[TEMPERATURE] = data.substring(splitIndex + 1, data.length()).toFloat();
  lcd.println(info[TEMPERATURE]);
  return info;
}

void sendDataToActuator(int dutyCycle){
  lcd.println(dutyCycle);
  BTSerial.print(info[HUMIDITY]);
  BTSerial.print("-");
  delay(100);
  BTSerial.print(info[TEMPERATURE]);
  BTSerial.print("-");
  delay(100);
  BTSerial.print(dutyCycle);
  BTSerial.print("$");
  delay(100);
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
  }
  if (c == '$'){
    virtualMonitor.println("MAIN: Received from sensor: ");
    float *sensorData = sensorInputProccess(entry);
    sendDataToActuator(sensorData);
    entry = "";
  }
}
