#include <XBee.h>

int statusLed = 13;

const int channels = 8;
uint16_t channelVals[channels];

char buff[100];
int buffInd = 0;
char recv;
bool received = false;

/*
   --- XBee ---
*/


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
    Serial.println(channelVals[i]);
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

  flashLed(statusLed, 3, 50);
  Serial.println("Remote Starting...");
}

void loop() {
  while (Serial1.available() > 0) {
    char recv = Serial1.read();
    if (recv == '[') {
      while (recv != ']') {
        buff[buffInd] = recv;
        recv = Serial1.read();
        buffInd++;
      }
      buff[buffInd] = recv;
      //buff[buffInd + 1] = '\0';
      
      received = true;
      Serial.println(buff);
      readChannelPayload(channels, buff, channelVals);
      printChannelVals(channels, channelVals);
      
      buffInd = 0;
      received = false;
    }
    else {
      Serial.print("Not at start: ");
      Serial.println(recv);
    }
  }
  //Serial.println("No data available");
  delay(50);
}
