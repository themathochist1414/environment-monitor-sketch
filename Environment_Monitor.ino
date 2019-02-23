#include <LiquidCrystal.h>

// Global Variables
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int thermistorPin = A0;               // Pin number for input from thermistor

int serialPrintCounter = 0;             // Serial Print Counter
const int serialPrintInterval = 3;      // serialPrintInterval * delayTime = how often serial prints
const int delayTime = 1000;             // in millisecond

void setup() {  
  // Open Serial port. Set Baud rate to 9600
  Serial.begin(9600);
  // Send out startup phrase
  Serial.println("Arduino Starting Up...");

  // intialize LCD display size
  lcd.begin(16,2);
  
  // print startup message to LCD display
  lcd.print("Arduino");
  lcd.setCursor(0,1);
  lcd.print("Starting Up...");
  delay(3000);
}

void loop() {
  int sensorVal = analogRead(thermistorPin); // Value read from thermistor
  /* convert the ADC reading to voltage
  * The sensor has a bit depth of 2^10 = 1024.
  * The max voltage from the arduino is 5 [V}.
  * This formula came from the Arduino Projects book.
  */
  float voltage = ( (float)sensorVal / 1024.0 ) * 5.0;   // [V]
  
  /* convert the voltage to temperature in degrees. 
   * This formula came from the Arduino Projects book
   */
  float temperature = (voltage - 0.5)*100;    // [degrees C]
  int toSerialPort = serialPrintCounter % serialPrintInterval;
  //Serial.println((String)toSerialPort);
  if (toSerialPort == 0){
    // Send message every serialPrintInterval minutes if delay(60000) at end of loop()
    String message = "@Sensor Value:  " + String(sensorVal) + 
                     ", Volts:  " + String(voltage) + 
                     ", degrees C: " + String(temperature);
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
  lcd.createChar(0, degreeSymbol);
  // Update LCD display
  lcd.clear();
  lcd.print("Temp: " + String((int)temperature));
  lcd.write(byte(0));
  lcd.print("C");
  
  // Wait to updaten LED display
  delay(delayTime);
  serialPrintCounter++;
}
