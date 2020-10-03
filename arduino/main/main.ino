
#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>

#define SIM_BOARD_RX 10
#define SIM_BOARD_TX 11
#define SIM_BOARD_RST 12

SoftwareSerial sim_board_SS(SIM_BOARD_TX, SIM_BOARD_RX);
Adafruit_FONA_3G adafruit_SS = Adafruit_FONA_3G(SIM_BOARD_RST);

void setup() {

    // Open serial communications and wait for port to open:
    Serial.begin(4800);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    pinMode(LED_BUILTIN,OUTPUT);
    // set the data rate for the SoftwareSerial port
    sim_board_SS.begin(4800);

    if (! adafruit_SS.begin(sim_board_SS)) { //annoying but required at the start
        while (1);                  //otherwise weird AT chars pop up at the start
    }

    Serial.println(F("Setup done!"));
}



void loop() { // run over and over

    if (sim_board_SS.available()) {
        Serial.write(sim_board_SS.read());
    }
    if (Serial.available()) {
        char b = Serial.read();
        if (b == '*'){
            sim_board_SS.write(0x1a); // to finish writing sms text
        }
        else if (b == '~'){
            write_sms("+37061002203", 12, "HI", 2);
        }
        else if (b == '!'){
            float lat, lon;
            get_gps(&lat, &lon);
            Serial.print("GPS:");
            Serial.print(lat,6);
            Serial.print(" ");
            Serial.println(lon,6);
        }
        else if (b == '@'){
            send_get_request(PSTR("78.60.181.9"), 51000, 1, PSTR("/api"));
        }
        else if (b == '#'){
            send_post_request(PSTR("78.60.181.9"), 51000, 1, PSTR("/api"), PSTR("{ \"lat\":\"X\", \"lon\":\"Y\"}"));
        }
        else if (b == '$'){
            char time_string[30] = "";
            if (get_time(time_string)){
                Serial.println(time_string);
            }
            else{
                Serial.println("Could not get time...");
            }

        }
        else{
            sim_board_SS.write(b);
        }
    }

    delay(1);
}
