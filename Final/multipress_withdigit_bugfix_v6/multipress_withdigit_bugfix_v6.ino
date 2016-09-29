#include <Keypad.h>
#include <LiquidCrystal.h>
#include <ctype.h>
#include "Morse.h"

#define MAX_CHAR 13
#define BAUD 9600
#define HOLD_TIME 1000
#define PRESS_TIME_OUT 1000
#define LCD_ROW 4
#define LCD_COL 20
#define LED_PIN 11
#define BUZZER_PIN 9
#define BUZZ_FREQ 1400
#define WPM 15

const int unit_time = calculateUnitTime(WPM);


Morse codec(LED_PIN, BUZZER_PIN, BUZZ_FREQ);

void switchOnState(char key);

const byte ROWS = 4;
const byte COLS = 4;

String message;

const char BACKSPACE_KEY = '|';
const char DOT = '.';
const char DASH = '-';
const char MODE_KEY = '?';
const char SPACE = ' ';
const char BACKSLASH = '/';
const char NUMBER_KEY = '#';

bool numkey = false;

bool encode_mode = true;

char alphaKeys[ROWS][COLS] = {
    { 'a','d','g', DOT },
    { 'j','m','p', DASH },
    { 's','v','y', BACKSPACE_KEY },
    { SPACE, BACKSLASH , NUMBER_KEY , MODE_KEY}
};

char numberKeys[ROWS][COLS] = {
  {'1', '4', '7', DOT},
  {'0', '\0', '\0', DASH},
  {'\0', '\0', '\0', BACKSPACE_KEY},
  {'\0', '\0', NUMBER_KEY, MODE_KEY}
};

byte rowPins[ROWS] = {31, 33, 35, 37}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {45, 43, 41, 39};

unsigned long startTime = 0;
const byte ledPin = 11;
static byte keypadState;
static byte buildCount;
static byte pressCount;

static char virtKey = NO_KEY;
static char physKey = NO_KEY;
static char buildString[MAX_CHAR];

boolean alpha = false;


class LCD : public LiquidCrystal {
  public:
    LCD(byte en, byte rs, byte D0, byte D1, byte D2, byte D3) : LiquidCrystal(en, rs, D0, D1, D2, D3) {}
    byte getCursorRow(void) {
       LCD::setCursor(0, 0);
    }

    void setCursor(byte y, byte x)
    {
      x = x;
      y = y;
      LiquidCrystal::setCursor(y, x);
    }

    void begin(byte cols, byte rows)
    {
      this->rows = rows;
      this->cols = cols;
      LiquidCrystal::begin(cols, rows);
    }

    byte getRowCount(void) const {
      return rows;
    }

    byte getColCount(void) const {
      return cols;
    }

    void print(String obj)
    {
      printObj = obj;
      LiquidCrystal::print(obj);
      calculateCursorPosition(obj);
      totalString.concat(obj);
    }

    void print(char c)
    {
      printObj = String(c);
      LiquidCrystal::print(printObj);
      calculateCursorPosition(printObj);
      totalString.concat(printObj);
    }

    void print(String obj, bool concatenation)
    {
      printObj = obj;
      LiquidCrystal::print(obj);
      calculateCursorPosition(obj);
    }

    void print(int obj)
    {
      printObj = String(obj);
      LiquidCrystal::print(obj);
    }

    void print(float obj)
    {
      printObj = String(obj);
      LiquidCrystal::print(obj);
    }

    void print(double obj)
    {
      printObj = String(obj);
      LiquidCrystal::print(obj);
    }

    void backspace(int times=1)
    {
      String str = totalString;
      clear();
      for (int i = 0; i < times; i++) pop(str);
      totalString = str;
      longStrPrint(totalString, 0, 0);
    }

    void left(void)
    {
      moveCursorBack();
    }

    void right(void)
    {
      moveCursorFront();
    }

    void clear(void)
    {
      printObj = "";
      x = 0;
      y = 0;
      setCursor(0, 0);
      totalString = "";
      LiquidCrystal::clear();
    }

    void nonBreakPrint(String &printObj, byte x = 0, byte y = 0)
    {
      longStrPrint(printObj, x, y);
    }

    String getTotalString(void)
    {
      return totalString;
    }


   protected:
      void calculateCursorPosition(String &po)
      {
        byte prev_x = x;
        byte prev_y = y;

        byte row = po.length() / getColCount();

        if (row > getRowCount()) row = getRowCount();

        byte col = po.length() % getColCount();
        
        x = prev_x + row;
        y = prev_y + col;

        if (y >= getColCount()) { y = 0; x++; }
        if (x >= getRowCount()) { x = 0; y = 0;}
        
        setCursor(y, x);
      }

      void pop(String &obj)
      {
        obj.remove(obj.length() - 1, 1);
      }

      void moveCursorBack(void)
      {
        if (x < 0 && y < 0) return;
        if (y >= 0) y--;
        if (y < 0 && x != 0)  { x--; y = getColCount() - 1; }
        setCursor(y, x);
      }

      void moveCursorFront(void)
      {
        if (y < getColCount()) y++;
        if (y == getColCount())  { x++; y = 0; }
        if (x == getRowCount() && y == getColCount()) { x = 0; y = 0; }
        setCursor(y, x);
      }

      void longStrPrint(String printObj, byte x, byte y)
      {
        setCursor(y, x);
        for (int i = 0; i < printObj.length(); i++){
          print(String(printObj[i]), false);
        }
      }

   private:
      int x;
      int y;
      byte rows;
      byte cols;
      String printObj;
      String totalString;
};

Keypad keyPad (makeKeymap(alphaKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins));
LiquidCrystal lcd1(2, 3, 4, 5, 6, 7);
LCD lcd(2, 3, 4, 5, 6, 7);


void alphaKeypadEvent(KeypadEvent key)
{
  keypadState = keyPad.getState();
  switchOnState(key);
}

unsigned long keyPressTime = 0;


void dit(void)
{
  digitalWrite( ledPin, HIGH );
  tone(BUZZER_PIN, BUZZ_FREQ);
  delay( unit_time );
  digitalWrite( ledPin, LOW );
  noTone(BUZZER_PIN);
  delay( unit_time );
}

void dah(void)
{
  digitalWrite( ledPin, HIGH );
  tone(BUZZER_PIN, BUZZ_FREQ);
  delay( unit_time *3 );
  digitalWrite( ledPin, LOW );
  noTone(BUZZER_PIN);
  delay( unit_time );
}

void pop(String &obj)
{
  obj.remove(obj.length() - 1, 1);
}

void switchOnState(char key)
{
  switch(keypadState){
    case PRESSED:
      if (key == MODE_KEY) {
        encode_mode = !encode_mode;
        lcd.clear();
        message = "";
        if (encode_mode)
          lcd.nonBreakPrint("Encode: " + message);
        else
          lcd.nonBreakPrint("Decode: " + message);
      }
    
      if ((isalpha(key) || isdigit(key)) && encode_mode){
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
            pop(message);
            message += String(virtKey);
          }
        lcd.clear();
        lcd.nonBreakPrint("Encode: " + message, 0, 0);
        Serial.println(message);
      } 

      else if (key == NUMBER_KEY){
        numkey = !numkey;
        if (numkey) { keyPad.begin(makeKeymap(numberKeys)); }
        else keyPad.begin(makeKeymap(alphaKeys));
        Serial.println(numkey);
      }

      
      else if (key == BACKSPACE_KEY)  { 
        lcd.backspace();
        pop(message);
        if (encode_mode) 
          lcd.nonBreakPrint("Encode: " + message);
        else
          lcd.nonBreakPrint("Decode: " + message);
      }

      else if ((key == ' ' || key == '.') && encode_mode) {
        message += String(key);
        lcd.clear();
        lcd.nonBreakPrint("Encode: " + message, 0, 0);
      }

      else if ((!encode_mode) && (key == DASH || key == DOT || key == BACKSLASH || key == SPACE)){
        if (key == DOT) dit();
        if (key == DASH) dah();
        message += String(key);
        lcd.clear();
        lcd.nonBreakPrint("Decode: " + message, 0, 0);
      }
      
        break;

      
     case HOLD:
        pop(message);
        lcd.clear();
        if (encode_mode){
//          if (isdigit(message[0])) message += "1";
          message += "1";
          Serial.println("msg: " + String(message));
          String encoded = codec.encode(message);
          lcd.nonBreakPrint("Encoded msg: " + encoded);
          Serial.println("encoded msg: " + encoded);
          for (int i = 0; i <= encoded.length(); i++){
          switch( encoded[i] )
          {
            case '.': //dit
              digitalWrite( ledPin, HIGH );
              tone(BUZZER_PIN, BUZZ_FREQ);
              delay( unit_time );
              digitalWrite( ledPin, LOW );
              noTone(BUZZER_PIN);
              delay( unit_time );
                
              break;
      
            case '-': //dah
              digitalWrite( ledPin, HIGH );
              tone(BUZZER_PIN, BUZZ_FREQ);
              delay( unit_time *3 );
              digitalWrite( ledPin, LOW );
              noTone(BUZZER_PIN);
              delay( unit_time );
                
              break;
      
            case ' ': //gap
              delay( unit_time );
              noTone(BUZZER_PIN);
              break;

            case '/':
              delay(unit_time * 7);
              noTone(BUZZER_PIN);
              break;
          }
        }
        } else {
          message += " .-";
          String decoded = codec.decode(message);
          Serial.println("decoded: " + decoded);
          lcd.nonBreakPrint("Decoded msg: " + decoded);
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
    lcd.begin(LCD_COL, LCD_ROW);
    lcd1.begin(LCD_COL, LCD_ROW);
    lcd1.blink();
    lcd.blink();
    lcd.setCursor(0, 0);
    lcd.backspace();
    lcd.backspace();
    lcd.print("Encode: ");
    codec.begin();
    digitalWrite(ledPin, LOW);
    Serial.println(codec.decode(".- -.-. -..."));
}

char key;

void loop()
{
  key = keyPad.getKey();
}


