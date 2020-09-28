

void write_sms(char *number, int numSize, char *message, int msgSize) {
    sim_board_SS.print("AT+CMGF=1\r");
    delay(500);
    sim_board_SS.print("AT+CMGS=\"");
    for (int i=0;i<numSize;++i){
        sim_board_SS.print(number[i]);
    }
    sim_board_SS.print("\"\r");
    delay(500);
    for (int i=0;i<msgSize;++i){
        sim_board_SS.print(message[i]);
    }
    delay(500);
    sim_board_SS.println((char)26);
    sim_board_SS.println();
    delay(5000);
}