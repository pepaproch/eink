#include <Arduino.h>
#include "decodeutf8.h"
#include "gfx8.h"

// Get a little boost in performance when scanning the charset for a given Unicode. But that only works
// if the code points in the charset are in increasing order. Probably best not to assume that in order
// to make the library as general as possible
//
//#define CHARSET_SORTED

gfx8charset_t _default = {};
/* equivalent to
  nullptr, //  uint16_t * charset;
  0,       //  uint16_t charcount;
  0,       //  uint8_t first;
  0        //  uint8_t aciicount;
*/

gfx8charset_t* defaultCharset = &_default;

void initGfx8charset(uint16_t* charset, uint16_t charcount, uint16_t first, uint8_t lastascii) {
  initGfx8charset(&_default, charset, charcount, first, lastascii);
};

void initGfx8charset(gfx8charset_t* gfxcharset, uint16_t* charset, uint16_t charcount, uint16_t first, uint8_t lastascii) {
  //memset(gfxcharset, 0, sizeof(gfx8charset_t));
  gfxcharset->charset = charset;
  gfxcharset->charcount = charcount;
  gfxcharset->first = first; 
  gfxcharset->asciicount = (lastascii) ? lastascii+1 : 0;
};


uint16_t recodeUTF8(uint8_t b, gfx8charset_t* gfx8charset) {

  uint16_t ucs2 = decodeUTF8(b);

   Serial.printf("decodeUTF(0x%02x) -> 0x%04x\n", b, ucs2);

  // pass through valid ASCII codes
  if (ucs2 == 0x0a || (gfx8charset->first <= ucs2 && ucs2 < gfx8charset->asciicount))
    return ucs2;
  
  // if the graphic library does not handle any control code consider changing the above to
  //   if (gfx8charset->first <= ucs2 && ucs2 < gfx8charset->asciicount)
  //     return ucs2;
  // 
  // if the graphic library handles more control codes than just the linefeed 0x0a then perhaps
  // this is better
  //   if (ucs2 < 0x20 || (gfx8charset->first <= ucs2 && ucs2 < gfx8charset->asciicount))
  //     return ucs2;

  uint16_t * find = gfx8charset->charset;
  for (int i = 0; i < gfx8charset->charcount; i++) {
#ifndef CHARSET_SORTED
    if (ucs2 == pgm_read_word(find++)) 
#else
    uint16_t n = pgm_read_word(find++);
    if ( ucs2 <  n) 
      return 0xFFFF;
    else if ( ucs2 == n)
#endif 
      return (gfx8charset->asciicount) ? gfx8charset->asciicount + i : gfx8charset->first + i;
  }   
  return 0xFFFF;  
}

// Convert String object from UTF8 string to GFX code points
String utf8tocp(String s, gfx8charset_t* gfx8charset) {
  String r="";
  uint16_t ucs2;

  Serial.printf("\n utf8tocp(String s =");
  for (int i=0; i<s.length(); i++) {
    Serial.printf(" 0x%02x", s.charAt(i));
  }
  Serial.println(")");

  resetUTF8decoder();
  for (int i=0; i<s.length(); i++) {
    ucs2 = recodeUTF8(s.charAt(i), gfx8charset);
    Serial.printf("s[%d]=0x%02x -> 0x%04x\n", i, (int) s.charAt(i), ucs2);
    if (ucs2 <= 0xFF)
      r += (char) ucs2;
  }
  r += (char) 0xaf;
  return r;
}


// In place conversion of a UTF8 string to GFX code points
void utf8tocp(char* s, gfx8charset_t* gfx8charset) {

    Serial.printf("\n utf8tocp(String s =");
    for (int i=0; i<strlen(s); i++) {
        Serial.printf(" 0x%02x", s[i]);
    }
    Serial.println(")");
  int k = 0;
  uint16_t ucs2;
  resetUTF8decoder();
    Serial.printf("\nstrlen(s)=%d\n", strlen(s));
  for (int i=0; i<strlen(s); i++) {
    ucs2 = recodeUTF8(s[i], gfx8charset);
    if (ucs2 <= 0xFF)
      s[k++] = (char) ucs2;
  }
  s[k]=0;
}
