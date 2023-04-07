
#ifndef GFX88592_H
#define GFX88592_H

// Convert a UTF-8 encoded String object to a GFX Latin 1 encoded String
String c_88592_tocp(String s);

// Convert a UTF-8 encoded string to a GFX Latin 9 encoded string
// Be careful, the in-situ conversion will "destroy" the UTF-8 string s.
void c_88592_tocp(char* s);

#endif
