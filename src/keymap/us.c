#include "defs.h"
#include "us.h"

uchar keymap[] = 
  {
    0, ESCKEY, 
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 0, 0,
    HOMEKEY, UPARR, PGUP,'-', LARR, '5', RARR, '+', ENDKEY, DOWNARR, PGDN, INSKEY, DELKEY, 0, 0, 0, F11, F12 
  };

uchar keymap_shift[] = 
  {
    0, ESCKEY, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 0, 0,
    HOMEKEY, UPARR, PGUP,'-', LARR, '5', RARR, '+', ENDKEY, DOWNARR, PGDN, INSKEY, DELKEY, 0, 0, 0, F11, F12 
  };
