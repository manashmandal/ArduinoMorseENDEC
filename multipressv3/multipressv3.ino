#include <Keypad.h>
#include <LiquidCrystal.h>
#include <ctype.h>
#define MAX_CHAR 13
#define BAUD 9600
#define HOLD_TIME 1000
#define PRESS_TIME_OUT 1000
#define LCD_ROW 4
#define LCD_COL 20


void switchOnState(char key);

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

    void fast_backspace(void)
    {
      moveCursorBack();
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

   protected:
      void calculateCursorPosition(String &po)
      {
        byte prev_x = x;
        byte prev_y = y;

        byte row = po.length() / getColCount();
        //For 0 indexing purpose

        if (row > getRowCount()) row = getRowCount();

        byte col = po.length() % getColCount();
        
        x = prev_x + row;
        y = prev_y + col;

        if (y >= getColCount()) { y = 0; x++; }
        if (x >= getRowCount()) { x = 0; y = 0;}

        Serial.println("row is: " + String(x));
        Serial.println("col is: " + String(y));
        
        setCursor(y, x);
      }

      void pop(String &obj)
      {
        obj.remove(obj.length() - 1, 1);
      }

      void longStrPrint(String &printObj, byte x, byte y)
      {
        setCursor(y, x);
        for (int i = 0; i < printObj.length(); i++){
          print(String(printObj[i]), false);
        }
      }

      void moveCursorBack(void)
      {
        if (x == 0 && y == 0) return;
        if (y >= 0) y--;
        if (y < 0 && x != 0)  { x--; y = getColCount() - 1; }
        setCursor(y, x);
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
LCD lcd(2, 3, 4, 5, 6, 7);



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

void pop(String &obj)
{
  obj.remove(obj.length() - 1, 1);
}

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
          message += String(key);
        } else if ((millis() - keyPressTime) < PRESS_TIME_OUT) {
          virtKey++;
          pressCount++;
//          Serial.println("time: " + String(millis() - keyPressTime));
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
//            uniqueKeyPressTime = 0;
//            firstKeyPressTime = 0;
//            secondKeyPressTime = 0;
//            thirdKeyPressTime = 0;
            pressCount = 0;
            virtKey = key;
          }
        
        Serial.println(virtKey);

     case HOLD:
        Serial.println("Message is: " + message);
        
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
  lcd.blink();
//  lcd.print("magu");
  lcd.setCursor(0, 0);
//  lcd.print(lcd.getRowCount());
//  lcd.print("12345678");
////  lcd.print("bal");
//  lcd.print("done");
//    lcd.setCursor(0, 0);
    lcd.print("balsabalsa");
//    lcd.print("balsabalsa");
//    lcd.print("balsabalsa");
//    lcd.print("balsabalsa");
//    lcd.setCursor(0, 2);
    lcd.print("balsabalsa");
    lcd.print("dhonso");
//    lcd.print("abalkhanki");
    lcd.backspace();
    lcd.backspace();
}

char key;

void loop()
{
  key = keyPad.getKey();
  if (key == '-') lcd.fast_backspace();
  else lcd.print(key);
}


