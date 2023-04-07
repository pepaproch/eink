/*

Topic: Demonstration programs for gfx8

Program: 1-gfx8_true_clock

Purpose: Testing utf8tocp() in gfx8 library with two GFX fonts with 
         different character sets with the TFT_22_ILI9225 library
         by Nkawu which handles GFXFonts.

Note: Finally, a program that does something useful. It shows the true time 
      using SNTP to set the clock and regularly pings a target, my home
      automation system, reporting when it's offline.

Hardware:
  ESP8266 dev board: nodeMCU with ESP8266-12E
  Display : SPI ILI9225 2" 16-bit color TFT display
    See 2.0inch Arduino SPI Module ILI9225 SKU:MAR2001
    @ http://www.lcdwiki.com/2.0inch_Arduino_SPI_Module_ILI9225_SKU:MAR2001

Libraries:
  TFT_22_ILI9225, version 1.4.4, 2020-07-19, https://github.com/Nkawu/TFT_22_ILI9225
  Time, version 1.6, 2019-12-11, https://github.com/PaulStoffregen/Time
  AsyncPing(esp8266), version 1.1.0, https://github.com/akaJes/AsyncPing

platformio.ini:
  [extra]
  baud = 115200

  [env:nodemcuv2]
  board = nodemcuv2
  platform = espressif8266
  framework = arduino
  monitor_speed = ${extra.baud}
  build_flags = -DBAUD=${extra.baud}
  monitor_flags = 
    --eol
    LF
    --echo
  lib_deps = 
    nkawu/TFT 22 ILI9225@^1.4.4
    paulstoffregen/Time@^1.6
    akajes/AsyncPing(esp8266)@^1.1.0

Author: Michel Deslierres @ https://michel/sigmdel.ca

Documenations en français :
  Police GFX avec jeu de caractères arbitraires @ https://sigmdel.ca/michel/program/misc/gfxfont_8bit_02_fr.html
  
English documentation:  
  GFX Font with Arbitrary Character Set @ https://sigmdel.ca/michel/program/misc/gfxfont_8bit_02_en.html

License:
  Zero Clause BSD license @ http://landley.net/toybox/license.html

*/

// SPDX-License-Identifier: 0BSD


#include <Arduino.h>
// time
#include <TimeLib.h>
// ESP8266 Wi-Fi
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "../lib/gfx8/examples/ili9225/secrets.h"  // Wi-Fi credentials etc.
// Domoticz ping
#include <Ticker.h>
#include "AsyncPing.h"

// display
#include "SPI.h"
#include "TFT_22_ILI9225.h"
// fonts
#include "FreeMono12ptClock.h" 
#include "FreeMono8ptFR.h" 
#include "gfx8.h"


// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

//const int timeZone = 1;     // Central European Time

const int timeZone = -4;  // Atlantic Standard Time (CDN)
//const int timeZone = -3;    // Atlantic Daylight Time (CND)

//const int timeZone = -5;  // Eastern Standard Time (North America)
//const int timeZone = -4;  // Eastern Daylight Time (North America)

//const int timeZone = -8;  // Pacific Standard Time (North America)
//const int timeZone = -7;  // Pacific Daylight Time (North America)

bool verbose = false;

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets


// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime() {
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  if (verbose)
    Serial.println("Transmiting NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  if (verbose) {
    Serial.print(ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
  }  
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      if (verbose)
        Serial.println("Received NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  if (verbose) Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// -- ping -- //

AsyncPing targetPinger;             // object to send successive pings to the target site
Ticker pingTimer;                   // object to time the sending of pings to target sites 
IPAddress targetIP;

unsigned long lastValidPing = 0;    // the last time a ping was received from a target


// send a ping request to the next valid targer IP address and increment its sent statistic
bool sendTargetPing(void) {  
  if (targetIP.isSet()) {
    targetPinger.begin(targetIP, 1, 5000);  // 1 ping, timeout in 5 seconds
    return true;
  }  
  return false;
}

// function that is called when a ping reply arrives from one of the target
bool targetPingerCallback(const AsyncPingResponse& response) {
  if (response.answer && response.addr == targetIP) {
    lastValidPing = millis();
    return true; 
  }  
}  

// pinout for 2" TFT display with ILI9225 driver chip using hardware SPI
//
#if defined (ARDUINO_ARCH_STM32F1)
#define TFT_RST PA1
#define TFT_RS  PA2
#define TFT_CS  PA0 // SS
#define TFT_SDI PA7 // MOSI
#define TFT_CLK PA5 // SCK
#define TFT_LED 0 // 0 if wired to +5V directly
/*
#elif defined(ESP8266)
#define TFT_RST 4   // D2
#define TFT_RS  5   // D1
#define TFT_CLK 14  // D5 SCK
//#define TFT_SDO 12  // D6 MISO
#define TFT_SDI 13  // D7 MOSI
#define TFT_CS  15  // D8 SS
#define TFT_LED 2   // D4     set 0 if wired to +5V directly -> D3=0 is not possible !!
*/
#elif defined(ESP8266)
#define TFT_RST  2 // D4 
#define TFT_RS   4 // D2 
#define TFT_CS  15 // D8 SS
#define TFT_LED  0 // set 0 if wired to +5V directly -> D3=0 is not possible !!
// the following are default values that do not need to be specified when using hardware SPI
//#define TFT_CLK 14   // D5 SCK
//#define TFT_SDI 13   // D7 MOSI
////#define TFT_SDO 12 // D6 MISO  MISO not used by display
#elif defined(ESP32)
#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
//#define TFT_SDO 12  // HSPI-MISO
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0 // 0 if wired to +5V directly
#else
#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 3   // 0 if wired to +5V directly
#endif

//#define TFT_BRIGHTNESS 200 -- not available on 2" model

TFT_22_ILI9225 display = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED);

// STRING is defined in TFT_22_ILI9225.h and can be String or const char *
// Assumed to be String below

// Setup the proportional font for time
GFXfont gfxTimefont = FreeMono12ptClock;  

// Setup the proportion font for the date
GFXfont gfxDatefont = FreeMono8ptFR;        

gfx8charset_t datecharset = {}; // for FreeMono8ptFR character set, default gfx8charset_t will be used for FreeMono12ptClock

// days of week and months strings in French which use some non ASCII glyphs from the FreeMono8ptFR character set
char* dows[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};

char* months[] = {"janvier", "février", "mars", "avril", "mai", "juin", "juillet", 
                   "août", "septembre", "octobre", "novembre", "décembre"};


enum zone {zDay = 0, zMonth, zYear, zTime, zMeridiem, zAlert, ZONE_COUNT};

typedef struct zone_struct {
    uint8_t top;
    uint8_t bottom;
    uint16_t background;
    uint16_t color;
    GFXfont* font;
    gfx8charset_t* charset;
} zone_t;

zone_t zones[ZONE_COUNT];
uint16_t bar = 0;

// Setup serial port and display
void setup() {
  Serial.begin(BAUD);
  while (!Serial) delay(10); // for Leonardo...
  Serial.println("\n\nClock setup...");

  delay(250);
  if (verbose) {
    Serial.println("TimeNTP Example");
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }  
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (verbose) Serial.print(".");
  }

  if (verbose) {
    Serial.print("\nIP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting UDP");
  }  
  Udp.begin(localPort);
  if (verbose) {
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    Serial.println("waiting for NTP sync");
  }  
  setSyncProvider(getNtpTime);
  setSyncInterval(300);  // 300 second resysncs = 5 minutes
  if (verbose)
    Serial.println("Synchronize with SNTP server every 5 minutes");

  if (WiFi.hostByName(pingTarget, targetIP)) {
    targetPinger.on(true,  targetPingerCallback);
    pingTimer.attach(120, sendTargetPing);  // send a ping to a target every 3 minutes
  } else {
    Serial.printf("ERROR: Ping target \"%s\" is not a valid host name or IP address\n", pingTarget);
  }

  // initialize the display
  display.begin();
  display.clear();
  display.setOrientation(0);
  
  initGfx8charset(&datecharset, (uint16_t*) FRCharset, FRCharcount, 0x20, 0x7e);
  initGfx8charset((uint16_t*) ClockCharset, ClockCharcount, 0x30, 0);


//enum zone {zDay = 0, zMonth, zYear, zTime, zMeridiem, ZONE_COUNT};
  // initialize rectangular text zones on screen
  uint16_t color = display.setColor(48, 48, 48);
  int y = 0;
  zones[zDay].top = y;
  y += gfxDatefont.yAdvance + 6;
  zones[zDay].bottom = y;
  zones[zDay].background = display.setColor(32, 74, 135);
  zones[zDay].color = color;
  zones[zDay].font = &gfxDatefont;
  zones[zDay].charset = &datecharset;

  y++;
  zones[zMonth].top = y;
  y += gfxDatefont.yAdvance + 6;
  zones[zMonth].bottom = y;
  zones[zMonth].background = display.setColor(52, 101, 164);
  zones[zMonth].color = color;
  zones[zMonth].font = &gfxDatefont;
  zones[zMonth].charset = &datecharset;

  y++;
  zones[zYear].top = y;
  y += gfxDatefont.yAdvance + 6;
  zones[zYear].bottom = y;
  zones[zYear].background = display.setColor(114, 159, 207);
  zones[zYear].color = color;
  zones[zYear].font = &gfxDatefont;
  zones[zYear].charset = &datecharset;

  y++;
  zones[zTime].top = y;
  y += gfxTimefont.yAdvance + 6;
  zones[zTime].bottom = y;
  zones[zTime].background = display.setColor(152, 182, 224);
  zones[zTime].color = color;
  zones[zTime].font = &gfxTimefont;
  zones[zTime].charset = defaultCharset;

  y++;
  zones[zMeridiem].top = y;
  y += gfxTimefont.yAdvance + 6;
  zones[zMeridiem].bottom = y;
  //zones[zMeridiem].background = display.setColor(203, 210, 255);
  zones[zMeridiem].background = display.setColor(152, 182, 224);
  zones[zMeridiem].color = color;
  zones[zMeridiem].font = &gfxTimefont;
  zones[zMeridiem].charset = defaultCharset;

  y++;
  zones[zAlert].top = y;
  y += gfxDatefont.yAdvance + 6;
  zones[zAlert].bottom = y;
  zones[zAlert].background = COLOR_BLACK; //COLOR_RED;
  zones[zAlert].color = COLOR_YELLOW;
  zones[zAlert].font = &gfxDatefont;
  zones[zAlert].charset = &datecharset;

  bar = display.maxY() - 3;
  
  if (verbose)
    Serial.println("setup() completed");
}

void printZone(zone aZone, String s) {
  int16_t w;
  int16_t h;
  int16_t width = display.maxX()+1;
  // erase previous zone content
  display.fillRectangle(0, zones[aZone].top, width-1, zones[aZone].bottom, zones[aZone].background);
  // select the zone GFXfont
  display.setGFXFont(zones[aZone].font);
  // convert the string to the correct code page
  String r = utf8tocp(s, zones[aZone].charset);
  // size the converted string
  display.getGFXTextExtent(r, 0, 0, &w, &h);  
  // display the string centered in zone
  display.drawGFXText((width - w) / 2, zones[aZone].top + zones[aZone].font->yAdvance, r, zones[zDay].color);
}


#define AM_STR "am"      
#define PM_STR "pm"      

bool timeFormat24hour = false;
time_t prevDisplay = 0; // when the digital clock was displayed
int oldMinute = -1;
int oldYear = -1;
int oldDay = -1; 

char timebuf[32];
String s;

bool dotOn = true;

void loop() {

  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();

      if (minute(prevDisplay) != oldMinute) {
        oldMinute = minute(prevDisplay);

        if (timeFormat24hour) {
          snprintf(timebuf, 32, "%02d:%02d", hour(prevDisplay), minute(prevDisplay));
          s = timebuf;
          printZone(zTime, s);
          s = "";
        } else {
          snprintf(timebuf, 32, "%d:%02d", hourFormat12(prevDisplay), minute(prevDisplay));
          s = timebuf;
          printZone(zTime, s);

          snprintf(timebuf, 32, "%s", (isAM(prevDisplay)) ? AM_STR : PM_STR);
          s =  timebuf;
        }
        printZone(zMeridiem, s);
      } 

      if (day(prevDisplay) != oldDay) {
        oldDay = day(prevDisplay);
        snprintf(timebuf, 32, "%s", dows[weekday(prevDisplay)-1]);
        s = timebuf;
        printZone(zDay, s);

        snprintf(timebuf, 32, "%d %s", day(prevDisplay), months[month(prevDisplay)-1]);
        s = timebuf;
        printZone(zMonth, s);
      }      

      if (year(prevDisplay) != oldYear) {
        oldYear = year(prevDisplay);
        snprintf(timebuf, 32, "%d", oldYear);
        s = timebuf;
        printZone(zYear, s);
      }
      

      if (targetIP.isSet())  {
        if (millis() - lastValidPing > 5*60*1000) {
          if (dotOn) {
            zones[zAlert].background = COLOR_RED;      
            s = "Domo en panne";
            printZone(zAlert, s);
          } else {
            s = "";
            printZone(zAlert, s);
          }  
        } else if (zones[zAlert].background == COLOR_RED)  {
            zones[zAlert].background = COLOR_BLACK; 
            s = "";
            printZone(zAlert, s);
        }
      }
      
      // seconds flash display, choose bar at bottom or red dot
      display.fillRectangle(0, bar, display.maxX(), bar+3, (dotOn) ? COLOR_NAVY : COLOR_SNOW);
      //display.fillCircle(display.maxX() - 12, display.maxY() - 12, 8, (dotOn) ? COLOR_RED : COLOR_BLACK);
      dotOn = !dotOn;
    }
  }
}
