#include <Wire.h> 
#include <LiquidCrystal_I2C.h>           // include I2C Library
#define DOT_DURATION   150               // Duration of a Morse Code "dot" (in milliseconds)
#define DASH_DURATION  DOT_DURATION * 2  // Duration of a Morse Code "dash" (in milliseconds)
#define SIGNAL_GAP     DOT_DURATION * 4     // Gap between dots/dashes of a single letter (in ms)
#define LETTER_GAP     DOT_DURATION * 7  // Gap between two letters (in milliseconds)
#define WORD_GAP       DOT_DURATION * 7  // Gap between two words (in milliseconds)

#define LED             6               // The digital connector port to LED anode
#define BUTTON          7                // The diginal connector port to the button
#define DOT             1                // DOT identifier
#define DASH            2                // DASH identifier
#define NONE            0                // Neither DOT nor DASH
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
boolean buttonWasPressed = false;        // Indicator of whether button was pressed in the last cycle
long lastTimestamp = 0;                  // Last recorded timestamp  (used for mesuring duration)
byte inputSignal[5];                     // Input signal buffer
int inputSignalIndex = 0;                // Index into the input signal buffer
int count=0;  // this count to lcd change
void resetInputSignal() {                // Reset the input signal buffer and index
  inputSignal[0] = NONE;
  inputSignal[1] = NONE; 
  inputSignal[2] = NONE;
  inputSignal[3] = NONE;
  inputSignal[4] = NONE;
  inputSignalIndex = 0;
}

void setup() {
  pinMode(LED, OUTPUT);                  // Set the LED output
  pinMode(BUTTON, INPUT);                // Set the button input
  digitalWrite(LED, LOW);                // Turn off the LED
  Serial.begin(9600);                    // Init the serial port
  resetInputSignal();                    // Reset input signal buffer
  
  lcd.begin(16, 2);      // Initialization LCD，
  // Initialization word
  lcd.setCursor(0, 0); // set in row 1
  lcd.print("Morse Game!");
  delay(1000);
  lcd.setCursor(0, 1); // set in row 2 
  lcd.print("Enjoy!");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Use Button");
  lcd.setCursor(0, 1);
  lcd.print("Type Morse now");
   delay(2000);
   lcd.clear();
  lcd.setCursor(0, 0);

}

// return true if s0-s4 match input signal
boolean matchInputSignal(byte s0, byte s1, byte s2, byte s3, byte s4) {
  return ((inputSignal[0] == s0) && 
          (inputSignal[1] == s1) && 
          (inputSignal[2] == s2) && 
          (inputSignal[3] == s3) &&  
          (inputSignal[4] == s4));
}

// convert input signal to letter or ? if not found
char currentInputSignalToLetter() {  
  if (matchInputSignal(DOT, DASH, NONE, NONE, NONE))  { return 'A'; }
  if (matchInputSignal(DASH, DOT, DOT, DOT, NONE))    { return 'B'; }
  if (matchInputSignal(DASH, DOT, DASH, DOT, NONE))   { return 'C'; }
  if (matchInputSignal(DASH, DOT, DOT, NONE, NONE))   { return 'D'; }
  if (matchInputSignal(DOT, NONE, NONE, NONE, NONE))  { return 'E'; }
  if (matchInputSignal(DOT, DOT, DASH, DOT, NONE))    { return 'F'; }
  if (matchInputSignal(DASH, DASH, DOT, NONE, NONE))  { return 'G'; }
  if (matchInputSignal(DOT, DOT, DOT, DOT, NONE))     { return 'H'; }
  if (matchInputSignal(DOT, DOT, NONE, NONE, NONE))   { return 'I'; }
  if (matchInputSignal(DOT, DASH, DASH, DASH, NONE))  { return 'I'; }
  if (matchInputSignal(DASH, DOT, DASH, NONE, NONE))  { return 'K'; }
  if (matchInputSignal(DOT, DASH, DOT, DOT, NONE))    { return 'L'; }
  if (matchInputSignal(DASH, DASH, NONE, NONE, NONE)) { return 'M'; }
  if (matchInputSignal(DASH, DOT, NONE, NONE, NONE))  { return 'N'; }
  if (matchInputSignal(DASH, DASH, DASH, NONE, NONE)) { return 'O'; }
  if (matchInputSignal(DOT, DASH, DASH, DOT, NONE))   { return 'P'; }
  if (matchInputSignal(DASH, DASH, DOT, DASH, NONE))  { return 'Q'; }
  if (matchInputSignal(DOT, DASH, DOT, NONE, NONE))   { return 'R'; }
  if (matchInputSignal(DOT, DOT, DOT, NONE, NONE))    { return 'S'; }
  if (matchInputSignal(DASH, NONE, NONE, NONE, NONE)) { return 'T'; }
  if (matchInputSignal(DOT, DOT, DASH, NONE, NONE))   { return 'U'; }
  if (matchInputSignal(DOT, DOT, DOT, DASH, NONE))    { return 'V'; }
  if (matchInputSignal(DOT, DASH, DASH, NONE, NONE))  { return 'W'; }
  if (matchInputSignal(DASH, DOT, DOT, DASH, NONE))   { return 'X'; }
  if (matchInputSignal(DASH, DOT, DASH, DASH, NONE))  { return 'Y'; }
  if (matchInputSignal(DASH, DASH, DOT, DOT, NONE))   { return 'Z'; }
  if (matchInputSignal(DOT, DASH, DASH, DASH, DASH))  { return '1'; }
  if (matchInputSignal(DOT, DOT, DASH, DASH, DASH))   { return '2'; }
  if (matchInputSignal(DOT, DOT, DOT, DASH, DASH))    { return '3'; }
  if (matchInputSignal(DOT, DOT, DOT, DOT, DASH))     { return '4'; }
  if (matchInputSignal(DOT, DOT, DOT, DOT, DOT))      { return '5'; }
  if (matchInputSignal(DASH, DOT, DOT, DOT, DOT))     { return '6'; }
  if (matchInputSignal(DASH, DASH, DOT, DOT, DOT))    { return '7'; }
  if (matchInputSignal(DASH, DASH, DASH, DOT, DOT))   { return '8'; }
  if (matchInputSignal(DASH, DASH, DASH, DASH, DOT))  { return '9'; }
  if (matchInputSignal(DASH, DASH, DASH, DASH, DASH)) { return '0'; }
  return '?';
}

// turn on the LED for the specified duration in milliseconds
void showLightForDuration(long duration) {
  digitalWrite(LED, HIGH);
  delay(duration);
  digitalWrite(LED, LOW);
}  

// show signal (DOT or DASH) 
boolean showSignal(byte dotDashNone) {
  switch(dotDashNone) {
    case DOT:
      showLightForDuration(DOT_DURATION);
      return true;
    case DASH:
      showLightForDuration(DASH_DURATION);
      return true;
    default:
      return false;
  }
}

// show letter from signals (DOTs and DASHes) via LED
void showLetterForSignals(byte s0, byte s1, byte s2, byte s3, byte s4) {
  if (showSignal(s0)) {
    delay(SIGNAL_GAP);
    if (showSignal(s1)) {
      delay(SIGNAL_GAP);
      if (showSignal(s2)) {
        delay(SIGNAL_GAP);
        if (showSignal(s3)) {
          delay(SIGNAL_GAP);
          showSignal(s4);
        }
      }
    }
  }
  delay(LETTER_GAP);
}  

// show letter from byte via LED
void showLetter(byte letter) {
  if (97 <= letter && letter <= 122) { // if a-z
    letter -= 32; // map to A-Z
  }
  switch(letter) {
    case 'A': showLetterForSignals(DOT, DASH, NONE, NONE, NONE); break;
    case 'B': showLetterForSignals(DASH, DOT, DOT, DOT, NONE); break;
    case 'C': showLetterForSignals(DASH, DOT, DASH, DOT, NONE); break;
    case 'D': showLetterForSignals(DASH, DOT, DOT, NONE, NONE); break;
    case 'E': showLetterForSignals(DOT, NONE, NONE, NONE, NONE); break;
    case 'F': showLetterForSignals(DOT, DOT, DASH, DOT, NONE); break;
    case 'G': showLetterForSignals(DASH, DASH, DOT, NONE, NONE); break;
    case 'H': showLetterForSignals(DOT, DOT, DOT, DOT, NONE); break;
    case 'I': showLetterForSignals(DOT, DOT, NONE, NONE, NONE); break;
    case 'J': showLetterForSignals(DOT, DASH, DASH, DASH, NONE); break;
    case 'K': showLetterForSignals(DASH, DOT, DASH, NONE, NONE); break;
    case 'L': showLetterForSignals(DOT, DASH, DOT, DOT, NONE); break;
    case 'M': showLetterForSignals(DASH, DASH, NONE, NONE, NONE); break;
    case 'N': showLetterForSignals(DASH, DOT, NONE, NONE, NONE); break;
    case 'O': showLetterForSignals(DASH, DASH, DASH, NONE, NONE); break;
    case 'P': showLetterForSignals(DOT, DASH, DASH, DOT, NONE); break;
    case 'Q': showLetterForSignals(DASH, DASH, DOT, DASH, NONE); break;
    case 'R': showLetterForSignals(DOT, DASH, DOT, NONE, NONE); break;
    case 'S': showLetterForSignals(DOT, DOT, DOT, NONE, NONE); break;
    case 'T': showLetterForSignals(DASH, NONE, NONE, NONE, NONE); break;
    case 'U': showLetterForSignals(DOT, DOT, DASH, NONE, NONE); break;
    case 'V': showLetterForSignals(DOT, DOT, DOT, DASH, NONE); break;
    case 'W': showLetterForSignals(DOT, DASH, DASH, NONE, NONE); break;
    case 'X': showLetterForSignals(DASH, DOT, DOT, DASH, NONE); break;
    case 'Y': showLetterForSignals(DASH, DOT, DASH, DASH, NONE); break;
    case 'Z': showLetterForSignals(DASH, DASH, DOT, DOT, NONE); break;
    case '1': showLetterForSignals(DOT, DASH, DASH, DASH, DASH); break;
    case '2': showLetterForSignals(DOT, DOT, DASH, DASH, DASH); break;
    case '3': showLetterForSignals(DOT, DOT, DOT, DASH, DASH); break;
    case '4': showLetterForSignals(DOT, DOT, DOT, DOT, DASH); break;
    case '5': showLetterForSignals(DOT, DOT, DOT, DOT, DOT); break;
    case '6': showLetterForSignals(DASH, DOT, DOT, DOT, DOT); break;
    case '7': showLetterForSignals(DASH, DASH, DOT, DOT, DOT); break;
    case '8': showLetterForSignals(DASH, DASH, DASH, DOT, DOT); break;
    case '9': showLetterForSignals(DASH, DASH, DASH, DASH, DOT); break;
    case '0': showLetterForSignals(DASH, DASH, DASH, DASH, DASH); break;
    case ' ': delay(WORD_GAP); break;
    default: 
      Serial.print("Don't understand [");
      Serial.print((char) letter);
      Serial.print("]");
      showLightForDuration(50);
      delay(50);
      showLightForDuration(50);
      delay(50);
      showLightForDuration(50);
  }
}

void loop() { // loop forever

  long currentTimestamp  = millis(); // get the current timestamp
  long duration = currentTimestamp - lastTimestamp; // get elapsed time
  if (digitalRead(BUTTON) == HIGH) { // if the button is pressed
    if (!buttonWasPressed) {  //  if the button was previously not pressed
      buttonWasPressed = true; // remember the button press
      digitalWrite(LED, HIGH); // turn on the LED
      lastTimestamp = currentTimestamp; // record the time of the button press
      if (duration > LETTER_GAP) {
        lcd.print(' ');
        count++; //to count word number to change row
         if (count ==15)  // this is to change row 
          {
          lcd.setCursor(0, 1);
          }
          if (count >=31) // is row 2 is full LCD to clear
          {
           lcd.clear();
           lcd.setCursor(0, 0); // set to row 1
           count =0;
          }
         
      }
    } // end of if (button was not pressed)111919
  } else { // the button is not pressed
    if (buttonWasPressed) {  // the button was just released
      if (duration < DOT_DURATION) { // if the button was pressed for up to DOT cutoff
        inputSignal[inputSignalIndex] = DOT; // remember the current signal as a DOT
      } else { // if the button was pressed for longer than DOT cutoff
        inputSignal[inputSignalIndex] = DASH; // remember the current signal as a DASH
      }
      inputSignalIndex++; // advance the index to the input signal buffer
      digitalWrite(LED, LOW); // turn off the LED
      buttonWasPressed = false; // consume previous button press
      lastTimestamp = currentTimestamp; // record the time the button was released
    } else { // the button was not just released
      if (inputSignalIndex > 0) { // if there is data in the input signal buffer
        if (duration > SIGNAL_GAP || inputSignalIndex >= 4) { // if we have a complete letter     
          lcd.print(currentInputSignalToLetter()); // print word
          count++; //to count word number to change row
          if (count ==15)  // this is to change row 
          {
          lcd.setCursor(0, 1);
          }
          if (count >=31) // is row 2 is full LCD to clear
          {
           lcd.clear();
           lcd.setCursor(0, 0); // set to row 1
           count =0;
          }
          resetInputSignal(); // reset the input signal buffer
        }
      }
    } // end of else (button was not previously pressed)
  } // end of else (button is not pressed)
  if (Serial.available() > 0) { // if there is data availalbe on the serial port
    showLetter(Serial.read()); // read the next byte and output it as morse code via LED
  }
} // end of loop
