#ifndef ZAKAROUF_HAYWIRE_COMPILER_H
#define ZAKAROUF_HAYWIRE_COMPILER_H

#include "../core/base.h"
#include "mod.h"
#include "scanner.h"
#include "vm.h"

typedef struct hw_State hw_State;
typedef struct hw_CompilerState hw_CompilerState;

typedef struct hw_VarMeta {
    hw_uint index;
    hw_uint ctor_idx;
    enum { ctor_method_data, ctor_method_var } ctor_method;
    hw_uint type;
    struct {
        hw_byte is_const;
    } attr;
} hw_VarMeta;

typedef struct hw_CompilerState hw_CompilerState;
struct hw_CompilerState {
    struct {
        z__HashStr(hw_VarMeta) var_states;
    } fn_info;
    hw_Module *module;
    FILE *logfp;
    hw_Func fn;
    hw_uint fn_id;
    hw_Scanner scanner;
    hw_Token tok, prev_tok;
    z__String source;
    hw_State vm;
};

hw_CompilerState *hw_Compiler_new(z__String const first_source, hw_Module *(*state_init)(hw_State *hw));
hw_Module *hw_Compiler_state_init(hw_State *hw);

void hw_Compile(hw_CompilerState *cs);

#endif

