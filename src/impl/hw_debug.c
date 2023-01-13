#include "debug.h"

#include "token.h"
#include "scanner.h"

#include "var.h"
#include "type_impl.h"
#include "mod.h"
#include "io.h"
#include "thread.h"
#include "compiler.h"
#include "vm.h"
#include <stdio.h>
#include <z_/prep/args.h>
#include <z_/types/arr.h>


#define _hw_Generate_Tokens_namesfn(name)  [hw_TokenType(name)] = (z__Str){ .data = #name, .len = sizeof(#name)-1}
#define _hw_Generate_Tokens(...) \
    static z__Str const zpp__CAT(hw_TokenType, _NAMES)[] = { zpp__Args_maplist(_hw_Generate_Tokens_namesfn, __VA_ARGS__) };

_hw_Generate_Tokens(
      ERROR,
      hw_Token_List()
);

#undef _hw_Generate_Tokens
#undef _hw_Generate_Tokens_namesfn


void hw_debug_print_source_tokens(const z__Str src)
{
    hw_Scanner s = {};
    hw_Scanner_new(&s, src);

    for (hw_Token tok = hw_Scanner_scan(&s); hw_Token_isNot(tok, END_OF_SOURCE); tok = hw_Scanner_scan(&s)) {
        hw_debug_print_token_name(tok); fputc('\n', stdout);
        if(hw_Token_is(tok, ERROR)) break;
    }
}

static void fprint_str(FILE *fp, char const *str, size_t length)
{
    for (size_t i = 0; i < length && str[i] != '\0'; i++) {
        fputc(str[i], fp);
    }
}

z__Str hw_debug_get_token_name(hw_Token token)
{
    if(hw_Token_isValid(token)) return hw_TokenType_NAMES[token.type];
    else return (z__Str){"", 0};
}

void hw_debug_print_token_name(hw_Token token)
{
    if(hw_Token_isNotValid(token)) { fputs("Broken Token\n", stdout); return; }
    fputc('`', stdout); 
    fprint_str(stdout, token.start, token.len); 
    fputc('`', stdout);
    z__print(" %i - %s", token.type, hw_TokenType_NAMES[token.type].data);
}

void hw_debug_disassemble_instruction(hw_codeChunk codeChunk)
{
    #define opcase_PRIV_PRINT(x)\
        hw_print("| "\
            zpp__to_string(zpp__PRIV__Args_get_2 x) " = "\
            zpp__PRIV__Args_get_1 x, codeChunk.zpp__PRIV__Args_get_2 x);

    #define opcase(OP, ...)\
        break; case hw_OpCode(OP): {\
            hw_print("%u:"#OP" => ", hw_OpCode(OP));\
            zpp__Args_map(opcase_PRIV_PRINT, __VA_ARGS__);\
        } break

    switch (codeChunk.op_code) {
        opcase(RET, ("%llu", raw));

        opcase(CTOR_COPY_DATA, ("%hu", A), ("%u", x32));
        opcase(CTOR_COPY_FILE, ("%hu", A), ("%u", x32));
        opcase(CTOR_COPY_VAR,  ("%hu", A), ("%hu", B));
        opcase(DTOR, ("%llu", raw));

        opcase(ACCESS, ("%llu", raw));
        opcase(ACCESS_SOFT, ("%llu", raw));

        #define tfn_bin_op(x)\
            opcase(zpp__CAT(TYPEFN_, x), ("%hu", A), ("%hu", B), ("%hu", C));

        tfn_bin_op(0);
        tfn_bin_op(1);
        tfn_bin_op(2);
        tfn_bin_op(3);
        tfn_bin_op(4);

        opcase(CALL, ("%hi", A), ("%hi", B));
        opcase(CALL_C, ("%llu", raw));

        opcase(EQ, ("%hu", A), ("%hu", B));
        opcase(LT, ("%hu", A), ("%hu", B));
        opcase(LE, ("%hu", A), ("%hu", B));

        opcase(CMP, ("%hu", A), ("%hu", B), ("%hu", C));

        opcase(JMP, ("%u", x32));

        opcase(PRINT, ("%hu", A));

        opcase(THREAD_NEW, ("%hu", A));

        default:
            hw_print("UNKNOWN OP CODE -> %u", codeChunk.op_code);
            break;
    }
}

void hw_debug_disassemble_func(hw_Func fn)
{
    for (size_t i = 0; i < hw_Func_getm(fn, code_size); i++) {
        hw_debug_disassemble_instruction(fn.code[i]);
        hw_putc('\n');
    }
}

void hw_debug_print_func_detail(hw_Func fn)
{
    hw_print(
        "Meta -> %p\n"
        "Data Start -> %llu\n"
        "Data Size -> %llu\n"
        "Code Size -> %llu\n"
        "Arg Count -> %llu\n"
        "Mut Count -> %llu\n"
        "Var Count -> %llu\n"
        , fn.meta
        , hw_Func_getm(fn, data_start)
        , hw_Func_getm(fn, data_size)
        , hw_Func_getm(fn, code_size)
        , hw_Func_getm(fn, arg_count)
        , hw_Func_getm(fn, mut_count)
        , hw_Func_getm(fn, var_count)
    );
    hw_println("argT =>");
    hw_uint *argT = (hw_uint *)hw_Func_get_argT(fn);
    for (size_t i = 0; i < hw_Func_getm(fn, arg_count); i++) {
        hw_println("%llu", argT[i]); 
    }
    
    hw_println("mutT =>");
    hw_uint *mutT = (hw_uint *)hw_Func_get_argT(fn);
    for (size_t i = 0; i < hw_Func_getm(fn, mut_count); i++) {
        hw_println("%llu", mutT[i]); 
    }
}

void hw_debug_disassemble_print_module(hw_Module *mod)
{
    hw_println("Instructions: %u", mod->code.lenUsed);
    hw_println("Data: %u", mod->data.lenUsed);
    hw_println("Functions: %u", mod->fnpoints.lenUsed);

    z__Arr_foreach(i, mod->fnpoints) {
        hw_Func fn = hw_Module_load_fn(mod, *i);
        hw_print("fn -> ");\
            hw_print_cstr(fn.name, hw_Func_getm(fn, name_size));
        hw_putc('\n');
        hw_debug_print_func_detail(fn);
        hw_putc('\n');
        hw_debug_disassemble_func(fn);
    }
}

#define fn_sanity(T)   zpp__CAT(hw_debug_sanity_, T) 
#define defn_sanity(T) char const * fn_sanity(T) (T const * self)
#define fn_sanity_call(T, v) fn_sanity(T)(v)

#define check(exp)     if(!(exp)) { return zpp__to_string(exp); }
#define ok()           NULL;

defn_sanity(z__String) {
    check(self->len > 0);
    check(self->data != NULL);
    return ok();
}

defn_sanity(hw_Type) {
    check(self->unitsize != 0);
    check(fn_sanity(z__String)(&self->name) == NULL);
    return ok();
}

defn_sanity(hw_TypeArr) {
    check(self->len > 0);
    check(self->data != NULL);
    for (size_t i = 0; i < self->lenUsed; i++) {
        check(fn_sanity(hw_Type)(self->data + i) == NULL);
    }
    return ok();
}

defn_sanity(hw_TypeSys) {
    check(fn_sanity(hw_TypeArr)(&self->types));
    z__Arr_foreach(i, (hw_TypeArr)(self->types)) {
        hw_Type const *str_p = hw_TypeSys_getreff(self, z__Str(i->name.data, i->name.lenUsed));
        check(str_p != NULL);        
        hw_Type const *int_p = hw_TypeSys_getreff(self, i->typeID);
        check(int_p != NULL);
        check(i == str_p && i == int_p);
    }

    return ok();
}

defn_sanity(hw_Var) {
    check(fn_sanity(hw_Type)(self->type) == NULL);
    return ok();
}

defn_sanity(hw_VarList) {
    check(self->len > 0);
    check(self->data != 0);
    for (size_t i = 0; i < self->lenUsed; i++) {
        check(fn_sanity(hw_Var)(self->data + i) == NULL);
    }

    return ok();
}

defn_sanity(hw_FnState) {
    check(self->pc != NULL);
    return ok();
}

defn_sanity(hw_Thread) {
    check(self->fn_states.len > 0);
    check(self->fn_states.data != NULL);
    for (size_t i = 0; i < self->fn_states.lenUsed; i++) {
        check(fn_sanity(hw_FnState)(self->fn_states.data + i) == NULL);
    }

    check(self->shared.parent_state != NULL);
    check(self->shared.ts != NULL);
    check(self->shared.global_modules != NULL);
    check(self->shared.global_modules_count != NULL);
    check(fn_sanity(hw_VarList)(&self->stack) == NULL);
    
    return ok();
}

defn_sanity(hw_ThreadArr) {
    check(self->len > 0);
    check(self->data != NULL);
    for (size_t i = 0; i < self->lenUsed; i++) {
        check(fn_sanity(hw_Thread)(self->data + i) == NULL);
    }
    return ok();
}

defn_sanity(hw_State) {
    check(fn_sanity(hw_ThreadArr)(&self->threads) == NULL);
    check(fn_sanity(hw_TypeSys)(&self->ts) == NULL);
    return ok();
}

