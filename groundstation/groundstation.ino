#include <XBee.h>
#include <PulsePosition.h>
#include <elapsedMillis.h>

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

#define rxTimeout 3000

// Receiving
char buff[100];
char telemRx[100];


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


  for (int chan = 0; chan < channels; chan++) {
    buff[pIndex] = channelArr[chan] >> 8 & 0xff;
    pIndex++;
    buff[pIndex] = channelArr[chan] & 0xff;
    pIndex++;
  }

  buff[pIndex] = ']';

  buff[pIndex + 1] = '\0';

}

void sendChannelPayload() {
  getChannelVals(ppmChannels, channelVals);
  loadChannelBuff(ppmChannels, channelVals, channelBuff);

  // Send to XBee
  Serial1.write(channelBuff);

  // Send to serial
  Serial.write(channelBuff);
  Serial.write('\n');
}

void readTelemPayload(char buff[], char telemRx[]) {
  elapsedMillis timeElapsed;
  if (Serial1.available() == 0) { // Wait to receive
    Serial.println("Waiting to receive from Remote");
    while (Serial1.available() == 0 && timeElapsed < rxTimeout) {
      delay(50); // wait to receive data, or to timeout
      //Serial.println("Waiting to receive from Remote1");
    }
    if (timeElapsed >= rxTimeout) {
      Serial.println("Timed out receiving from Remote");
    }
  }
  
  int buffInd = 0;

  while (Serial1.available() > 0) {
    //Serial.println("Received data");
    delay(10);
    char recv = Serial1.read();
    if (recv == '[') {
      recv = Serial1.read();
      while (recv != ']') {
        if (recv == '[') {
          Serial.println("Error: Two start chars");
        }
        else {
          buff[buffInd] = recv;
          recv = Serial1.read();
          buffInd++;
        }
      }
      buff[buffInd] = '\0';

      Serial.println(buff);
    }
    else {
      Serial.print("Not at start: ");
      Serial.println(recv);
    }
  }
}

void setup() {
  // USB Serial
  Serial.begin(9600);
  Serial.println("GS Starting...");

  Serial.println("Setting output pin");
  pinMode(statusLed, OUTPUT);

  // PPM
  Serial.println("Starting PPM");
  ppmIn.begin(ppmPin);

  // XBee Serial
  Serial.println("Starting XBee Serial");
  Serial1.begin(9600);

  flashLed(statusLed, 3, 50);

}

void loop() {

  /* Send Data */
  sendChannelPayload();


  /* Receive Data */
  readTelemPayload(buff, telemRx);

  delay(10);
}
