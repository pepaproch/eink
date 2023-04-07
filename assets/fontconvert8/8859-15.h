/*
 * 
 * ISO 8859-15 (Latin-9) printable characters
 * ******************************************
 * 
 */

const int charcount = 192;

struct chardef chartable[192] = {
    {  0x0020, " "},
    {  0x0021, "!"},
    {  0x0022, "\""},
    {  0x0023, "#"},
    {  0x0024, "$"},
    {  0x0025, "%"},
    {  0x0026, "&"},
    {  0x0027, "'"},
    {  0x0028, "("},
    {  0x0029, ")"},
    {  0x002a, "*"},
    {  0x002b, "+"},
    {  0x002c, ","},
    {  0x002d, "-"},
    {  0x002e, "."},
    {  0x002f, "/"},
    {  0x0030, "0"},
    {  0x0031, "1"},
    {  0x0032, "2"},
    {  0x0033, "3"},
    {  0x0034, "4"},
    {  0x0035, "5"},
    {  0x0036, "6"},
    {  0x0037, "7"},
    {  0x0038, "8"},
    {  0x0039, "9"},
    {  0x003a, ":"},
    {  0x003b, ";"},
    {  0x003c, "<"},
    {  0x003d, "="},
    {  0x003e, ">"},
    {  0x003f, "?"},
    {  0x0040, "@"},
    {  0x0041, "A"},
    {  0x0042, "B"},
    {  0x0043, "C"},
    {  0x0044, "D"},
    {  0x0045, "E"},
    {  0x0046, "F"},
    {  0x0047, "G"},
    {  0x0048, "H"},
    {  0x0049, "I"},
    {  0x004a, "J"},
    {  0x004b, "K"},
    {  0x004c, "L"},
    {  0x004d, "M"},
    {  0x004e, "N"},
    {  0x004f, "O"},
    {  0x0050, "P"},
    {  0x0051, "Q"},
    {  0x0052, "R"},
    {  0x0053, "S"},
    {  0x0054, "T"},
    {  0x0055, "U"},
    {  0x0056, "V"},
    {  0x0057, "W"},
    {  0x0058, "X"},
    {  0x0059, "Y"},
    {  0x005a, "Z"},
    {  0x005b, "["},
    {  0x005c, "\\"},
    {  0x005d, "]"},
    {  0x005e, "^"},
    {  0x005f, "_"},
    {  0x0060, "`"},
    {  0x0061, "a"},
    {  0x0062, "b"},
    {  0x0063, "c"},
    {  0x0064, "d"},
    {  0x0065, "e"},
    {  0x0066, "f"},
    {  0x0067, "g"},
    {  0x0068, "h"},
    {  0x0069, "i"},
    {  0x006a, "j"},
    {  0x006b, "k"},
    {  0x006c, "l"},
    {  0x006d, "m"},
    {  0x006e, "n"},
    {  0x006f, "o"},
    {  0x0070, "p"},
    {  0x0071, "q"},
    {  0x0072, "r"},
    {  0x0073, "s"},
    {  0x0074, "t"},
    {  0x0075, "u"},
    {  0x0076, "v"},
    {  0x0077, "w"},
    {  0x0078, "x"},
    {  0x0079, "y"},
    {  0x007a, "z"},
    {  0x007b, "{"},
    {  0x007c, "|"},
    {  0x007d, "}"},
    {  0x007e, "~"},
    {  0x007f, "<control> DELETE"},
    {  0x00a0, "NO-BREAK SPACE"},
    {  0x00a1, "INVERTED EXCLAMATION MARK"},
    {  0x00a2, "CENT SIGN"},
    {  0x00a3, "POUND SIGN"},
    {  0x20ac, "EURO SIGN *"},
    {  0x00a5, "YEN SIGN"},
    {  0x0160, "LATIN CAPITAL LETTER S WITH CARON *"},
    {  0x00a7, "SECTION SIGN"},
    {  0x0161, "LATIN SMALL LETTER S WITH CARON *"},
    {  0x00a9, "COPYRIGHT SIGN"},
    {  0x00aa, "FEMININE ORDINAL INDICATOR"},
    {  0x00ab, "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK"},
    {  0x00ac, "NOT SIGN"},
    {  0x00ad, "SOFT HYPHEN"},
    {  0x00ae, "REGISTERED SIGN"},
    {  0x00af, "MACRON"},
    {  0x00b0, "DEGREE SIGN"},
    {  0x00b1, "PLUS-MINUS SIGN"},
    {  0x00b2, "SUPERSCRIPT TWO"},
    {  0x00b3, "SUPERSCRIPT THREE"},
    {  0x017d, "LATIN CAPITAL LETTER Z WITH CARON *"},
    {  0x00b5, "MICRO SIGN"},
    {  0x00b6, "PILCROW SIGN"},
    {  0x00b7, "MIDDLE DOT"},
    {  0x017e, "LATIN SMALL LETTER Z WITH CARON *"},
    {  0x00b9, "SUPERSCRIPT ONE"},
    {  0x00ba, "MASCULINE ORDINAL INDICATOR"},
    {  0x00bb, "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK"},
    {  0x0152, "LATIN CAPITAL LIGATURE OE *"},
    {  0x0153, "LATIN SMALL LIGATURE OE *"},
    {  0x0178, "LATIN CAPITAL LETTER Y WITH DIAERESIS *"},
    {  0x00bf, "INVERTED QUESTION MARK"},
    {  0x00c0, "LATIN CAPITAL LETTER A WITH GRAVE"},
    {  0x00c1, "LATIN CAPITAL LETTER A WITH ACUTE"},
    {  0x00c2, "LATIN CAPITAL LETTER A WITH CIRCUMFLEX"},
    {  0x00c3, "LATIN CAPITAL LETTER A WITH TILDE"},
    {  0x00c4, "LATIN CAPITAL LETTER A WITH DIAERESIS"},
    {  0x00c5, "LATIN CAPITAL LETTER A WITH RING ABOVE"},
    {  0x00c6, "LATIN CAPITAL LETTER AE"},
    {  0x00c7, "LATIN CAPITAL LETTER C WITH CEDILLA"},
    {  0x00c8, "LATIN CAPITAL LETTER E WITH GRAVE"},
    {  0x00c9, "LATIN CAPITAL LETTER E WITH ACUTE"},
    {  0x00ca, "LATIN CAPITAL LETTER E WITH CIRCUMFLEX"},
    {  0x00cb, "LATIN CAPITAL LETTER E WITH DIAERESIS"},
    {  0x00cc, "LATIN CAPITAL LETTER I WITH GRAVE"},
    {  0x00cd, "LATIN CAPITAL LETTER I WITH ACUTE"},
    {  0x00ce, "LATIN CAPITAL LETTER I WITH CIRCUMFLEX"},
    {  0x00cf, "LATIN CAPITAL LETTER I WITH DIAERESIS"},
    {  0x00d0, "LATIN CAPITAL LETTER ETH"},
    {  0x00d1, "LATIN CAPITAL LETTER N WITH TILDE"},
    {  0x00d2, "LATIN CAPITAL LETTER O WITH GRAVE"},
    {  0x00d3, "LATIN CAPITAL LETTER O WITH ACUTE"},
    {  0x00d4, "LATIN CAPITAL LETTER O WITH CIRCUMFLEX"},
    {  0x00d5, "LATIN CAPITAL LETTER O WITH TILDE"},
    {  0x00d6, "LATIN CAPITAL LETTER O WITH DIAERESIS"},
    {  0x00d7, "MULTIPLICATION SIGN"},
    {  0x00d8, "LATIN CAPITAL LETTER O WITH STROKE"},
    {  0x00d9, "LATIN CAPITAL LETTER U WITH GRAVE"},
    {  0x00da, "LATIN CAPITAL LETTER U WITH ACUTE"},
    {  0x00db, "LATIN CAPITAL LETTER U WITH CIRCUMFLEX"},
    {  0x00dc, "LATIN CAPITAL LETTER U WITH DIAERESIS"},
    {  0x00dd, "LATIN CAPITAL LETTER Y WITH ACUTE"},
    {  0x00de, "LATIN CAPITAL LETTER THORN"},
    {  0x00df, "LATIN SMALL LETTER SHARP S"},
    {  0x00e0, "LATIN SMALL LETTER A WITH GRAVE"},
    {  0x00e1, "LATIN SMALL LETTER A WITH ACUTE"},
    {  0x00e2, "LATIN SMALL LETTER A WITH CIRCUMFLEX"},
    {  0x00e3, "LATIN SMALL LETTER A WITH TILDE"},
    {  0x00e4, "LATIN SMALL LETTER A WITH DIAERESIS"},
    {  0x00e5, "LATIN SMALL LETTER A WITH RING ABOVE"},
    {  0x00e6, "LATIN SMALL LETTER AE"},
    {  0x00e7, "LATIN SMALL LETTER C WITH CEDILLA"},
    {  0x00e8, "LATIN SMALL LETTER E WITH GRAVE"},
    {  0x00e9, "LATIN SMALL LETTER E WITH ACUTE"},
    {  0x00ea, "LATIN SMALL LETTER E WITH CIRCUMFLEX"},
    {  0x00eb, "LATIN SMALL LETTER E WITH DIAERESIS"},
    {  0x00ec, "LATIN SMALL LETTER I WITH GRAVE"},
    {  0x00ed, "LATIN SMALL LETTER I WITH ACUTE"},
    {  0x00ee, "LATIN SMALL LETTER I WITH CIRCUMFLEX"},
    {  0x00ef, "LATIN SMALL LETTER I WITH DIAERESIS"},
    {  0x00f0, "LATIN SMALL LETTER ETH"},
    {  0x00f1, "LATIN SMALL LETTER N WITH TILDE"},
    {  0x00f2, "LATIN SMALL LETTER O WITH GRAVE"},
    {  0x00f3, "LATIN SMALL LETTER O WITH ACUTE"},
    {  0x00f4, "LATIN SMALL LETTER O WITH CIRCUMFLEX"},
    {  0x00f5, "LATIN SMALL LETTER O WITH TILDE"},
    {  0x00f6, "LATIN SMALL LETTER O WITH DIAERESIS"},
    {  0x00f7, "DIVISION SIGN"},
    {  0x00f8, "LATIN SMALL LETTER O WITH STROKE"},
    {  0x00f9, "LATIN SMALL LETTER U WITH GRAVE"},
    {  0x00fa, "LATIN SMALL LETTER U WITH ACUTE"},
    {  0x00fb, "LATIN SMALL LETTER U WITH CIRCUMFLEX"},
    {  0x00fc, "LATIN SMALL LETTER U WITH DIAERESIS"},
    {  0x00fd, "LATIN SMALL LETTER Y WITH ACUTE"},
    {  0x00fe, "LATIN SMALL LETTER THORN"},
    {  0x00ff, "LATIN SMALL LETTER Y WITH DIAERESIS"}
};
