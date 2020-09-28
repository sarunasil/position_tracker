

void send_http(char *hostname, int hostnameLen, unsigned int port, int http, char *request, int requestLen){

    //check if net is open and only if it's not, open it
    sim_board_SS.print("AT+CHTTPSSTATE\r");
    if (wait_response("OK", 2)){

        if (wait_response("+CHTTPSSTATE: 0", 15)){

            //open net connection if not opened yet
            sim_board_SS.print("AT+CHTTPSSTART\r");
            Serial.println("AT+CHTTPSSTART\r");
            if (!wait_response("OK", 2)){
                Serial.println("STOPPED chttps start");
                return;
            }


        }
    }
    else{
        Serial.println("STOPPED state check");
        return;
    }

    sim_board_SS.print("AT+CHTTPSOPSE=\"");
    Serial.print("AT+CHTTPSOPSE=\"");
    for (int i=0;i<hostnameLen;++i){
        sim_board_SS.print(hostname[i]);
        Serial.print(hostname[i]);
    }
    sim_board_SS.print("\",");
    Serial.print("\",");
    sim_board_SS.print(port);
    Serial.print(port);
    sim_board_SS.print(",");
    Serial.print(",");
    if (http == 1){
        sim_board_SS.print(1);
        Serial.println(1);
    }
    else if (http == 2){
        sim_board_SS.print(2);
    }
    else{
        Serial.println("Not expected 'http' value..");
        sim_board_SS.print(0);
    }
    sim_board_SS.print("\r\n");
    if (!wait_response("OK", 2)){
        Serial.println("STOPPED 2");
        return;
    }

    sim_board_SS.print("AT+CHTTPSSEND=");
    Serial.print("AT+CHTTPSSEND=");
    sim_board_SS.print(requestLen+4);
    Serial.print(requestLen+4);
    sim_board_SS.print("\r");
    Serial.println("\r");
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
        return;
    }

//   sim_board_SS.print("AT+CHTTPSSTATE\r");
//   delay(500);
//   if (!wait_response("+CHTTPSSTATE: 7", 15)){
//     Serial.println("STOPPED 4");
//     return;
//   }

    sim_board_SS.print("AT+CHTTPSRECV?");
//   if (!wait_response("+CHTTPSSTATE: 0", 15)){
//     Serial.println("STOPPED 5");
//     return;
//   }

//   sim_board_SS.print("AT+CHTTPSCLSE\r");

//   sim_board_SS.print("AT+CHTTPSSTOP\r");
//   delay(2000);
//   Serial.println("done");

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