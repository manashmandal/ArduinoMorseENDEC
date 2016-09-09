#include <Keypad.h>
#include <ctype.h>
#define MAX_CHAR 13
#define BAUD 9600
#define HOLD_TIME 1000
#define PRESS_TIME_OUT 1000

const byte ROWS = 4;
const byte COLS = 4;

char alphaKeys[ROWS][COLS] = {
    { 'a','d','g', ';' },
    { 'j','m','p', '-' },
    { 's','v','y', '_' },
    { ' ','.','#', '?' }
};

byte rowPins[ROWS] = {31, 33, 35, 37}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {45, 43, 41, 39};

unsigned long startTime = 0;
const byte ledPin = 13;
static byte keypadState;
static byte buildCount;
static byte pressCount;

static char virtKey = NO_KEY;
static char physKey = NO_KEY;
static char buildString[MAX_CHAR];

boolean alpha = false;

Keypad keyPad (makeKeymap(alphaKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins));

void alphaKeypadEvent(KeypadEvent key)
{
  keypadState = keyPad.getState();
  switchOnState(key);
}

unsigned long keyPressTime = 0;

//bool isPressedAgain(void)
//{
//  return (uniqueKeyPressTime - millis() 
//}


void switchOnState(char key)
{
  switch(keypadState){
    case PRESSED:
      if (isalpha(key)){
        if (physKey != key){
          keyPressTime = millis();
          pressCount = 0;
          virtKey = key;
          physKey = key;
        } else if ((millis() - keyPressTime) < PRESS_TIME_OUT) {
          virtKey++;
          pressCount++;
//          Serial.println("time: " + String(millis() - keyPressTime));
          keyPressTime = millis();
        } else {
          keyPressTime = millis();
          pressCount = 0;
          virtKey = key;
        }

          if (pressCount > 2){
//            uniqueKeyPressTime = 0;
//            firstKeyPressTime = 0;
//            secondKeyPressTime = 0;
//            thirdKeyPressTime = 0;
            pressCount = 0;
            virtKey = key;
          }
        
        Serial.println(virtKey);
        
      }
  }
}

void setup()
{
  Serial.begin(BAUD);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  keyPad.begin(makeKeymap(alphaKeys));
  keyPad.setHoldTime(HOLD_TIME);
  keyPad.addEventListener(alphaKeypadEvent);
}

char key;

void loop()
{
  key = keyPad.getKey();
}


