#include "clock.h"
#include "equalizer.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <TTP229.h>

// RGB 8x8
#define RGB_PIN         A0
#define RGB_NUMPIXELS   64
#define RGB_BRIGHTNESS  10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(RGB_NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);
//

// Button 4x4
#define BTN44_SCL_PIN   2
#define BTN44_SDO_PIN   3

TTP229 ttp229(BTN44_SCL_PIN, BTN44_SDO_PIN);

uint8_t keyboard = 0;

void setup() {
  Serial.begin(9600);

  clockMatrix.SETUP();
  equalizerShow.SETUP();

  pixels.setBrightness(RGB_BRIGHTNESS);
  pixels.begin();
  pixels.show();

  //rings.setBrightness(RGB_BRIGHTNESS);
  //rings.begin();
  //rings.show();
}

int start1_i = 0;

void loop() {
  uint8_t sel_keyboard = ttp229.GetKey16();
  if (sel_keyboard) {
    if (keyboard != sel_keyboard) {
      keyboard = sel_keyboard;
      start1_i = -1;
    }
  }

  //Serial.println(String(keyboard) + " - " + String(sleep));

  switch (keyboard) {
    case 1:
      heart();
      break;
    case 2:
      start1();
      break;
    case 16:
      equalizerShow.GO();
      break;
    default:
      heart();
      break;
  }

  clockMatrix.PRINT_TIME();

  delay(1);
}

typedef bool charMapType[8][8];

const charMapType heart4 = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {0, 1, 1, 0, 0, 1, 1, 0}
};

void heart() {
  int z = 0;

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      bool v = (heart4)[y][x];

      if (v) {
        pixels.setPixelColor(z, pixels.Color(255, 0, 0));
      } else {
        pixels.setPixelColor(z, pixels.Color(0, 0, 255));
      }

      z++;
    }
  }

  pixels.show();
}

void start1() {
  //Serial.println("start1");

  if (start1_i < RGB_NUMPIXELS) {
    start1_i++;
  } else {
    start1_i = 0;
  }

  int r = random(0, 255);
  int g = random(0, 255);
  int b = random(0, 255);

  pixels.setPixelColor(start1_i, pixels.Color(r, g, b));
  pixels.show();
}
