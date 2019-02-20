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
uint16_t channelVals[ppmChannels];
//uint8_t payload[ppmChannels * 2];

// ppm reader
PulsePositionInput ppmIn;

/*
 * --- XBee ---
 */
// vars
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x415463DE);
ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *)channelVals, sizeof(channelVals));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

// XBee obj
XBee xbee = XBee();


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
  Serial.print("Payload size: ");
  Serial.println(sizeof(channelVals));
  

  flashLed(statusLed, 1, 50);
  xbee.send(zbTx);

  if (xbee.readPacket(1000)) {
    Serial.println("Response Received");
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

  delay(50);
}
