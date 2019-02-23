// Global Variables
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int sensorPin = A0;               // Pin number for input from thermistor

int serialPrintCounter = 0;             // Serial Print Counter
const int serialPrintInterval = 3;      // serialPrintInterval * delayTime = how often serial prints
const int delayTime = 1000;             // in millisecond

void setup() {
  
  // Open Serial port. Set Baud rate to 9600
  Serial.begin(9600);
  // Send out startup phrase
  Serial.println("Arduino Starting Up...");
  
  turnOffPins();
  for (int n = greatestPinNum; n>=yellow; n--)
    blinkLED(n);
}

void loop() {
  int sensorVal = analogRead(sensorPin); // Value read from thermistor
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
  
  // Update LED display
  displayInBinary(temperature);

  /* // For debugging binary LED readout
  countFromStartNumToEndNum(-63, 63);
  */
  
  // Wait to updaten LED display
  delay(delayTime);
  serialPrintCounter++;
}

/* displayInBinary description
 * Display a number in binary using LEDs
 *     INPUT - float inNum: number to be displayed on LEDs
 *    OUTPUT - displays number to LEDs. Does not return anything to parent function
 */
void displayInBinary(float inNum){
  int inNumRnd = (int)inNum;            // Round down input number. LED display not setup to handle decimals
  int inNumRndCpy = inNumRnd;
  
  /* // For Debugging
  Serial.println(inNumRndCpy);
  //Serial.println((String)inNum + " " + (String)inNumRnd);
  String binaryRep = "";
  String inNumRndCpyStr = (String)inNumRndCpy;
  */
  if (inNumRnd < 0) {
      Serial.println("Negative");
      digitalWrite(yellow, HIGH);
      inNumRnd = (-1)*inNumRnd;
      /* // For Degbugging
      Serial.println(inNumRnd);
      binaryRep = binaryRep + "1";
      */
    } else {
      digitalWrite(yellow, LOW);
      /* // For Debugging
      Serial.println("Positive");
      binaryRep = binaryRep + "0";
      */
    }
  /* // For debugging 
  String inNumRndStr = (String)inNumRnd;  
  Serial.println((String)inNumRndCpy + " " + inNumRndStr);
  */
  // Find binary representation of inNum.
  for (int n = greatestPinNum - leastPinNum; n >=0; n--){
    int pinNum = greatestPinNum - n;
    /*  Proof that PinNum = greatestPinNum - n
     *      n = greatestPinNum - leastPinNum
     *   => PinNum = greatestPinNum - n
     *                 = greatestPinNum - (greatestPinNum - leastPinNum)
     *                 = leastPinNum
     *      n = 0
     *   => PinNum = greatestPinNum - n
     *                 = greatestPinNum - 0
     *                 = greatestPinNum
     *  For every k in  [ 1 , (greatestPinNum - leastPinNum) - 1 ]
     *      n = greatestPinNum - leastPinNum - k // The minus k comes from n--
     *   => PinNum = greatestPinNum - n
     *                 = greatestPinNum - (greatestPinNum - leastPinNum - k)
     *                 = leastPinNum + k
     */
    int twoToTheN = exponentiateIntegers(2, n); // 2^n
    int remainder = inNumRnd - twoToTheN;
    
    /* // For Debugging
    Serial.println(inNumRndStr + " " + twoToTheN);
    
    String nStr = (String)(n);
    String PinNumStr = (String)pinNum;
    String twoToTheNStr = (String)twoToTheN;
    String remainderStr = (String)remainder;
    Serial.println(twoToTheNStr);
    Serial.println(remainder);
    */
    if ( remainder >= 0 ){
      digitalWrite(pinNum, HIGH);
      inNumRnd = inNumRnd - twoToTheN;
      /*// For Debugging
      inNumRndStr = inNumRnd;
      Serial.println(inNumRndStr + " - " + twoToTheNStr + " = " + remainderStr);
      binaryRep = binaryRep + "1";
      */
    } else{
      digitalWrite(pinNum, LOW);
      /* // For Debugging
      Serial.println(inNumRndStr + " - " + twoToTheNStr + " = " + remainderStr);
      binaryRep = binaryRep + "0";
      */
    }
    /* // For Debugging
    //Serial.println(nStr + " " + pinNumStr);
    */
  }
  /* // For Debugging
  //Serial.println(inNumRndCpyStr + " " + binaryRep);
  */
}

/* blinkLED description
 * This method turns on the given LED, waits for 0.5 seconds, turns off LED,
 *  then waits for another 0.5 seconds
 *  INPUT - int pinNum: pin number of LED one wishes to blink
 * OUTPUT - blink LED but does not return anything to parent function
 */
void blinkLED(int pinNum){
  digitalWrite(pinNum, HIGH);
  delay(250);
  digitalWrite(pinNum, LOW);
  delay(250);
}

/* turnOffPins description
 * Set all pins to output and set them to low
 *  INPUT - none
 * OUTPUT - set all PinNums from leastPinNum to greatestPinNum 
 *          to output and turn them off
 */
void turnOffPins(){
  for (int pinNum = yellow; pinNum <=  greatestPinNum; pinNum++){
      pinMode(pinNum, OUTPUT);
      digitalWrite(pinNum, LOW);
    }
}

/* exponentiateIntegers description
 * x^y where x, y are Integers
 *  INPUT - x: base
 *           y: exponent
 * OUTPUT - result: x^y
 */
int exponentiateIntegers(int x, int y){
  int result = 1;
  for (int n = 1; n <= y; n++){
    result = result*x;
  }
  return result;
}

/* countFromStartNumToEndNum description
 * Count from start number to end number. Both endpoints are
 *  included in the count
 *    INPUT - int startNum: first number in count
 *            int endNum:   last number in count
 *   OUTPUT - display number in binary on LEDs
 */
void countFromStartNumToEndNum(int startPnt, int endPnt){
  for ( int n = startPnt; n <= endPnt; n++){
    displayInBinary(n);
    delay(250);
  }
}
