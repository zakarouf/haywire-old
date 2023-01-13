#ifndef ZAKAROUF_HAYWIRE_CHUNK_H
#define ZAKAROUF_HAYWIRE_CHUNK_H

#include "../core/base.h"

#if 0
struct hw_codeChunk {
    hw_byte op_code;
    union {
        hw_uint data_index;
        hw_int jmp_offset;
        struct {
            z__u16 A;
            z__u16 B;
            z__u32 C;
        } v_index;
    };
};
#else

typedef union hw_codeChunk hw_codeChunk;
typedef z__Arr(hw_codeChunk) hw_codeArr;

union hw_codeChunk {
    hw_uint raw;
    struct {
        z__u8 op_code, _useless;
        z__u16 A;
        union {
            z__u32 x32;
            z__i32 xi32;
            struct {
                z__u16 B, C;
            };
        };
    };
};

#endif

#endif

