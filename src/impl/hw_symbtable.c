#include "var.h"

typedef struct hw_SymbTable hw_SymbTable;
struct hw_SymbTable {
    struct {hw_Var var; hw_uint index;} *values;
    union {
        z__Str *keys;
        z__Str *symbs;
    };
    z__u8 *is_set;
    z__u32 len, lenUsed;
};


