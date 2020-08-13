#include <Adafruit_FONA.h>

#include <SoftwareSerial.h>

SoftwareSerial fonaSS(8,9); // RX, TX

void WriteSMS () {
  fonaSS.print("AT+CMGF=1\r");
  delay(1000);
  fonaSS.print("AT+CMGS=\"+37061002203\"\r");
  delay(1000);
  fonaSS.println("Arduino veikia ;)");
  fonaSS.print("\r");
  delay(1000);
  fonaSS.println((char)26);
  fonaSS.println();
  delay(10000);
}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(4800);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(LED_BUILTIN,OUTPUT);
  // set the data rate for the SoftwareSerial port
  fonaSS.begin(4800);
  
  Serial.println("Setup done!");
}

void loop() { // run over and over

  if (fonaSS.available()) {
    Serial.write(fonaSS.read());
  }
  if (Serial.available()) {
    byte b = Serial.read();
    if (b == '*'){
      fonaSS.write(0x1a); // to finish writing sms text
    }
    else if (b == '~'){
      WriteSMS();
    }
    else{
      fonaSS.write(b);
    }
  }
}
