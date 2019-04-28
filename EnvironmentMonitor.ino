#include <LiquidCrystal.h>

#define THERMISTOR_PIN          A0              // Pin number for input from thermistor
#define LDR_PIN                 A1              // Pin number for light dependent resistor
#define LCD_POWER_PIN           8               // Pin number to toggle power to LCD screen
#define LCD_TIMEOUT             5000            // milliseconds until screen turns off
#define LCD_PERIOD              1000            // in milliseconds
#define SERIAL_PERIOD           1*LCD_PERIOD    // how often serial prints
#define MAX_ADC_READING         1023            // max num` on analog to digital converter
#define ADC_REF_VOLTAGE         5               // max voltage that could appear on ADC
#define BUTTON_PIN              7               // pin number for button input
#define SERIAL_DATA_ARRAY_SIZE  3               // how many pieces of data sent to serial

// Global Variables
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long startMillisSerial;
unsigned long currentMillis;

unsigned long startMillisLCD;

int currentButtonState = 0;
int previousButtonState = 0;

boolean firstRun = true;

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

void setup() {
  startMillisSerial = millis();  // initial start time
  // Open Serial port. Set Baud rate to 9600
  Serial.begin(9600);
  // Send out startup phrase
  Serial.println("Arduino Starting Up...");

  // intialize LCD display size
  lcd.begin(16,2);

  // initialize pin 8 to power LCD
  pinMode(LCD_POWER_PIN, OUTPUT);
  digitalWrite(LCD_POWER_PIN, HIGH);

  // initialize pin to listen for button press
  pinMode(BUTTON_PIN, INPUT);

  // print startup message to LCD display
  lcd.print("Arduino");
  lcd.setCursor(0,1);
  lcd.print("Starting Up...");
  delay(3000);
  lcd.clear();
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

  currentMillis = millis();
  if ((currentMillis - startMillisSerial >= SERIAL_PERIOD)||(firstRun)){
    printDataToSerial(serialData);
    startMillisSerial = currentMillis;
  }

  if (firstRun){
    printDataToLCD(temperature, ldrResistance);
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
    printDataToLCD(temperature, ldrResistance);
    startMillisLCD = millis();  // initial start time
  } else if ((currentButtonState == LOW)&&(previousButtonState == HIGH)){
    //Serial.println(String(currentMillis - startMillisLCD));
    if (currentMillis - startMillisLCD >= LCD_TIMEOUT){
      turnOffLCD();
      startMillisLCD = millis();
      previousButtonState = currentButtonState;
    }
  }
}

void printDataToSerial(String serialData[]){
  String message = "@";
  for (int i = 0; i < SERIAL_DATA_ARRAY_SIZE; i++){
    if (i != SERIAL_DATA_ARRAY_SIZE -1) message += serialData[i] + ", ";
    else message += serialData[i];
  }
  Serial.println(message);
}

void printDataToLCD(int temperature, long ldrResistance){

  lcd.createChar(0, degreeSymbol);
  lcd.createChar(1, omega);

  // Update LCD display
  lcd.clear();
  lcd.print(String(String("Temp: ") + String((int)temperature)));
  lcd.write(byte(0));
  lcd.print(String("C"));
  lcd.setCursor(0,1);

  double ldrDouble = ldrResistance;

  String prefix = " ";
  if (ldrResistance > 1000000){
    ldrDouble = (ldrResistance) / 1000000.0;
    prefix = String("M");
  } else if (ldrResistance > 1000){
    ldrDouble = (ldrResistance) / 1000.0;
    prefix = String("k");
  } else if (ldrResistance < 0){
    prefix = "OL";
  }
  //Serial.println(prefix);
  if (prefix.equals("OL")){
    lcd.print(String("LDR: INF"));
    lcd.write(byte(1));
  } else if (!prefix.equals(" ")){
    lcd.print(String(String("LDR: ")  + String(ldrDouble, 2) + prefix));
    lcd.write(byte(1));
  } else {
    lcd.print(String(String("LDR: ")  + String(ldrDouble, 2)));
    lcd.write(byte(1));
  }
}

void turnOffLCD(){
  lcd.clear();
  digitalWrite(LCD_POWER_PIN, LOW);
}
