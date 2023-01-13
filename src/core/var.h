#ifndef ZAKAROUF_HAYWIRE_CORE_VAR_H
#define ZAKAROUF_HAYWIRE_CORE_VAR_H

#include "base.h"

/** Type Forward Declare */
typedef struct hw_Type hw_Type;
typedef struct hw_TypeSys hw_TypeSys;

/* Variable Type Union
 * should meet the condition `sizeof(hw_VarUnion) == 8` */
typedef union hw_VarUnion hw_VarUnion;
union hw_VarUnion {

    #ifdef HW_VARS
        HW_VARS;
    #endif
};

typedef struct hw_Var hw_Var;
struct hw_Var {
    hw_VarUnion val;
    hw_Type const *type;
};

#define hw_Var_call(self, method, ...)\
    ({hw_DEBUG_CODE(\
        hw_assert((self) != NULL, "");\
        hw_assert((self)->type, "");\
        hw_assert((self)->type->interface.method, ""));\
    (self)->type->interface.method(self,##__VA_ARGS__);})

#endif // !ZAKAROUF_HAYWIRE_VAR_H
