#include "var.h"

#include <z_/imp/u8arr.h>
#include <z_/types/arr.h>
#include <z_/types/mem.h>
#include <z_/types/string.h>
#define HW_INTERFACE_IMPLEMENTATION
#include "../core/typefn.h"

#include "scanner.h"
#include "token.h"
#include "type_impl.h"


defn(string, core, ctor_load_data, z__u8 const *, z__u8 const *data) {
    /* len */
    hw_uint *len = (hw_uint *)data;

    /* lenUsed */
    len += 1;

    /*  */
    z__String *string = z__MALLOC(sizeof(*self->val._string));
    *string = z__String_newFromStr((char *)(len + 1), *len);
    
    self->val._string = string;
    return (data + *len + (2 * sizeof(hw_uint)));
}

defn_core(string, ctor_copy_var, hw_Var const *v){
    self->val._string = z__MALLOC(sizeof(*self->val._string));
    *self->val._string = z__String_newCopy(*v->val._string);
    return hw_Status_OK();
}

defn_core(string, ctor_from_str, z__Str *str){
    z__String *string = z__New0(z__String, 1);
    self->val._string = string;
    
    if(str->data[0] == '"') {
        return hw_Status_ERR();
    }

    str->data += 1;
    hw_uint len = 0;
    while(str->data[len] != '"') {
        len += 1;
    }

    *string = z__String_newFromStr(str->data, len);
    str->data += len;
    str->len -= len - 1;

    return hw_Status_OK();
}

defn_core(string, dtor)
{
    z__FREE(self->val._string->data);
    z__FREE(self->val._string);
    return hw_Status_OK();
}

defn(string, core, dump_data, void, z__u8Arr *result) {
    z__String *string = self->val._string;

    /**/
    hw_uint len = string->len;
    z__u8Arr_pushStream(result, &len, sizeof(len));

    /**/
    len = string->lenUsed;
    z__u8Arr_pushStream(result, &len, sizeof(len));

    /**/
    z__u8Arr_pushStream(result, string->data, string->lenUsed);
}

defn_unq(string, append) {
    z__String_join(self->val._string, B->val._string);
    return hw_Status_OK();
}

/*
defn_unq(string, split) {
    z__String_split(self->val._string, (z__Str){ .data = B->val._string->data, .len = B->val._string->lenUsed});
    return hw_Status_OK();
}
*/

defn(string, cmp, eq, int, hw_Var *B) {
    return z__String_cmp(self->val._string, B->val._string) == 0;
}

defn(string, cmp, lt, int, hw_Var *B) {
    return z__String_cmp(self->val._string, B->val._string) < 0;
}

defn(string, cmp, le, int, hw_Var *B) {
    return z__String_cmp(self->val._string, B->val._string) > 0;
}

void hw_interface_set_for_string(hw_TypeFn *tfn)
{
    tfn->core.ctor_copy_var = fn_core(string, ctor_copy_var);
    tfn->core.ctor_load_data = fn_core(string, ctor_load_data);
    tfn->core.ctor_from_str = fn_core(string, ctor_from_str);
    tfn->core.dtor = fn_core(string, dtor);
    tfn->core.dump_data = fn_core(string, dump_data);
    
    tfn->core.recycle = NULL;
    tfn->core.reassign_load_data = NULL;
    tfn->core.reassign_copy_var = NULL;
    tfn->core.hash = NULL;


    tfn->unq.string.append = fn_unq(string, append);
    tfn->unq.string.split = NULL;//fn_unq(string, split);

    tfn->cmp.eq = fn(string, cmp, eq);
    tfn->cmp.lt = fn(string, cmp, lt);
    tfn->cmp.le = fn(string, cmp, le);
    tfn->cmp.cmp = NULL;//fn(string, cmp, cmp);
}

