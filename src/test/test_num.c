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

#define TEST_SUITE type_int
#define TEST(name) z__test_def(TEST_SUITE, name)

TEST(i64_ctor_from_str) {
    hw_State hw;
    hw_State_new(&hw, hw_TypeSys_init_default);

    hw_Var num = hw_Var_new_from_tstr(&hw.ts, z__Str("int", 3));

    #define z__CSTR(str) str, (sizeof(str) - 1)

    const struct {
        z__Str str;
        hw_int val;
        hw_uint restlen;
    } test_vals[] = {
        #define val(str, val, restlen) {{z__CSTR(#str)}, val, restlen}
        val(0, 0, 0),
        val(1, 1, 0),
        val(12, 12, 0),
        val(123, 123, 0),
        val(7723123, 7723123, 0),
        val(772_3123, 772, 5),
        val(jacob, 0, 5),
    };

    const hw_uint length = sizeof(test_vals)/sizeof(*test_vals);
    for (size_t i = 0; i < length; i++) {
        z__Str str = test_vals[i].str;
        hw_Var_call(&num, core.ctor_from_str, &str);
        z__test_assert(num.val._i64 == test_vals[i].val, "Num found to be `%lli`, expected `%lli`"
                     , num.val._i64, test_vals[i].val);
        z__test_assert(str.len == test_vals[i].restlen, "For string `%s`, expected restlen `%llu`, found `%u`"
                     , test_vals[i].str.data
                     , test_vals[i].restlen
                     , str.len);
    }

    z__test_assert(1, "");
    z__test_done();
}

z__test_impl(TEST_SUITE, i64_ctor_from_str);

