#ifndef ZAKAROUF_HAYWIRE_CORE_TYPEFN_H
#define ZAKAROUF_HAYWIRE_CORE_TYPEFN_H

#include <z_/types/fnptr.h>
#include <z_/types/obj.h>
#include <z_/types/hashset.h>
#include <z_/types/vector.h>

#include "base.h"

typedef struct hw_Var hw_Var;

/**
 * Core Interface for a list object,
 * Handles:
 *  - del => delete an object; implicitly also calls `hw_Var_free`
 *  - expand => expand the size of a list
 *  - push => pushes a or a stream of elements to the list.
 *  - push_by_list => pushes elements from a different list. Must be hard copy
 *  - pop => pops out a or a stream of elements from the top of the list. 
 */

#if 0
z__objvt(hw_Var, TypeFn_Core,
    (create,            hw_Status, )
    (del,               hw_Status),
    (expand,            hw_Status, hw_uint by),
    (push,              hw_Status, hw_uint len, void **data),
    (push_by_list,      hw_Status, hw_uint src_from, hw_uint len, hw_Var const *src),
    (pop,               hw_Status, hw_uint by)
);

z__objvt(hw_Var, TypeFn_Mut,
    (set, hw_Status, hw_uint self_from, hw_uint len, void **data),
    (set_copy, hw_Status, hw_uint self_from, hw_uint src_from, hw_uint len, hw_Var const *src),
    (set_soft_copy, hw_Status, hw_uint self_from, hw_uint src_from, hw_uint len, hw_Var const *src),
    (remove, hw_Status, hw_uint from, hw_uint to)
);

#define fnptr(name, ...) z__fnptr(name, hw_Status, hw_Var *self,##__VA_ARGS__)

/**
 * Performs arithmatic operations on a single element of a list with an element
 * of a different list and stores it on the same index.
 */
typedef z__Vector(fnptr(, hw_uint index, hw_Var const *with, hw_uint with_index),
        add, sub, mul, div) z__objvt_type(hw_Var, TypeFn_Operator);

typedef z__Vector(fnptr(, hw_uint dest_index, hw_Var const *v, hw_uint from, hw_uint len),
        add, sub, mul, div) z__objvt_type(hw_Var, TypeFn_OperatorStream);

typedef z__Vector(fnptr(, hw_uint index, hw_Var const *lhs, hw_uint lhs_index, hw_int *result),
        eq, gt, lt) z__objvt_type(hw_Var, TypeFn_Compare);

#undef fnptr

#endif

typedef z__fnptr(hw_InterfaceFn, hw_Status, hw_Var *self, void **args);

typedef struct hw_TypeFn hw_TypeFn;
struct hw_TypeFn {

    struct hw_TypeFn_Core {

        #define fnptr(name, ...) z__fnptr(name, hw_Status, hw_Var *self ,##__VA_ARGS__)

        z__fnptr(ctor_load_data, z__u8 const *, hw_Var *self, z__u8 const *data);
        fnptr(ctor_copy_var,    hw_Var const *src);
        fnptr(ctor_from_str,    z__Str *str);

        fnptr(dtor);
        fnptr(recycle);

        z__fnptr(reassign_load_data, z__u8 const *, hw_Var *self, z__u8 const *data);
        fnptr(reassign_copy_var,     hw_Var const *src);

        z__fnptr(dump_data, void, hw_Var *self, z__u8Arr *result);
        z__fnptr(hash, hw_uint, hw_Var *self);
        /*
        hw_InterfaceFn ctor,
                       ctor_copy_var,
                       ctor_load_data,
                       dtor,
                       dump_data;
                    */
        #undef fnptr
    } core;

    struct hw_TypeFn_cmp {
        z__fnptr(cmp, hw_Status, hw_Var *A, hw_Var *B, hw_Var *C);
        z__fnptr(eq, int, hw_Var *A, hw_Var *B);
        z__fnptr(lt, int, hw_Var *A, hw_Var *B);
        z__fnptr(le, int, hw_Var *A, hw_Var *B);
    } cmp;

    union hw_TypeFn_Unq {
        #define fnptr(name, ...) z__fnptr(name, hw_Status, hw_Var *self, hw_Var *_B, hw_Var *_C)
        z__typeof(fnptr()) raw[6];
        
        struct {
            fnptr(add);
            fnptr(sub);
            fnptr(div);
            fnptr(mul);
            fnptr(mod);
        } number;
        struct {
            fnptr(push);
            fnptr(pop);
            fnptr(combine);
            fnptr(filter);
        } list;
        struct {
            fnptr(set);
            fnptr(combine);
            fnptr(filter);
        } hashset;
        struct {
            fnptr(append);
            fnptr(split);;
        } string;
        #undef fnptr
    } unq;

    union hw_TypeFn_Access {
        z__fnptr(raw[2], hw_Status, hw_Var *self, void const *args, hw_Var *v);

        struct {
            z__fnptr(copy, hw_Status, hw_Var *self, void const *args, hw_Var *v);
            z__fnptr(reff, hw_Status, hw_Var *self, void const *args, hw_Var *v);
        };
    } access; 
};

/**
 * object.fn(args| a, b, c) -> ret.1 => int, ret.2 => list
 */

#define hw_args_get(args, index, as_T) ((as_T)(args[index]))

void hw_interface_set_for_i64(hw_TypeFn *tfn);
void hw_interface_set_for_u64(hw_TypeFn *tfn);
void hw_interface_set_for_f64(hw_TypeFn *tfn);
void hw_interface_set_for_list(hw_TypeFn *tfn);

/**
 */

#ifdef HW_INTERFACE_IMPLEMENTATION

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z_/imp/print.h>
#include <z_/types/arr.h>
#include <z_/types/mem.h>
#include <z_/types/obj.h>

#define HW_SANITY_CHECK_CODE_ENABLE
#include "base.h"
#include "var.h"
#include "type.h"
#include "assert.h"

#define alias(name) zpp__CAT(_hw_interface_, name)
#define make_alias(name) alias(name)

#define fn(typename, catagory, of) alias( zpp__CAT5(typename, _, catagory, _, of) )
#define defn(typename, catagory, of, rT, ...) static rT fn(typename, catagory, of) (hw_Var *self ,##__VA_ARGS__)

#define fn_unq(typename, opname) fn(typename, unq, opname)
#define defn_unq(typename, opname) defn(typename, unq, opname, hw_Status, hw_Var *B, hw_Var *C)

#define fn_core(typename, opname) fn(typename, core, opname)
#define defn_core(typename, opname, ...) defn(typename, core, opname, hw_Status ,##__VA_ARGS__)


#endif

#endif

