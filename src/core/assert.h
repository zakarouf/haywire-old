#ifndef ZAKAROUF_HAYWIRE_CORE_ASSERT_H
#define ZAKAROUF_HAYWIRE_CORE_ASSERT_H

#include <z_/prep/nm/cond.h>
#include "base.h"

/**
 * NOTE: Be sure that you've included list.h and type.h while including assert.h
 */

#define hw_assert(exp, fmt, ...)\
    zpp__doif_construct(!(exp)\
        , (hw__PRIV__assert_expection_method, stdout, "For Expression `" zpp__to_string(exp) "`\n" fmt "\n",##__VA_ARGS__)\
        , (abort))

#define hw_assert_list(list)\
    hw_assert((list) != NULL, "Passed refference of list is `NULL`"); else {            \
        hw_assert((list)->data && (list)->len > 0, "Data of List not initialized");     \
        hw_assert((list)->type != NULL, "Type of List not Initialized");                \
    }

#define hw_assert_type(type)\
    hw_assert((type) != NULL, "Passed type refference is NULL"); else { \
        hw_assert((type)->name.data, "Name of the type is NULL");       \
        hw_assert((type)->name.len, "Name Length of the type is 0");    \
        hw_assert((type)->unitsize, "Unit Size of the type is 0");      \
    }

#define hw_assert_list_type(list1, list2)\
    hw_assert((list1)->type == (list2)->type\
        , "Type of " zpp__to_string(list1) " is not equal to " zpp__to_string(list2)) {        \
        hw__PRIV__printlist_type(list1);\
        hw__PRIV__printlist_type(list2);\
    }

#define hw__PRIV__printlist_type(list)\
    {                                                                                               \
        if((list) != NULL && (list)->type) {                                                        \
            hw_logprint(2, "Type of '" zpp__to_string(list1) "'is `%s`", (list)->type->name.data);  \
        } else {                                                                                    \
            hw_logprint(2, "List Type Not Initialized " zpp__to_string(list));                      \
        }                                                                                           \
    }                                                                                               \

#define hw__PRIV__assert_expection_method(file, fmt, ...)\
    ({ z__fprint_cl256f(file, 1,\
        z__ansi_fmt((bold)) "Assertion Failure" z__ansi_fmt((plain)) " in "\
        __FILE__ ":" zpp__to_string(__LINE__) ": %s()\n"\
        fmt , __func__ ,##__VA_ARGS__); })


#endif

