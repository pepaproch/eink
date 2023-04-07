/* fontconvert8x.c
TrueType to Adafruit_GFX font converter.

Currently this utility extracts up to 224 (256-32) printable glyphs from
a TrueType font and outputs to stdout a corresponding GFXfont header
file that be used with the Adafruit_GFX Arduino library and other
compatible display libraries.

To create a GFX font
1. Adjust display resolution if needed in line 73 below

2. Create a chartable header file such as clock.h
   Other included example files: fr.h, latin1.h, latin9.h

3. Ensure that the CHARSET_NAME macro is correctly defined in that
   file and that the MAKE_CHARSET and INCLUDE_ASCII macros are
   include or not as required

4. Include the chartable header file in line 89 or so below

5. Compile this modified source code. For UNIX-like systems this
   can be done with make:

   ~$ make -f Makefile8x

   which will create the executable, fontconvert8x, in the same directory.

6. Convert the desired TTF font file to GFXFont of desired size

    ~$ ./fontconvert8x /usr/share/fonts/truetype/freefont/FreeMono.ttf 12 > FreeMono12ptClock.h


Derived from but NOT a replacement for Adafruit fontconvert.c
See <https://sigmdel.ca/michel/program/misc/xxxxxxxxxx> for          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXxx
details, Michel Deslierres 2020-10-17
*/

/* fontconvert.c (original notice)

TrueType to Adafruit_GFX font converter.  Derived from Peter Jakobs'
Adafruit_ftGFX fork & makefont tool, and Paul Kourany's Adafruit_mfGFX.

NOT AN ARDUINO SKETCH.  This is a command-line tool for preprocessing
fonts to be used with the Adafruit_GFX Arduino library.

For UNIX-like systems.  Outputs to stdout; redirect to header file, e.g.:
  ./fontconvert ~/Library/Fonts/FreeSans.ttf 18 > FreeSans18pt7b.h

REQUIRES FREETYPE LIBRARY.  www.freetype.org

Currently this only extracts the printable 7-bit ASCII chars of a font.
Will eventually extend with some int'l chars a la ftGFX, not there yet.
Keep 7-bit fonts around as an option in that case, more compact.

See notes at end for glyph nomenclature & other tidbits.
*/

#ifndef ARDUINO

#include <ctype.h>
#include <ft2build.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include FT_GLYPH_H
#include FT_MODULE_H
#include FT_TRUETYPE_DRIVER_H
#include "gfxfont.h" // Adafruit_GFX font structures

// Structure of chartable used to define the characters to be
// included in the font character set. For each character
// set ucode to 16 bit Unicode code point and name to a description
// example:  latin 1 character é: U+00E9 LATIN SMALL LETTER E WITH ACUTE
// could have entry
//    ucode = 0x00E9
//    name = "LATIN SMALL LETTER E WITH ACUTE"
//
#define NAME_SIZE 66
struct chardef {
  uint16_t ucode;
  char name[NAME_SIZE];
};

// **** <USER DEFINED OPTIONS> ****

#define DPI 141 // Approximate res. of Adafruit 2.8" TFT

// Include the file containing the definition of the chardef chartable[]
//#include "clock.h"
//#include "clockalt.h"
//#include "fr.h"
#include "latin1.h"
//#include "latin9.h"

// **** </USER DEFINED OPTIONS> ****

// Include ASCII chartable if INCLUDE_ASCII is defined in previous file
#include "ascii.h"

// Accumulate bits for output, with periodic hexadecimal byte write
void enbit(uint8_t value) {
  static uint8_t row = 0, sum = 0, bit = 0x80, firstCall = 1;
  if (value)
    sum |= bit;          // Set bit if needed
  if (!(bit >>= 1)) {    // Advance to next bit, end of byte reached?
    if (!firstCall) {    // Format output table nicely
      if (++row >= 12) { // Last entry on line?
        printf(",\n  "); //   Newline format output
        row = 0;         //   Reset row counter
      } else {           // Not end of line
        printf(", ");    //   Simple comma delim
      }
    }
    printf("0x%02X", sum); // Write byte value
    sum = 0;               // Clear for next byte
    bit = 0x80;            // Reset bit counter
    firstCall = 0;         // Formatting flag
  }
}

int main(int argc, char *argv[]) {
  int i, err, size, bitmapOffset = 0;
  int x, y, byte;
  uint16_t code;
  char *fontName, c, *ptr;
  FT_Library library;
  FT_Face face;
  FT_Glyph glyph;
  FT_Bitmap *bitmap;
  FT_BitmapGlyphRec *g;
  GFXglyph *table;
  uint8_t bit;

  int asciicount = 95;
  if (sizeof(asciitable) / sizeof (asciitable[0]) != asciicount) {
	 fprintf(stderr, "asciitable[] in ascii.h does not containt %d caracters", asciicount);
	 return 1;
  }
  #ifndef INCLUDE_ASCII
  asciicount = 0;
  #endif

  int charcount = sizeof(chartable) / sizeof (chartable[0]);
  int totalcount = charcount + asciicount;

  // No more than 2^8 characters less 32 control codes in a GFXfont
  if (totalcount > 224) {
	 fprintf(stderr, "Too many characters (%d > 224) in the %s character set\n", totalcount, CHARSET_NAME);
	 return 1;
  }

  // Define first. By default and always if the ascci characters are
  // loaded first = 32. However, if not all ascii characters are loaded
  // then first can be the Unicode of the first character in chartable[]
  // as long as both first and last < 256.
  int first = 0x20;

  if (!asciicount && totalcount + chartable[0].ucode - 1 < 256)
    first = chartable[0].ucode;

  int last = first + totalcount - 1;

  // Parse command line.  Valid syntaxes are:
  //   fontconvert filename size
  if ((argc < 3) || (argc > 3)) {
    fprintf(stderr, "Usage: %s fontfile size\n", argv[0]);
    return 1;
  }

  size = atoi(argv[2]);

  ptr = strrchr(argv[1], '/'); // Find last slash in filename
  if (ptr)
    ptr++; // First character of filename (path stripped)
  else
    ptr = argv[1]; // No path; font in local dir.

  // Allocate space for font name and glyph table
  if ((!(fontName = malloc(strlen(ptr) + strlen(CHARSET_NAME) + 20))) ||
      (!(table = (GFXglyph *)malloc(totalcount * sizeof(GFXglyph))))) {
    fprintf(stderr, "Malloc error\n");
    return 1;
  }

  // Derive font table names from filename.  Period (filename
  // extension) is truncated and replaced with the font size & bits.
  strcpy(fontName, ptr);
  ptr = strrchr(fontName, '.'); // Find last period (file ext)
  if (!ptr)
    ptr = &fontName[strlen(fontName)]; // If none, append

  // Insert font size and charset name. fontName was alloc'd w/extra
  // space to allow this, we're not sprintfing into Forbidden Zone.
  sprintf(ptr, "%dpt%s", size, CHARSET_NAME);

  // Space and punctuation chars in name replaced w/ underscores.
  for (i = 0; (c = fontName[i]); i++) {
    if (isspace(c) || ispunct(c))
      fontName[i] = '_';
  }

  // Init FreeType lib, load font
  if ((err = FT_Init_FreeType(&library))) {
    fprintf(stderr, "FreeType init error: %d", err);
    return err;
  }

  // Use TrueType engine version 35, without subpixel rendering.
  // This improves clarity of fonts since this library does not
  // support rendering multiple levels of gray in a glyph.
  // See https://github.com/adafruit/Adafruit-GFX-Library/issues/103
  FT_UInt interpreter_version = TT_INTERPRETER_VERSION_35;
  FT_Property_Set(library, "truetype", "interpreter-version",
                  &interpreter_version);

  if ((err = FT_New_Face(library, argv[1], 0, &face))) {
    fprintf(stderr, "Font load error: %d", err);
    FT_Done_FreeType(library);
    return err;
  }

  // << 6 because '26dot6' fixed-point format
  FT_Set_Char_Size(face, size << 6, 0, DPI, 0);

  // Currently all symbols from 'first' to 'last' are processed.
  // Fonts may contain WAY more glyphs than that, but this code
  // will need to handle encoding stuff to deal with extracting
  // the right symbols, and that's not done yet.
  // fprintf(stderr, "%ld glyphs\n", face->num_glyphs);

  printf("const uint8_t %sBitmaps[] PROGMEM = {\n  ", fontName);

  //printf("\n\nasciicount: %d, charcount: %d, totalcount: %d\n", asciicount, charcount, totalcount);

  // Process glyphs and output huge bitmap data array
  for (i = 0; i < totalcount; i++) {
	 if (i < asciicount)
	   code = asciitable[i].ucode;
	 else
	   code = chartable[i-asciicount].ucode;

	 //printf("\n\n----i: %d, j: %d, inAscii: %s, code: 0x%x04", i, j, (inAscii) ? "true" : "false", code);

    // MONO renderer provides clean image with perfect crop
    // (no wasted pixels) via bitmap struct.
    if ((err = FT_Load_Char(face, code, FT_LOAD_TARGET_MONO))) {
      fprintf(stderr, "Error %d loading char U+%04x\n", err, chartable[i].ucode);
      continue;
    }

    if ((err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO))) {
      fprintf(stderr, "Error %d rendering char U+%04x\n", err, chartable[i].ucode);
      continue;
    }

    if ((err = FT_Get_Glyph(face->glyph, &glyph))) {
      fprintf(stderr, "Error %d getting glyph U+%04x\n", err, chartable[i].ucode);
      continue;
    }

    bitmap = &face->glyph->bitmap;
    g = (FT_BitmapGlyphRec *)glyph;

    // Minimal font and per-glyph information is stored to
    // reduce flash space requirements.  Glyph bitmaps are
    // fully bit-packed; no per-scanline pad, though end of
    // each character may be padded to next byte boundary
    // when needed.  16-bit offset means 64K max for bitmaps,
    // code currently doesn't check for overflow.  (Doesn't
    // check that size & offsets are within bounds either for
    // that matter...please convert fonts responsibly.)
    table[i].bitmapOffset = bitmapOffset;
    table[i].width = bitmap->width;
    table[i].height = bitmap->rows;
    table[i].xAdvance = face->glyph->advance.x >> 6;
    table[i].xOffset = g->left;
    table[i].yOffset = 1 - g->top;

    for (y = 0; y < bitmap->rows; y++) {
      for (x = 0; x < bitmap->width; x++) {
        byte = x / 8;
        bit = 0x80 >> (x & 7);
        enbit(bitmap->buffer[y * bitmap->pitch + byte] & bit);
      }
    }

    // Pad end of char bitmap to next byte boundary if needed
    int n = (bitmap->width * bitmap->rows) & 7;
    if (n) {     // Pixel count not an even multiple of 8?
      n = 8 - n; // # bits to next multiple
      while (n--)
        enbit(0);
    }
    bitmapOffset += (bitmap->width * bitmap->rows + 7) / 8;

    FT_Done_Glyph(glyph);
  }

  printf(" };\n\n"); // End bitmap array

  // Output glyph attributes table (one per character)
  printf("const GFXglyph %sGlyphs[] PROGMEM = {\n", fontName);
  for (i = 0; i < totalcount; i++) {
    printf("  { %5d, %3d, %3d, %3d, %4d, %4d }",
           table[i].bitmapOffset,
           table[i].width,
           table[i].height,
           table[i].xAdvance,
           table[i].xOffset,
           table[i].yOffset);
    /*
    if (i < charcount-1) {
      printf(",   // 0x%02x '%s' U+%04X\n", i+first, chartable[i].name, chartable[i].ucode);
    }
    */

    if (i < totalcount-1) {
	  if (i < asciicount)
        printf(",   // 0x%02x '%s' U+%04X\n", i+first, asciitable[i].name, asciitable[i].ucode);
	  else
        printf(",   // 0x%02x '%s' U+%04X\n", i+first, chartable[i-asciicount].name, chartable[i-asciicount].ucode);
    }
  }

  /*
  printf(" }; // 0x%02x '%s' U+%04X\n\n", charcount+first-1, chartable[charcount-1].name, chartable[charcount-1].ucode);
  */
  if (i < asciicount)
    printf(" }; // 0x%02x '%s' U+%04X\n\n", totalcount+first-1, asciitable[totalcount-1].name, asciitable[totalcount-1].ucode);
  else
    printf(" }; // 0x%02x '%s' U+%04X\n\n", totalcount+first-1, chartable[totalcount-1-asciicount].name, chartable[totalcount-1-asciicount].ucode);


  // Output font structure
  printf("const GFXfont %s PROGMEM = {\n", fontName);
  printf("  (uint8_t  *)%sBitmaps,\n", fontName);
  printf("  (GFXglyph *)%sGlyphs,\n", fontName);
  if (face->size->metrics.height == 0) {
    // No face height info, assume fixed width and get from a glyph.
    printf("  0x%02X, 0x%02X, %d };\n\n", first, last, table[0].height);
  } else {
    printf("  0x%02X, 0x%02X, %ld };\n\n", first, last,
           face->size->metrics.height >> 6);
  }
  printf("// Approx. %d bytes\n", bitmapOffset + (last - first + 1) * 7 + 7);
  // Size estimate is based on AVR struct and pointer sizes;
  // actual size may vary.

  FT_Done_FreeType(library);

#ifdef MAKE_CHARSET
  printf("\n\n");
  printf("// Only one charset table is needed when more\n");
  printf("// than one font with the same encoding is used\n\n");
  printf("#ifndef CHARSET_"CHARSET_NAME"\n");
  printf("#define CHARSET_"CHARSET_NAME"\n\n");

  printf("const uint8_t %sCharcount = %d;\n", CHARSET_NAME, totalcount-asciicount);
  //printf("//const uint16_t encoding[%d] = {  // for inclusion in encode.cpp\n", totalcount-asciicount);
  printf("const uint16_t %sCharset[%d] PROGMEM = {\n", CHARSET_NAME, totalcount-asciicount);

  #ifdef INCLUDE_ASCII
  for (i = asciicount; i < totalcount-1; i++) {
  #else
  for (i = 0; i < totalcount-1; i++) {
  #endif
    printf("  0x%04x,  // [0x%02x] '%s'\n", (i < asciicount) ? asciitable[i].ucode : chartable[i-asciicount].ucode,
                       i+first, (i < asciicount) ? asciitable[i].name : chartable[i-asciicount].name);
  }
  i = totalcount-1;
  printf("  0x%04x}; // [0x%02x] '%s'\n\n",
    (i < asciicount) ? asciitable[i].ucode : chartable[i-asciicount].ucode,
    i+first,
    (i < asciicount) ? asciitable[i].name : chartable[i-asciicount].name);
  printf("#endif\n");
#endif

  return 0;
}

/* -------------------------------------------------------------------------

Character metrics are slightly different from classic GFX & ftGFX.
In classic GFX: cursor position is the upper-left pixel of each 5x7
character; lower extent of most glyphs (except those w/descenders)
is +6 pixels in Y direction.
W/new GFX fonts: cursor position is on baseline, where baseline is
'inclusive' (containing the bottom-most row of pixels in most symbols,
except those with descenders; ftGFX is one pixel lower).

Cursor Y will be moved automatically when switching between classic
and new fonts.  If you switch fonts, any print() calls will continue
along the same baseline.

                    ...........#####.. -- yOffset
                    ..........######..
                    ..........######..
                    .........#######..
                    ........#########.
   * = Cursor pos.  ........#########.
                    .......##########.
                    ......#####..####.
                    ......#####..####.
       *.#..        .....#####...####.
       .#.#.        ....##############
       #...#        ...###############
       #...#        ...###############
       #####        ..#####......#####
       #...#        .#####.......#####
====== #...# ====== #*###.........#### ======= Baseline
                    || xOffset

glyph->xOffset and yOffset are pixel offsets, in GFX coordinate space
(+Y is down), from the cursor position to the top-left pixel of the
glyph bitmap.  i.e. yOffset is typically negative, xOffset is typically
zero but a few glyphs will have other values (even negative xOffsets
sometimes, totally normal).  glyph->xAdvance is the distance to move
the cursor on the X axis after drawing the corresponding symbol.

There's also some changes with regard to 'background' color and new GFX
fonts (classic fonts unchanged).  See Adafruit_GFX.cpp for explanation.
*/

#endif /* !ARDUINO */
