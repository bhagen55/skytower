#include <XBee.h>

/*
 * --- XBee ---
 */
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

int statusLed = 13;

const int channels = 8;
uint16_t channelVals[channels];


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
  pinMode(statusLed, OUTPUT);
  
  // start serial
  Serial.begin(9600);

  // start XBee serial
  Serial1.begin(9600);
  xbee.begin(Serial1);
  
  flashLed(statusLed, 3, 50);
  Serial.println("Starting...");
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    flashLed(statusLed, 1, 50);
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      Serial.println("Got packet");
      // got something
      
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        Serial.println("Got a receipt packet");
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            Serial.println("Sender received our ack");
            // the sender got an ACK
            flashLed(statusLed, 10, 10);
        } else {
            Serial.println("Sender did not receive our ack");
            Serial.print("getOption: ");
            Serial.println(rx.getOption(), HEX);
            // we got it (obviously) but sender didn't get an ACK
            flashLed(statusLed, 2, 20);
        }

        // Process Channel Data
        Serial.print("Data length: ");
        Serial.println(rx.getDataLength());

        if (rx.getDataLength() == sizeof(channelVals)) {
          for (int i = 0; i < channels; i++) {
            channelVals[i] = rx.getData(i*2) | rx.getData(i*2+1) << 8;
            Serial.println(channelVals[i]);
          }
        } else { // received data length doesn't match up with channel
          Serial.println("Data received not of correct size");
          Serial.print("Was of size ");
          Serial.println(rx.getDataLength());
        }
        
      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
        
        if (msr.getStatus() == ASSOCIATED) {
          Serial.println("Associated with other XBee");
          // yay this is great.  flash led
          flashLed(statusLed, 10, 10);
        } else if (msr.getStatus() == DISASSOCIATED) {
          Serial.println("Disassociated with other XBee");
          // this is awful.. flash led to show our discontent
          flashLed(statusLed, 10, 10);
        } else {
          // another status
          flashLed(statusLed, 5, 10);
        }
      } else {
        // not something we were expecting
        Serial.println("Something unexpected happened");
        flashLed(statusLed, 1, 25);    
      }
    } else if (xbee.getResponse().isError()) {
      Serial.print("Error reading packet.  Error code: ");  
      Serial.println(xbee.getResponse().getErrorCode());
    }
}
