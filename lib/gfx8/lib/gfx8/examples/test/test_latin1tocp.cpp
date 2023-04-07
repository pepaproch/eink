/*
Topic: Demonstration programs for gfx8

Program: test_latin1tocp_clock

Purpose: Test latin1tocp() function

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
#include "gfxlatin1.h"

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
  index += 32;
  String r = latin1tocp(s);

  Serial.printf("Converted string length: %d ", r.length());
  if (r.length() != 16) {
    Serial.println("Error not 16 bytes long!");
    errcount++;
  }
  Serial.println();

  for (int i=0; i < r.length(); i++) {
    yield();
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

void dumpString(String s, char * msg ) {
#ifdef SHOW_UNMAPPED_CODES
  Serial.printf("\nTesting %s (SHOW_UNMAPPED_CODES defined)\n", msg);
#else  
  Serial.printf("\nTesting %s (SHOW_UNMAPPED_CODES not defined)\n", msg);
#endif  
  Serial.printf("SourceStr: ");
  StringToHex(s);
  Serial.print("\nutf8tocp-> ");
  String r = latin1tocp(s);
  StringToHex(r);

  Serial.print("  \"");
  for (int i=0; i < r.length(); i++) {
    yield();
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

  // test all characters in GFX Latin 1

  String ts(" !\"#$%&'()*+,-./");
  testString(ts, 0x00);

  ts = "0123456789:;<=>?";
  testString(ts, 0x10);

  ts = "@ABCDEFGHIJKLMNO";
  testString(ts, 0x20);
 
  ts = "PQRSTUVWXYZ[\\]^_";
  testString(ts, 0x30);
 
  ts = "`abcdefghijklmno";
  testString(ts, 0x40);
  
  ts = "pqrstuvwxyz{|}~\x7F";
  testString(ts, 0x50);

  ts = "\xc2\xA0¡¢£¤¥¦§¨©ª«¬\xc2\xad®¯";
  testString(ts, 0x60);

  ts = "°±²³´µ¶·¸¹º»¼½¾¿";
  testString(ts, 0x70);

  ts = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ";
  testString(ts, 0x80);

  ts = "ÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß";
  testString(ts, 0x90);

  ts = "àáâãäåæçèéêëìíîï";
  testString(ts, 0xa0);
  
  ts = "ðñòóôõö÷øùúûüýþÿ";
  testString(ts, 0xb0);

  ts = "Ă"; // U+0102 LATIN CAPITAL LETTER A WITH BREVE
  dumpString(ts, "Non latin1 char U+0102 LATIN CAPITAL LETTER A WITH BREVE");
  // if SHOW_UNMAPPED_CODES macro is defined,  ts will be converted to "\x7f"
  // otherwise it will be converted to "" the empty string.
  // SHOW_UNMAPPED_CODES is defined in the [extra] section of platformio.ini 
 }

void loop() {
}

