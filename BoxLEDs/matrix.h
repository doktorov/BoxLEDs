#ifndef matrix_h
#define matrix_h

#include <Arduino.h>
#include <SPI.h>
#include "SdFat.h"

class mclass {
  private:
    SdFat SD;

  public:
    mclass();

    void SETUP();
    char* GET(int pos, String fileName);
};

extern mclass matrixFromFile;

#endif
