#include <Keypad.h>
#include <ctype.h>
#define MAX_CHAR 13
#define BAUD 9600
#define HOLD_TIME 1000
#define PRESS_TIME_OUT 1000

const byte ROWS = 4;
const byte COLS = 4;

String message;

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


void pop(String &obj)
{
  obj.remove(obj.length() - 1, 1);
}

void switchOnState(char key)
{
  switch(keypadState){
    case PRESSED:
      if (isalpha(key) || true){
        if (physKey != key){
          keyPressTime = millis();
          pressCount = 0;
          virtKey = key;
          physKey = key;
          message += String(key);
        } else if ((millis() - keyPressTime) < PRESS_TIME_OUT) {
          virtKey++;
          pressCount++;
          keyPressTime = millis();
          pop(message);
          message += String(virtKey);
        } else {
          keyPressTime = millis();
          pressCount = 0;
          virtKey = key;
          message += String(virtKey);
        }

          if (pressCount > 2){
            pressCount = 0;
            virtKey = key;
          }
      }

      if (key == '-') {
        pop(message);
      }
      Serial.println(message);
      break;
    
      case HOLD:
        pop(message);
        Serial.println("Message is: " + message);
        break;
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


