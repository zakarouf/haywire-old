#ifndef ZAKAROUF_HAYWIRE_VM_H
#define ZAKAROUF_HAYWIRE_VM_H

#include "../core/base.h"

#include "mod.h"
#include "thread.h"

#include "chunk.h"
#include "var.h"
#include <z_/types/hashset.h>

typedef struct hw_State hw_State;

struct hw_State {
    struct { hw_Module *data; hw_uint len, lenUsed; } mods;
    struct {
        z__HashStr(hw_Module *) m_str;
        z__HashInt(hw_Module *) m_int;
    } mod_cache;
    hw_ThreadArr threads;
    hw_TypeSys ts;
};

void hw_State_new(hw_State *hw, void (typesys_init)(hw_TypeSys *ts));
void hw_State_delete(hw_State *hw);

hw_Thread *hw_State_push_thread(hw_State *hw);
hw_Module *hw_State_push_mod(hw_State *hw);
hw_Module *hw_State_push_bind_mod(hw_State *hw, hw_Module *mod);

void hw_State_clear_thread(hw_State *hw, hw_uint thread_id);


void hw_vm_run(hw_Thread *t);
#endif

