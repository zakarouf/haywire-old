#include "var.h"
#include <z_/imp/u8arr.h>
#include <z_/types/arr.h>
#include <z_/types/mem.h>
#define HW_INTERFACE_IMPLEMENTATION
#include "../core/typefn.h"

#include "scanner.h"
#include "token.h"
#include "type_impl.h"
/* For Number */

/**/
static inline hw_VarList *new_list(hw_uint len) {
    hw_VarList *v = z__MALLOC(sizeof(*v));
    z__Arr_new(v, len);
    return v;
}

// TODO: THIS
defn(list, core, ctor_load_data, z__u8 const *, z__u8 const *data) {
    
    /* len */
    hw_uint _uint = *(hw_uint *)(data); data += sizeof(hw_uint);
    hw_VarList *list = new_list(_uint);
    self->val._list = list;

    /* lenUsed */
    list->lenUsed = *(hw_uint *)(data); data += sizeof(hw_uint);

    /* data */
    for (size_t i = 0; i < list->lenUsed; i++) {
        hw_Var *v = list->data + i;
        _uint = *(hw_uint *)(data); data += sizeof(hw_uint);
        v->type = hw_TypeSys_getreff_tid(self->type->parent, _uint);
        data = hw_Var_call(v, core.ctor_load_data, data);
    }

    return data;
}

defn_core(list, ctor_copy_var, hw_Var const *src) {

    hw_DEBUG_CODE(hw_assert(src->type == self->type ,""));
    hw_VarList *src_list = src->val._list;

    self->val._list = new_list(src->val._list->lenUsed);
    hw_VarList *list = self->val._list;

    for (size_t i = 0; i < src_list->lenUsed; i++) {
        list->data[i].type = src_list->data[i].type;
        hw_Var_call(&list->data[i], core.ctor_copy_var, src_list->data + i);
    }

    list->lenUsed = src_list->lenUsed;

    return hw_Status_OK();
}

defn_core(list, ctor_from_str, z__Str *str) {
    hw_VarList *list = new_list(8);
    self->val._list = list;
    
    hw_Scanner s;
    hw_Scanner_new(&s, *str);

    hw_Token t = hw_Scanner_scan(&s);
    if(hw_Token_isNot(t, SQR_BRACE_LEFT)) {
        return hw_Status_ERR();
    }

    t = hw_Scanner_scan_skipws(&s);

    while(hw_Token_isNot(t, SQR_BRACE_RIGHT)) {
        hw_Var v = hw_Var_new_from_tokt(self->type->parent, t.type);
        z__Str str_new = {.data = (char *)t.start, .len = s.end - t.start};
        hw_Var_call(&v, core.ctor_from_str, &str_new);
        z__Arr_push(list, v);
        s.current = str_new.data;
        t = hw_Scanner_scan_skipws(&s);
    }

    str->data = (char *)s.current;
    self->val._list = list;
    return hw_Status_OK();
}

defn_core(list, dtor) {
    z__Arr_delete(self->val._list);
    z__FREE(self->val._list);
    return hw_Status_OK();
}

defn(list, core, dump_data, void, z__u8Arr *result) {

    /* len */
    z__u8Arr_pushStream(result, &self->val._list->lenUsed, sizeof(self->val._list->lenUsed));

    /* lenUsed */
    z__u8Arr_pushStream(result, &self->val._list->lenUsed, sizeof(self->val._list->lenUsed));
    
    /* data */
    for (size_t i = 0; i < self->val._list->lenUsed; i++) {
        z__u8Arr_pushStream(result, &self->val._list->data[i].type->typeID, sizeof(hw_uint));
        hw_Var_call(self->val._list->data + i, core.dump_data, result);
    }

}

defn_unq(list, push) {
    (void)C;
    hw_Var var = { .type = B->type };
    hw_Var_call(&var, core.ctor_copy_var, B);
    z__Arr_push(self->val._list, var);
    return hw_Status_OK();
}

defn_unq(list, pop) {
    (void)C;
    hw_uint by = B->val._u64;
    hw_VarList *ls = self->val._list;
    if(by > ls->lenUsed) { by = ls->lenUsed; }

    for (size_t i = 0; i < by; i++) {
        hw_Var_call(&z__Arr_getTop(*ls), core.dtor);
    }

    ls->lenUsed -= by;

    return hw_Status_OK();
}

defn_unq(list, combine) {
    (void)C;
    hw_VarList *dest = self->val._list;
    hw_VarList const *with = B->val._list;
    hw_Var *var = &z__Arr_getTop(*dest);

    for (size_t i = 0; i < with->lenUsed; i++) {
        z__Arr_pushInc(dest);
        var ++;
        var->type = with->data[i].type;
        hw_Var_call(var, core.ctor_copy_var, with->data + i);
    }
    return hw_Status_OK();
}

defn(list, cmp, eq, int, hw_Var *B) {
    if(B->val._list->lenUsed != self->val._list->lenUsed) {
        return false;
    }
    hw_Var *i_self = self->val._list->data;
    hw_Var *i_B = B->val._list->data;

    for (size_t i = 0; i < self->val._list->lenUsed; i++) {
        if(i_self->type != i_B->type) { return false; }
        if(!hw_Var_call(i_self + i, cmp.eq, i_B += i)) {return false;}
    }

    return true;
}

defn(list, cmp, lt, int, hw_Var *B) {
    if(B->val._list->lenUsed != self->val._list->lenUsed) {
        return self->val._list->lenUsed < B->val._list->lenUsed;
    }
    hw_Var *i_self = self->val._list->data;
    hw_Var *i_B = B->val._list->data;

    for (size_t i = 0; i < self->val._list->lenUsed; i++) {
        if(i_self->type != i_B->type) { return i_self < i_B; }
        if(!hw_Var_call(i_self + i, cmp.lt, i_B += i)) { return false; }
    }

    return true;
}

defn(list, cmp, le, int, hw_Var *B) {
    if(B->val._list->lenUsed != self->val._list->lenUsed) {
        return B->val._list->lenUsed < self->val._list->lenUsed;
    }
    hw_Var *i_self = self->val._list->data;
    hw_Var *i_B = B->val._list->data;

    for (size_t i = 0; i < self->val._list->lenUsed; i++) {
        if(i_self->type != i_B->type) { return i_self <= i_B; }
        if(!hw_Var_call(i_self + i, cmp.le, i_B += i)) {return false;}
    }

    return true;
}

void hw_interface_set_for_list(hw_TypeFn *tfn)
{
    tfn->core.ctor_copy_var = fn_core(list, ctor_copy_var);
    tfn->core.ctor_load_data = fn_core(list, ctor_load_data);
    tfn->core.ctor_from_str = fn_core(list, ctor_from_str);
    tfn->core.dtor = fn_core(list, dtor);
    tfn->core.dump_data = fn_core(list, dump_data);
    
    tfn->core.recycle = NULL;
    tfn->core.reassign_load_data = NULL;
    tfn->core.reassign_copy_var = NULL;
    tfn->core.hash = NULL;


    tfn->unq.list.push = fn_unq(list, push);
    tfn->unq.list.pop = fn_unq(list, pop);
    tfn->unq.list.combine = fn_unq(list, combine);

    tfn->cmp.eq = fn(list, cmp, eq);
    tfn->cmp.lt = fn(list, cmp, lt);
    tfn->cmp.le = fn(list, cmp, le);
}

