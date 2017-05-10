#include "Arduino.h"
#include "equalizer.h"

eclass::eclass() {

}

void eclass::SETUP() {
  pixels.setBrightness(RGB_BRIGHTNESS);
  pixels.begin();
  pixels.show();

  sample = 0;
  soundWhile = 0;
}

void eclass::GO() {
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

void eclass::setSoundLevel(int pos, int level) {
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

eclass equalizerShow = eclass();
