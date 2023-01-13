#include "base.h"
#include "type.h"
#include "var.h"
#include "assert.h"

#include <stdlib.h>
#include <string.h>
#include <z_/types/arr.h>
#include <z_/types/hashset.h>
#include <z_/types/string.h>

hw_Status hw_Type_new(hw_Type *type
        , hw_uint tid, char const *name, hw_uint name_size, hw_uint unitsize, hw_TypeFn const *interface)
{
    hw_DEBUG_CODE(
        hw_assert(name && name_size > 0, "Invalid type name pass with `%lld` length and `%s` value",
            name_size, name == NULL? "null(litreally null, as in 0x0 pointer value)": name);
    );
    
    type->name = z__String_newFromStr(name, name_size);
    type->typeID = tid;
    type->unitsize = unitsize;
    type->parent = NULL;
    memcpy(&type->interface, interface, sizeof(*interface));

    return hw_Status_OK();
}

void hw_Type_delete(hw_Type *type)
{
    z__String_delete(&type->name);
    memset(type, 0, sizeof(*type));
}

void hw_TypeSys_new(hw_TypeSys *ts)
{
    hw_DEBUG_CODE(
        hw_assert(ts, "Type Sys is a nullptr");
    )
    z__Arr_new(&ts->types, 16);
    z__HashStr_new(&ts->cache.tname_hashset);
    z__HashInt_new(&ts->cache.tid_hashset);
}

void hw_TypeSys_delete(hw_TypeSys *ts)
{
    z__HashStr_delete(&ts->cache.tname_hashset);
    z__HashInt_delete(&ts->cache.tid_hashset);
    z__Arr_delete(&ts->types);
    memset(ts, 0, sizeof(*ts));
}

hw_uint hw_TypeSys_addtype(hw_TypeSys *ts, char const *name, hw_uint name_size, hw_uint unitsize, hw_TypeFn const *interface)
{
    hw_uint tid = ts->types.lenUsed;
    z__Arr_pushInc(&ts->types);
    hw_Type *type = &z__Arr_getTop(ts->types);
    #ifdef HW_DEBUG_CODE_ENABLE
        hw_assert(hw_Type_new(type, tid, name, name_size, unitsize, interface).status, "Was not able to create a new type.");
    #else
        hw_Type_new(type, tid, name, name_size, unitsize, interface);
    #endif
   
    type->parent = ts;

    z__HashStr_set(&ts->cache.tname_hashset, z__Str((char *)name, name_size), type);
    z__HashInt_set(&ts->cache.tid_hashset, (z__i32)tid, type);
    
    return ts->types.lenUsed;
}

hw_Type const *hw_TypeSys_getreff_str(hw_TypeSys const *ts, z__Str const name)
{
    hw_Type **t;
    z__HashStr_getreff(&ts->cache.tname_hashset, name, &t);
    return *t;
}

hw_Type const *hw_TypeSys_getreff_tid(hw_TypeSys const *ts, hw_uint tid)
{
    hw_DEBUG_CODE(
        hw_assert(tid < ts->types.lenUsed, "Type ID overflow `%llu`, Types `%u`", tid, ts->types.lenUsed);
    );
    hw_Type const *t = ts->types.data + tid;
    #if 0
        hw_assert(tid < ts->types.lenUsed, "tid is %llu", tid);
        hw_Type **tt = NULL;
        z__HashInt_getreff(&ts->cache.tid_hashset, (z__i32)tid, &tt);
        hw_assert(tt != NULL, "For tid `%llu` tt is null", tid);
        hw_assert(t == *tt, "Type Cached not equal to the list\ntid = '%llu' `%llu:%s` != `%llu:%s`", tid, t->typeID, t->name.data, (*tt)->typeID, (*tt)->name.data);
    #else
    hw_DEBUG_CODE(
        hw_assert(tid < ts->types.lenUsed, "tid is %llu", tid);
    )
    #endif

    return t;
}

