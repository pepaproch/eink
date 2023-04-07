/*
 *
 * ISO 8859-2 (Latin-2) printable characters
 * ******************************************
 *
 */
#define CHARSET_NAME "CS"

#define MAKE_CHARSET

//#define INCLUDE_ASCII

const int charcount = 1;

struct chardef chartable[2] = {
        {0x007f, "<CONTROL> DEL"},
        {0x017D,  "LATIN CAPITAL LETTER Z WITH CARON" },
};
