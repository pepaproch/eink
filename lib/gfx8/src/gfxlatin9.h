/*
 * Converting UTF-8 encoded strings to GFX Latin 9 encoded strings
 * 
 * The GFX Latin 9 encoding is described in 
 *   https://sigmdel.ca/michel/program/misc/gfxfont_8bit_02_en.html
 * 
 * 
 * To display the converted string, an Adafruit GFXfont containing the 192 
 * printable ISO 8859-15 characters corresponding to the GFX Latin 9 character 
 * set must be used. See fontconvert8 to obtain such a font from TTF fonts
 *   https://sigmdel.ca/michel/program/misc/gfxfont_8bit_en.html#fontconvert8
 * 
 */

#ifndef GFXLATIN9_H
#define GFXLATIN9_H

// Convert a UTF-8 encoded String object to a GFX Latin 1 encoded String
String latin9tocp(String s);

// Convert a UTF-8 encoded string to a GFX Latin 9 encoded string 
// Be careful, the in-situ conversion will "destroy" the UTF-8 string s.
void latin9tocp(char* s);    

#endif
