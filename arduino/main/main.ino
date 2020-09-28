
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

    if (! adafruit_SS.begin(sim_board_SS)) {
        while (1);
    }

    Serial.println("Setup done!");
}


bool wait_response(char *expectedResponse, int responseLen){
    while ( !sim_board_SS.available() );

    const char first_char = '\r';
    const char second_char = '\n';
    int i=0;

    while (sim_board_SS.available()){
        char c = sim_board_SS.read();
        Serial.print(i);
        Serial.print("-> ");
        Serial.print((int)c);
        Serial.print(" ");
        Serial.println(c);

        if (i == 0 && c == first_char){
            char c = sim_board_SS.read();
            if (c == second_char){
                Serial.println("Init string OK");
                continue; //new message start marked
            }
            else{
                return false; //first_char not followed by second_char
            }
        }
        else if (c == first_char){ //signifies end of message end of message (if not at the front)
            char c = sim_board_SS.read();
            if (c == second_char){ //Second end char separator char matches

                //Check if end of expected response
                if (responseLen == i){//Matched everything but new msg might be on buffer
                    Serial.println("End string OK, response finished");
                    break;
                }
                else{
                    Serial.println("End string OK, responseLen NOT MATCH");
                    return false; //not full expected response
                }

            }
            //It's assumed that first_char && second_char won't appear randomly in any message
            else{
                return false; //first_char not followed by second_char
            }
        }

        Serial.print("Matching: ");
        Serial.print(c);
        Serial.print(" VS ");
        Serial.println(expectedResponse[i]);
        if (c != expectedResponse[i]){

            Serial.println("FAIL. Printing rest of string:");
            while (sim_board_SS.available()){
                char c = sim_board_SS.read();
                Serial.print(c);
            }
            Serial.println();

            return false;
        }

        i++;
    }

    Serial.println("MATCH. Go on");
    return true;
}

bool wait_return_response(char* expectedHeader, int expectedHeaderLen, char headerValueSeparator, char* returnValue){

}

void loop() { // run over and over

    if (sim_board_SS.available()) {
        Serial.write(sim_board_SS.read());
    }
    if (Serial.available()) {
        byte b = Serial.read();
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
            Serial.print(lat);
            Serial.print(" ");
            Serial.println(lon);
        }
        else if (b == '@'){
            send_http("78.60.181.9", 11, 51000, 1, "GET /api HTTP/1.1", 17);
        }
        else{
            sim_board_SS.write(b);
        }
    }
}
