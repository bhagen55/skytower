#include <PulsePosition.h>

int ppmOutPin = 20;


// PPM
PulsePositionOutput ppmOut;

void setup() {
  // put your setup code here, to run once:
  ppmOut.begin(ppmOutPin);

}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 8; i++) {
    ppmOut.write(i + 1, 1650);
  }
}
