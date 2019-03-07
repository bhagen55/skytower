#include <XBee.h>
#include <PulsePosition.h>

// Pins
const byte ppmPin = 20;
const byte statusLed = 13;

/*
   --- PPM ---
*/
// vars
const int ppmChannels = 8;
uint16_t channelVals[ppmChannels];
//uint8_t payload[ppmChannels * 2];

// ppm reader
PulsePositionInput ppmIn;

// Serial sender
char channelBuff[(ppmChannels * 2) + 3]; 


/*
  Get the values of ppm channels and store it in an array
  TODO: use .available() to find available channels and dynamically allocate array.
      Use first array spot to hold number of channels being sent.
*/
void getChannelVals(int channels, uint16_t channelArr[]) {
  if (ppmIn.available() == ppmChannels) {
    for (int chan = 1; chan <= channels; chan++) {
      channelArr[chan - 1] = ppmIn.read(chan);
      //channelArr[chan] = 1500;
    }
  } else {
    Serial.println("No PPM or wrong number of PPM channels");
  }
}


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

void loadChannelBuff(int channels, uint16_t channelArr[], char buff[]) {
  int pIndex = 0;

  buff[pIndex] = '[';

  pIndex++;
  

  for (int chan = 0; chan <= channels; chan++) {
    buff[pIndex] = channelArr[chan] >> 8 & 0xff;
    pIndex++;
    buff[pIndex] = channelArr[chan] & 0xff;
    pIndex++;
  }

  buff[pIndex] = ']';

  buff[pIndex+1] = '\0';

}

void setup() {
  pinMode(statusLed, OUTPUT);
  
  // PPM
  ppmIn.begin(ppmPin);

  // USB Serial
  Serial.begin(9600);

  // XBee Serial
  Serial1.begin(9600);

  flashLed(statusLed, 3, 50);
  Serial.println("GS Starting...");
}

void loop() {
  //Serial.println("Sending");
  getChannelVals(ppmChannels, channelVals);
  loadChannelBuff(ppmChannels, channelVals, channelBuff);

  Serial1.write(channelBuff);
  Serial.write(channelBuff);
  Serial.print("hello");

  delay(200);
}
