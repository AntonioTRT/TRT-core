#include <Adafruit_NeoPixel.h>

#define PIN 7
#define WIDTH 32
#define HEIGHT 8
#define NUM_LEDS 256

const int DISPLAY_TIME_MS = 70;

Adafruit_NeoPixel matrix(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int XY(int x, int y)
{
  if (x % 2 == 0)
    return x * HEIGHT + y;
  else
    return x * HEIGHT + (HEIGHT - 1 - y);
}

// Corazón pequeño
const byte heartSmall[8] =
{
  B00000000,
  B00100100,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00000000,
  B00000000
};

// Corazón grande
const byte heartLarge[8] =
{
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

void drawBitmap(const byte bmp[8], int xOffset, uint32_t color)
{
  for (int y = 0; y < 8; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      if (bmp[y] & (1 << (7 - x)))
      {
        matrix.setPixelColor(XY(xOffset + x, y), color);
      }
    }
  }
}

void showHeart(const byte bmp[8], int block)
{
  matrix.clear();

  drawBitmap(
    bmp,
    block * 8,
    matrix.Color(255, 0, 0)
  );

  matrix.show();
}

void setup()
{
  matrix.begin();
  matrix.clear();
  matrix.show();
}

void showTwoHearts(const byte bmp[8], int block1, int block2)
{
  matrix.clear();

  drawBitmap(bmp, block1 * 8, matrix.Color(255, 0, 0));
  drawBitmap(bmp, block2 * 8, matrix.Color(255, 0, 0));

  matrix.show();
}

void loop()
{
  // Bloques 1 y 3
  showTwoHearts(heartSmall, 0, 2);
  delay(DISPLAY_TIME_MS);

  showTwoHearts(heartLarge, 0, 2);
  delay(DISPLAY_TIME_MS);

  showTwoHearts(heartLarge, 0, 2);
  delay(DISPLAY_TIME_MS);

  showTwoHearts(heartSmall, 0, 2);
  delay(DISPLAY_TIME_MS);

  matrix.clear();
  matrix.show();
  delay(300);

  // Bloques 2 y 4
  showTwoHearts(heartSmall, 1, 3);
  delay(DISPLAY_TIME_MS);

  showTwoHearts(heartLarge, 1, 3);
  delay(DISPLAY_TIME_MS);

  showTwoHearts(heartLarge, 1, 3);
  delay(DISPLAY_TIME_MS);

  showTwoHearts(heartSmall, 1, 3);
  delay(DISPLAY_TIME_MS);

  matrix.clear();
  matrix.show();
  delay(300);
}