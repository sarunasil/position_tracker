
int msg_separator_symbol(char c){
    const static char first_char = '\r';
    const static char second_char = '\n';

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


bool wait_response(const char *expectedResponse){
    // Serial.print("Wainting for ");
    // Serial.println(expectedResponse);
    byte responseLen = strlen(expectedResponse);
    while ( !sim_board_SS.available() );
    // Serial.print("Buffer queue: ");
    // Serial.println(sim_board_SS.available());

    int i=0;
    while (sim_board_SS.available()){
        delay(1);
        // Serial.print("\navailable:");
        // Serial.print(sim_board_SS.available());
        char c = sim_board_SS.read();
        // Serial.print(i);
        // Serial.print("-> ");
        // Serial.print((int)c);
        // Serial.print(" ");
        // Serial.print(c);

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

bool wait_value_response(const char* expectedHeader, char *returnValue){
    // Serial.print("Wainting for header ");
    // Serial.println(expectedHeader);
    byte expectedHeaderLen = strlen(expectedHeader);
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
        // Serial.print(c);

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

        // Serial.println(i);
        i++;
    }

    returnValue[i - expectedHeaderLen] = '\0';
    // Serial.print("Wait value is:");
    // Serial.println(returnValue);
    return true;
}

bool read_http_response(char *data, int response_size){
    const static byte read_len = 64;

    int i = 0;
    for ( int read_acc=0; read_acc < response_size; read_acc += read_len ){

        sim_board_SS.print(F("AT+CHTTPSRECV="));
        sim_board_SS.print(read_len);
        sim_board_SS.print(F("\r"));

        // Serial.print(F("AT+CHTTPSRECV="));
        // Serial.print(read_len);
        // Serial.println(F("\r"));

        if (wait_response("OK")){
            char vali[5] = "---";//actually not used but just there for the param
            wait_value_response("+CHTTPSRECV: DATA,",vali);

            // Serial.println(F("---------------"));
            static char c;
            while (i < read_acc + atoi(vali)){
                if ( i >= response_size ){ //request too large - truncate the rest
                    data[i] = '\0';
                    while (sim_board_SS.available()){
                        sim_board_SS.read(); //clear anything that's left (if any)
                        delay(5);
                    }
                    sim_board_SS.print(F("AT+CHTTPSRECV="));//get anything that's still
                                                    //not sent
                    sim_board_SS.print(response_size);
                    sim_board_SS.print(F("\r"));
                    wait_response("OK");
                    wait_value_response("+CHTTPSRECV: DATA,",vali);
                    while (sim_board_SS.available()){
                        sim_board_SS.read(); //clear anything that's left (if any)
                        delay(5);
                    }
                    // Serial.println(i);
                    return true; // response too large
                }
                delay(1);
                c = sim_board_SS.read();
                // Serial.print("\navail:");
                // Serial.print(sim_board_SS.available());
                // Serial.print(" ");
                // Serial.print(c);

                data[i] = c;

                ++i;
            }
            // Serial.println(F("---------------"));
            // Serial.println(i);

            if ( !wait_response("+CHTTPSRECV: 0") ){
                Serial.println(F("Failed to wait +chttpsrecv: 0"));
                return false; //critical failure
            }
            if ( i < response_size ){
                if ( !wait_response("+CHTTPS: RECV EVENT") ){
                    Serial.println(F("Failed to wait +chttps: recv event"));
                    return false; //critical failure
                }
            }
        }
        else{
            Serial.println(F("NOT OK.."));
            return false;
        }

    }
    data[i]='\0';
    return true;

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