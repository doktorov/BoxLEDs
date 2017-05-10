#ifndef equalizer_h
#define equalizer_h

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#define RGB_PIN         A0
#define RGB_NUMPIXELS   64
#define RGB_BRIGHTNESS  10

#define MIC_PIN 7

#define EQ_SAMPLE_WINDOW  50

class eclass {
  private:
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(RGB_NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

    unsigned int sample;
    int array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int soundWhile;

    void setSoundLevel(int pos, int level);
  public:
    eclass();

    void SETUP();
    void GO();
};

extern eclass equalizerShow;

#endif
