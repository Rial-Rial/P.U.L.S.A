#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
const int pulser1 = 13;
const int pulser2 = 12;
const int pulser3 = 11;
const int pulser4 = 10;
const int pulser5 = 9;
const int motorstep = 7;
const int motordir = 8;
const int tempsignal = A0;
const int tempcontrol = 5;

float temperatureValue = 0.0;
float steinhart;
int constantforcontrol = 100;
float sensorValue;
float tempC;

int pulsespeed = 1000;

void setup() {

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("WELLCOME TO");
  lcd.setCursor(3, 1);
  lcd.print("P.U.L.S.A");
  delay(1000);
  lcd.clear();


  pinMode(pulser1, INPUT);
  pinMode(pulser2, INPUT);
  pinMode(pulser3, INPUT);
  pinMode(pulser4, INPUT);
  pinMode(pulser5, INPUT);
  pinMode(motorstep, OUTPUT);
  pinMode(motordir, OUTPUT);
  pinMode(tempsignal, INPUT);
  pinMode(tempcontrol, OUTPUT);

  digitalWrite(motordir, LOW);

  Serial.begin(9600);

  Wire.begin();
  Serial.println("Scanning for I2C devices...");
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.println(i, HEX);
    }
  }
}

void bottons(){
  Serial.println("Checking buttons...");
  if (digitalRead(pulser1) == HIGH) {
    constantforcontrol = constantforcontrol + 1 ;
    delayMicroseconds(100);
    Serial.println("Pulser1 is pressed. Incrementing constantforcontrol.");
  }
  if (digitalRead(pulser2) == HIGH){
    constantforcontrol = constantforcontrol - 1 ;
    delayMicroseconds(100);
    Serial.println("Pulser2 is pressed. Decrementing constantforcontrol.");
  }
  if (digitalRead(pulser3) == HIGH){
    pulsespeed = pulsespeed + 10  ;
    delayMicroseconds(100);
    Serial.println("Pulser3 is pressed. Increasing pulsespeed.");
  }
  if (digitalRead(pulser4) == HIGH){
   pulsespeed = pulsespeed - 10 ;
    delayMicroseconds(100);
    Serial.println("Pulser4 is pressed. Decreasing pulsespeed.");
  }
}

void motor (){
  digitalWrite(motorstep, HIGH );
  delayMicroseconds(pulsespeed);
  digitalWrite(motorstep, LOW);
  delayMicroseconds(pulsespeed);

}

void readtemperature(){
  int samples = 5;
  const float SERIESRESISTOR = 100000.0; 
  const float NOMINAL_RESISTANCE = 100000.0; 
  const float NOMINAL_TEMPERATURE = 25.0;  
  const float BCOEFFICIENT = 3950.0;     

  for (int i = 0; i < samples; i++) {
    sensorValue += analogRead(tempsignal);
    delayMicroseconds(100);
  }
  sensorValue /= samples; 
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  sensorValue = 1023.0 / sensorValue - 1.0;
  float resistance = SERIESRESISTOR / sensorValue;

  steinhart = resistance / NOMINAL_RESISTANCE;     // (R/Ro)
  steinhart = log(steinhart);                      // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                       // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                     // Invert to get Kelvin
  
  tempC = steinhart - 273.15;                // Convert Kelvin to Celsius

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println("°C");
  Serial.print("Steinhart-Hart Temperature: ");
  Serial.print(steinhart);
  Serial.println(" K");
  Serial.println();
  delayMicroseconds (100);

}

void setTemperature(){
  analogWrite(tempcontrol, constantforcontrol);
  Serial.print("constatntforcontrol: ");
  Serial.println(constantforcontrol);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(12, 0);
  lcd.print(tempC);
  lcd.setCursor(0, 1);
  lcd.print("Speed:");
  lcd.setCursor(9, 1);
  lcd.print(pulsespeed);

bottons();
Serial.println (pulsespeed);
motor();
readtemperature();
setTemperature();
}