#ifndef ZAKAROUF_HAYWIRE_COMPILER_H
#define ZAKAROUF_HAYWIRE_COMPILER_H

#include "../core/base.h"
#include "mod.h"
#include "scanner.h"
#include "vm.h"

typedef struct hw_State hw_State;
typedef struct hw_CompilerState hw_CompilerState;

hw_CompilerState *hw_Compiler_new(z__String const first_source, hw_Module *(*state_init)(hw_State *hw));
hw_Module *hw_Compiler_state_init(hw_State *hw);
hw_State *hw_Compiler_get_state(hw_CompilerState *cs);

void hw_Compile(hw_CompilerState *cs);

#endif

