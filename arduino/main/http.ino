
char get_chttps_state(){

    char value[3] = "-";
    sim_board_SS.print("AT+CHTTPSSTATE\r");
    if (wait_value_response("+CHTTPSSTATE: ", 14, value)){
        Serial.print("get value:");
        Serial.println(value[0]);

        wait_response("OK",2);
        return value[0];
    }
    else{
        //Error
        return '-';
    }
}

bool start_net(){

    char state = get_chttps_state();
    Serial.print("State is :");
    Serial.println(state);
    //check if net is open and only if it's not, open it
    if (state == '0'){ //expected state is 0 - net closed

        //open net connection if not opened yet
        sim_board_SS.print("AT+CHTTPSSTART\r");
        Serial.println("AT+CHTTPSSTART\r");
        if (!wait_response("OK", 2)){
            Serial.println("STOPPED chttps start");
            return false;
        }
    }
    else if (state == '4'){ //net opened
        return true;
    }
    else{
        return false;
    }

    return true;
}

bool start_session(char *hostname, int hostnameLen, unsigned int port, int http){

    if (get_chttps_state() == '4'){
        sim_board_SS.print("AT+CHTTPSOPSE=\"");
        Serial.print("AT+CHTTPSOPSE=\"");
        for (int i=0;i<hostnameLen;++i){
            sim_board_SS.print(hostname[i]);
            Serial.print(hostname[i]);
        }
        sim_board_SS.print("\"," + String(port) + "," + String(http) + "\r\n");
        Serial.print("\"," + String(port) + "," + String(http) + "\r\n");
        if (!wait_response("OK", 2)){
            Serial.println("STOPPED 2");
            return false;
        }
    }

    if (get_chttps_state() != '7'){
        Serial.println("state NOT 7 but "+ get_chttps_state());
        return false;
    }

    return true;
}

bool send_request(char *request, int requestLen){

    if (get_chttps_state() == '7'){
        sim_board_SS.print("AT+CHTTPSSEND=" + String(requestLen+4) + "\r" );
        Serial.println("AT+CHTTPSSEND=" + String(requestLen+4) + "\r" );
        if (!wait_response(">", 1)){
            Serial.println("STOPPED 2.5");
            return;
        }

        for (int i=0;i<requestLen;++i){
            sim_board_SS.print(request[i]);
            Serial.print(request[i]);
        }
        sim_board_SS.print("\r\n\r\n");
        Serial.print("\r\n\r\n");
        if (!(
                wait_response("OK", 2) &&
                wait_response("+CHTTPS: RECV EVENT", 19) &&
                wait_response("+CHTTPSNOTIFY: PEER CLOSED", 26)
            )){
            Serial.println("STOPPED 3");
            return false;
        }
    }
    else{
        Serial.println("couldn't send - state not 7");
        return false;
    }
    return true;
}

int get_response_len(){
    int value_size=5;
    char value[value_size] = "---";

    sim_board_SS.print("AT+CHTTPSRECV?\r");
    if (wait_value_response("+CHTTPSRECV: LEN,", 17, value)){
        int response_len = atoi(value);

        wait_response("OK",2);
        return response_len;
    }
    else{
        //Error
        return -1;
    }
}

int send_http(char *hostname, int hostnameLen, unsigned int port, int http, char *request, int requestLen){

    if (!start_net()){
        Serial.println("net fail");
        return;
    }
    Serial.println("net done");

    if (!start_session(hostname, hostnameLen, port, http)){
        Serial.println("session fail");
        return;
    }
    Serial.println("session done");

    if (!send_request(request, requestLen)){
        Serial.println("request fail");
        return;
    }
    Serial.println("request done");

    int len = get_response_len();
    Serial.print("Received response len is ");
    Serial.println(len);

    char data[ len ]; // Just be careful and keep received resp len low
                      // as the whole thing is saved in memeory at once...
    read_response(data, len);

    int start = get_resp_cont_start_index(data, len);
    if ( start < 0 ){
        Serial.println("Could not find content separator");
        start = 0;
        // return;
    }
    for (int i=start;i<len;++i){
        Serial.print(data[i]);
    }


//   sim_board_SS.print("AT+CHTTPSSTOP\r");
    Serial.println("<done>");
    return 0;
}

/*
 * AT+CHTTPSSTART
 * AT+CHTTPSOPSE="78.60.181.9",51000,1 // 1 - http, 2 - https
 * AT+CHTTPSSEND=<length>
 * > <request> // each line length + 2 (for \r\n) + 2 empty lines
 * AT+CHTTPSRECV? // gives length of http response received
 * AT+CHTTPSRECV=<max_recv_buff_len>
 * AT+CHTTPSSTATE // 0 None; 1 Accr'd HTTPS; 2 Opening net; 3 Closing net; 4 Opened net; 5 closing sess; 6 opening sess; 7 opened sess;
 * 
 * 
 * AT+CHTTPSCLSE
 * AT+CHTTPSSTOP
 */