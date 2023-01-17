#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/_types/_va_list.h>
#include <z_/types/hashset.h>
#include <z_/types/mem.h>
#include <z_/types/string.h>

#include "../core/assert.h"

#include "mod.h"
#include "vm.h"
#include "type_impl.h"
#include "scanner.h"
#include "token.h"
#include "debug.h"
#include "io.h"

#include "compiler.h"

typedef struct hw_TypeMeta hw_TypeMeta;
typedef struct hw_TypeMeta {
    hw_uint typeID;
    hw_uint subT_count;
    hw_TypeMeta *subT;
} hw_TypeMeta;

typedef struct hw_ArgProto {
    hw_uint arg_count;
    hw_uint mut_count;
    hw_TypeMeta *argTm;
    hw_TypeMeta *mutTm;
} hw_ArgProto;

typedef z__Arr(hw_ArgProto) hw_ArgProtoArr;

typedef struct hw_TypeSysMeta {
    hw_uint typeID;
    struct {
        z__String name;
        hw_ArgProtoArr arg_proto; //Multiple for argument overloading
   } typefn_bin[6];

} hw_TypeSysMeta;

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



z__CStr hw_Scanner_get_context(hw_Scanner const *s, hw_Token const tok)
{
    if(tok.start < s->start
    &&(tok.start + tok.len) > s->end) return (z__CStr) { .data = tok.start, .len = tok.len };

    char const *data = tok.start;
    z__CStr slice = {.data = data};
    hw_uint i = 0;

    for (i = 0; i < 16; i++, data--) {
        if(data == s->start) break;
        if(*data == '\n' ||*data == '\0') {
            data += 1;
            break;
        }
    }

    data = tok.start;
    for(i = 0; i < 16; i++, data++) {
        if(data == s->end) break;
        if(*data == '\n' ||*data == '\0') {
            data -= 1;
            break;
        }

    }
    return slice;
}

#define hw_Compiler_error(cs, fmt, ...) hw_Compiler_make_error(cs, "Error :: @ line %lu" fmt "\n", cs->tok.line ,##__VA_ARGS__)
#define hw_cassert(exp, cs, fmt, ...) if(!(exp)) { hw_Compiler_error(cs, fmt, __VA_ARGS__); }
void hw_Compiler_make_error(hw_CompilerState *cs, const char * restrict fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    int val = vfprintf(cs->logfp, fmt, arg);
    z__CStr slice = hw_Scanner_get_context(&cs->scanner, cs->tok);
    hw_print_cstr(slice.data, slice.len);
    hw_putc('\n');

    va_end(arg);
    exit(-1);
}

void hw_Compiler_clear_fn_info(hw_CompilerState *cs)
{
    z__HashSet_foreach(i, &cs->fn_info.var_states) {
        z__FREE(i.key->data);
    }
    cs->fn_info.var_states.lenUsed = 0;
    memset(cs->fn_info.var_states.is_set, 0, cs->fn_info.var_states.len);
}

hw_VarMeta *hw_Compiler_get_var_meta(hw_CompilerState *cs, z__Str const name)
{
    hw_VarMeta *vmeta;
    z__HashStr_getreff(&cs->fn_info.var_states, name, &vmeta);
    return vmeta;
}

hw_VarMeta *hw_Compile_set_var_meta(hw_CompilerState *cs, z__Str const name)
{
    hw_VarMeta *meta;
    z__HashStr_set(
              &cs->fn_info.var_states
            , name
            , (hw_VarMeta){.index = cs->fn_info.var_states.lenUsed + 2}
            , meta = &cs->fn_info.var_states.values[_idx]
        );
    return meta;
}

hw_CompilerState *hw_Compiler_new(z__String const first_source, hw_Module *(*state_init)(hw_State *hw))
{
    hw_CompilerState *cs = z__MALLOC(sizeof(hw_CompilerState));
    cs->logfp = stdout;
    cs->module = state_init(&cs->vm);
    cs->source = z__String_newCopy(first_source);
    hw_Scanner_new(&cs->scanner, (z__Str){.data = cs->source.data, .len = cs->source.lenUsed});
    z__HashStr_new(&cs->fn_info.var_states);
    return cs;
}

void hw_Compiler_delete(hw_CompilerState *cs)
{
    hw_State_delete(&cs->vm);
    z__HashStr_delete(&cs->fn_info.var_states);
    z__String_delete(&cs->source);
    z__FREE(cs);
}

hw_State *hw_Compiler_get_state(hw_CompilerState *cs)
{
    return &cs->vm;
}

hw_Module *hw_Compiler_state_init(hw_State *hw)
{
    hw_State_new(hw, hw_TypeSys_init_default);
    return hw_State_push_mod(hw);
}


#define t cs->tok
#define s cs->scanner

#define scan() t = hw_Scanner_scan(&s)

#define cmp(s) (strncmp(s, t.start, t.len) == 0)
#define icmp(s) if(cmp(s))
#define ccmp(s) else icmp(s)

#define hw_assert_token_expect(t, except_t)\
        hw_assert(hw_Token_is(t, except_t), "Expected Token `" #except_t "`, But Found `%s` at Line: %lld", hw_debug_get_token_name(t).data, t.line);

#define hw_exit_with_msg(str, ...)\
    {                                               \
        hw_print(                                   \
            z__ansi_fmt((cl256_fg, 1))              \
                "Haywire Exit:" str                 \
            z__ansi_fmt((plain)) ,##__VA_ARGS__);   \
        exit(EXIT_FAILURE);                         \
    }

void hw_Compile(hw_CompilerState *cs)
{
    cs->module = hw_State_push_mod(&cs->vm);
    cs->fn = hw_Module_add_blank_fn(cs->module, "main", 4, 0, (hw_uint []){hw_VARID(list)}, 0, (hw_uint []){0});;
    cs->fn_id = z__Arr_getTop(cs->module->fnpoints);

    #define emit_ins(m, opc, ...)\
        hw_Module_emit_code(m, .op_code = hw_OpCode(opc) ,##__VA_ARGS__)
    #define emit_data(m, fn, data, size)\
        hw_Module_emit_data(m, data, size)

    while(!hw_Scanner_isAtEnd(&s)) {
        t = hw_Scanner_scan_until(&s, hw_TokenType(AT));
        t = hw_Scanner_scan(&s);

        hw_assert_token_expect(t, SYMBOL);
            
        icmp("func") {
            hw_Func_setm(cs->fn, var_count, cs->fn_info.var_states.lenUsed);
            hw_Func_setm(cs->fn, arg_count, 0);
            hw_Func_setm(cs->fn, mut_count, 0);
            emit_ins(cs->module, RET);

            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);
            
            cs->fn = hw_Module_add_blank_fn(cs->module, t.start, t.len, 0, NULL, 0, NULL);
            cs->fn_id = z__Arr_getTop(cs->module->fnpoints);
            hw_Compiler_clear_fn_info(cs);
        }

        ccmp("ctor_data") {
            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            if(!vmeta) {
                vmeta = hw_Compile_set_var_meta(cs, z__Str((char *)t.start, t.len));
            } else {
                emit_ins(cs->module, DTOR, .A = vmeta->index);
            }
            
            hw_DEBUG_CODE(
                hw_print("-> Setting Variable, ");
                hw_print_cstr(t.start, t.len);
                hw_println("");
            )
            
            t = hw_Scanner_scan_skipws(&s);
            hw_Var v = hw_Var_new_from_tokt(&cs->vm.ts, t.type);
            hw_assert_type(v.type);

            z__Str str_data = {.data = (char *)t.start, .len = t.len};
            hw_DEBUG_CODE(
                hw_print("Data -> ");
                hw_print_str(str_data);
                hw_println("");
            )

            hw_Var_call(&v, core.ctor_from_str, &str_data);
            vmeta->ctor_method = ctor_method_data;
            vmeta->type = v.type->typeID;

            hw_uint data_start = hw_Module_emit_data_dumpvar(cs->module, v);

            hw_DEBUG_CODE(
                hw_assert(data_start < UINT32_MAX, "Data index overflow -> %llu > UINT32_MAX<%u>", data_start, UINT32_MAX);
            )

            emit_ins(cs->module, CTOR_COPY_DATA, .A = vmeta->index, .x32 = data_start);

            hw_Var_call(&v, core.dtor);
        }

        ccmp("ctor_var") {
            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            if(!vmeta) {
                vmeta = hw_Compile_set_var_meta(cs, z__Str((char *)t.start, t.len));
            } else {
                emit_ins(cs->module, DTOR, .A = vmeta->index);
            }

            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta_2nd = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            hw_assert(vmeta_2nd != NULL, "COMPILER ERROR: Variable `%s` is not initialized", t.start);
            
            emit_ins(cs->module, CTOR_COPY_VAR, .A = vmeta->index, .B = vmeta_2nd->index);
        }

        ccmp("dtor") {
            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            hw_assert(vmeta != NULL, "COMPILER ERROR: Variable `%s` is not initialized", t.start);

            emit_ins(cs->module, DTOR, .A = vmeta->index);
        }
        
        ccmp("typefn_bin") {
            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta_A = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            hw_assert(vmeta_A != NULL, "COMPILER ERROR: Variable `%s` is not initialized", t.start);

            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, NUMBER);

            hw_uint tfn_number = strtoull(t.start, NULL, 10);
            hw_assert(tfn_number < 5, "Type Fn Overflow");

            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta_B = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            hw_assert(vmeta_B != NULL, "COMPILER ERROR: Variable `%s` is not initialized", t.start);

            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta_C = hw_Compiler_get_var_meta(cs, z__Str((char *)t.start, t.len));
            hw_assert(vmeta_C != NULL, "COMPILER ERROR: Variable `%s` is not initialized", t.start);

            emit_ins(cs->module, TYPEFN_0 + tfn_number, .A = vmeta_A->index, .B = vmeta_B->index, .C = vmeta_C->index);
        }
        ccmp("print") {
            t = hw_Scanner_scan_skipws(&s);
            hw_assert_token_expect(t, SYMBOL);

            hw_VarMeta *vmeta = hw_Compiler_get_var_meta(
                    cs, z__Str((char *)t.start, t.len));
            hw_assert(vmeta != NULL,
                "COMPILER ERROR: Variable `%s` is not initialized", t.start);

            emit_ins(cs->module, PRINT, .A = vmeta->index);
        }
    }

    hw_Func_setm(cs->fn, var_count, cs->fn_info.var_states.lenUsed);
    hw_Func_setm(cs->fn, arg_count, 0);
    hw_Func_setm(cs->fn, mut_count, 0);
    emit_ins(cs->module, RET);

    hw_DEBUG_CODE(
        hw_println("End of <source>");
    );
}



#undef t
#undef s
#undef icmp
#undef cmp
#undef ccmp

#undef fn
#undef scan


