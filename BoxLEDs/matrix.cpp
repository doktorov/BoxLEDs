#include "Arduino.h"
#include "matrix.h"

#include "SdFat.h"

// SD
#define SD_CS   35

mclass::mclass() {

}

void mclass::SETUP() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

char* mclass::GET(int pos) {
  static char r[65];

  File myFile = SD.open("HEART.TXT", FILE_READ);

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
        if (cnt == pos) {
          break;
        }

        memset(r, 0, sizeof(r));
        chPos = 0;
        cnt++;
      } else if (chPos < 65) {
        r[chPos] = ch;

        chPos++;
      }
    }

    myFile.close();
  } else {
    Serial.println("error opening clock.txt");
  }

  //  for (int i = 0; i < 65; ++i) {
  //    r[i] = char(i);
  //  }

  return r;
}

mclass matrixFromFile = mclass();

