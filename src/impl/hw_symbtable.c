#include <stdlib.h>
#include <z_/types/hashset.h>
#include <z_/types/string.h>

#include "../core/type.h"
#include "var.h"

typedef struct hw_SymbTable hw_SymbTable;
typedef struct hw_VarIPair hw_VarIPair;
struct hw_VarIPair {
    hw_Var var;
    hw_uint index;
};
struct hw_SymbTable {
    hw_VarIPair *values;
    union {
        z__Str *keys;
        z__Str *symbs;
    };
    z__u8 *is_set;
    z__u32 len, lenUsed;
};

hw_SymbTable hw_SymbTable_new(void)
{
    hw_SymbTable st = {0};
    z__HashStr_new(&st);
    return st;
}

void hw_SymbTable_delete(hw_SymbTable *st)
{
    #define key_decon(str)\
        { z__FREE((str)->data); (str)->len = 0; }

    #define val_decon(val)\
        { if((val)->var.type) hw_Var_call((&(val)->var), core.dtor); }

    z__HashSet_delete_with_decon(st, key_decon, val_decon)
}

void hw_SymbTable_set(hw_SymbTable *st, z__Str key, hw_uint index, hw_Var val)
{
    hw_VarIPair _pair = {.var = val, .index = index};
    z__HashSet_set(st, key, _pair, z__Str_newCopy
            , z__PRIV__HashStr_hashfn, z__Str_isequal);
}

hw_VarIPair* hw_SymbTable_getreff(hw_SymbTable *st, z__Str key)
{
    return z__HashSet_getreff_r(st, key, z__PRIV__HashStr_hashfn, z__Str_isequal);
}

