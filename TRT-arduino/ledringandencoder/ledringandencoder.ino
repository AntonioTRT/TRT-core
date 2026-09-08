#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUMPIXELS 12

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Encoder
#define S1 2
#define S2 4
#define KEY 3

int lastS1;
int posicion = 0;

int lastButtonState = HIGH;

// Colores
int colorIndex = 0;

uint32_t colores[] = {
  Adafruit_NeoPixel::Color(150, 0, 0),    // Rojo
  Adafruit_NeoPixel::Color(0, 150, 0),    // Verde
  Adafruit_NeoPixel::Color(0, 0, 150),    // Azul
  Adafruit_NeoPixel::Color(150, 150, 0)   // Amarillo
};

int numColores = sizeof(colores) / sizeof(colores[0]);

unsigned long lastActivity = 0;
const unsigned long timeout = 5000;

bool encendido = true;

void setup() {

  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(KEY, INPUT_PULLUP);

  Serial.begin(9600);

  lastS1 = digitalRead(S1);
  lastActivity = millis();

  updateLED();
}

void loop() {

  bool evento = false;

  // -------- ENCODER --------
  int currentS1 = digitalRead(S1);

  if (lastS1 != currentS1 && currentS1 == LOW) {

    if (!encendido) {

      encendido = true;
      updateLED();
      lastActivity = millis();

    } else {

      // Detección de dirección
      if (digitalRead(S2) != currentS1) {
        posicion++;
      } else {
        posicion--;
      }

      if (posicion < 0) {
        posicion = NUMPIXELS - 1;
      }

      if (posicion >= NUMPIXELS) {
        posicion = 0;
      }

      Serial.print("Posicion: ");
      Serial.println(posicion);

      evento = true;
    }
  }

  lastS1 = currentS1;

  // -------- BOTON --------
  int buttonState = digitalRead(KEY);

  if (buttonState == HIGH && lastButtonState == LOW) {

    if (!encendido) {

      encendido = true;
      updateLED();
      lastActivity = millis();

    } else {

      colorIndex++;

      if (colorIndex >= numColores) {
        colorIndex = 0;
      }

      evento = true;
    }
  }

  lastButtonState = buttonState;

  if (evento) {
    lastActivity = millis();
    updateLED();
  }

  // -------- AUTO OFF --------
  if (encendido && (millis() - lastActivity > timeout)) {

    pixels.clear();
    pixels.show();

    encendido = false;

    Serial.println("OFF");
  }
}

// -------- LED --------
void updateLED() {

  pixels.clear();

  pixels.setPixelColor(posicion, colores[colorIndex]);

  pixels.show();
}
