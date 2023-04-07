#include <Arduino.h>
#include "decodeutf8.h"
#include "gfxlatin9.h"

// Define the macro to flag as unmapped those Latin 1 characters
// that have been replaced with Latin 9 characters.

//#define INVALIDATE_OVERWRITTEN_LATIN_1_CHARS

#ifdef SHOW_UNMAPPED_CODES          
#define UNMAPPED_CODE 0x7F
#else
#define UNMAPPED_CODE 0xFFFF
#endif

uint16_t recode(uint8_t b) {

  uint16_t ucs2 = decodeUTF8(b);

  if (ucs2 > 0x7F) {
#ifdef INVALIDATE_OVERWRITTEN_LATIN_1_CHARS     
    if (0xA4 <= ucs2 && ucs2 <= 0xBE) {
      switch (ucs2) {
        case 0xa4: 
        case 0xa6:
        case 0xa8:
        case 0xb4:
        case 0xb8:
        case 0xbc:
        case 0xbd:
        case 0xbe: return UNMAPPED_CODE;
      }
    } 
#endif     
    switch (ucs2) {
      case 0x0152: return 0xbc; break;
      case 0x0153: return 0xbd; break;
      case 0x0160: return 0xa6; break;
      case 0x0161: return 0xa8; break;
      case 0x0178: return 0xbe; break;
      case 0x017D: return 0xb4; break;
      case 0x017E: return 0xb8; break;
      case 0x20AC: return 0xa4; break;
    }
  }	  
  return ucs2;
}

// Convert String object from UTF8 string to extended ASCII
String latin9tocp(String s) {
  String r="";
  uint16_t ucs2;
  resetUTF8decoder();
  for (int i=0; i<s.length(); i++) {
    ucs2 = recode(s.charAt(i));

    //dbg:: Serial.printf("s[%d]=0x%02x -> 0x%04x\n", i, (int) s.charAt(i), ucs2);    

    if (ucs2 <= 0x7F)
      r += (char) ucs2;
    else if (0xA0 <= ucs2 && ucs2 <= 0xFF)
      r += (char) (ucs2 - 32);
#ifdef SHOW_UNMAPPED_CODES      
    else if (ucs2 < 0xFFFF) //(0xFF < ucs2 && ucs2 < 0xFFFF)
      r += (char) 0x7F;        
#endif      
  }
  return r;
}


// In place conversion of a UTF8 string to extended ASCII string (ASCII is shorter!)
void latin9tocp(char* s) {      
  int k = 0;
  uint16_t ucs2;
  resetUTF8decoder();
  for (int i=0; i<strlen(s); i++) {
    ucs2 = recode(s[i]);
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

