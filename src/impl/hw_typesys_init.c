#include "var.h"
#include "type_impl.h"

#include "../core/type.h"

void hw_TypeSys_init_default(hw_TypeSys *ts)
{
    hw_TypeSys_new(ts);
    hw_TypeFn tfn;

    /* 00 */hw_interface_set_for_u64(&tfn);
    /* 00 */hw_TypeSys_addtype(ts, "nil", 3, 1, &tfn);

    /* 01 */hw_interface_set_for_list(&tfn);
            hw_TypeSys_addtype(ts, "list", 4, sizeof(hw_VarList), &tfn);

    /* 02 */hw_interface_set_for_i64(&tfn);
            hw_TypeSys_addtype(ts, "int", 3, sizeof(hw_int), &tfn);

    /* 03 */hw_interface_set_for_u64(&tfn);
            hw_TypeSys_addtype(ts, "uint", 4, sizeof(hw_uint), &tfn);

    /* 04 */hw_interface_set_for_f64(&tfn);
            hw_TypeSys_addtype(ts, "float", 5, sizeof(hw_float), &tfn);

    /* 05 */hw_interface_set_for_string(&tfn);
            hw_TypeSys_addtype(ts, "string", 6, sizeof(z__String), &tfn);
}


