/*

Topic: Demonstration programs for gfx8

Program: 7-sdd1306_gfx8_clock2

Purpose: Testing utf8tocp() in gfx8 library with two GFX fonts with 
         different character sets.

Hardware:
  ESP8266 dev board: D1 mini
  Display : NONAME I²C SDD1306 0.96" monochrome OLED display with 0x3C I²C address 

    See ESP8266 0.96 inch OLED Display with Arduino IDE
    @ https://randomnerdtutorials.com/esp8266-0-96-inch-oled-display-with-arduino-ide/

Libraries:
  Adafruit_BusIO,   version 1.6.0, 2020-10-27, https://github.com/adafruit/Adafruit_BusIO
  Adafruit_GFX,     version 1.10.2, 2020-10-19, https://github.com/adafruit/Adafruit-GFX-Library
  Adafruit_SSD1306, version 2.4.0, 2020-08-28, https://github.com/adafruit/Adafruit_SSD1306

platformio.ini:
  [extra]
    BAUD = 115200
  ;  SHOW_UNMAPPED_CODES = -DSHOW_UNMAPPED_CODES
    SHOW_UNMAPPED_CODES =
  ;  CHARSET_SORTED = -DCHARSET_SORTED
    CHARSET_SORTED =
  [env:d1_mini]
  platform = espressif8266
  board = d1_mini
  framework = arduino
  build_flags = -DBAUD=${extra.BAUD} ${extra.SHOW_UNMAPPED_CODES} ${extra.CHARSET_SORTED}
  monitor_speed = ${extra.BAUD}
  monitor_flags = 
    --eol
    LF
    --echo
  lib_deps = 
    adafruit/Adafruit GFX Library@^1.10.3
    adafruit/Adafruit SSD1306@^2.4.1

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
#include <SPI.h>
#include "Adafruit_I2CDevice.h" 
#include "Adafruit_GFX.h"       
#include "Adafruit_SSD1306.h"
#include "FreeMono12ptClock.h" 
#include "FreeMono8ptFR.h" 
#include "gfx8.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // default is 4, use -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT  64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Setup the proportional font for time
GFXfont gfxTimefont = FreeMono12ptClock;   
// Setup the proportion font for the date
GFXfont gfxDatefont = FreeMono8ptFR;        

gfx8charset_t datecharset = {}; // for FreeMono8ptFR character set, default gfx8charset_t will be used for FreeMono12ptClock

// months and days of week stringd in French which use some non ASCII glyphs from the FreeMono8ptFR character set
String months[] = {"janvier", "février", "mars", "avril", "mai", "juin", "juillet", 
                   "août", "septembre", "octobre", "novembre", "décembre"};

String dows[] = {"lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche"};

void displaygfx8charset(gfx8charset_t* cs) {
  Serial.printf("gfx8charset address %p\n", cs);
  Serial.printf("  charset address %p\n", cs->charset);
  Serial.printf("  charcount %d\n", cs->charcount);
  Serial.printf("  first %d\n", cs->first);
  Serial.printf("  asciicount %d\n", cs->asciicount);
}

// Setup serial port and display
void setup() {
  Serial.begin(BAUD);
  while (!Serial) delay(10); // for Leonardo...
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // default I²C address 0x3D
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("SSD1306 allocation failed");  
    for(;;) { ; // Don't proceed, loop forever
      digitalWrite(LED_BUILTIN, 1-digitalRead(LED_BUILTIN));
      delay(100);
    }
  }

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  
  initGfx8charset(&datecharset, (uint16_t*) FRCharset, FRCharcount, 0x20, 0x7e);
  initGfx8charset((uint16_t*) ClockCharset, ClockCharcount, 0x30, 0);

  Serial.println("\n\nsetup() completed");
}

#define UPDATE_INTERVAL 5  // update time display every 5 seconds

#define AM_STR " am"      
#define PM_STR " pm"      

bool timeFormat24hour = true;
int year = 2020;
int month = 0;
int day = 1;
int dow = 0;

int hour = 11;
int minute = 58;
int second = 0;

unsigned long updatetime = millis();
bool timechanged = true;

char timebuf[32];
char meridiem[4] = {0};
int  lochour;

void loop() {
  if (millis() - updatetime >= UPDATE_INTERVAL*1000) {
    updatetime = millis();
    timechanged = true;  // once every 5 second
    second += UPDATE_INTERVAL;
    
    if (second > 59) {
      second = 0;
      minute++;
      if (minute > 59) {
        minute = 0;
        hour++;
        if (hour > 23) {
          hour = 0;
        }
      }
    }
  }

  // update display
  if (timechanged) {
    display.clearDisplay();
   
    // print month and day of week with Datefont 
    String s = months[month];
    month = (month + 1) % 12;

    display.setCursor(0, gfxDatefont.yAdvance);  // ** cursor y position is the character baseline in
    String r = utf8tocp(s, &datecharset);
    Serial.printf("\"%s\" -> \"%s\"\n", s.c_str(), r.c_str());
    display.setFont(&gfxDatefont);   // Use the proportional font
    display.print(r);

    s = dows[dow];
    dow = (dow + 1) % 7;
    display.setCursor(0, 2*gfxDatefont.yAdvance);  // ** cursor y position is the character baseline in
    r = utf8tocp(s, &datecharset);
    Serial.printf("\"%s\" -> \"%s\"\n\n", s.c_str(), r.c_str());
    display.print(r);
   
    // print time with default (time) font
    if (timeFormat24hour) {
      sprintf(timebuf, "%02d:%02d", hour, minute);
    } else {
      // using 12 PM = noon and 12 AM = midnight convention
      strcpy(meridiem, (hour > 11) ? PM_STR : AM_STR);  
      lochour = (hour > 12) ? hour - 12 : hour;
      sprintf(timebuf, "%02d:%02d%s", lochour, minute, meridiem);        
    }  
    Serial.printf("timebuf=\"%s\" -> ", timebuf);
  
    utf8tocp(timebuf);                       // convert to GFX code points  
    Serial.printf("\"%s\"\n", timebuf);

    display.setCursor(0, (int16_t) (14*gfxDatefont.yAdvance)/4);  // cursor y position is the character baseline in GFXFont
    display.setFont(&gfxTimefont);   // Use the proportional font
    display.print(timebuf);
    display.display();

    timechanged = false;
    timeFormat24hour = ! timeFormat24hour;  // toggle time format between 12 hour and 24 hour format
  }
}
