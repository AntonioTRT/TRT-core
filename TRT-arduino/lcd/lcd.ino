#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

int contador = 1;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Mensaje ");
  lcd.print(contador);

  Serial.print("Mensaje ");
  Serial.println(contador);

  contador++;

  if (contador > 10) {
    contador = 1;
  }

  delay(1000);
}