#include <LiquidCrystal.h>

#define BUTTON_PIN_MINUS 6
#define BUTTON_PIN_PLUS 7
#define COLORS_COUNT 5

// LCD settings
#define LCD_COLUMN_NUMBER 16
#define LCD_ROW_NUMBER 2
#define LCD_RS 12 // Register Select
#define LCD_E 11  // Enable
#define LCD_DB4 5 // BD4
#define LCD_DB5 4 // BD5
#define LCD_DB6 3 // BD6
#define LCD_DB7 2 // BD7

class Button {
private:
  bool _state;
  uint8_t _pin;

public:
  Button(uint8_t pin) : _pin(pin) {}

  void begin() {
    pinMode(_pin, INPUT_PULLUP);
    _state = digitalRead(_pin);
  }

  bool isPressed() {
    bool v = digitalRead(_pin);
    delay(10);
    v = digitalRead(_pin);

    if (v != _state) {
      _state = !v;
      if (_state) {
        return true;
      }
    }

    return false;
  }

  bool isReleased() {
    bool v = digitalRead(_pin);
    delay(10);
    v = digitalRead(_pin);

    if (v != _state) {
      _state = v;
      if (_state) {
        return true;
      }
    }

    return false;
  }
};

Button minusButton(BUTTON_PIN_MINUS);
Button plusButton(BUTTON_PIN_PLUS);
String colorModes[COLORS_COUNT] = {"Red", "Green", "Blue", "Purple",
                                   "Ocean wave"};
int count = 0;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);

void clearLCDLine(int lineNumber) {
  lcd.setCursor(0, lineNumber);

  for (int n = 0; n < LCD_COLUMN_NUMBER; n++) {
    lcd.print(" ");
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize the buttons
  minusButton.begin();
  plusButton.begin();

  // Initialize the LCD
  lcd.begin(LCD_COLUMN_NUMBER, LCD_ROW_NUMBER);
  lcd.print("Choose the mode:");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(colorModes[count]);

  if (minusButton.isPressed()) {
    Serial.println(F("Pressed"));
  }

  if (minusButton.isReleased()) {
    if (count > 0) {
      clearLCDLine(1);
      count--;
    }
  }

  if (plusButton.isReleased()) {
    if (count < COLORS_COUNT - 1) {
      clearLCDLine(1);
      count++;
    }
  }
}
