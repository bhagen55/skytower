#include <XBee.h>
#include <PPMReader.h>

// Pins
const byte ppmPin = 3;

/*
--- PPM ---
*/
// vars
const int ppmChannels = 8;
unsigned long channelVals[ppmChannels];
uint8_t payload[ppmChannels * 2];

// ppm reader
PPMReader ppm(ppmPin, ppmChannels);

/*
--- XBee ---
*/
// vars
XBeeAddress64 addr64 = XBeeAddress64(0x13A200, 0x415B1139);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

// XBee obj
XBee xbee = XBee();


/*
Get the values of ppm channels and store it in an array
*/
void getChannelVals(int channels, unsigned long channelArr[]) {
  for (int chan = 0; chan <= channels; chan++) {
    channelArr[chan] = ppm.latestValidChannelValue(chan, 0);
  }
}

/*
Load channel vals into payload
*/
void loadPayload(int channels, unsigned long channelArr[], uint8_t payload[]) {
  int pIndex = 0;

  for (int chan = 0; chan <= channels; chan++) {
    payload[pIndex] = channelArr[chan] >> 8 & 0xff;
    pIndex++;
    payload[pIndex] = channelArr[chan] & 0xff;
    pIndex++;
  }
}

void setup() {
  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() {
  getChannelVals(ppmChannels, channelVals);
  loadPayload(ppmChannels, channelVals, payload);

  xbee.send(zbTx);

  delay(1000);
}
