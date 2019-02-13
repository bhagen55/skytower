#include <PulsePosition.h>

// Pins
const byte ppmPin = 20;

// PPM
const int channels = 8;
PulsePositionInput ppmIn;

int channelVals[channels];
uint8_t payload[channels * 2];

int newVals[channels];

/*
Load channel vals into payload
*/
void loadPayload(int channels, int channelArr[], uint8_t payload[]) {
  int pIndex = 0;

  for (int chan = 0; chan <= channels; chan++) {
    payload[pIndex] = channelArr[chan] >> 8 & 0xff;
    pIndex++;
    payload[pIndex] = channelArr[chan] & 0xff;
    pIndex++;
  }
}

void setup() {
  // PPM
  ppmIn.begin(ppmPin);

  Serial.begin(9600);

}

void loop() {
  // Print channel values
  Serial.print("Raw Values:\n");
  for (int channel = 1; channel <= channels; ++channel) {
    int value = ppmIn.read(channel);
    channelVals[channel - 1] = value;
  }

  for (int i=0; i < channels; i++) {
    Serial.print(String(channelVals[i]) + " ");
  }
  Serial.println();


  loadPayload(channels, channelVals, payload);
  Serial.print("Split Values:\n");
  for (int i=0; i < channels * 2; i++) {
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("New Vals:\n");
  int pI = 0;
  for (int i=0; i < channels; i++) {
    uint8_t high = payload[pI];
    pI++;
    uint8_t low = payload[pI];
    pI++;

    newVals[i] = (high << 8) | low;
    Serial.print(newVals[i]);
    Serial.print(" ");
  }
  Serial.println();

  delay(1000);
}
