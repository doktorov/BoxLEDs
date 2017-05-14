#include "clock.h"

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

//
#define MIC_PIN 7
#define EQ_SAMPLE_WINDOW  50

unsigned int sample;
int array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int soundWhile;
//

// Button 4x4
#define BTN44_SCL_PIN   2
#define BTN44_SDO_PIN   3

TTP229 ttp229(BTN44_SCL_PIN, BTN44_SDO_PIN);

uint8_t keyboard = 0;

void setup() {
  Serial.begin(9600);

  clockMatrix.SETUP();

  pixels.setBrightness(RGB_BRIGHTNESS);
  pixels.begin();
  pixels.show();

  sample = 0;
  soundWhile = 0;
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
      equalizer();
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

void equalizer() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < EQ_SAMPLE_WINDOW) {
    sample = analogRead(MIC_PIN);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  array[soundWhile] = peakToPeak;

  soundWhile++;

  if (soundWhile == 8) {
    soundWhile = 0;

    for (int i = 0; i < 8; i++) {
      setSoundLevel(i, array[i]);
    }

    pixels.show();
  }

  delay(1);
}

void setSoundLevel(int pos, int level) {
  unsigned int setPos = pos * 8;

  pixels.setPixelColor(setPos, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 3, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 4, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
  pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));

  if (level <= 65) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
  }

  if ((level > 65) && (level <= 130)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
  }

  if ((level > 130) && (level <= 195)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
  }

  if ((level > 195) && (level <= 230)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
  }

  if ((level > 230) && (level <= 295)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
  }

  if ((level > 295) && (level <= 360)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
  }

  if ((level > 360) && (level <= 425)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(255, 0, 0));
  }

  if (level > 425) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(255, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(255, 0, 0));
  }
}


