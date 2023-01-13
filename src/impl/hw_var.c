#include <stdlib.h>
#include <string.h>
#include <z_/imp/u8arr.h>
#include <z_/types/mem.h>
#include <z_/z__exp.h>

#include "var.h"
#include "io.h"
#include "../core/type.h"
#include "../core/assert.h"

hw_Var hw_Var_new_from_tid(hw_TypeSys const *ts, hw_uint tid)
{
    return (hw_Var) {
        .type = hw_TypeSys_getreff_tid(ts, tid)
    };
}

hw_Var hw_Var_new_from_tstr(hw_TypeSys const *ts, z__Str tstr)
{
    return (hw_Var) {
        .type = hw_TypeSys_getreff_str(ts, tstr)
    };
}

hw_Var hw_Var_new_from_data(hw_TypeSys const *ts, z__u8 const *data, z__u8 const **rest)
{
    hw_uint *type = (void *)data;

    hw_Var var = hw_Var_new_from_tid(ts, *type);
    data += sizeof(hw_uint);
    *rest = hw_Var_call(&var, core.ctor_load_data, data);
    return var;
}


__attribute__((always_inline)) inline hw_uint hw_Var_dump(hw_Var var, z__u8Arr *result) hw_Var_dump_macro(var, result)

__attribute__((always_inline))
__attribute__((const))
inline hw_Var hw_Var_0(void)
{
    return (hw_Var){{0}, NULL};
}

hw_Var hw_Var_new_from_tokt(hw_TypeSys const *ts, hw_TokenType tokt)
{
    switch (tokt) {
        case hw_TokenType(NUMBER): {
            return hw_Var_new_from_tid(ts, hw_VARID_int);
       } break;

        case hw_TokenType(FLOAT): {
            return hw_Var_new_from_tid(ts, hw_VARID_float);
        } break;
        
        case hw_TokenType(SQR_BRACE_LEFT): {
            return hw_Var_new_from_tid(ts, hw_VARID_list);
        } break;

        default: {
            return hw_Var_0();
        } break;
    }
}

#if 0
void hw_List_new_from_tid(hw_List *list, hw_uint len, hw_TypeSys *ts, hw_uint tid)
{
    hw_DEBUG_CODE(hw_assert(list, "List is a null ptr"));
    memset(list, 0, sizeof(*list));

    list->type = hw_TypeSys_getreff_tid(ts, tid);
    hw_DEBUG_CODE(hw_assert(list->type, "No List with Type ID:%llu exists", tid));

    list->data = z__MALLOC(hw_List_unitsize(list) * len);
    list->len = len;
}

void hw_List_new_from_tstr(hw_List *list, hw_uint len, hw_TypeSys *ts, z__Str tstr)
{
    hw_DEBUG_CODE(hw_assert(list, "List is a null ptr"));
    memset(list, 0, sizeof(*list));

    list->type = hw_TypeSys_getreff_str(ts, tstr);
    hw_DEBUG_CODE(hw_assert(list->type, "No List with Type Name:%s exists", tstr.data));

    list->data = z__MALLOC(hw_List_unitsize(list) * len);
    list->len = len;
}

void hw_List_new_from_type(hw_List *list, hw_uint len, hw_Type const *type)
{
    hw_DEBUG_CODE(hw_assert(list, "List is a null ptr"));
    hw_DEBUG_CODE(hw_assert_type(type));
    memset(list, 0, sizeof(*list));
    
    list->type = type;
    list->data = z__MALLOC(list->type->unitsize * len);
    list->len = len;    
}

void hw_List_new_from_list(hw_List *list, hw_List const *src)
{
    hw_DEBUG_CODE(hw_assert(src, "Src List is a null ptr"));
    hw_List_new_from_type(list, src->lenUsed, src->type);   
    
    hw_List_call(list, core.push_by_list, 0, src->lenUsed, src);
}

#endif

