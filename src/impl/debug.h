#ifndef ZAKAROUF_HAYWIRE_DEBUG_H
#define ZAKAROUF_HAYWIRE_DEBUG_H

#include <z_/prep/map.h>
#include "../core/base.h"
#include "chunk.h"
#include "token.h"
#include "var.h"
#include "vm.h"

void hw_debug_print_source_tokens(z__Str const src);
void hw_debug_print_token_name(hw_Token token);
z__Str hw_debug_get_token_name(hw_Token token);

void hw_debug_disassemble_instruction(hw_codeChunk codeChunk);
void hw_debug_disassemble_func(hw_Func fn);
void hw_debug_disassemble_module(hw_Module *mod);
void hw_debug_print_func_detail(hw_Func fn);

#define fn_sanity(T) char const * zpp__CAT(hw_debug_sanity_, T)(T const *self);
zpp__Args_map(fn_sanity, hw_Type, hw_TypeSys, hw_Var, hw_VarList, hw_Thread, hw_ThreadArr, hw_State);
#undef fn_sanity

#endif // !ZAKAROUF_HAYWIRE_DEBUG_H
