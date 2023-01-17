#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <z_/imp/u8arr.h>
#include <z_/types/arr.h>
#include <z_/types/bytes.h>
#include <z_/types/hashset.h>
#include <z_/types/mem.h>
#include <z_/types/string.h>
#include <z_/types/typeof.h>

#include "../core/base.h"
#include "../core/type.h"
#include "../core/assert.h"

#include "chunk.h"
#include "debug.h"
#include "mod.h"
#include "var.h"
#include "io.h"


void hw_Module_new(hw_Module *mod)
{
    z__Arr_new(&mod->data, 512);
    z__Arr_new(&mod->code, 128);
    z__Arr_new(&mod->fnpoints, 8);
    z__HashStr_new(&mod->fnsymbs);
}

void hw_Module_delete(hw_Module *mod)
{
    z__HashStr_delete(&mod->fnsymbs);
    z__Arr_delete(&mod->fnpoints);
    z__Arr_delete(&mod->code);
    z__Arr_delete(&mod->data);
}

void hw_Module_serialize(hw_Module *mod, z__u8Arr *result)
{
    z__u8Arr_pushStream(result
            , &mod->fnpoints.lenUsed, sizeof(mod->fnpoints.lenUsed));
    z__u8Arr_pushStream(result
            , mod->fnpoints.data
            , sizeof(*mod->fnpoints.data) * mod->fnpoints.lenUsed);

    z__u8Arr_pushStream(result
            , &mod->data.lenUsed, sizeof(mod->data.lenUsed));
    z__u8Arr_pushStream(result
            , mod->data.data
            , sizeof(*mod->data.data) * mod->data.lenUsed);

    z__u8Arr_pushStream(result
            , &mod->code.lenUsed, sizeof(mod->code.lenUsed));
    z__u8Arr_pushStream(result
            , mod->code.data
            , sizeof(*mod->code.data) * mod->code.lenUsed);
}

z__u8* hw_Module_deserialize(hw_Module *mod, z__u8 const *from)
{
    hw_uint *data_len = (void *)from;
    z__u8 *data = (void *)(data_len + 1);
    z__Arr_newFromPtr(&mod->fnpoints, (void *)data, *data_len);

    data_len = (hw_uint *)(data + (sizeof(mod->fnpoints.data) * *data_len));
    data = (void *)data_len + 1;
    z__Arr_newFromPtr(&mod->data, (void *)data, *data_len);

    data_len = (hw_uint *)(data + (sizeof(mod->data.data) * *data_len));
    data = (void *)data_len + 1;
    z__Arr_newFromPtr(&mod->code, (void *)data, *data_len);

    return(data + (sizeof(mod->data.data) * *data_len));
}

hw_uint hw_Module_emit_code_raw(hw_Module *mod, hw_codeChunk code)
{
    hw_Func fn = hw_Module_load_topfn(mod);
    z__Arr_push(&mod->code, code);
    hw_Func_getm(fn, code_size) += 1;
    return mod->code.lenUsed - 1;
}

hw_uint hw_Module_emit_data(hw_Module *mod, void const *data, hw_uint data_size)
{
    hw_Func fn = hw_Module_load_topfn(mod);
    hw_uint const start = hw_Func_getm(fn, data_size);
    z__u8Arr_pushStream(&mod->data, data, data_size);
    hw_Func_getm(fn, data_size) += data_size;
    return start;
}

hw_uint hw_Module_emit_data_dumpvar(hw_Module *mod, hw_Var var)
{
    hw_Func fn = hw_Module_load_topfn(mod);
    hw_uint const start = hw_Func_getm(fn, data_size);
    hw_uint const data_size = hw_Var_dump(var, &mod->data);
    hw_Func_getm(fn, data_size) += data_size;
    return start;
}

hw_Func hw_Module_add_blank_fn(
      hw_Module *mod
    , char const *name, hw_uint const name_size
    , hw_uint const arg_count, hw_uint const *argT
    , hw_uint const mut_count, hw_uint const *mutT)
{
    hw_DEBUG_CODE(
        hw_uint *_u = NULL;
        z__HashStr_getreff(&mod->fnsymbs, z__Str((char *)name, name_size), &_u);
        hw_assert(_u == NULL, "Function %s, exist", name);
    );
    z__HashStr_set(&mod->fnsymbs, z__Str((char *)name, name_size), mod->code.lenUsed);

    z__Arr_push(&mod->fnpoints, mod->code.lenUsed);  
    z__Arr_expand_ifneeded(&mod->code, 11);

    hw_Func fn = { 
        .meta = mod->code.data + mod->code.lenUsed
    };
    
    hw_Func_setm(fn, data_start, mod->data.lenUsed); // 0
    hw_Func_setm(fn, data_size, 0);                  // 1
    hw_Func_setm(fn, code_size, 1);                  // 2
    hw_Func_setm(fn, name_size, name_size);          // 3
    hw_Func_setm(fn, arg_count, arg_count);          // 4
    hw_Func_setm(fn, mut_count, mut_count);          // 5
    hw_Func_setm(fn, var_count, 0);                  // 6

    /* 7 Name Data */
    hw_Func_setm(fn, name_start, hw_Module_emit_data(mod,
            name, name_size));

    /* 8 Arg Type Data */
    hw_Func_setm(fn, argT_start, hw_Module_emit_data(mod,
            argT, arg_count * sizeof(*argT)));

    /* 9 Mut Type Data */
    hw_Func_setm(fn, mutT_start, hw_Module_emit_data(mod,
            mutT, mut_count * sizeof(*argT)));

    fn.code = hw_Func_get_code(fn);
    mod->code.lenUsed += 10;

    return fn;
}

hw_Func hw_Module_load_fn(hw_Module *mod, hw_uint fn_num)
{
    hw_DEBUG_CODE(
        hw_assert(fn_num < mod->fnpoints.lenUsed
            , "Fn Num Overflow %llu", fn_num);
    )
    hw_Func fn = {
        .meta = mod->code.data + mod->fnpoints.data[fn_num]
    };
    fn.data = mod->data.data + hw_Func_getm(fn, data_start);
    fn.name = (z__char *)hw_Func_get_name(fn);
    fn.argT = (hw_uint *)hw_Func_get_argT(fn);
    fn.mutT = (hw_uint *)hw_Func_get_mutT(fn);
    fn.code = fn.meta + 10;

    return fn;
}

