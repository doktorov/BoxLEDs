#include "clock.h"
#include "matrix.h"

#include <Adafruit_NeoPixel.h>
#include <Keypad.h>

// SD
#define SD_CS   35

// RGB 8x8
#define RGB_PIN         6
#define RGB_NUMPIXELS   64
#define RGB_BRIGHTNESS  10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(RGB_NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);
//

//
#define RGB_RINGS_PIN         7
#define RGB_RINGS_NUMPIXELS   61
#define RGB_RINGS_BRIGHTNESS  10

Adafruit_NeoPixel rings = Adafruit_NeoPixel(RGB_RINGS_NUMPIXELS, RGB_RINGS_PIN, NEO_GRB + NEO_KHZ800);
//

//
#define MIC_PIN 7
#define EQ_SAMPLE_WINDOW  5

unsigned int sample;
int sounds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int soundWhile;
//

// Button 4x4
#define BTN44_SCL_PIN   2
#define BTN44_SDO_PIN   3

//
int matrixWait;
int matrixPos;
uint32_t first;
uint32_t second;

//
#define RING_WAIT 2
int ringPos;
uint32_t rnd_color;

char sel_matrix_keyboard;
char sel_rings_keyboard;

//
#define KEYBOARD_ROWS   4
#define KEYBOARD_COLS   4

char hexaKeys[KEYBOARD_ROWS][KEYBOARD_COLS] = {
  {'1', '4', '7', '*'},
  {'2', '5', '8', '0'},
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};
byte rowPins[KEYBOARD_ROWS] = {25, 24, 23, 22};
byte colPins[KEYBOARD_COLS] = {29, 28, 27, 26};

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, KEYBOARD_ROWS, KEYBOARD_COLS);


byte selTypeRGB;

int runRings2Pos;
int ringLevel;
int ringDelay;

#define RGB_RINGS_NUMPIXELS_24    61
#define RGB_RINGS_NUMPIXELS_16    37
#define RGB_RINGS_NUMPIXELS_12    21
#define RGB_RINGS_NUMPIXELS_8     9

#define RGB_RINGS_DELAY           50

void setup() {
  Serial.begin(9600);

  clockMatrix.SETUP(SD_CS);
  matrixFromFile.SETUP(SD_CS);

  pixels.setBrightness(RGB_BRIGHTNESS);
  pixels.begin();
  pixels.show();

  rings.setBrightness(RGB_RINGS_BRIGHTNESS);
  rings.begin();
  rings.show();

  sample = 0;
  soundWhile = 0;

  matrixWait = 0;
  matrixPos = 0;
  first = 0;
  second = 0;

  ringPos = 0;

  sel_matrix_keyboard = 0;
  sel_rings_keyboard = 0;

  selTypeRGB = 1;

  ringPos = RGB_RINGS_NUMPIXELS_16;
  ringLevel = 0;
  ringDelay = RGB_RINGS_DELAY;

  rnd_color = getRandomRingsRGB();

  pinMode(34, OUTPUT);
  pinMode(33, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  //pinMode(31, INPUT_PULLUP);
  //pinMode(30, INPUT_PULLUP);
  digitalWrite(34, LOW);
}

void loop() {
  if (digitalRead(32) == 0) {
    selTypeRGB = 1;
  }
  if (digitalRead(33) == 0) {
    selTypeRGB = 2;
  }

  char customKey = customKeypad.getKey();
  if (customKey) {
    switch (selTypeRGB) {
      case 1:
        if (sel_matrix_keyboard != customKey) {
          sel_matrix_keyboard = customKey;

          pixels.begin();
          pixels.show();

          matrixWait == 0;
          matrixPos = 0;
        }
        break;
      case 2:
        if (sel_rings_keyboard != customKey) {
          sel_rings_keyboard = customKey;

          rings.begin();
          rings.show();

          ringPos = 0;

          runRings2Pos = RGB_RINGS_NUMPIXELS_16;
          ringLevel = 0;
          ringDelay = RGB_RINGS_DELAY;

          rnd_color = getRandomRingsRGB();
        }
        break;
    }
  }

  switch (sel_matrix_keyboard) {
    case '1':
      heart("HEART.TXT", 156, 50);
      break;
    case '2':
      heart("BOX.TXT", 207, 10);
      break;
    case 'A':
      rainbowMatrix();
      break;
    case 'B':
      randomMatrix();
      break;
    case 'D':
      equalizer();
      break;
    default:
      rainbowMatrix();
      break;
  }

  switch (sel_rings_keyboard) {
    case 'A':
      rainbowRings();
      break;
    case 'B':
      randomRings();
      break;
    case 'C':
      randomRings1();
      break;
    case 'D':
      runRings2();
      break;
    default:
      rainbowRings();
      break;
  }

  rings.show();

  clockMatrix.PRINT_TIME();

  delay(1);
}

uint32_t getRandomMatrixRGB() {
  int r = random(0, 256);
  int g = random(0, 256);
  int b = random(0, 256);

  return pixels.Color(r, g, b);
}

void heart(String fileName, int mPos, int wait) {
  if (matrixWait >= wait) {
    char *p;
    p = matrixFromFile.GET(matrixPos, fileName);

    if (matrixPos == 0) {
      first = getRandomMatrixRGB();

      second = getRandomMatrixRGB();
    }

    matrixPos++;
    if (matrixPos == mPos) {
      matrixPos = 0;
    }

    int z = 0;
    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
        int val = int(*(p + z) - '0');

        switch (val) {
          case 1:
            pixels.setPixelColor(z, first);
            break;
          default:
            pixels.setPixelColor(z, second);
            break;
        }

        z++;
      }
    }

    matrixWait = 0;

    pixels.show();
  } else {
    matrixWait++;
  }
}

void equalizer() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

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

  sounds[soundWhile] = peakToPeak;

  soundWhile++;

  if (soundWhile == 8) {
    soundWhile = 0;

    for (int i = 0; i < 8; i++) {
      setSoundLevel(i, sounds[i]);
    }

    pixels.show();
  }
}

void setSoundLevel(int pos, int level) {
  unsigned int setPos = pos * 8;

  if (level <= 65) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
  }

  if ((level > 65) && (level <= 130)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
  }

  if ((level > 130) && (level <= 195)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
  }

  if ((level > 195) && (level <= 230)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
  }

  if ((level > 230) && (level <= 295)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
  }

  if ((level > 295) && (level <= 360)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(0, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
  }

  if ((level > 360) && (level <= 425)) {
    pixels.setPixelColor(setPos, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(setPos + 3, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 4, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 5, pixels.Color(255, 255, 0));
    pixels.setPixelColor(setPos + 6, pixels.Color(255, 0, 0));
    pixels.setPixelColor(setPos + 7, pixels.Color(0, 0, 0));
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

void randomMatrix() {
  if (matrixPos < RGB_NUMPIXELS) {
    matrixPos++;
  } else {
    matrixPos = 0;
  }

  pixels.setPixelColor(matrixPos, getRandomMatrixRGB());
  pixels.show();
}

void rainbowMatrix() {
  for (int i = 0; i < RGB_NUMPIXELS; i++) {
    pixels.setPixelColor(i, Wheel(((i * 256 / RGB_NUMPIXELS) + matrixPos) & 255));
  }
  pixels.show();

  matrixPos++;
  if (matrixPos == 1281) matrixPos = 0;
}


// Rings
uint32_t getRandomRingsRGB() {
  int r = random(0, 256);
  int g = random(0, 256);
  int b = random(0, 256);

  return rings.Color(r, g, b);
}

void randomRings() {
  if (ringPos < RGB_RINGS_NUMPIXELS) {
    ringPos++;
  } else {
    ringPos = 0;
  }

  rings.setPixelColor(ringPos, getRandomRingsRGB());
}

void rainbowRings() {
  for (int i = 0; i < RGB_RINGS_NUMPIXELS; i++) {
    rings.setPixelColor(i, Wheel(((i * 256 / RGB_RINGS_NUMPIXELS) + ringPos) & 255));
  }

  ringPos++;
  if (ringPos == 1281) ringPos = 0;
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return rings.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return rings.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return rings.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void randomRings1() {
  if (ringDelay == RGB_RINGS_DELAY) {
    if (ringPos < RGB_RINGS_NUMPIXELS) {
      ringPos++;
    } else {
      ringPos = 0;

      rnd_color = getRandomRingsRGB();
    }

    rings.setPixelColor(ringPos, rnd_color);
    ringDelay = 0;
  } else {
    ringDelay++;
  }
}

void runRings2() {
  if (ringDelay == RGB_RINGS_DELAY) {
    if (ringLevel == 0) {
      if (runRings2Pos < RGB_RINGS_NUMPIXELS_24) {
        rings.setPixelColor(runRings2Pos, rnd_color);
        runRings2Pos++;
      } else {
        runRings2Pos = RGB_RINGS_NUMPIXELS_12;

        rnd_color = getRandomRingsRGB();

        ringLevel = 1;
      }
    }

    if (ringLevel == 1) {
      if (runRings2Pos < RGB_RINGS_NUMPIXELS_16) {
        rings.setPixelColor(runRings2Pos, rnd_color);
        runRings2Pos++;
      } else {
        runRings2Pos = RGB_RINGS_NUMPIXELS_8;

        rnd_color = getRandomRingsRGB();

        ringLevel = 2;
      }
    }

    if (ringLevel == 2) {
      if (runRings2Pos < RGB_RINGS_NUMPIXELS_12) {
        rings.setPixelColor(runRings2Pos, rnd_color);
        runRings2Pos++;
      } else {
        runRings2Pos = 1;

        rnd_color = getRandomRingsRGB();

        ringLevel = 3;
      }
    }

    if (ringLevel == 3) {
      if (runRings2Pos < RGB_RINGS_NUMPIXELS_8) {
        rings.setPixelColor(runRings2Pos, rnd_color);
        runRings2Pos++;
      } else {
        runRings2Pos = 0;

        rnd_color = getRandomRingsRGB();

        ringLevel = 4;
      }
    }

    if (ringLevel == 4) {
      rings.setPixelColor(0, rnd_color);

      rnd_color = getRandomRingsRGB();

      runRings2Pos = RGB_RINGS_NUMPIXELS_16;

      ringLevel = 0;
    }

    ringDelay = 0;
  } else {
    ringDelay++;
  }
}
