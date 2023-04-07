/*
Topic: Demonstration programs for gfx8

Program: test_decodeutf8

Purpose: Sketch showing that the decoderUTF8 function is a "naive" implementation
         which decodes invalid sequences.
 
         See section 5.0 UTF-8 Encoding to GFXFont Latin 1 and Latin 9 Encodings in 
         8 Bit GFXFonts @ https://sigmdel.ca/michel/program/misc/gfxfont_8bit_en.html#utf-8-to-gfxlatin-x"

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
#include "decodeutf8.h"

void dumpString(String s, char * msg) {
  Serial.printf("\nTesting %s\n", msg);
  resetUTF8decoder(); // reset decoder state
  for (int i=0; i < s.length(); i++) {
    uint16_t ucs2 = decodeUTF8(s.charAt(i));
    Serial.printf("  s[%d] = 0x%02x, decoderUTF8 --> 0x%04x\n", i, s.charAt(i), ucs2);
  }  
}

void setup() {  
  Serial.begin(BAUD); 
  while(! Serial) 
    delay(10); 

  Serial.print("\n\nSome errors with decodeUTF8...");  
  delay(2000);
  Serial.println();

  String ts="\xc1\x85";
  dumpString(ts, "Invalid UTF-8 encoding, yet yields 'E'");

  ts="\xc0\x80";
  dumpString(ts, "Invalid UTF-8 encoding, yet yields NUL");

  ts="\xED\xA1\x8C\xED\xBE\xB4";
  dumpString(ts, "Valid UTF-8 encoding of surrogate pair, decoder incorrect");

  ts="/../";
  dumpString(ts, "Valid ASCII string");

  ts="\x2F\xC0\xAE\x2E\x2F";
  dumpString(ts, "Invalid UTF-8 encoding yielding previous valid ASCII string");

  ts="\xC0\xAE";
  dumpString(ts, "The culprit the invalid 0xC0 xxxx encoding as in 0xC0 0xAE yielding 0x2E = '.'");
  
  ts="\xF0\x90\x8C\x94"; //"𐌔";
  dumpString(ts, "Valid UTF-8 4 byte encoding for U+10314 OLD ITALIC LETTER ES (UTF-16: 0xD800 0xDF14) which is ignored");
}

void loop(void) {

}

