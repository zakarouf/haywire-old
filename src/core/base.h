#ifndef ZAKAROUF_HAYWIRE_CORE_BASE_H
#define ZAKAROUF_HAYWIRE_CORE_BASE_H

#include <z_/types/base.h>
#include <z_/types/arr.h>
#include <z_/types/bytes.h>
#include <z_/types/string.h>
#include <z_/types/hashset.h>

#include <z_/imp/print.h>
#include <z_/imp/u8arr.h>

typedef z__u64 hw_uint;
typedef z__i64 hw_int;
typedef z__f64 hw_float;
typedef z__byte hw_byte;
typedef z__u64 hw_size;
typedef struct hw_ifrac { hw_int n, d; } hw_ifrac;
typedef struct hw_ufrac { hw_uint n, d; } hw_ufrac;
typedef z__String hw_String;

enum hw_ErrorT {
    HW_ERRORT_Basic = 1,
    HW_ERRORT_Compiler,
    HW_ERRORT_VM,
    HW_ERRORT_Type,

    HW_ERRORT_TOTAL
};

enum hw_Error_Basic {
    HW_ERROR_Basic_Error = 1,
    HW_ERROR_Basic_REALLOC_FAIL,
    HW_ERROR_Basic_TOTAL
};

typedef union hw_Status hw_Status;
union hw_Status {
    /* Just Raw Bit Representation */
    hw_uint raw;

    /* Representation in Bit Field Structure */
    struct {
        hw_byte status:1;
    };
};

#define hw_Status_OK() ((hw_Status){.status = 1})
#define hw_Status_ERR()\
    ((hw_Status){.status = 0})

#define hw_Status_isERR(s) ((s).status == 0)

#define hw_logfprint(file, f, fmt, ...) z__fprint_cl256f(file, f, fmt "\n" ,##__VA_ARGS__) 
#define hw_logprint(f, fmt, ...) hw_logfprint(stdout, f, fmt ,##__VA_ARGS__)

#define hw_make_arg(...) (void *[]){ __VA_ARGS__, NULL}
#define hw_make_datalist(...) (void *[]){ __VA_ARGS__, NULL}


//#define HW_DEBUG_CODE_ENABLE
#ifdef HW_DEBUG_CODE_ENABLE
    #define hw_DEBUG_CODE(...) __VA_ARGS__
#else 
    #define hw_DEBUG_CODE(...)
#endif

#ifdef HW_SANITY_CHECK_CODE_ENABLE
    #define hw_SANITY_CHECK_CODE(...) __VA_ARGS__
#else
    #define hw_SANITY_CHECK_CODE(...)
#endif

#define hw_DP(b, f, fmt, ...)\
    hw_DEBUG_CODE(\
            hw_logprint(b, f, "HW_DEBUG:: " fmt "\n" ,##__VA_ARGS__);\
        )


#endif
