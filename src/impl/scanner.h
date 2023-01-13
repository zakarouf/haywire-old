#ifndef ZAKAROUF_HAYWIRE_SCANNER_H
#define ZAKAROUF_HAYWIRE_SCANNER_H

#include "../core/base.h"
#include "token.h"

typedef struct hw_Scanner {
    char const *end, *current, *start;
    hw_uint line;
} hw_Scanner;

void hw_Scanner_new(hw_Scanner *s, z__Str const source);
void hw_Scanner_delete(hw_Scanner *s);
hw_Token hw_Scanner_scan(hw_Scanner *s);
hw_Token hw_Scanner_scan_skipws(hw_Scanner *s);
hw_Token hw_Scanner_scan_until(hw_Scanner *scanner, hw_TokenType tokt);

#define hw_Scanner_isAtEnd(s) ((s)->current[0] == '\0' || (s)->current == (s)->end)

#endif // !ZAKAROUF_HAYWIRE_SCANNER_H

