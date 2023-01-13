#ifndef ZAKAROUF_HAYWIRE_CORE_TYPE_H
#define ZAKAROUF_HAYWIRE_CORE_TYPE_H

#include "base.h"
#include "typefn.h"
#include <z_/types/fnptr.h>

#define hw_typeoflm(C, m) z__typeof(z__cont_typeof(C, m))

typedef struct hw_Type hw_Type;
typedef z__Arr(hw_Type) hw_TypeArr;

typedef struct hw_TypeSys hw_TypeSys;

/**
 * hw_Type
 * Type Storage in haywire
 */
struct hw_Type {
    hw_uint typeID;
    hw_uint unitsize;
    hw_TypeSys const *parent;
    z__String name;
    hw_TypeFn interface;
};

struct hw_TypeSys {
    hw_TypeArr types;
    struct {
        z__HashInt(hw_Type *) tid_hashset;
        z__HashStr(hw_Type *) tname_hashset;
    } cache;
};

/**/
hw_Status hw_Type_new(hw_Type *type
        , hw_uint tid, char const *name, hw_uint name_size, hw_uint unitsize, hw_TypeFn const *interface);

void hw_Type_delete(hw_Type *type);

/**/
void hw_TypeSys_new(hw_TypeSys *ts);
void hw_TypeSys_delete(hw_TypeSys *ts);

hw_uint hw_TypeSys_addtype(hw_TypeSys *ts, char const *name, hw_uint name_size, hw_uint unitsize, hw_TypeFn const *interface);

hw_Type const *hw_TypeSys_getreff_tid(hw_TypeSys const *ts, hw_uint const tid);
hw_Type const *hw_TypeSys_getreff_str(hw_TypeSys const *ts, z__Str const name);

#define hw_TypeSys_getreff(ts, x)\
    _Generic((x),\
        hw_uint: hw_TypeSys_getreff_tid\
      , z__Str: hw_TypeSys_getreff_str)(ts, x)

#endif

