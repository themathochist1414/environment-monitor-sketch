#include <LiquidCrystal.h>

// Global Variables
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int thermistorPin = A0;            // Pin number for input from thermistor
const int ldrPin = A1;                   // Pin number for light dependent resistor

int serialPrintCounter = 0;              // Serial Print Counter
const int serialPrintInterval = 15;       // serialPrintInterval * delayTime = how often serial prints
const long delayTime = 60000;              // in millisecond

const int MAX_ADC_READING = 1023;
const int ADC_REF_VOLTAGE = 5;

void setup() {  
  // Open Serial port. Set Baud rate to 9600
  Serial.begin(9600);
  // Send out startup phrase
  Serial.println("Arduino Starting Up...");

  // intialize LCD display size
  lcd.begin(16,2);

  // initialize pin 8 to power LCD
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  // print startup message to LCD display
  lcd.print("Arduino");
  lcd.setCursor(0,1);
  lcd.print("Starting Up...");

  delay(3000);
}

void loop() {
  int thermistorSensorVal = analogRead(thermistorPin);
  int ldrSensorVal = analogRead(ldrPin);
  
  /* convert the ADC reading to thermistorVoltage
  * The sensor has a bit depth of 2^10 = 1024.
  * The max voltage from the arduino is 5 [V}.
  * This formula came from the Arduino Projects book.
  */
  
  float thermistorVoltage = ( (float)thermistorSensorVal / MAX_ADC_READING ) * ADC_REF_VOLTAGE;   // [V]
  float resistorVoltage = ( (float)ldrSensorVal / MAX_ADC_READING ) * ADC_REF_VOLTAGE;   // [V]
  float ldrVoltage = 5.0 - resistorVoltage;
  float ldrResistance = (ldrVoltage/resistorVoltage)*10000; //10k is resistor value
  
  float temperature = (thermistorVoltage - 0.5)*100;    // [degrees C]
  int toSerialPort = serialPrintCounter % serialPrintInterval;
  //Serial.println((String)toSerialPort);
  if (toSerialPort == 0){
    // Send message every serialPrintInterval minutes if delay(60000) at end of loop()
    String message = "@degrees C: " + String(temperature) +
                     ", ldr sensor val: " + String(ldrSensorVal) + 
                     ", ldr resistance in Ohms: " + String(ldrResistance);
    Serial.println(message);
  }

  byte degreeSymbol[8] = {
    B00111,
    B00101,
    B00111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000, 
  };

  byte omega[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B01010,
    B01010,
    B11011, 
  };
  
  lcd.createChar(0, degreeSymbol);
  lcd.createChar(1, omega);
  // Update LCD display
  lcd.clear();
  lcd.print("Temp: " + String((int)temperature));
  lcd.write(byte(0));
  lcd.print("C");
  lcd.setCursor(0,1);

  String prefix = " ";
  if (ldrResistance > 1000000000){
    ldrResistance = ldrResistance / 1000000000;
    prefix = "G";
  } else if (ldrResistance > 1000000){
    ldrResistance = ldrResistance / 1000000;
    prefix = "M";
  } else if (ldrResistance > 1000){
    ldrResistance = ldrResistance / 1000;
    prefix = "k";
  }

  if (prefix != " "){
    lcd.print("LDR: "  + String(ldrResistance) + prefix);
    lcd.write(byte(1));
  } else {
    lcd.print("LDR: "  + String(ldrResistance));
    lcd.write(byte(1));
  }
  
  // Wait to update LCD display
  delay(delayTime);
  serialPrintCounter++;
}
