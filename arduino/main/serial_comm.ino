
int msg_separator_symbol(char c){
    const char first_char = '\r';
    const char second_char = '\n';

    if (c == first_char){
        delay(1);
        char c = sim_board_SS.read();
        // Serial.print("-> ");
        // Serial.print((int)c);
        // Serial.print(" ");
        // Serial.println(c);
        if (c == second_char){
            return 0; // control symbol found
        }
        else{
            return 1; // first_char NOT followed by second_char
        }
    }
    return -1; //not control symbol
}


bool wait_response(const char *expectedResponse, int responseLen){
    while ( !sim_board_SS.available() );

    int i=0;
    while (sim_board_SS.available()){
        delay(1);
        char c = sim_board_SS.read();
        // Serial.print(i);
        // Serial.print("-> ");
        // Serial.print((int)c);
        // Serial.print(" ");
        // Serial.println(c);

        int msg_sep_status = msg_separator_symbol(c);
        if (msg_sep_status > 0){
            return false;
        }
        else if (msg_sep_status == 0){
            if (i == 0){ //start of message
                // Serial.println("Init string PASS");
                continue; //new message start marked
            }
            else{ //end of message
                //Check if end of expected response
                if (responseLen == i){//Matched everything but new msg might be on buffer
                    // Serial.println("End string PASS, response finished");
                    break;
                }
                else{
                    Serial.println(F("End string PASS, responseLen NOT MATCH"));
                    return false; //not full expected response
                }
            }
        }
        //msg_sep_status < 0: continue as normal
        // Serial.print("Matching: ");
        // Serial.print(c);
        // Serial.print(" VS ");
        // Serial.println(expectedResponse[i]);
        if (c != expectedResponse[i]){

            Serial.println(F("FAIL. Printing rest of string:"));
            while (sim_board_SS.available()){
                delay(1);
                char c = sim_board_SS.read();
                Serial.print(c);
            }
            Serial.println();

            return false;
        }

        i++;
    }

    // Serial.println("MATCH. Go on");
    return true;
}

bool wait_value_response(const char* expectedHeader, int expectedHeaderLen, char *returnValue){
    while ( !sim_board_SS.available() );

    int i=0;
    while (sim_board_SS.available()){
        // Serial.println("In while");
        delay(1);
        char c = sim_board_SS.read();
        // Serial.print(i);
        // Serial.print("-> ");
        // Serial.print((int)c);
        // Serial.print(" ");
        // Serial.println(c);

        int msg_sep_status = msg_separator_symbol(c);
        if (msg_sep_status > 0){
            return false;
        }
        else if (msg_sep_status == 0){
            if (i == 0){ //start of message
                // Serial.println("Init string PASS");
                continue; //new message start marked
            }
            else{ //end of message
                // Serial.println("End string PASS");
                break;
            }
        }
        //msg_sep_status < 0: continue as normal
        if (i > expectedHeaderLen-1){
            // Serial.print(" ->>>> c is:");
            // Serial.print(c);
            // Serial.print("; ");
            // Serial.println(i - expectedHeaderLen);
            returnValue[ i - expectedHeaderLen ] = c;
        }
        else if (c != expectedHeader[i]){
            Serial.println(F("FAIL. Not expected header."));
            return false;
        }

        i++;
    }

    // Serial.print("Wait value is:");
    // Serial.println(returnValue);
    return true;
}

void read_response(char *data, int response_size){
    // const static byte = 32;

    sim_board_SS.print(F("AT+CHTTPSRECV="));
    sim_board_SS.print(response_size);
    sim_board_SS.print(F("\r"));

    Serial.print(F("AT+CHTTPSRECV="));
    Serial.print(response_size);
    Serial.println(F("\r"));

    if (wait_response("OK", 2)){
        char vali[5] = "---";//actually not used but just there for the param
        wait_value_response("+CHTTPSRECV: DATA,",18,vali);

        Serial.print("In buffer:");
        Serial.println(sim_board_SS.available());
        Serial.println("---------------");
        int i = 0;
        byte c;
        Serial.println("before while");
        while (i < response_size ){
            while ( !sim_board_SS.available() ){
                Serial.write(0x40);
            }
            Serial.print("available:");
            Serial.println(sim_board_SS.available());
            c = sim_board_SS.read();
            // Serial.print(i);
            // Serial.print(" =");
            // Serial.write(c);
            // Serial.println("|");

            data[i] = char(c);

OK

+CHTTPSRECV: DATA,10
ontent-Len
+CHTTPSRECV: 0

+CHTTPS: RECV EVENT


            ++i;
        }
        Serial.println("---------------");
        Serial.println(i);
        data[i]='\0';
    }
}

int get_resp_cont_start_index(char *response, int resp_len){
    char first_sep='\r';
    char second_sep = '\n';

    for (int i=0;i<resp_len-4;++i){

        if (
            response[i] == first_sep &&
            response[i+1] == second_sep &&
            response[i+2] == first_sep &&
            response[i+3] == second_sep
         ){
             return i+4;
         }
    }
    return -1;

}