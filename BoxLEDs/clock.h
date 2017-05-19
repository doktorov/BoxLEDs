#ifndef clock_h
#define clock_h

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "SdFat.h"

// CLK - 13, CS - 10, DN - 11
#define PANEL884_CS   10
#define PANEL884_NHD  1
#define PANEL884_NVD  4

class cclass {
  private:
    Max72xxPanel matrix = Max72xxPanel(PANEL884_CS, PANEL884_NHD, PANEL884_NVD);
    SdFat SD;

    boolean time_blink;
    int time_min;
    int time_sec;

    void getNumbers(int h1, int h2, int m1, int m2);
    void showH(char h1[65], int pos);
  public:
    cclass();

    void SETUP(int sd);
    void PRINT_TIME();
};

extern cclass clockMatrix;

#endif
