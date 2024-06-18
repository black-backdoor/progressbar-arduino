#include <LiquidCrystal_I2C.h>

// Display Size
const int lcd_rows = 4;
const int lcd_columns = 20;

LiquidCrystal_I2C lcd(0x27, lcd_columns, lcd_rows); // I2C Address: "0x3F" or "0x27"


String trimString(String str, int maxLength) {
  if (str.length() > maxLength) {
    str.remove(maxLength);
  }
  return str;
}


byte START_DIV_0_OF_4[8] = {B01111, B11000, B10000, B10000, B10000, B10000, B11000, B01111};
byte START_DIV_1_OF_4[8] = {B01111, B11000, B10000, B10000, B10000, B10011, B11000, B01111};
byte START_DIV_2_OF_4[8] = {B01111, B11000, B10000, B10000, B10111, B10011, B11000, B01111};
byte START_DIV_3_OF_4[8] = {B01111, B11000, B10000, B10111, B10111, B10011, B11000, B01111};
byte START_DIV_4_OF_4[8] = {B01111, B11000, B10011, B10111, B10111, B10011, B11000, B01111};
byte DIV_0_OF_8[8] = {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111};
byte DIV_1_OF_8[8] = {B11111, B00000, B00000, B00000, B00000, B11000, B00000, B11111};
byte DIV_2_OF_8[8] = {B11111, B00000, B00000, B00000, B11000, B11000, B00000, B11111};
byte DIV_3_OF_8[8] = {B11111, B00000, B00000, B11000, B11000, B11000, B00000, B11111};
byte DIV_4_OF_8[8] = {B11111, B00000, B11000, B11000, B11000, B11000, B00000, B11111};
byte DIV_5_OF_8[8] = {B11111, B00000, B11000, B11000, B11000, B11011, B00000, B11111};
byte DIV_6_OF_8[8] = {B11111, B00000, B11000, B11000, B11011, B11011, B00000, B11111};
byte DIV_7_OF_8[8] = {B11111, B00000, B11000, B11011, B11011, B11011, B00000, B11111};
byte DIV_8_OF_8[8] = {B11111, B00000, B11011, B11011, B11011, B11011, B00000, B11111};
byte END_DIV_0_OF_4[8] = {B11110, B00011, B00001, B00001, B00001, B00001, B00011, B11110};
byte END_DIV_1_OF_4[8] = {B11110, B00011, B00001, B00001, B00001, B11001, B00011, B11110};
byte END_DIV_2_OF_4[8] = {B11110, B00011, B00001, B00001, B11101, B11001, B00011, B11110};
byte END_DIV_3_OF_4[8] = {B11110, B00011, B00001, B11101, B11101, B11001, B00011, B11110};
byte END_DIV_4_OF_4[8] = {B11110, B00011, B11001, B11101, B11101, B11001, B00011, B11110};


void initialize_progressbar() {
  lcd.createChar(0, START_DIV_4_OF_4);
  lcd.createChar(1, DIV_0_OF_8);
  lcd.createChar(2, DIV_8_OF_8);
  lcd.createChar(3, END_DIV_0_OF_4);
}

void switch_progressbar_bank(byte bank) {
  // Progressbar Symbol Bank
  switch (bank) {
    case 0:
      lcd.createChar(4, START_DIV_0_OF_4);
      lcd.createChar(5, START_DIV_1_OF_4);
      lcd.createChar(6, START_DIV_2_OF_4);
      lcd.createChar(7, START_DIV_3_OF_4);
      break;

    case 1:
      lcd.createChar(4, DIV_1_OF_8);
      lcd.createChar(5, DIV_2_OF_8);
      lcd.createChar(6, DIV_3_OF_8);
      lcd.createChar(7, DIV_4_OF_8);
      break;

    case 2:
      lcd.createChar(4, DIV_4_OF_8);
      lcd.createChar(5, DIV_5_OF_8);
      lcd.createChar(6, DIV_6_OF_8);
      lcd.createChar(7, DIV_7_OF_8);
      break;

    case 3:
      lcd.createChar(4, END_DIV_1_OF_4);
      lcd.createChar(5, END_DIV_2_OF_4);
      lcd.createChar(6, END_DIV_3_OF_4);
      lcd.createChar(7, END_DIV_4_OF_4);
      break;
  }
}

void render_progressbar(byte progress, String message = "", bool displayPercentage = true, int textRow = lcd_rows-2, int barRow = lcd_rows-1) {
  initialize_progressbar();

  // If displayPercentage is true, display the progress percentage along with optional message.
  if (displayPercentage) {
    if (message != "") {
      message = trimString(message, lcd_columns - 5);

      // If there's a message provided, print the message at the specified textRow.
      lcd.setCursor(0, textRow);
      lcd.print(message);
      // Print the percentage at the right end of the text row.
      lcd.setCursor(lcd_columns - 4, textRow);
      lcd.print(progress);
      lcd.print(F("%"));
    } else {
      // If no message is provided, just print the percentage.
      lcd.setCursor(0, textRow);
      lcd.print(progress);
      lcd.print(F("%"));
    }
  }

  // Move the cursor to the start of the progress bar row.
  lcd.setCursor(0, barRow);

  // Calculate the number of columns to be filled based on the progress percentage.
  byte filledColumns = map(progress, 0, 100, 0, lcd_columns * 2 * 4 - 2 * 4);

  // Loop through each column of the LCD.
  for (byte i = 0; i < lcd_columns; ++i) {
    if (i == 0) {
      // For the first column, handle the start of the progress bar.
      if (filledColumns > 4) {
        lcd.write((byte)0);
        filledColumns -= 4;
      } else if (filledColumns == 4) {
        lcd.write((byte)0);
        filledColumns = 0;
      } else {
        switch_progressbar_bank(0);
        lcd.setCursor(i, barRow);
        lcd.write(filledColumns + 4);
        filledColumns = 0;
      }
    } else if (i == lcd_columns - 1) {
      // For the last column, handle the end of the progress bar.
      if (filledColumns > 0) {
        switch_progressbar_bank(3);
        lcd.setCursor(i, barRow);
        lcd.write(filledColumns + 3);
      } else {
        lcd.write(3);
      }
    } else {
      // For the middle columns, handle the body of the progress bar.
      if (filledColumns == 0) {
        lcd.write(1); // Empty segment.
      } else if (filledColumns >= 8) {
        lcd.write(2); // Full segment.
        filledColumns -= 8;
      } else if (filledColumns >= 4 && filledColumns < 8) {
        switch_progressbar_bank(2);
        lcd.setCursor(i, barRow);
        lcd.write(filledColumns);
        filledColumns = 0;
      } else if (filledColumns < 4) {
        switch_progressbar_bank(1);
        lcd.setCursor(i, barRow);
        lcd.write(filledColumns + 3);
        filledColumns = 0;
      }
    }
  }
}

void setup() {
  lcd.init(); // LCD Display init
  lcd.backlight(); // LCD Display activate Backlight
}

void loop() {
  static byte percent = 0;

  render_progressbar(percent, "Stings longer then the display can hold will be trimmed.");

  if (++percent == 101) {
    percent = 0;
    lcd.clear();
    delay(2000);
  }

  delay(100);
}