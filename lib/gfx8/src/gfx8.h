/*
 * Converting UTF-8 encoded strings to GFX Latin 1 encoded strings
 * 
 * The GFX Latin 1 encoding is described in 
 *   https://sigmdel.ca/michel/program/misc/gfxfont_8bit_en.html#latin-x
 * 
 * 
 * To display the converted string, an Adafruit GFXfont containing the 192 
 * printable ISO 8859-1 characters corresponding to the GFX Latin 1 character 
 * set must be used. See fontconvert8 to obtain such a font from TTF fonts
 *   https://sigmdel.ca/michel/program/misc/gfxfont_8bit_en.html#fontconvert8
 * 
 */

#ifndef GFX8_H
#define GFX8_H

typedef struct gfx8charset_struct {
    uint16_t* charset;   // array of Unicodes of glyphs in the font
    uint16_t charcount;  // number of Unicodes
    uint16_t first;      // Unicode of the first glyph
    uint8_t asciicount;  // number of consecutive ASCII codes starting at first 
} gfx8charset_t ;

extern gfx8charset_t* defaultCharset;

// Initialize a gfx8charset_t structure with data needed to convert UTF-8 encoded strings
// to GFX encoded strings using one of the utf8tocp() functions.
// The first version intializes the defaut structure, the second a given structure.
void initGfx8charset(uint16_t* charset, uint16_t charcount, uint16_t first = 0x20, uint8_t lastascii = 0);
void initGfx8charset(gfx8charset_t* gfxcharset, uint16_t* charset, uint16_t charcount, uint16_t first = 0x20, uint8_t lastascii = 0);


// Convert a UTF-8 encoded String object to a GFX encoded String where the target 
// encoding is described by gfx8charset.
String utf8tocp(String s, gfx8charset_t* gfx8charset = defaultCharset);

// Convert a UTF-8 encoded string to a GFX encoded string where the target encoding
// is described by gfx8charset. 
// Be careful, the in-situ conversion will "destroy" the UTF-8 string s.
void utf8tocp(char* s, gfx8charset_t* gfx8charset = defaultCharset);    

#endif
