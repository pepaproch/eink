/*
 * 
 * clock character set
 * *******************
 * 
 * Suitable for 09:36, 19:45 and 9:36 am, 7:45 pm  
 * 
 */

#define CHARSET_NAME "Clock"

#define MAKE_CHARSET

//#define INCLUDE_ASCII 

struct chardef chartable[] = {
    {0x0030, "0"},
    {0x0031, "1"},
    {0x0032, "2"},
    {0x0033, "3"},
    {0x0034, "4"},
    {0x0035, "5"},
    {0x0036, "6"},
    {0x0037, "7"},
    {0x0038, "8"},
    {0x0039, "9"},
    {0x003a, ":"},
    {0x0061, "a"},
    {0x006d, "m"},
    {0x0070, "p"},
    {0x0020, " "}
};

