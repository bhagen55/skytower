/*
Based off example: https://github.com/andrewrapp/xbee-arduino/blob/master/examples/Series2_Tx/Series2_Tx.pde
*/

#include <XBee.h>

XBee xbee = XBee();

uint8_t payload[] = { 0, 0 };

// Remote address
XBeeAddress64 addr64 = XBeeAddress64(0x13A200, 0x415B1139);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int pin5 = 0;

int statusLed = 13;
int errorLed = 13;

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
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);

  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() {
  pin5 = 2;
  payload[0] = pin5 >> 8 & 0xff;
  payload[1] = pin5 & 0xff;

  xbee.send(zbTx);

  // Flash TX indicator
  flashLed(statusLed, 1, 100);

  // Wait for status response
  if (xbee.readPacket(500)) {

    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // Get delivery status, 5th bit
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        flashLed(statusLed, 5, 50);
      } else {
        flashLed(errorLed, 3, 500);
      }
    }
  } else if (xbee.getResponse().isError()) {
    // got an error
  } else {
    // Local XBee timed out
    flashLed(errorLed, 2, 50);
  }
  delay(1000);
}
