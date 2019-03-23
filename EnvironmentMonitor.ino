#include <LiquidCrystal.h>

// Global Variables
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int THERMISTOR_PIN = A0;            // Pin number for input from thermistor
const int LDR_PIN = A1;                   // Pin number for light dependent resistor

const int LCD_POWER_PIN = 8;

unsigned long START_MILLIS_SERIAL;
unsigned long CURRENT_MILLIS;

unsigned long START_MILLIS_LCD;
unsigned long LCD_TIMER = 0;
const unsigned long LCD_TIMEOUT = 5000; // in milliseconds

const unsigned long LCD_PERIOD = 1000;              // in millisecond
const unsigned long SERIAL_PERIOD = 15*LCD_PERIOD;       // how often serial prints

const int MAX_ADC_READING = 1023;
const int ADC_REF_VOLTAGE = 5;

const int BUTTON_PIN = 13;
int currentButtonState = 0;
int previousButtonState = 0;

const int SERIAL_DATA_ARRAY_SIZE = 3;

boolean firstRun = true;

void setup() {  
  START_MILLIS_SERIAL = millis();  // initial start time
  // Open Serial port. Set Baud rate to 9600
  Serial.begin(9600);
  // Send out startup phrase
  Serial.println("Arduino Starting Up...");

  // intialize LCD display size
  lcd.begin(16,2);

  // initialize pin 8 to power LCD
  pinMode(LCD_POWER_PIN, OUTPUT);
  digitalWrite(LCD_POWER_PIN, HIGH);

  // initialize pin 13 to listen for button press
  pinMode(BUTTON_PIN, INPUT);
  
  // print startup message to LCD display
  lcd.print("Arduino");
  lcd.setCursor(0,1);
  lcd.print("Starting Up...");
  delay(3000);
}

void loop() {
  int thermistorSensorVal = analogRead(THERMISTOR_PIN);
  int ldrSensorVal = analogRead(LDR_PIN);
  
  /* convert the ADC reading to thermistorVoltage
  * This formula came from the Arduino Projects book.
  */
  // figure out temperature
  float thermistorVoltage = ( (float)thermistorSensorVal / MAX_ADC_READING ) * ADC_REF_VOLTAGE;   // [V]
  float temperature = (thermistorVoltage - 0.5)*100;    // [degrees C]

  // figure out ldrResistance
  double resistorVoltage = ( (double)ldrSensorVal / MAX_ADC_READING ) * ADC_REF_VOLTAGE;   // [V]
  double ldrVoltage = 5.0 - resistorVoltage;
  double ldrResistance = (ldrVoltage/resistorVoltage)*10000; //10k is resistor value

  // package data to send to serial
  String data0 = String("degrees C: " + String(temperature));
  String data1 = String("ldr sensor val: " + String(ldrSensorVal));
  String data2 = String("ldr resistance val: " + String(ldrResistance));
  String serialData[SERIAL_DATA_ARRAY_SIZE] = {data0, data1, data2};
  
  CURRENT_MILLIS = millis();
  if ((CURRENT_MILLIS - START_MILLIS_SERIAL >= SERIAL_PERIOD)||(firstRun)){
    printDataToSerial(serialData);
    START_MILLIS_SERIAL = CURRENT_MILLIS;
  }

  if (firstRun){
    printInfoToLCD(temperature, ldrResistance);
    firstRun = false;
    delay(3000); // wait 3 seconds
    turnOffLCD();
  }
  currentButtonState = digitalRead(BUTTON_PIN);
  //Serial.println(String(currentButtonState));
  //Serial.println(String(currentButtonState) + " " + String(previousButtonState));
  if ((currentButtonState == HIGH)&&(previousButtonState == LOW)){
    previousButtonState = currentButtonState;
    digitalWrite(LCD_POWER_PIN, HIGH);
    printInfoToLCD(temperature, ldrResistance);
    START_MILLIS_LCD = millis();  // initial start time
  } else if ((currentButtonState == LOW)&&(previousButtonState == HIGH)){
    //Serial.println(String(CURRENT_MILLIS - START_MILLIS_LCD));
    if (CURRENT_MILLIS - START_MILLIS_LCD >= LCD_TIMEOUT){
      turnOffLCD();
      START_MILLIS_LCD = millis();
      previousButtonState = currentButtonState;
    }
  }
}

void printDataToSerial(String serialData[]){
  String message = "@";
  for (int i = 0; i < SERIAL_DATA_ARRAY_SIZE; i++){
    if (i != SERIAL_DATA_ARRAY_SIZE -1) message += serialData[i] + " , ";
    else message += serialData[i];
  }
  Serial.println(message);
}

void printInfoToLCD(int temperature, long ldrResistance){
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

  double ldrDouble = ldrResistance;
  
  String prefix = " ";
  if (ldrResistance > 1000000){
    ldrDouble = ((double)ldrResistance) / 1000000.0;
    prefix = "M";
  } else if (ldrResistance > 1000){
    ldrDouble = ((double)ldrResistance) / 1000.0;
    prefix = "k";
  }

  if (prefix != " "){
    lcd.print("LDR: "  + String(ldrDouble) + prefix);
    lcd.write(byte(1));
  } else {
    lcd.print("LDR: "  + String(ldrDouble));
    lcd.write(byte(1));
  }
}

void turnOffLCD(){
  digitalWrite(LCD_POWER_PIN, LOW); 
  lcd.clear();
}
