#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <z_/prep/map.h>
#include <z_/prep/args.h>

#include "scanner.h"
#include "debug.h"

#define _peek(s) (*(s)->current)
#define _make_token(t, s) hw_Scanner_make_token(hw_TokenType(t), s)
#define _make_token_err(s, w) hw_Scanner_make_token_error(s, z__Str(w, sizeof(w)-1))
#define _make_token_not_found(s) hw_Scanner_make_token_not_found(s)
#define _start(s) hw_Scanner_start(s)
#define _match_char(s, ch) hw_Scanner_match_char(s, ch)
#define _advance(s) hw_Scanner_advance(s)
#define _scan_as(W, s) zpp__CAT(hw_Scanner_scan_as_, W)(s)

void hw_Scanner_new(hw_Scanner *s, z__Str const src)
{
    s->end = src.data + src.len;
    s->current = src.data;
    s->start = src.data;
    s->line = 1;
}

void hw_Scanner_delete(hw_Scanner *s)
{
    memset(s, 0, sizeof(*s));
}

static hw_Token hw_Scanner_make_token(hw_TokenType tt, hw_Scanner const *s)
{
    return (hw_Token) {
         .type = tt
        ,.start = s->start
        ,.len = s->current - s->start
        ,.line = s->line
    };
}

static hw_Token hw_Scanner_make_token_error(hw_Scanner const *s, z__Str const what)
{
    return (hw_Token) {
        .type = hw_TokenType(ERROR)
       ,.start = what.data
       ,.len = what.len
       ,.line = s->line
    };
}

static hw_Token hw_Scanner_make_token_not_found(hw_Scanner *s)
{
    return (hw_Token) {
        .type = hw_TokenType(NOT_FOUND)
       ,.start = s->start
       ,.len = 1
       ,.line = s->line
    };
}

static inline char hw_Scanner_advance(hw_Scanner *s)
{
    s->current += 1;
    return s->current[-1];
}

static inline const char *hw_Scanner_start(hw_Scanner *s)
{
    s->start = s->current;
    return s->start;
}

static hw_byte hw_Scanner_match_char(hw_Scanner *s, char const ch)
{
    if(hw_Scanner_isAtEnd(s)
    || *s->current != ch) return false;

    s->current += 1;
    return true;
}

static hw_Token hw_Scanner_scan_as_literal_string(hw_Scanner *s)
{
    #define peek() _peek(s)
    if(s->start[0] == '"') {
        while(peek() != '"' && !hw_Scanner_isAtEnd(s)) {
            if(peek() == '\n') s->line ++;
            _advance(s);
        }
    } else {
        return _make_token_not_found(s);
    }

    #undef peek

    if(hw_Scanner_isAtEnd(s)) {
        return _make_token_err(s, "Unterminated String");
    }

    _advance(s);
    return _make_token(STRING, s);
}

static inline hw_byte _isDigit(char c) { return c >= '0' && c <= '9'; }

static hw_Token hw_Scanner_scan_as_literal_number(hw_Scanner *s)
{
    if(!_isDigit(s->start[0])) {
        return _make_token_not_found(s);
    }
    
    hw_byte dot = 0;

    if(_peek(s) == '.') {
        dot = 1;
        _advance(s);
    }

    while(_isDigit(_peek(s))) {
        _advance(s);
        if(_peek(s) == '.') {
            if(dot == 1) {
                return _make_token(FLOAT, s);
            } else {
                dot = 1;
            }
        }
    }

    if(dot) return _make_token(FLOAT, s);
    else    return _make_token(NUMBER, s);
}

static hw_Token hw_Scanner_scan_as_literal(hw_Scanner *s)
{
    hw_Token token = hw_Scanner_scan_as_literal_string(s);    
    if(hw_Token_isNot(token, NOT_FOUND)) return token;
   
    token = hw_Scanner_scan_as_literal_number(s);
    if(hw_Token_isNot(token, NOT_FOUND)) return token;
    
    return _make_token_not_found(s);
}

static hw_TokenType checkKeyword(hw_uint skip, char const *with, hw_uint with_len, hw_Token token, hw_TokenType type) {
    if(with_len + skip == token.len
    && memcmp(with, token.start + skip, sizeof(*with) * with_len) == 0) {
        return type;
    }
    return hw_TokenType(SYMBOL);
}
#define checkKeyword(skip, str, tok, type) checkKeyword(skip, str, sizeof(str)-1, tok, type)

static hw_TokenType hw_Scanner_promote_symbol_to_keyword(hw_Token token)
{
    char const *c = token.start;
    switch (c[0]) {
        #define checkK(c, str, tokenType) case c: return checkKeyword(1, str, token, hw_TokenType(tokenType)); break
        #define ccase(w) case w

        checkK('l', "et", LET);
        checkK('a', "nd", AND);
        checkK('o', "r", OR);
        checkK('t', "rue", TRUE);
        checkK('i', "f", IF);
        checkK('w', "hile", WHILE);
        checkK('r', "eturn", RETURN);

        ccase('e'): {
            if(token.len == 4 && c[1] == 'l') {
                if(c[2] == 's' && c[3] == 'e') {
                    return hw_TokenType(ELSE);
                } else if(c[2] == 'i' && c[3] == 'f') {
                    return hw_TokenType(ELIF);
                } else {
                    return hw_TokenType(SYMBOL);
                }
            }
        } break;

        ccase('f'): {
            if(token.len > 1) {
                switch (c[1]) {
                    ccase('a'): return checkKeyword(2, "lse", token, hw_TokenType(FALSE));
                    ccase('o'): return checkKeyword(2, "r", token, hw_TokenType(FOR));
                    ccase('n'): return hw_TokenType(FUNCTION);
                }
            }
        } break;
    }
    return hw_TokenType(SYMBOL);
}

static hw_Token hw_Scanner_scan_as_symbol(hw_Scanner *s)
{
    if(isalpha(s->start[0]) || s->start[0] == '_') {
    } else {
        return _make_token_not_found(s);
    }

    while(isalnum(_peek(s)) || _peek(s) == '_') {_advance(s);}

    hw_Token token = _make_token(SYMBOL, s);
    token.type = hw_Scanner_promote_symbol_to_keyword(token);
    return token;
}

static hw_Token hw_Scanner_scan_as_char(hw_Scanner *s)
{
    switch (s->start[0]) {

        /* Single Char Case */
        #define scc(ch, Token) break; case ch: return _make_token(Token, s); break
        scc(' ', SPACE);
        scc('\t', TABSPACE);
        case '\n': {
            s->line += 1; 
            return _make_token(NEWLINE, s);
        } break;

        scc(EOF, END_OF_SOURCE);

        scc('(', PAREN_LEFT);
        scc(')', PAREN_RIGHT);
        scc('{', BRACE_LEFT);
        scc('}', BRACE_RIGHT);
        scc('[', SQR_BRACE_LEFT);
        scc(']', SQR_BRACE_RIGHT);

        scc(',', COMMA);
        scc('.', DOT);
        scc(':', COLON);
        scc(';', SEMI_COLON);
        scc('@', AT);

        scc('\\', BACK_SLASH);
    
        /**/

        #define check2_if(c, T) if(_match_char(s, c)){ return _make_token(T, s); };
        #define check2_ifexpand(x) check2_if x
        #define check2(c1, T1, ...)\
                break; case c1: zpp__Args_map(check2_ifexpand, __VA_ARGS__);\
                                return _make_token(T1, s); break
        
        check2('!', BANG
            , ('=', BANG_EQUAL));
        
        check2('=', EQUAL
             , ('=', EQUAL_EQUAL));

        check2('>', GREATER
             , ('=', GREATER_EQUAL));

        check2('<', LESS
             , ('=', LESS_EQUAL));

        check2('+', PLUS
             , ('=', PLUS_EQUAL));

        check2('-', MINUS
             , ('=', MINUS_EQUAL));

        check2('*', ASTER
             , ('=', ASTER_EQUAL));

        check2('/', SLASH
             , ('=', SLASH_EQUAL));

    break;
    }

    #undef scc

    return _make_token_not_found(s);

}

#define HW_DEBUG_SCANNER_PRINT_ENABLED 0
#if HW_DEBUG_SCANNER_PRINT_ENABLED == 1
#include "io.h"
hw_Token _hw_Scanner_scan(hw_Scanner *s);
hw_Token hw_Scanner_scan(hw_Scanner *s)
{
    static int counter = 1;
    hw_Token t = _hw_Scanner_scan(s);
    hw_print("%i <%s> - `", counter, hw_debug_get_token_name(t).data);
    hw_print_cstr(t.start, t.len);
    hw_print("` - %p\n", t.start);

    counter ++;

    return t;
}

hw_Token _hw_Scanner_scan(hw_Scanner *s)
#else
hw_Token hw_Scanner_scan(hw_Scanner *s)
#endif
{
    if(hw_Scanner_isAtEnd(s)) return _make_token(END_OF_SOURCE, s);   

    _start(s);
    _advance(s);

    hw_Token token = {0};
    
    #define scan_as(W) _scan_as(W, s)
    #define scan_mapfn(as_fn)                                   \
        token = scan_as(as_fn);                                 \
        if(hw_Token_isNot(token, NOT_FOUND)) return token;

    #define scan_map(...) zpp__Args_map(scan_mapfn, __VA_ARGS__)

    scan_map(char, literal, symbol);
    
    return _make_token_not_found(s);
}

hw_Token hw_Scanner_scan_skipws(hw_Scanner *s)
{
    hw_Token t = hw_Scanner_scan(s);
    while(hw_Token_isWhiteSpace(t)) {
        t = hw_Scanner_scan(s);
    }
    return t;
}

hw_Token hw_Scanner_scan_until(hw_Scanner *scanner, hw_TokenType tokt)
{
    hw_Token token = hw_Scanner_scan(scanner);
    while(token.type != tokt
       && token.type != hw_TokenType(END_OF_SOURCE)
       && token.type != hw_TokenType(ERROR)) {
        token = hw_Scanner_scan(scanner);
    }
    return token;
}


