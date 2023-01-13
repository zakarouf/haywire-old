#ifndef ZAKAROUF_HAYWIRE_IMPL_TOKEN_H
#define ZAKAROUF_HAYWIRE_IMPL_TOKEN_H

#include "../core/base.h"
#include <z_/prep/base.h>
#include <z_/prep/args.h>

#define hw_TokenType(T) zpp__PRIMITIVE_CAT(hw_TOKEN_, T)

#if 1

#define _hw_Generate_Tokens(T, ...) \
    typedef enum T { hw_TokenType(ERROR) = 0, zpp__Args_maplist(hw_TokenType, __VA_ARGS__) } T;\

#define hw_Token_List() \
    /* Single Character Tokes */              \
      SPACE                                   \
    , TABSPACE                                \
    , NEWLINE                                 \
                                              \
    , PAREN_LEFT, PAREN_RIGHT                 \
    , BRACE_LEFT, BRACE_RIGHT                 \
    , SQR_BRACE_LEFT, SQR_BRACE_RIGHT         \
                                              \
    , COMMA, DOT, COLON, SEMI_COLON, AT       \
    , BACK_SLASH                              \
                                              \
    /* Single or Multi Character Tokens */    \
    , PLUS, MINUS, SLASH, ASTER               \
    , PLUS_EQUAL, MINUS_EQUAL                 \
    , SLASH_EQUAL, ASTER_EQUAL                \
                                              \
    , BANG, BANG_EQUAL          \
    , EQUAL, EQUAL_EQUAL        \
    , GREATER, GREATER_EQUAL    \
    , LESS, LESS_EQUAL          \
                                \
    /* Literals */    \
    , SYMBOL          \
    , NUMBER, FLOAT   \
    , STRING          \
                      \
    /* Keywords */    \
    , RETURN          \
    , FUNCTION        \
    , LET             \
    , AND, OR         \
    , TRUE, FALSE     \
    , IF, ELIF, ELSE  \
    , FOR, WHILE      \
                      \
    /* End of source code passed */                     \
    , END_OF_SOURCE                                     \
                                                        \
    /* Token Not Found, for internal evaluation only */ \
    , NOT_FOUND                                         \
                                                        \
    /* Total number of defined tokens */                \
    , TOTAL

_hw_Generate_Tokens(
  hw_TokenType,
  hw_Token_List()
);

#undef _hw_Generate_Tokens
#else

/* Auto Generate Enums and their names */
typedef enum hw_TokenType {
    /* Error */
      hw_TokenType(ERROR) = 0

    /* Single Character Tokens */
    , hw_TokenType(SPACE)
    , hw_TokenType(TABSPACE) 
    , hw_TokenType(NEWLINE)

    , hw_TokenType(PAREN_LEFT), hw_TokenType(PAREN_RIGHT)
    , hw_TokenType(BRACE_LEFT), hw_TokenType(BRACE_RIGHT)
    , hw_TokenType(SQR_BRACE_LEFT), hw_TokenType(SQR_BRACE_RIGHT)

    , hw_TokenType(COMMA), hw_TokenType(DOT), hw_TokenType(SEMI_COLON)
    , hw_TokenType(PLUS), hw_TokenType(MINUS), hw_TokenType(SLASH), hw_TokenType(ASTER)
    , hw_TokenType(BACK_SLASH)

    

    /* Literals */
    , hw_TokenType(IDENTIFIER)
    , hw_TokenType(NUMBER), hw_TokenType(FLOAT), hw_TokenType(STRING)

    /* Keywords */
    , hw_TokenType(RETURN)
    , hw_TokenType(FUNCTION)
    , hw_TokenType(AND), hw_TokenType(OR)
    , hw_TokenType(TRUE), hw_TokenType(FALSE)
    , hw_TokenType(IF), hw_TokenType(ELIF), hw_TokenType(ELSE)
    , hw_TokenType(FOR), hw_TokenType(WHILE)

    , hw_TokenType(END_OF_SOURCE)
    , hw_TokenType(TOTAL)
} hw_TokenType;

#endif

typedef struct hw_Token {
    hw_TokenType type;
    char const *start;
    hw_uint len, line;
} hw_Token;

#define hw_Token_is(t, s) ((t).type == hw_TokenType(s))
#define hw_Token_isNot(t, s) (!hw_Token_is(t, s))

#define hw_Token_isValid(t) ((t).type < hw_TokenType(TOTAL))
#define hw_Token_isNotValid(t) (!hw_Token_isValid(t))

#define hw_Token_isWhiteSpace(t) ((t).type == hw_TokenType(SPACE)\
                                ||(t).type == hw_TokenType(TABSPACE)\
                                ||(t).type == hw_TokenType(NEWLINE))
#define hw_Token_isNotWhiteSpace(t) (!hw_Token_isWhiteSpace(t))

#endif

