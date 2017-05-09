#include "Arduino.h"
#include "clock.h"
#include <DS1302.h>
#include <SD.h>

// Часы DS1302
#define CLOCK_CLK A2
#define CLOCK_DAT A3
#define CLOCK_RST A4

// SD
#define SD_CS   4

cclass::cclass() {

}

void cclass::SETUP() {
  matrix.setIntensity(0);
  matrix.setRotation(3);

  time_blink = false;
  time_min = -1;
  time_sec = -1;

  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void cclass::PRINT_TIME() {
  DS1302 rtc(CLOCK_RST, CLOCK_DAT, CLOCK_CLK);

  Time t = rtc.time();

  if (t.min != time_min) {
    time_min = t.min;

    char bufh[3];
    snprintf(bufh, sizeof(bufh), "%02d", t.hr);

    char bufm[3];
    snprintf(bufm, sizeof(bufm), "%02d", t.min);

    getNumbers(int(bufh[0] - '0'), int(bufh[1] - '0'), int(bufm[0] - '0'), int(bufm[1] - '0'));
  }

  if (t.sec != time_sec) {
    time_sec = t.sec;

    if (time_blink) {
      matrix.drawPixel(15, 2, HIGH);
      matrix.drawPixel(15, 3, HIGH);
      matrix.drawPixel(15, 5, HIGH);
      matrix.drawPixel(15, 6, HIGH);

      time_blink = false;
    } else {
      matrix.drawPixel(15, 2, LOW);
      matrix.drawPixel(15, 3, LOW);
      matrix.drawPixel(15, 5, LOW);
      matrix.drawPixel(15, 6, LOW);

      time_blink = true;
    }
  }

  matrix.write();
}

void cclass::getNumbers(int h1, int h2, int m1, int m2) {
  File myFile = SD.open("clock.txt", FILE_READ);
  if (myFile) {
    int chPos = 0;

    char numbh1[65];
    char numbh2[65];
    char numbm1[65];
    char numbm2[65];

    int cnt = 0;
    while (myFile.available()) {
      char ch = myFile.read();

      if (chPos == 65) {
        if (h1 == cnt) {
          showH(numbh1, 0);
        } else {
          memset(numbh1, 0, sizeof(numbh1));
        }

        if (h2 == cnt) {
          showH(numbh2, 8);
        } else {
          memset(numbh2, 0, sizeof(numbh2));
        }

        if (m1 == cnt) {
          showH(numbm1, 16);
        } else {
          memset(numbm1, 0, sizeof(numbm1));
        }

        if (m2 == cnt) {
          showH(numbm2, 24);
        } else {
          memset(numbm2, 0, sizeof(numbm2));
        }

        chPos = 0;
        cnt++;
      } else if (chPos < 65) {
        numbh1[chPos] = ch;
        numbh2[chPos] = ch;
        numbm1[chPos] = ch;
        numbm2[chPos] = ch;

        chPos++;
      }
    }

    myFile.close();
  } else {
    Serial.println("error opening clock.txt");
  }
}

void cclass::showH(char h1[65], int pos) {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      int val = int(h1[(x * 8) + y] - '0');

      switch (val) {
        case 1:
          matrix.drawPixel(y + pos, x, HIGH);
          break;
        default:
          matrix.drawPixel(y + pos, x, LOW);
          break;
      }
    }
  }
}

cclass clockMatrix = cclass();


