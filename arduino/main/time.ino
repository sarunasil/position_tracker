
bool get_time(char* time_buff){ //recomended buff size = 30

    sim_board_SS.print(F("AT+CCLK?\r"));

    if (!
        ( wait_value_response("+CCLK: ", time_buff) &&
          wait_response("OK")
        )){
        sim_board_SS.print(F("AT+CHTPSERV=\"ADD\",\"www.google.com\",80,1\r"));
        if (!wait_response("OK")){
            return false; //big error
        }

        sim_board_SS.print(F("AT+CHTPUPDATE\r"));
        if (!wait_response("OK")){
            return false; //big error
        }

        sim_board_SS.print(F("AT+CCLK?\r"));

        if (!
        ( wait_value_response("+CCLK: ", time_buff) &&
          wait_response("OK")
        )){
            return false; //big error
        }

    }

    return true;
}
