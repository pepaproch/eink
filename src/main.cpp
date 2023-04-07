/*
* Example code shows values from SHT40 (temperature/humidity), battery voltage on eInk/ePaper display
*
* Compile as ESP32 dev module
* ESPink - ESP32 board for ePaper: https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
* SHT40 (https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/) is connected through uSup connector
*
*
* -------- ESPink pinout -------
* MOSI/SDI 23
* CLK/SCK 18
* SS 5 //CS
* DC 17
* RST 16
* BUSY 4
* -------------------------------
*
* Libraries:
* Analog read with calibration data: https://github.com/madhephaestus/ESP32AnalogRead/
* SHT40: https://github.com/adafruit/Adafruit_SHT4X
* EPD library: https://github.com/ZinggJM/GxEPD2
*
* made by laskakit (c) 2022
*/


#include <WiFi.h>
#include <GxEPD2_BW.h>
#include <SPI.h>

#include <NTPClient.h>
#include <HTTPClient.h>
#include "WiFi.h"
#include "ArduinoJson.h"
// WiFi credentials.
const char* WIFI_SSID = "UPC3329700";
const char* WIFI_PASS = "wp5prdseSvxw";
// Fonts


#include "Roboto_Bold32pt8b47.h"
#include "Roboto_Bold16pt8b47.h"
#include "Roboto_Bold12pt8b47.h"
#include "Roboto_Bold8pt8b47.h"
#include "weather16pt8b47.h"
#include <algorithm>

#include "gfx8.h"
/*----------------- Pinout of ESPink -------------*/
// MOSI/SDI 23
// CLK/SCK 18
// CS 5
#define SS 5 //SS
#define DC 17 // D/C
#define RST 16  // RES
#define BUSY 4  //BUSY
/* ---------------------------------------------- */

/*------------------------  Define EPD driver - uncomment the used one  -----------------------------*/
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 1.54" b/w
//GxEPD2_BW<GxEPD2_154_M10, GxEPD2_154_M10::HEIGHT> display(GxEPD2_154_M10(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 1.54" b/w DES
//GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 2.13" b/w
//GxEPD2_BW<GxEPD2_213_M21, GxEPD2_213_M21::HEIGHT> display(GxEPD2_213_M21(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 2.13" b/w DES
//GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 4.2" b/w
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 7.5" b/w 800x480
/*  ------------------------------------------------------------------------------------------------- */
// Note: all supported ePapers you can find on https://github.com/ZinggJM/GxEPD2
#if defined(ESP32)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#endif

const char temperature[] = "Teplota";



void WIFIInit();
void printTime();
String getTime();
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


const uint32_t partial_update_period_s = 1;
const uint32_t full_update_period_s = 6 * 60 * 60;

WiFiClient client;
uint32_t start_time;
uint32_t next_time;
uint32_t previous_time;
uint32_t previous_full_update;

uint32_t total_seconds = 0;
uint32_t seconds, minutes, hours, days;
float t_in;
const char appiHost[] = "192.168.0.178";
const char apiToken[]= "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI3NDQ2MDY1ZjY4M2E0ODk5YTFiZWIyNzA2MTM3ZmRhMCIsImlhdCI6MTY3OTY0ODEwOCwiZXhwIjoxOTk1MDA4MTA4fQ.Eqt0up5mL9DpkcUallbkHdsiRxH77_HbNwZ_py1_uDs";
String get_temp_url = "api/states";
String id_temp_in_sensor = "sensor.teplota_co2_cidlo";
void  WiFiInit();

void showPartialUpdate(String content, uint16_t box_x, uint16_t box_y, uint16_t box_w, uint16_t box_h);

void refreshTopBar();
double getEntityState(String entity_id);
void topBar();
void inTemp();

void setup() {
    Serial.begin(460800);
    WiFiInit();
 //   getEntityState(id_temp_in_sensor);
    delay(1000);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH); // enable power supply for ePaper
    delay(500);
    display.init(); // disable diagnostic output on Serial
    Serial.println("setup done");
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(0);
    //display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);

    start_time = next_time = previous_time = previous_full_update = millis();
    display.fillScreen(GxEPD_WHITE);
  //  topBar();
    inTemp();



}
void topBar() {

    display.drawFastHLine(0,0+32,display.width() , GxEPD_BLACK);
    display.drawFastHLine(0,0+31,display.width() , GxEPD_BLACK);
}

void inTemp() {


    double temperature = getEntityState(id_temp_in_sensor);
    int16_t tbx, tby; uint16_t tbw, tbh;
    // center text

    // utf8tocp(state);
    display.setFont(&Roboto_Bold8ptCS);


    //int s = display.width()/16 ;
    for(int i=0; i<16 ; i++) {
        display.setCursor( (48*i)+45 +3 ,40);
        char header[20];
        sprintf(header,"%02x" , i);
        display.print(header);

        display.drawFastVLine((48*i)+40 , 0,display.height() , GxEPD_BLACK);

    }
    display.setFont(&pe_icon_set_weather16ptWH);
    display.setCursor( 0 ,0);
    for(int r=0; r<16 ; r++) {
        display.setCursor( 0 ,25*r+80);
        char header[20];
        sprintf(header,"%02x" , r);
        display.print(header);
        display.setFont(&pe_icon_set_weather16ptWH);
       for(int z = 0 ; z<16 ; z++) {
           int s = 32;
            if((r*16)+z>127)
                s = -32;
          // char c = (char) str.charCodeAt(0))+s;
           display.setCursor( (48*z)+45 + 6 ,25*r+80);
           display.print(c);
       }

    }


    display.display(false);
    display.display(false);


 //   display.getTextBounds(state, 25, 100 +64 , &tbx, &tby, &tbw, &tbh);
  ///  showPartialUpdate(state, tbx-10, tby-10, tbw+20, tbh+20);



}

void loop()
{
    Serial.println("loop");
    uint32_t actual = millis();
    while (actual < next_time) {
        // the "BlinkWithoutDelay" method works also for overflowed millis
        if ((actual - previous_time) > (partial_update_period_s * 1000)) {
            //Serial.print(actual - previous_time); Serial.print(" > "); Serial.println(partial_update_period_s * 1000);
            break;
        }
        delay(100);
        actual = millis();
    }
    //Serial.print("actual: "); Serial.print(actual); Serial.print(" previous: "); Serial.println(previous_time);
    if ((actual - previous_full_update) > full_update_period_s * 1000) {

     //   display.display(true);

        previous_full_update = actual;
    }
    previous_time = actual;
    next_time += uint32_t(partial_update_period_s * 1000);
    total_seconds += partial_update_period_s;
    seconds = total_seconds % 60;
    minutes = (total_seconds / 60) % 60;
    hours = (total_seconds / 3600) % 24;
    days = (total_seconds / 3600) / 24;

   // inTemp();

}





void showPartialUpdate(String content, uint16_t box_x ,uint16_t box_y  ,uint16_t box_w , uint16_t box_h   );
void WiFiInit(){
    // Connecting to WiFi
    Serial.println();
    Serial.print("Connecting to...");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        i++;
        if (i == 10) {
            i = 0;
            Serial.println("Not able to connect");
            digitalWrite(2, LOW); // disable power supply for ePaper
            // deep sleep mode
            esp_sleep_enable_timer_wakeup(60 * 1000000);
            delay(200);
            esp_deep_sleep_start();
        } else {
            Serial.print(".");
        }
    }
    Serial.println("");
    Serial.println("Wi-Fi connected successfully");
}

double getEntityState(String entity_id) {
    // connect to web server on port 80:
    String payload = "";
    DynamicJsonDocument doc(1024);
    if(client.connect(appiHost, 8123)) {
        // if connected:
        // make a HTTP request:
        // send HTTP header
        client.println("GET /api/states/" + entity_id + " HTTP/1.1");
        client.println("Host: " + String(appiHost));
        client.println("Authorization: Bearer " + String(apiToken));
        client.println("Connection: close");
        client.println(); // end HTTP header

        // send HTTP body
        client.println();
        Serial.println("Client connected " + client.connected() );
        char endOfHeaders[] = "\r\n\r\n";
        if (!client.find(endOfHeaders)) {
            Serial.println(F("Invalid response"));
            client.stop();
            return 0;
        }

        // Allocate the JSON document
        // Use https://arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = JSON_OBJECT_SIZE(50) + JSON_ARRAY_SIZE(2) + 60;


        // Parse JSON object
        DeserializationError error = deserializeJson(doc, client);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            client.stop();
            return 0;
        }

        // Extract values
        Serial.println(F("Response:"));
        Serial.println(doc["entity_id"].as<const char*>());
        Serial.println(doc["state"].as<double>());

        // the server's disconnected, stop the client:
        client.stop();
        Serial.println();
        Serial.println("disconnected");
    } else {// if not connected:
        Serial.println("connection failed");
    }

    return doc["state"].as<double>();

}


void showPartialUpdate(String content, uint16_t box_x, uint16_t box_y, uint16_t box_w, uint16_t box_h) {

    //display.setFont(&font);
    display.fillScreen(GxEPD_WHITE);
    uint16_t cursor_y = box_y + box_h;
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(content);
    display.displayWindow(box_x, box_y, box_w, box_h);

}





