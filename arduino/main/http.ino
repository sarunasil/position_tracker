
char get_chttps_state(){

    char value[3] = "-";
    sim_board_SS.print(F("AT+CHTTPSSTATE\r"));
    if (wait_value_response("+CHTTPSSTATE: ", value)){
        Serial.print(F("chttps state:"));
        Serial.println(value[0]);

        wait_response("OK");
        return value[0];
    }
    else{
        //Error
        return '-';
    }
}

bool start_net(){

    char state = get_chttps_state();
    //check if net is open and only if it's not, open it
    if (state == '0'){ //expected state is 0 - net closed

        //open net connection if not opened yet
        sim_board_SS.print(F("AT+CHTTPSSTART\r"));
        Serial.println(F("AT+CHTTPSSTART\r"));
        if (!wait_response("OK")){
            Serial.println(F("STOPPED chttps start"));
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

bool start_session(const char *hostname, unsigned int port, int http){

    if (get_chttps_state() == '4'){

        //it's stupid to do it this way
        //but it saves up RAM
        sim_board_SS.print(F("AT+CHTTPSOPSE=\""));
        sim_board_SS.print(hostname);
        sim_board_SS.print(F("\","));
        sim_board_SS.print(port);
        sim_board_SS.print(F(","));
        sim_board_SS.print(http);
        sim_board_SS.print(F("\r\n"));

        Serial.print(F("AT+CHTTPSOPSE=\""));
        Serial.print(hostname);
        Serial.print(F("\","));
        Serial.print(port);
        Serial.print(F(","));
        Serial.print(http);
        Serial.print(F("\r\n"));
        if (!wait_response("OK")){
            Serial.println(F("STOPPED 2"));
            return false;
        }
    }

    if (get_chttps_state() != '7'){
        Serial.print(F("state NOT 7 but "));
        Serial.println(get_chttps_state());
        return false;
    }

    return true;
}

bool send_request(const char *request, int requestLen){

    if (get_chttps_state() == '7'){
        sim_board_SS.print(F("AT+CHTTPSSEND="));
        sim_board_SS.print(requestLen+4);
        sim_board_SS.print(F("\r"));

        Serial.print(F("AT+CHTTPSSEND="));
        Serial.print(requestLen+4);
        Serial.println(F("\r"));

        if (!wait_response(">")){
            Serial.println(F("STOPPED 2.5"));
            return;
        }

        sim_board_SS.print(request);
        sim_board_SS.print(F("\r\n\r\n"));

        Serial.print(request);
        Serial.print(F("\r\n\r\n"));
        if (!(
                wait_response("OK") &&
                wait_response("+CHTTPS: RECV EVENT") &&
                wait_response("+CHTTPSNOTIFY: PEER CLOSED")
            )){
            Serial.println(F("STOPPED 3"));
            return false;
        }
    }
    else{
        Serial.println(F("couldn't send - state not 7"));
        return false;
    }
    return true;
}

int get_response_len(){
    int value_size=5;
    char value[value_size] = "---";

    sim_board_SS.print(F("AT+CHTTPSRECV?\r"));
    Serial.println(F("AT+CHTTPSRECV?\r"));
    if (wait_value_response("+CHTTPSRECV: LEN,", value)){
        int response_len = atoi(value);

        wait_response("OK");
        return response_len;
    }
    else{
        //Error
        return -1;
    }
}

int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

int send_http_request(const char *hostname, unsigned int port, int http, const char *request, int requestLen){

    if (!start_net()){
        Serial.println(F("net fail"));
        return;
    }
    Serial.println(F("net done"));

    if (!start_session(hostname, port, http)){
        Serial.println(F("session fail"));
        return;
    }
    Serial.println(F("session done"));

    if (!send_request(request, requestLen)){
        Serial.println(F("request fail"));
        return;
    }
    Serial.println(F("request done"));

    int len = get_response_len();
    Serial.print(F("Received response len is "));
    Serial.println(len);

    const int max_recv_buff_len = 400;
    int recv_buff_len = len+1; //+1 is for \0 at the end
    if (len > max_recv_buff_len){
        Serial.print("Will have to cut response to ");
        Serial.println(max_recv_buff_len);
        recv_buff_len = max_recv_buff_len;
    }

    char data[ recv_buff_len ]; // Just be careful and keep received resp len low
                    // as the whole thing is saved in memeory at once...
    bool read_res = read_response(data, recv_buff_len-1); // -1 to leave space for '\0'
    if ( !read_res ){
        Serial.println(F("READING INTERRUPTED BY UNEXPECTED ERROR")); //try again?
    }

    int start = get_resp_cont_start_index(data, recv_buff_len);
    // Serial.print("start=");
    // Serial.println(start);
    if ( start < 0 ){
        Serial.println(F("Could not find content separator"));
        start = 0;
        // return;
    }
    for (int i=start;i<recv_buff_len;++i){
        // Serial.print(i);
        // Serial.print(" -> ");
        // Serial.write(data[i]);
        // Serial.println();
        Serial.print(data[i]);
    }

//   sim_board_SS.print("AT+CHTTPSSTOP\r");
    Serial.println(F("<done>"));
    return 0;
}

void send_get_request(const char *hostname, unsigned int port, int http, const char *url){

    char temp[50];
    sprintf(temp, "GET %s HTTP/1.1", url);

    int a = send_http_request(hostname, port, http, temp, strlen(temp));
    Serial.println("done with send_get_request");
}

void send_post_request(const char *hostname, unsigned int port, int http, const char *url, const char *data){

    // const static char req_headers[] = "POST %s HTTP/1.1\r\nHost: %s:%d\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s";
    // char temp[250];
    // sprintf(temp, req_headers, url, hostname, port, strlen(data), data);

    char temp[250];
    sprintf(temp, 
"POST %s HTTP/1.1\r\nHost: %s:%d\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s"
, url, hostname, port, strlen(data), data);

    send_http_request(hostname, port, http, temp, strlen(temp));
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