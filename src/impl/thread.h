#ifndef ZAKAROUF_HAYWIRE_THREAD_H
#define ZAKAROUF_HAYWIRE_THREAD_H

#include "../core/base.h"
#include "../core/type.h"

#include "mod.h"
#include "var.h"
#include "chunk.h"

typedef struct hw_Thread hw_Thread;
typedef struct hw_State hw_State;
typedef z__Arr(hw_Thread) hw_ThreadArr;
typedef struct hw_FnState hw_FnState;
typedef z__Arr(hw_FnState) hw_FnStateArr;

struct hw_FnState {
    hw_uint var_start;
    hw_uint var_count;
    hw_Func fn;
    hw_codeChunk const *pc;
};

struct hw_Thread {

    struct {
        hw_TypeSys *ts;
        hw_State *parent_state;
        hw_Module **global_modules;
        hw_uint *global_modules_count;
    } shared;

    hw_VarList stack;
    hw_FnStateArr fn_states;
    hw_Module *current_mod;
    hw_uint current_func;
};

void hw_Thread_new(hw_Thread *t, hw_TypeSys *ts);
void hw_Thread_delete(hw_Thread *t);

hw_FnState *hw_Thread_push_fn(hw_Thread *t, hw_Module *mod, hw_uint fn);
hw_FnState *hw_Thread_pop_fn(hw_Thread *t);

#endif
