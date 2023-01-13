#include <stdio.h>
#include <stdlib.h>

#include <z_/imp/test.h>
#include <z_/prep/args.h>

#include "../core/assert.h"

#define import_sin(x) z__test_implext(x);
#define import(...) zpp__Args_map(import_sin, __VA_ARGS__)

#define testrun_sin(test)\
    {\
        int total = 0;\
        z__test_callsu(test, &total);\
    }\

#define testrun(...)\
    zpp__Args_map(testrun_sin, __VA_ARGS__)

#define test_list \
      compiler, \
      type_int, type_list\

import(test_list);

void test_run(void)
{
    testrun(test_list);
}

