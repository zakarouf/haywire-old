#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <z_/imp/ansifmt.h>
#include <z_/imp/print.h>
#include <z_/imp/test.h>
#include <z_/prep/nm/cond.h>
#include <z_/prep/nm/string.h>
#include <z_/types/arr.h>
#include <z_/types/bytes.h>
#include <z_/types/fnptr.h>
#include <z_/types/hashset.h>
#include <z_/types/mem.h>
#include <z_/types/string.h>

#include "../core/base.h"
#include "../core/type.h"
#include "../core/typefn.h"
#include "../core/assert.h"

#include "../impl/debug.h"
#include "../impl/chunk.h"
#include "../impl/opcode.h"
#include "../impl/mod.h"
#include "../impl/io.h"
#include "../impl/scanner.h"
#include "../impl/token.h"
#include "../impl/var.h"
#include "../impl/type_impl.h"
#include "../impl/thread.h"
#include "../impl/vm.h"
#include "../impl/compiler.h"

#define TEST_SUITE compiler
#define TEST(name) z__test_def(TEST_SUITE, name)

int hw_main(hw_uint argc, hw_byte const **argv)
{
    (void)argc;
    (void)argv;
    hw_State hw;
    hw_State_new(&hw, hw_TypeSys_init_default);

    return 0;
}

TEST(compile_and_run) {
    const char *src = \
    ""\
    "   @ctor_data x 10"\
    "   @ctor_data y 12"\
    "   @typefn_bin y 3 y x"\
    "   @print y";

    /* Initialize and Compile */
    hw_CompilerState *compiler = hw_Compiler_new(z__String(src), hw_Compiler_state_init);
    hw_Compile(compiler);

    // Run the Code
    hw_Thread *t = hw_State_push_thread(&compiler->vm);
    hw_Func fn = hw_Module_load_fn(compiler->module, compiler->fn_id);

    hw_DEBUG_CODE(
        hw_debug_print_func_detail(fn);
        hw_debug_disassemble_func(fn);
    );

    hw_Thread_push_fn(t, compiler->module, compiler->fn_id);
    hw_vm_run(t);

    z__test_assert(1, "");
    z__test_done();
}

TEST(compile_add_int) {
    hw_CompilerState *compiler = hw_Compiler_new(
            z__String("@ret"), hw_Compiler_state_init);
    hw_Thread *t = hw_State_push_thread(&compiler->vm);
    compiler->fn = hw_Module_add_blank_fn(
              compiler->module, "main", 4
            , 0, (hw_uint[]){0}
            , 0, (hw_uint[]){0});

    compiler->fn_id = z__Arr_getTop(compiler->module->fnpoints);

    hw_Var int_var = {
        .type = compiler->vm.ts.types.data + hw_VARID(int)
    };

    z__test_assert((int_var.type - compiler->vm.ts.types.data) < compiler->vm.ts.types.lenUsed, "type = %lu", int_var.type - compiler->vm.ts.types.data);

    int_var.val._i64 = 11; 
    hw_Module_emit_code(compiler->module
            , .op_code = hw_OpCode(CTOR_COPY_DATA)
            , .A = 2
            , .x32 = hw_Module_emit_data_dumpvar(compiler->module, int_var));
    
    int_var.val._i64 = 12; 
    hw_Module_emit_code(compiler->module
            , .op_code = hw_OpCode(CTOR_COPY_DATA)
            , .A = 3
            , .x32 = hw_Module_emit_data_dumpvar(compiler->module, int_var));

    int_var.val._i64 = 0; 
    hw_Module_emit_code(compiler->module
            , .op_code = hw_OpCode(CTOR_COPY_DATA)
            , .A = 4
            , .x32 = hw_Module_emit_data_dumpvar(compiler->module, int_var));

    hw_Module_emit_code(compiler->module
            , .op_code = hw_OpCode(TYPEFN_0)
            , .A = 4
            , .B = 2
            , .C = 3);

    hw_Module_emit_code(compiler->module, .op_code = hw_OpCode(PRINT), .A = 4);
    hw_Module_emit_code(compiler->module, .op_code = hw_OpCode(RET));

    hw_Func_setm(compiler->fn, var_count, 3);
    hw_Thread_push_fn(t, compiler->module, compiler->fn_id);
    hw_vm_run(t);

    z__test_assert(t->stack.data[4].val._i64 == 23, "Value is %llu", t->stack.data[4].val._i64);
    z__test_done();
}

TEST(dummy) {
    hw_CompilerState *compiler = hw_Compiler_new(z__String("@ret"), hw_Compiler_state_init);
    hw_Thread *t = hw_State_push_thread(&compiler->vm);
    
    compiler->fn = hw_Module_add_blank_fn(compiler->module, "main", 4, 0, (hw_uint[]){0}, 0, (hw_uint[]){0});
    compiler->fn_id = z__Arr_getTop(compiler->module->fnpoints);
    
    (void)t;
    

    z__test_assert(1, "");
    z__test_done();
}

z__test_impl(TEST_SUITE, dummy, compile_add_int, compile_and_run);
