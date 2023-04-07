/*

Topic: Demonstration programs for gfx8

Program: 2-gfx_control_codes

Purpose: Explore control code treatment by GFX library

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

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // default is 4, use -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT  64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


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

  display.setTextSize(2);      // scale font 2:1
  display.setTextColor(WHITE); // Draw white text

  Serial.println("\n\nsetup() completed");
}

#define UPDATE_INTERVAL 5  // update time display every 5 seconds

unsigned long updatetime = millis()-UPDATE_INTERVAL*1000;
char cc = 0;  // control code

void loop() {
  if (millis() - updatetime >= UPDATE_INTERVAL*1000) {
    updatetime = millis();
    display.clearDisplay();
    display.setCursor(0, 0); 
    display.printf("cc %02x\n\n>>", cc);
    display.print(cc);
    display.print("<<");
    display.display();
    cc += 1;
    if (cc == 32) 
      cc = 0x7f;
    else if (cc > 0x7f)
      cc = 0;
  }
}
