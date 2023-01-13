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

#define TEST_SUITE type_list
#define TEST(name) z__test_def(TEST_SUITE, name)

TEST(ctor_from_str) {
    hw_State hw;
    hw_State_new(&hw, hw_TypeSys_init_default);

    hw_Var list = hw_Var_new_from_tstr(&hw.ts, z__Str("list", 4));

    #define z__CSTR(str) str, (sizeof(str) - 1)

    const z__Str test_vals[] = {
        #define val(str) {z__CSTR(#str)}
        val([1 2 3 4 5]),
        val([1.2 42.0 24]),
        val([[35.1 64.0034] [12 53]])
        #undef val
    };

    const hw_uint length = sizeof(test_vals)/sizeof(*test_vals);
    for (size_t i = 0; i < length; i++) {
        z__Str str = test_vals[i];
        hw_Var_call(&list, core.ctor_from_str, &str);
        hw_DEBUG_CODE(
            hw_describe_var(stdout, list);
            hw_describe_var_data(stdout, list);
        );
    }

    z__test_assert(1, "");
    z__test_done();
}

TEST(serialize_and_deserialize) {
    _description_ = "Create a List deserialize and serialize it into a new list\n";

    hw_State hw;
    hw_State_new(&hw, hw_TypeSys_init_default);
    hw_Var list = hw_Var_new_from_tstr(&hw.ts, z__Str("list", 4));

    #define z__CSTR(str) str, (sizeof(str) - 1)

    const z__Str test_vals = {
        #define val(str) z__CSTR(#str)
        val([[35.1 64.0034] [12 53]])
        #undef val
    };

    z__Str str = test_vals;
    hw_Var_call(&list, core.ctor_from_str, &str);

    z__u8Arr data_dump;
    z__Arr_new(&data_dump, 128);
    hw_Var_dump(list, &data_dump);

    z__u8 const *rest;
    hw_Var list2 = hw_Var_new_from_data(&hw.ts, data_dump.data, &rest);

    hw_DEBUG_CODE(
        hw_describe_var(stdout, list);
        hw_describe_var_data(stdout, list);
        puts("");

        hw_describe_var(stdout, list2);
        hw_describe_var_data(stdout, list2);
        puts("");
    );

    z__test_assert(hw_Var_call(&list, cmp.eq, &list2), "List is not equal");

    z__test_assert(1, "");
    z__test_done(
        z__Arr_delete(&data_dump);
    );
}

z__test_impl(TEST_SUITE, ctor_from_str, serialize_and_deserialize);


