/* convert8x.cpp
 *
 * Routine to encode string literals with non ASCII characters
 * using encoding generated with fontconvet8.c
 *
 * Used with Adafruit GFXFonts generated with fontconvert8x.c
 *
 * Michel Deslierres <https://sigmdel.ca/michel>
 * November 23, 2020
 *
 * A version of this source will need to be compiled for each
 * GFX charset used in the sketch.
 *
 * 1. Use fontconvert8 to create a GFX font and include the
 *    #define MAKE_CHARSET line so that a charset is generated
 *    such as
 *
 *    const uint8_t ClockCharcount = 15;
 *    const uint16_t ClockCharset[15] PROGMEM = {
 *      0x0030,  // [0x30] '0'
 *      0x0031,  // [0x31] '1'
 *      0x0032,  // [0x32] '2'
 *      0x0033,  // [0x33] '3'
 *      0x0034,  // [0x34] '4'
 *      0x0035,  // [0x35] '5'
 *      0x0036,  // [0x36] '6'
 *      0x0037,  // [0x37] '7'
 *      0x0038,  // [0x38] '8'
 *      0x0039,  // [0x39] '9'
 *      0x003a,  // [0x3a] ':'
 *      0x0061,  // [0x3b] 'a'
 *      0x006d,  // [0x3c] 'm'
 *      0x0070,  // [0x3d] 'p'
 *      0x0020}; // [0x3e] ' '
 *
 * 2. Remove the leading encodings that are consecutive ASCII
 *    codes from the list, remove the line containing the
 *    Charcount, the size of the charset and PROGMEM and rename
 *    the array encoding
 *
 *     const uint16_t encoding[] = {
 *       0x0061,  // [0x3b] 'a'
 *       0x006d,  // [0x3c] 'm'
 *       0x0070,  // [0x3d] 'p'
 *       0x0020}; // [0x3e] ' '
 *
 * 3. Place this array in the source below at around line 78.
 *
 * 4. Adjust the value of firstcp just above in line 76.
 *    It should be equal to the GFX code point of the first
 *    glyph in the encoding array. This is shown in [ ]
 *
 *    const uint8_t firstcp = 0x3b;
 *
 * 5. Compile
 *    In Linux
 *
 *     ..$ g++ -Wall convert8x.cpp -o convert8x_clock
 *
 *    or use make (see Makefile) and rename the executable after
 *
 *     ...$ make
 *     ...$ mv convert8x convert8x_clock
 *
 */

// SPDX-License-Identifier: 0BSD

#ifndef ARDUINO

#include <codecvt>
#include <locale>
#include <string>
#include <iostream>

const uint8_t firstcp = 0x7f;

const uint16_t encoding[] = {
        0x00ab,  // [0x7f] 'LEFT-POINTING DOUBLE ANGLE QUOTATION MARK'
        0x00b0,  // [0x80] 'DEGREE SIGN'
        0x00bb,  // [0x81] 'RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK'
        0x00c0,  // [0x82] 'LATIN CAPITAL LETTER A WITH GRAVE'
        0x00c2,  // [0x83] 'LATIN CAPITAL LETTER A WITH CIRCUMFLEX'
        0x00c4,  // [0x84] 'LATIN CAPITAL LETTER A WITH DIAERESIS'
        0x00c6,  // [0x85] 'LATIN CAPITAL LETTER AE'
        0x00c7,  // [0x86] 'LATIN CAPITAL LETTER C WITH CEDILLA'
        0x00c8,  // [0x87] 'LATIN CAPITAL LETTER E WITH GRAVE'
        0x00c9,  // [0x88] 'LATIN CAPITAL LETTER E WITH ACUTE'
        0x00ca,  // [0x89] 'LATIN CAPITAL LETTER E WITH CIRCUMFLEX'
        0x00cb,  // [0x8a] 'LATIN CAPITAL LETTER E WITH DIAERESIS'
        0x00ce,  // [0x8b] 'LATIN CAPITAL LETTER I WITH CIRCUMFLEX'
        0x00cf,  // [0x8c] 'LATIN CAPITAL LETTER I WITH DIAERESIS'
        0x00d4,  // [0x8d] 'LATIN CAPITAL LETTER O WITH CIRCUMFLEX'
        0x00d6,  // [0x8e] 'LATIN CAPITAL LETTER O WITH DIAERESIS'
        0x00d9,  // [0x8f] 'LATIN CAPITAL LETTER U WITH GRAVE'
        0x00db,  // [0x90] 'LATIN CAPITAL LETTER U WITH CIRCUMFLEX'
        0x00dc,  // [0x91] 'LATIN CAPITAL LETTER U WITH DIAERESIS'
        0x00e0,  // [0x92] 'LATIN SMALL LETTER A WITH GRAVE'
        0x00e2,  // [0x93] 'LATIN SMALL LETTER A WITH CIRCUMFLEX'
        0x00e4,  // [0x94] 'LATIN SMALL LETTER A WITH DIAERESIS'
        0x00e6,  // [0x95] 'LATIN SMALL LETTER AE'
        0x00e7,  // [0x96] 'LATIN SMALL LETTER C WITH CEDILLA'
        0x00e8,  // [0x97] 'LATIN SMALL LETTER E WITH GRAVE'
        0x00e9,  // [0x98] 'LATIN SMALL LETTER E WITH ACUTE'
        0x00ea,  // [0x99] 'LATIN SMALL LETTER E WITH CIRCUMFLEX'
        0x00eb,  // [0x9a] 'LATIN SMALL LETTER E WITH DIAERESIS'
        0x00ee,  // [0x9b] 'LATIN SMALL LETTER I WITH CIRCUMFLEX'
        0x00ef,  // [0x9c] 'LATIN SMALL LETTER I WITH DIAERESIS'
        0x00f4,  // [0x9d] 'LATIN SMALL LETTER O WITH CIRCUMFLEX'
        0x00f6,  // [0x9e] 'LATIN SMALL LETTER O WITH DIAERESIS'
        0x00f9,  // [0x9f] 'LATIN SMALL LETTER U WITH GRAVE'
        0x00fb,  // [0xa0] 'LATIN SMALL LETTER U WITH CIRCUMFLEX'
        0x00fc,  // [0xa1] 'LATIN SMALL LETTER U WITH DIAERESIS'
        0x00ff,  // [0xa2] 'LATIN SMALL LETTER Y WITH DIAERESIS'
        0x0152,  // [0xa3] 'LATIN CAPITAL LIGATURE OE'
        0x0153,  // [0xa4] 'LATIN SMALL LIGATURE OE'
        0x0178,  // [0xa5] 'LATIN CAPITAL LETTER Y WITH DIAERESIS'
        0x20ac}; // [0xa6] 'EURO SIGN'

const int count = sizeof(encoding) / sizeof(uint16_t);

uint8_t recode(char16_t u) {
    for (int i=0; i<count; i++) {
        if (u == encoding[i])
            return firstcp + i;
    }
    return 0;
}

void sendout(std::string const & utf8str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > conv2utf8;
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv2utf16;
    std::u16string utf16str = conv2utf16.from_bytes(utf8str);
    bool inquote = false;
    bool incomment = false;
    for (uint i=0; i<utf16str.length(); i++) {
        char16_t u = utf16str[i];
        if (inquote) {
            if (u == '\\') {
                switch (utf16str[i+1]) {
                    case '\\': i++; break;
                    case '"': u = '"'; i++; break;
                    case 'n': printf("\\n"); i++; continue; break;
                    case 't': printf("\\t"); i++; continue; break;
                    default : printf("invalid escape sequence"); return; break;
                }
            } else if (u == '"') {
                inquote = false;
                putchar('"');
                continue;
            }
            uint8_t c = recode(u);
            if (c) {
                printf("\\x%02x", c); // replace the char with GFX encoding
                continue;
            }
            if (u > 0x7f)
                continue;    // ignore non ASCII code not in table
            // fallthrough if u is ASCII
        }
        if (u == 0x0a) {
            incomment = false;
        } else if (u == '/' &&  utf16str[i+1] == '/') {
            incomment = true;
        } else if (u == '"' && !incomment) {
            inquote = true;
        }
        //putchar((char) u); will not work with utf-8 chars outside of quotes !!
        std::string bs = conv2utf8.to_bytes(u);  // convert back to UTF-8
        std::cout << bs;
    }
}


int main(int argc, char ** argv) {
    for (std::string line; std::getline(std::cin, line);) {
        sendout(line);
        std::cout << std::endl;
    }
    return 0;
}

#endif
