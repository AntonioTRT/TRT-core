#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUMPIXELS 12

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Encoder
#define S1 2
#define S2 3
#define KEY 4

int lastS1;
int posicion = 0;

int lastButtonState = HIGH;

void setup() {
  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(KEY, INPUT_PULLUP);

  Serial.begin(9600);

  lastS1 = digitalRead(S1);
}

void loop() {

  // -------- ENCODER --------
  int currentS1 = digitalRead(S1);

  if (lastS1 == HIGH && currentS1 == LOW) {

    if (digitalRead(S2) == HIGH) {
      posicion++;   // CW
    } else {
      posicion--;   // CCW
    }

    // wrap circular
    if (posicion < 0) posicion = NUMPIXELS - 1;
    if (posicion >= NUMPIXELS) posicion = 0;

    Serial.print("Posicion: ");
    Serial.println(posicion);

    updateLED();
  }

  lastS1 = currentS1;

  // -------- BOTÓN (reset al soltar) --------
  int buttonState = digitalRead(KEY);

  if (buttonState == HIGH && lastButtonState == LOW) {
    posicion = 0;
    Serial.println("Reset");

    updateLED();
  }

  lastButtonState = buttonState;
}

// -------- ACTUALIZAR LED --------
void updateLED() {
  pixels.clear();
  pixels.setPixelColor(posicion, pixels.Color(0, 150, 0));
  pixels.show();
}