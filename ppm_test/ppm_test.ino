/*
Uses https://github.com/Nikkilae/PPM-reader
Must be extracted into the Arduino library folder
*/
#include <PPMReader.h>

// Pins
const byte interruptPin = 3;

// PPM
int channels = 8;
PPMReader ppm(interruptPin, channels);

void setup() {
  Serial.begin(9600);

}

void loop() {
  // Print channel values
  for (int channel = 1; channel <= channels; ++channel) {
    int value = ppm.latestValidChannelValue(channel, 0);
    Serial.print(String(value) + " ");
  }
  Serial.println();
}
