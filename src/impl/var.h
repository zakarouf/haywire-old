#ifndef ZAKAROUF_HAYWIRE_IMPL_VAR_H
#define ZAKAROUF_HAYWIRE_IMPL_VAR_H

#include "../core/base.h"

/** "../core/typefn.h" Forward Declare **/
typedef struct hw_TypeFn hw_TypeFn;

/** "../core/var.h" Forward Declare */
typedef struct hw_Var hw_Var;
typedef union hw_VarUnion hw_VarUnion;

/** Mods Forward Declare */
typedef struct hw_Func hw_Func;
typedef struct hw_Module hw_Module;

/** Var List **/
typedef struct hw_VarList { hw_Var *data; hw_uint len, lenUsed;} hw_VarList;

/** Var Array **/
typedef struct hw_VarArray hw_VarArray;

/** Optimized Hash Table **/
typedef z__HashStr(hw_Var) hw_HashStr;
typedef z__HashInt(hw_Var) hw_HashInt;


// TODO: Do This;
/** Generic Hash Table **/
typedef struct hw_HashVar {
    hw_Var *keys;
    hw_Var *values;
    hw_uint len, lenUsed;
} hw_HashVar;


#define HW_VARS\
    hw_int      _i64, *_i64p, **_i64pp; \
    hw_uint     _u64, *_u64p, **_u64pp; \
    hw_float    _f64, *_f64p, **_f64pp; \
                                \
    hw_VarList  *_list;         \
    hw_VarArray *_array;        \
                                \
    z__String   *_string;       \
                                \
    hw_HashStr  *_hashstr;      \
    hw_HashInt  *_hashint;      \
    hw_HashVar  *_hashvar;      \
                                \
    hw_Func     *_function;     \
    hw_Module   *_module;


#include "../core/var.h"
#undef HW_VARS

struct hw_VarArray {
    union {
        void *data;
        hw_VarUnion data_as;
    };
    hw_TypeFn *tfn;
    z__u32 len, lenUsed;
};

#define hw_VARID(vname) zpp__CAT(hw_VARID_, vname)
typedef enum {
      hw_VARID(nil) = 0

    , hw_VARID(list)

    , hw_VARID(int)
    , hw_VARID(uint)
    , hw_VARID(float)

    , hw_VARID(string)

    , hw_VARID(hashstr)
    , hw_VARID(hashint)

    , hw_VARID(function)
    , hw_VARID(module)
} hw_VarID;


/**/
hw_Var hw_Var_new_from_tid(hw_TypeSys const *ts, hw_uint tid);
hw_Var hw_Var_new_from_tstr(hw_TypeSys const *ts, z__Str tstr);
hw_Var hw_Var_new_from_data(hw_TypeSys const *ts, z__u8 const *data, z__u8 const **rest);
hw_uint hw_Var_dump(hw_Var var, z__u8Arr *result);
#define hw_Var_dump_macro(var, result)\
    {                                           \
        hw_uint start = (result)->lenUsed;      \
        z__u8Arr_pushStream(result              \
                , &(var).type->typeID           \
                , sizeof((var).type->typeID));  \
        hw_Var_call((&var), core.dump_data, result);\
        return (result)->lenUsed - start;           \
    }


#include "token.h"
hw_Var hw_Var_new_from_tokt(hw_TypeSys const *ts, hw_TokenType tokt);

#define hw_Var_new_from_type(ts, x)\
    _Generic((x),\
        hw_uint: hw_Var_new_from_tid\
      , z__Str: hw_Var_new_from_tstr)(ts, x)

#endif

