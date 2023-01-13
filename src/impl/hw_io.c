#include "../core/type.h"

#include "var.h"
#include "io.h"
#include <stdio.h>

void hw_describe_var(FILE *fp, hw_Var const v)
{
    hw_Type const *type = v.type;
    z__fprint(fp, "Type: %s(%llu) UnitSize: %llu",
            type->name.data, type->typeID,  type->unitsize);
}

void hw_putc(char const ch) { fputc(ch, stdout); }

void hw_describe_var_data(FILE *fp, hw_Var const v)
{
#define ffp(l, fmt, ...)\
    fputs("[ ", fp);                            \
    for (size_t i = 0; i < (l)->lenUsed; i++) { \
        z__fprint(fp, fmt, __VA_ARGS__);        \
    }                                           \
    fputs("]", fp);

    switch (v.type->typeID) {
        break; case hw_VARID(int):  {
           z__fprint(fp, "%lli", v.val._i64);
        } 
        break; case hw_VARID(uint):  {
           z__fprint(fp, "%llu", v.val._u64);
        }
        break; case hw_VARID(float):  {
           z__fprint(fp, "%f", v.val._f64);
        }
        break; case hw_VARID(list): {
            hw_VarList const *list = v.val._list;
            fputs("[ ", fp);
            for (size_t i = 0; i < list->lenUsed; i++) {
                hw_describe_var_data(fp, list->data[i]); fputc(' ', fp);
            }
            fputs("]", fp);
        }
        break; default: fputs("[ UNKNOWN ]", fp);
    }
}

void hw_print_cstr(const char *str, hw_uint len)
{
    fwrite(str, len, 1, stdout);
}

void hw_print_str(const z__Str str)
{
    fwrite(str.data, str.len, 1, stdout);
}


