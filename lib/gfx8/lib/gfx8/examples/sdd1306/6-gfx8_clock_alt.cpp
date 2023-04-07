/*

Topic: Demonstration programs for gfx8

Program: 6-gfx8_clock_alt

Purpose: Testing utf8tocp() in gfx8 library using a different charset definition

Note:    This program is the same as 5-gfx8_clock except that the characters
         '0' to ':' are treaded as sequential ASCII characters beginning at 0x30 and
         only 'a', 'm', 'p', and ' ' (SP) are in the FreeMono12ptClockAlt charset.
         So the meridiem indicators are UTF-8 encoded literals converted on the
         fly to GFX coded literals with utf8tocp functions in the gfx8 library.

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
#include "FreeMono12ptClockAlt.h" 
#include "gfx8.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // default is 4, use -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT  64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Setup the proportional font
GFXfont gfxfont = FreeMono12ptClockAlt;           

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

  display.setFont(&gfxfont);   // Use the proportional font
  
  initGfx8charset((uint16_t*) ClockAltCharset, ClockAltCharcount, 0x30, 0x3a);

  Serial.println("\n\nsetup() completed");
}

#define UPDATE_INTERVAL 5  // update time display every 5 seconds

#define AM_STR " am"    
#define PM_STR " pm"    

bool timeFormat24hour = true;
int hour = 11;
int minute = 58;
int second = 0;

unsigned long updatetime = millis();
bool timechanged = true;

char timebuf[48];
char meridiem[8] = {0};
int  lochour;

void loop() {
  // update timer
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

    display.clearDisplay();
    display.setCursor(0, gfxfont.yAdvance);  // cursor y position is the character baseline in GFXFont
    display.print(timebuf);
    display.display();

    timechanged = false;
    timeFormat24hour = ! timeFormat24hour;  // toggle time format between 12 hour and 24 hour format
  }
}
