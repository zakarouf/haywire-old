#include "var.h"
#include <stdlib.h>
#include <string.h>
#include <z_/types/bytes.h>

#define HW_INTERFACE_IMPLEMENTATION
#include "../core/typefn.h"
#include "type_impl.h"

#include <math.h>

#define _fngen_Number_Unq(typename, opname, type, op)\
    defn_unq(typename, opname) {\
        _Static_assert(1, "");\
        self->val.zpp__CAT(_, typename) = B->val.zpp__CAT(_, typename) op C->val.zpp__CAT(_, typename);\
        return hw_Status_OK();\
    }

#define _fngen_Number_Unq_make(typename, type)\
    _fngen_Number_Unq(typename, add, type, +)\
    _fngen_Number_Unq(typename, sub, type, -)\
    _fngen_Number_Unq(typename, mul, type, *)\
    _fngen_Number_Unq(typename, div, type, /)\

#define fn_cmp(typename, method) fn(typename, cmp, method)
#define _fngen_number_cmp(typename, method, exp)    \
    defn(typename, cmp, method, int, hw_Var *B) {   \
        return exp;                                 \
    }

#define _fngen_number_cmp_make(typename)\
    _fngen_number_cmp(typename, eq, self->val.zpp__CAT(_, typename) == B->val.zpp__CAT(_, typename)) \
    _fngen_number_cmp(typename, lt, self->val.zpp__CAT(_, typename) <  B->val.zpp__CAT(_, typename)) \
    _fngen_number_cmp(typename, le, self->val.zpp__CAT(_, typename) <= B->val.zpp__CAT(_, typename)) \

#define _gen_interface(x)\
    void zpp__CAT(hw_interface_set_for_, x) (hw_TypeFn *tfn)        \
    {                                                               \
        tfn->core.ctor_load_data = fn_core(number, ctor_load_data); \
        tfn->core.ctor_copy_var = fn_core(number, ctor_copy_var);   \
        tfn->core.ctor_from_str = fn_core(                          \
                zpp__CAT(number_, x), ctor_from_str);               \
        tfn->core.dtor = fn_core(number, dtor);                     \
        tfn->core.dump_data = fn_core(number, dump_data);           \
                                                                    \
        tfn->core.reassign_load_data = fn_core(number, ctor_load_data);  \
        tfn->core.reassign_copy_var = fn_core(number, reassign_copy_var);    \
        tfn->core.recycle = fn_core(number, recycle);                        \
                                                                    \
        _Static_assert(sizeof(tfn->unq) == 8 * 6, "");              \
                                                                    \
        tfn->unq.number.add = fn_unq(x, add);   \
        tfn->unq.number.sub = fn_unq(x, sub);   \
        tfn->unq.number.mul = fn_unq(x, mul);   \
        tfn->unq.number.div = fn_unq(x, div);   \
        tfn->unq.number.mod = fn_unq(x, mod);   \
                                                \
        tfn->cmp.eq = fn_cmp(x, eq);            \
        tfn->cmp.lt = fn_cmp(x, lt);            \
        tfn->cmp.le = fn_cmp(x, le);            \
    }

defn_core(number, ctor_copy_var, hw_Var const *src) {
    *self = *src;
    return hw_Status_OK();
}

defn(number, core, ctor_load_data, z__u8 const *, z__u8 const *data) {
    memcpy(&self->val, data, self->type->unitsize);
    data += self->type->unitsize;
    return data;
}

defn_core(number_i64, ctor_from_str, z__Str *str) {
    char *rest = NULL;
    self->val._i64 = strtoll(str->data, &rest, 10);
    str->len = (str->len) - (rest - str->data);
    str->data = rest; 
    return hw_Status_OK();
}

defn_core(number_f64, ctor_from_str, z__Str *str) {
    char *rest = NULL;
    self->val._f64 = strtod(str->data, &rest);
    str->len = (str->len) - (rest - str->data);
    str->data = rest; 
    return hw_Status_OK();
}

defn_core(number_u64, ctor_from_str, z__Str *str) {
    char *rest = NULL;
    self->val._u64 = strtoull(str->data, &rest, 10);
    str->len = (str->len) - (rest - str->data);
    str->data = rest; 
    return hw_Status_OK();
}

defn_core(number, dtor) {
    (void)self;
    return hw_Status_OK();
}

defn(number, core, dump_data, void, z__u8Arr *result) {
    z__u8Arr_pushStream(result, &self->val, sizeof(self->val));
}

defn_core(number, recycle) {
    (void)self;
    return hw_Status_OK();
}

defn_core(number, reassign_copy_var, hw_Var const *v) {
    *self = *v;
    return hw_Status_OK();
}

_fngen_Number_Unq_make(i64, hw_int);
_fngen_Number_Unq_make(u64, hw_uint);
_fngen_Number_Unq_make(f64, hw_float);

_fngen_Number_Unq(u64, mod, hw_uint, %);
_fngen_Number_Unq(i64, mod, hw_int, %);
defn_unq(f64, mod) {
    _Static_assert(1, "");
    self->val._f64 = fmod(B->val._f64, C->val._f64);
    return hw_Status_OK();
}

_fngen_number_cmp_make(i64);
_fngen_number_cmp_make(u64);
_fngen_number_cmp_make(f64);

_gen_interface(i64);
_gen_interface(u64);
_gen_interface(f64);


