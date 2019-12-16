#include <GyverRGB.h>
#include <LiquidCrystal.h>

#define BUTTON_PIN_MINUS 6
#define BUTTON_PIN_PLUS 7
#define COLORS_COUNT 5

// LCD settings
#define LCD_COLUMN_NUMBER 16
#define LCD_ROW_NUMBER 2
#define LCD_RS_PIN 13 // Register Select
#define LCD_E_PIN 12  // Enable
#define LCD_DB4_PIN 5 // BD4
#define LCD_DB5_PIN 4 // BD5
#define LCD_DB6_PIN 3 // BD6
#define LCD_DB7_PIN 2 // BD7

// RGB LED settings
#define LED_RED_PIN 9    // R
#define LED_GREEN_PIN 10 // G
#define LED_BLUE_PIN 11  // B
#define LED_HUE_START 0  // Initial fire color
#define LED_HUE_GAP 10
#define LED_SMOOTH_K 0.15  // Fire smoothness coefficient
#define LED_MIN_BRIGHT 80  // Min fire brightness
#define LED_MAX_BRIGHT 255 // Max fire brightness
#define LED_MIN_SAT 255    // Min saturation
#define LED_MAX_SAT 255    // Max saturation

int ledHueStart = LED_HUE_START;

class ColorMode {
private:
  int _id;
  String _name;
  int _value;

public:
  ColorMode(int id, String name, int value)
      : _id(id), _name(name), _value(value) {}

  int getId() { return _id; }
  String getName() { return _name; }
  int getValue() { return _value; }
};

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
ColorMode colorModes[] = {ColorMode(0, "Red", 0), ColorMode(1, "Green", 80),
                          ColorMode(2, "Blue", 140),
                          ColorMode(3, "Purple", 190),
                          ColorMode(4, "Ocean wave", 240)};
int count = 0;

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN,
                  LCD_DB7_PIN);
GRGB strip(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN);

void clearLCDLine(int lineNumber) {
  lcd.setCursor(0, lineNumber);

  for (int n = 0; n < LCD_COLUMN_NUMBER; n++) {
    lcd.print(" ");
  }
}

void fireTick() {
  static uint32_t prevTime, prevTime2;
  static byte fireRnd = 0;
  static float fireValue = 0;

  // Set fire direction
  if (millis() - prevTime > 100) {
    prevTime = millis();
    fireRnd = random(0, 10);
  }

  // Move the flame
  if (millis() - prevTime2 > 20) {
    prevTime2 = millis();
    fireValue = (float)fireValue * (1 - LED_SMOOTH_K) +
                (float)fireRnd * 10 * LED_SMOOTH_K;
    strip.setHSV(
        constrain(
            map(fireValue, 20, 60, ledHueStart, ledHueStart + LED_HUE_GAP), 0,
            255), // H
        constrain(map(fireValue, 20, 60, LED_MAX_SAT, LED_MIN_SAT), 0,
                  255), // S
        constrain(map(fireValue, 20, 60, LED_MIN_BRIGHT, LED_MAX_BRIGHT), 0,
                  255) // V
    );
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

  // Initialize the LED
  strip.setBrightness(255);
  strip.setDirection(REVERSE);
}

void loop() {
  fireTick();

  lcd.setCursor(0, 1);
  lcd.print(colorModes[count].getName());

  Serial.println(colorModes[count].getId());
  Serial.println(colorModes[count].getName());
  Serial.println(colorModes[count].getValue());

  if (minusButton.isPressed()) {
    Serial.println(F("Pressed"));
  }

  if (minusButton.isReleased() && count > 0) {
    clearLCDLine(1);
    count--;
    ledHueStart = colorModes[count].getValue();
  }

  if (plusButton.isReleased() && count < COLORS_COUNT - 1) {
    clearLCDLine(1);
    count++;
    ledHueStart = colorModes[count].getValue();
  }
}
