#ifndef ZAKAROUF_HAYWIRE_CORE_CFN_H
#define ZAKAROUF_HAYWIRE_CORE_CFN_H

#include "base.h"
#include <z_/types/fnptr.h>

typedef struct hw_Var hw_Var;
typedef z__fnptr(hw_cFunc, void, hw_Var *args, hw_uint arg_count, hw_Var *muts, hw_uint mut_count);
typedef z__fnptr(hw_tFunc, void, hw_Var *self, hw_Var *args, hw_uint arg_count, hw_Var *muts, hw_uint mut_count);

#endif

