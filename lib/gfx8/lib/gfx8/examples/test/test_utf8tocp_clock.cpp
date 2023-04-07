/*
Topic: Demonstration programs for gfx8

Program: test_utf8tocp_clock

Purpose: Test gfx8tocp() function with Clock font charset

platformio.ini
    [extra]
    baud = 115200
    show_unmapped_codes = 
    ;show_unmapped_codes =  SHOW_UNMAPPED_CODES
    charset_sorted = 
    ;charset_sorted = CHARSET_SORTED


    [env:d1_mini_test]
    board = d1_mini
    platform = espressif8266
    framework = arduino
    monitor_speed = ${extra.baud}
    build_flags = -DBAUD=${extra.baud} ${extra.show_unmapped_codes} ${extra.charset_sorted}
    monitor_flags = 
        --eol
        LF
        --echo
*/

#include <Arduino.h>
#include "../lib/gfx8/examples/test/gfxfont.h"  // usually pulled from Adafruit_GFX
#include "FreeMono12ptClock.h" 
#include "gfx8.h"

int errcount = 0;

void printErrcount(void) {
  Serial.print("Test completed with ");
  if (errcount)
    Serial.printf("%d error%s.\n", errcount, (errcount == 1) ? "" : "s");
  else
    Serial.println("no errors.");
}

void testString(String s, uint16_t index) {
  Serial.printf("\nTesting \"%s\"\n", s.c_str());
  errcount = 0;
  //index += 32;
  String r = utf8tocp(s);

  Serial.printf("Converted string length: %d ", r.length());
  if (r.length() != 15) {
    Serial.println("Error not 16 bytes long!");
    errcount++;
  }
  Serial.println();

  for (int i=0; i < r.length(); i++) {
    //dbg: Serial.printf("r.charAt(%d)=0x%02x ", i, r.charAt(i));
    if (r.charAt(i) != index) {
      Serial.printf("char at %d is 0x%02x expected 0x%02x\n", i, (uint8_t) r.charAt(i), index);
      errcount++;
    }
    index++;
  } 
  printErrcount();     
}

void StringToHex(String s) {
  for (int i=0; i < s.length(); i++) 
    Serial.printf("0x%02x ", s.charAt(i));
}

void dumpString(String s, char * msg) {
  Serial.printf("\nTesting %s", msg);
  Serial.printf("\nSourceStr: ");
  StringToHex(s);
  Serial.print("\nutf8tocp-> ");
  String r = utf8tocp(s);
  StringToHex(r);

  Serial.print("  \"");
  for (int i=0; i < r.length(); i++) {
    char c = r.charAt(i);
    if (c < 0x20 || c > 127)
      c = 0x20;
    Serial.print(c);
  }  
  Serial.println("\"");
}

void setup() {  
  Serial.begin(BAUD); 
  while(! Serial) 
    delay(10); 

  Serial.println("\n\n");  

  // setup the default charset definition for utf8tocp() 
  initGfx8charset((uint16_t*) ClockCharset, ClockCharcount, FreeMono12ptClock.first, 0); 

  // test all characters in font

  String ts("0123456789:amp ");
  testString(ts, FreeMono12ptClock.first);

  ts = "Ă"; // U+0102 LATIN CAPITAL LETTER A WITH BREVE 
  dumpString(ts, "a character not in font - U+0102 LATIN CAPITAL LETTER A WITH BREVE");
    // if SHOW_UNMAPPED_CODES macro is defined, ts will be converted to "\x7f"
    // otherwise it will be converted to "", the empty string.
    // SHOW_UNMAPPED_CODES is defined in the [extra] section of platformio.ini   
  }

void loop() {
}
