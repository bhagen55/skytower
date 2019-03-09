#include <XBee.h>
#include <PulsePosition.h>

int statusLed = 13;

const int channels = 8;
uint16_t channelVals[channels];

char buff[100];
int buffInd = 0;
char recv;
bool received = false;


// Pins
int ppmOutPin = 20;

// PPM
PulsePositionOutput ppmOut;

/*
   Flash error led
*/
void flashLed(int pin, int times, int wait) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void readChannelPayload(int channels, char buff[], uint16_t channelVals[]) {
  for (int i = 0; i < channels; i++) {
    channelVals[i] = buff[(i)*2] | buff[(i)*2+1] << 8;
  }
}

void setChannelValues(int channels, uint16_t channelVals[], PulsePositionOutput ppmOut) {
  for (int i = 0; i < channels; i++) {
    ppmOut.write(i, channelVals[i]);
    channelVals[i] = buff[(i) * 2] | buff[(i) * 2 + 1] << 8;
  }
}

void printChannelVals(int channels, uint16_t channelVals[]) {
  for (int i = 0; i < channels; i++) {
    Serial.print(channelVals[i]);
    Serial.print(",");
  }
  Serial.println();
}



void setup() {
  pinMode(statusLed, OUTPUT);

  // start serial
  Serial.begin(9600);

  // start XBee serial
  Serial1.begin(9600);

  // PPM
  Serial.println("Starting PPM Output");
  ppmOut.begin(ppmOutPin);

  flashLed(statusLed, 3, 50);
  Serial.println("Remote Starting...");
}

void loop() {

  /* Receive Data */
  if (Serial1.available() == 0) {
    Serial.println("Waiting to receive");
    while (Serial1.available() == 0) {
      delay(50); // Wait for data to be available
    }
  }
  while (Serial1.available() > 0) {
    delay(100);
    char recv = Serial1.read();
    if (recv == '[') {
      while (recv != ']') {
        buff[buffInd] = recv;
        recv = Serial1.read();
        buffInd++;
      }
      buff[buffInd] = recv;

      readChannelPayload(channels, buff, channelVals);
      setChannelValues(channels, channelVals, ppmOut);
      printChannelVals(channels, channelVals);

      buffInd = 0;
    }
    else {
      Serial.print("No start character: ");
      Serial.println(recv);
    }
  }

  /* Send Data */
  Serial1.write("[test]");
  delay(40);
}
