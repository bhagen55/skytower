#include <XBee.h>
#include <PulsePosition.h>

// Pins
const byte ppmPin = 20;
const byte statusLed = 13;

/*
 * --- PPM ---
 */
// vars
const int ppmChannels = 8;
int channelVals[ppmChannels];
uint8_t payload[ppmChannels * 2];

// ppm reader
PulsePositionInput ppmIn;

/*
 * --- XBee ---
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
void getChannelVals(int channels, int channelArr[]) {
  for (int chan = 0; chan <= channels; chan++) {
    //channelArr[chan] = ppm.latestValidChannelValue(chan, 0);
    channelArr[chan] = 1500;
  }
}

/*
 * Load channel vals into payload
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


/*
 * Flash error led
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

void setup() {
  pinMode(13, OUTPUT);
  // PPM
  ppmIn.begin(ppmPin);

  // USB Serial
  Serial.begin(9600);

  // XBee Serial
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
}

void loop() {
  getChannelVals(ppmChannels, channelVals);
  loadPayload(ppmChannels, channelVals, payload);

  xbee.send(zbTx);

  // Error Checking:
  if (xbee.readPacket(500)) {
    // Response received
    // should be a znet tx status              
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success, packet delivered
        Serial.println("Info: Packet delivered");
        flashLed(statusLed, 5, 50);
      } else {
        // the remote XBee did not receive our packet
        Serial.println("Error: Remote XBee did not receive packet");
        flashLed(statusLed, 3, 500);
      }
    }
  } else if (xbee.getResponse().isError()) {
    // Error received
    Serial.print("Error: Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("Error: Timeout with local XBee");
    flashLed(statusLed, 2, 250);
  }
}
