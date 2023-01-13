#ifndef ZAKAROUF_HAYWIRE_MODULE_H
#define ZAKAROUF_HAYWIRE_MODULE_H

#include "../core/base.h"
#include "var.h"

#include "chunk.h"
#include "opcode.h"

#include <z_/types/fnptr.h>

typedef struct hw_Func hw_Func;
typedef struct hw_Module hw_Module;
typedef z__Arr(hw_Func) hw_FnArr;

struct hw_Func {
    hw_codeChunk    *meta;
    z__char const   *name;
    z__u8           *data;
    hw_codeChunk    *code;
    hw_uint         *argT;
    hw_uint         *mutT;
};

#define hw_Func_meta(F, n)      ((F).meta[n].raw)

#define hw_Func_meta_data_start(F)   hw_Func_meta(F, 0)
#define hw_Func_meta_data_size(F)    hw_Func_meta(F, 1)
#define hw_Func_meta_code_size(F)    hw_Func_meta(F, 2)
#define hw_Func_meta_name_size(F)    hw_Func_meta(F, 3)
#define hw_Func_meta_arg_count(F)    hw_Func_meta(F, 4)
#define hw_Func_meta_mut_count(F)    hw_Func_meta(F, 5)
#define hw_Func_meta_var_count(F)    hw_Func_meta(F, 6)
#define hw_Func_meta_name_start(F)   hw_Func_meta(F, 7)
#define hw_Func_meta_argT_start(F)   hw_Func_meta(F, 8)
#define hw_Func_meta_mutT_start(F)   hw_Func_meta(F, 9)
#define hw_Func_meta_code_start(F)   (10)

#define hw_Func_getm(Fn, _what) zpp__CAT(hw_Func_meta_, _what)(Fn)
#define hw_Func_setm(Fn, _what, value) hw_Func_getm(Fn, _what) = value 

#define hw_Func_get_data(M, F)      ((M)->data.data + hw_Func_getm(F, data_start))
#define hw_Func_get_name(F)         ((F).data + hw_Func_getm(F, name_start))
#define hw_Func_get_argT(F)         ((F).data + hw_Func_getm(F, argT_start))
#define hw_Func_get_mutT(F)         ((F).data + hw_Func_getm(F, mutT_start))
#define hw_Func_get_code(F)         ((F).meta + hw_Func_getm(F, code_start))
#define hw_Func_get_data_end(F)     ((F).data + hw_Func_getm(F, data_size))
#define hw_Func_get_code_end(F)     ((F).code + hw_Func_getm(F, code_size))


struct hw_Module {
    z__u8Arr data;
    z__Arr(hw_uint) fnpoints;
    hw_codeArr code;
};

/*
hw_Func hw_Func_new(
    char const *name, hw_uint name_size
  , hw_uint arg_count, hw_uint *argT
  , hw_uint mut_count, hw_uint *mutT
);
void hw_Func_delete(hw_Func *fn);
hw_uint hw_Func_emit_data(hw_Func *fn, void const *data, hw_uint size);
hw_uint hw_Func_emit_var(hw_Func *fn, hw_Var var);
hw_uint hw_Func_emit_code(hw_Func *fn, hw_codeChunk code);
*/

void hw_Module_new(hw_Module *mod);
void hw_Module_delete(hw_Module *mod);

void hw_Module_serialize(hw_Module *mod, z__u8Arr *result);
z__u8* hw_Module_deserialize(hw_Module *mod, z__u8 const *from);

hw_Func hw_Module_add_blank_fn(
      hw_Module *mod
    , char const *name, hw_uint const name_size
    , hw_uint const arg_count, hw_uint const *arg_req_types
    , hw_uint const mut_count, hw_uint const *mut_req_types);
hw_Func hw_Module_load_fn(hw_Module *mod, hw_uint fn_num);

hw_uint hw_Module_emit_code_raw(hw_Module *mod, hw_codeChunk code);
hw_uint hw_Module_emit_data(hw_Module *mod, void const *data, hw_uint data_size);
hw_uint hw_Module_emit_data_dumpvar(hw_Module *mod, hw_Var var);


#define hw_Module_emit_code(mod, ...)\
    hw_Module_emit_code_raw(mod, (hw_codeChunk){ __VA_ARGS__})

#define hw_Module_load_topfn(mod)\
    hw_Module_load_fn(mod, z__Arr_getTop((mod)->fnpoints))

#endif

