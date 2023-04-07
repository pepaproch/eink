#include <Arduino.h>
#include "decodeutf8.h"
#include "gfx88592.h"

// Convert String object from UTF8 string to extended ASCII
String c_88592_tocp(String s) {
    String r="";
    uint16_t ucs2;
    resetUTF8decoder();
    for (int i=0; i<s.length(); i++) {
        ucs2 = decodeUTF8(s.charAt(i));
         Serial.printf("s[%d]=0x%02x -> 0x%04x\n", i, (int) s.charAt(i), ucs2);
        if (ucs2 <= 0x7F)
            r += (char) ucs2 - 32;
        else if (0xA0 <= ucs2 && ucs2 <= 0xFF)
            r += (char) (ucs2 - 32);
        else if (0xA0 <= ucs2 && ucs2 <= 0xDD)
            r += (char) (ucs2 );
        else if (ucs2 < 0xFFFF) //(0xFF < ucs2 && ucs2 < 0xFFFF)
      r += (char) 0x7F;

    }
    return r;
}


// In place conversion of a UTF8 string to extended ASCII string (ASCII is shorter!)
void c_88592_tocp(char* s) {
    int k = 0;
    uint16_t ucs2;
    resetUTF8decoder();
    for (int i=0; i<strlen(s); i++) {
        ucs2 = decodeUTF8(s[i]);
        if (ucs2 <= 0x7F)
            s[k++] = (char) ucs2;
        else if (0xA0 <= ucs2 && ucs2 <= 0xFF)
            s[k++] = (char) (ucs2 - 32);
#ifdef SHOW_UNMAPPED_CODES
        else if (ucs2 < 0xFFFF) //(0xFF < ucs2 && ucs2 < 0xFFFF)
      s[k++] = (char) 0x7F;
#endif
    }
    s[k]=0;
}

//
// Created by pepa on 28.3.23.
//

#include "gfx88592.h"
