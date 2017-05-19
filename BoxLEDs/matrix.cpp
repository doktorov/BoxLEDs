#include "Arduino.h"
#include "matrix.h"

#include "SdFat.h"

mclass::mclass() {

}

void mclass::SETUP(int sd) {
  if (!SD.begin(sd)) {
    return;
  }
}

char* mclass::GET(int pos, String fileName) {
  static char r[65];

  File myFile = SD.open(fileName, FILE_READ);

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
  
  return r;
}

mclass matrixFromFile = mclass();

