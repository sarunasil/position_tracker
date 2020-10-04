

void get_gps(float *lat, float *lon) {

    // *lat = 1;
    // *lon = 2;
    // return ;

    float gps_success = 0;
    float speed_kph, heading, altitude;

    adafruit_SS.enableGPS(true);
    while(!gps_success){
        delay(1000);
        gps_success = adafruit_SS.getGPS(lat, lon, &speed_kph, &heading, &altitude);
        if (gps_success) {
            wait_response("OK");
            Serial.print("GPS lat:");
            Serial.println(*lat, 6);
            Serial.print("GPS long:");
            Serial.println(*lon, 6);
        }
    }
}
