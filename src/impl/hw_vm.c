#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z_/types/arr.h>
#include <z_/types/hashset.h>

#include "../core/type.h"
#include "../core/assert.h"

#include "vm.h"

#include "opcode.h"
#include "thread.h"
#include "mod.h"
#include "chunk.h"
#include "var.h"
#include "io.h"
#include "debug.h"

#define HW_DEBUG_TEST_V2_ENABLE
#ifdef HW_DEBUG_TEST_V2_ENABLE
#define hw_asrt(exp, excep_fmt, ...) hw_assert(exp, excep_fmt ,##__VA_ARGS__)
#else
#define hw_asrt(...) 
#endif

void hw_Thread_new(hw_Thread *t, hw_TypeSys *ts)
{
    t->shared.ts = ts;
    z__Arr_new(&t->stack, 16);
    z__Arr_new(&t->fn_states, 8);
}

void hw_Thread_delete(hw_Thread *t)
{
    z__Arr_foreach(i, t->stack) {
        hw_Var_call(i, core.dtor);
    }
    z__Arr_delete(&t->stack);

    z__Arr_delete(&t->fn_states);
}

#define hw_VM_HEAD_VAR_COUNT 2
hw_FnState *hw_Thread_push_fn(hw_Thread *t, hw_Module *mod, hw_uint fn)
{
    z__Arr_pushInc(&t->fn_states);
    hw_FnState *s = t->fn_states.data + t->fn_states.lenUsed - 1;

    s->fn = hw_Module_load_fn(mod, fn);

    s->pc = s->fn.code;

    hw_VarList * const v = &t->stack;
    s->var_start = v->lenUsed;
    s->var_count = hw_Func_getm(s->fn, var_count) + hw_VM_HEAD_VAR_COUNT;


    if(v->len <= v->lenUsed + s->var_count) {
        z__Arr_expand(v, s->var_count);
    }
    
    v->lenUsed += s->var_count;
    return s; // top
}

hw_FnState *hw_Thread_pop_fn(hw_Thread *t)
{
    t->fn_states.lenUsed -= 1;
    return t->fn_states.data + t->fn_states.lenUsed - 1; //new top
}


void hw_State_new(hw_State *hw, void (typesys_init)(hw_TypeSys *ts))
{
    z__Arr_new(&hw->threads, 4);
    z__Arr_new(&hw->mods, 8);
    z__HashInt_new(&hw->mod_cache.m_int);
    z__HashStr_new(&hw->mod_cache.m_str);
    hw_TypeSys_new(&hw->ts);
    typesys_init(&hw->ts);
}

void hw_State_delete(hw_State *hw)
{
    z__Arr_foreach(i, hw->threads) {
        hw_Thread_delete(i);
    }
    z__Arr_delete(&hw->threads);

    z__Arr_foreach(i, hw->mods) {
        hw_Module_delete(i);
    }
    z__Arr_delete(&hw->mods);

    z__HashInt_delete(&hw->mod_cache.m_int);
    z__HashStr_delete(&hw->mod_cache.m_str);

    hw_TypeSys_delete(&hw->ts);
}

hw_Thread *hw_State_push_thread(hw_State *hw)
{
    z__Arr_pushInc(&hw->threads);
    hw_Thread *t = &z__Arr_getTop(hw->threads);
    hw_Thread_new(t, &hw->ts);
    t->shared.parent_state = hw;
    t->shared.global_modules = &hw->mods.data;
    t->shared.global_modules_count = &hw->mods.lenUsed;
    return t;
}

hw_Module *hw_State_push_mod(hw_State *hw)
{
    z__Arr_pushInc(&hw->mods);
    hw_Module *m = &z__Arr_getTop(hw->mods);
    hw_Module_new(m);
    return m;
}

hw_Module *hw_State_push_bind_mod(hw_State *hw, hw_Module *mod)
{
    hw_Module *m = hw_State_push_mod(hw);
    memcpy(m, mod, sizeof(*mod));
    return m;
}

void hw_State_clear_thread(hw_State *hw, hw_uint thread_id)
{
    hw_Thread *t = hw->threads.data + thread_id;
    z__Arr_foreach(i, t->fn_states) {
        hw_Var *v = t->stack.data + i->var_start;

        // Skip first index as it will always be a list type of soft copies
        for (size_t j = 1; j < i->var_count; j++) {
            hw_Var_call(v + j, core.dtor);
        }
    }
    t->fn_states.lenUsed = 0;
    t->stack.lenUsed = 0;
}

static void print_var(hw_Var *v, hw_uint tabspace)
{
    for (size_t i = 0; i < tabspace; i++) {
        fputs("    ", stdout);
    }

    switch (v->type->typeID) {
        case hw_VARID(list): {
            fputs("[ ", stdout);
            for (size_t i = 0; i < v->val._list->lenUsed; i++) {
                print_var(v->val._list->data + i, tabspace + 1);
            }
            fputs("]", stdout);
        } break;

        case hw_VARID(int): {
            printf("%lli ", v->val._i64);
        } break;

        default:
            printf("<UNKNOWN> ");
            break;
    }
}

void hw_vm_run(hw_Thread *t)
{
    hw_Var *A = NULL, *B = NULL, *C = NULL;
    hw_Var *vars = NULL;

    #define get(X) cfs->pc->X
    #define setv(X) X = vars + get(X)
    #define setABC()\
        setv(A);\
        setv(B);\
        setv(C);\

    //L_func_run:;
    hw_FnState *cfs = t->fn_states.data + t->fn_states.lenUsed -1;
    vars = t->stack.data + cfs->var_start;

    hw_uint counter = 0;

    while(true) {
        #define opcode(x) break; case hw_OpCode(x)
        
        #define i cfs->pc
        hw_DEBUG_CODE(
            hw_print("%llu => ", counter++);
            hw_debug_disassemble_instruction(*cfs->pc);
            hw_putc('\n');
        )
        #undef i

        switch (cfs->pc->op_code) {
            case hw_OpCode(RET): {
                goto L_return;
            }

            opcode(CTOR_COPY_DATA): {
                setv(A);
                z__u8 const *data;
                *A = hw_Var_new_from_data(t->shared.ts, cfs->fn.data + get(x32), &data);
            }

            opcode(CTOR_COPY_VAR): {
                setv(A); setv(B); 
                A->type = B->type;
                hw_Var_call(A, core.ctor_copy_var, B);
            }

            opcode(DTOR): {
                setv(A);
                hw_Var_call(A, core.dtor);
            }

            opcode(ACCESS): {
                setABC();
                hw_Var_call(B, access.copy, C, A);
            }

            opcode(ACCESS_SOFT): {
                setABC();
                hw_Var_call(B, access.reff, C, A);
            }

            opcode(EQ): {
                setv(A); setv(B);
                if(hw_Var_call(A, cmp.eq, B)) cfs->pc += 1;
            }

            opcode(LT): {
                setv(A); setv(B);
                if(hw_Var_call(A, cmp.lt, B)) cfs->pc += 1;
            }

            opcode(LE): {
                setv(A); setv(B);
                if(hw_Var_call(A, cmp.le, B)) cfs->pc += 1;
            }

            opcode(CMP): {
                setABC();
                hw_Var_call(B, cmp.cmp, C, A);
            }

            #define typefn(X) opcode(zpp__CAT(TYPEFN_, X)): { setABC(); hw_Var_call(A, unq.raw[X], B, C); }
            typefn(0);
            typefn(1);
            typefn(2);
            typefn(3);
            typefn(4);
            #undef typefn
            
            opcode(JMP): cfs->pc += cfs->pc->xi32;
            opcode(PRINT): {
                setv(A);
                hw_DEBUG_CODE(
                    hw_print_str(z__Str("> PRINT -> ", sizeof("> PRINT -> ")));
                )
                print_var(A, 0);
                puts("");
            }

            default:;
        }
        
        _Static_assert(sizeof(hw_codeChunk) == 8, "");
        cfs->pc += 1;
    }


    L_return:
    return;
}

