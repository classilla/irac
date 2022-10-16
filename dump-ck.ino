/* dump.ino Example sketch for IRLib2
   Illustrates how to receive an IR signal, decode it and print
   information about it to the serial monitor.
*/
//This includes everything. Not generally recommended.
//It's better to include only the parts of library you really need.
//But for this example it's quick and easy. See "comboDump" example
//for a more efficient way.
#include <IRLibAll.h>

IRrecvPCI myReceiver(2); //create receiver and pass pin number
IRdecode myDecoder;   //create decoder
uint16_t bytepos, window, sample, flength;
uint8_t mode;
#define BYTES_PER_PACKET 64

void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
}

void loop() {
  //Continue looping until you get a complete signal received
  if (myReceiver.getResults()) {
    // Dump raw first
    Serial.println(F(""));
    if(recvGlobal.recvLength > RECV_BUF_LENGTH) {
      Serial.println(F("ERROR: Length of transmission exceeds sample buffer"));
      Serial.println(recvGlobal.recvLength, DEC);
      return;
    }
    if(recvGlobal.recvLength < 2) return; // ????
    // The first sample is the gap, but we don't give that to the sender.
    flength = (recvGlobal.recvLength - 1);
    bytepos = 0;
    mode = 0; // emit continuations
    for(;;) {
      window = flength - bytepos;
      if (window > BYTES_PER_PACKET) window = BYTES_PER_PACKET; else mode = 1;
      window += bytepos;

      if (mode) Serial.print(F("S,")); else Serial.print(F("P,"));
      // change these if recvLength is a uint16_t
      if (bytepos < 16) Serial.print(F("0"));
      Serial.print(bytepos, HEX);
      if (window < 16) Serial.print(F("0"));
      Serial.print(window, HEX);

      for(bufIndex_t i=bytepos;i<window;i++) {
        sample = recvGlobal.recvBuffer[i+1]; // as if we started at 1

        if (sample < 4096) Serial.print(F("0"));
        if (sample < 256) Serial.print(F("0"));
        if (sample < 16) Serial.print(F("0"));
        Serial.print(sample, HEX);
      }
      Serial.println(F(""));
      if (mode) break;

      bytepos = window;
    }
    Serial.println(F(""));

    // Dump decode
    myDecoder.decode();           //Decode it
    myDecoder.dumpResults(true);  //Now print results. Use false for less detail
    myReceiver.enableIRIn();      //Restart receiver
  }
}
