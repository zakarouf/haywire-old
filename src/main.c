/**/
#include <stdlib.h>
#include <z_/imp/fio.h>
#include <z_/imp/test.h>

#include "core/base.h"
#include "core/assert.h"
#include "impl/io.h"

void test_run(void);

int main (void)
{
    if(1) test_run();
    return 0;
}
/*****************************************************************/
/*****************************************************************/

/*
typedef enum hw_fnret {
    hw_fnret_END = 0,
    hw_fnret_CALL
} hw_fnret;

#define next(b) at += b;
#define setc(b) at = b;
#define getnext(V)\
    V = *z__bytes_get(&at->raw, V, 0); next(sizeof(V));

hw_bcursor *_fn_set_var_push_const(hw_bcursor *at, hw_Module const *mod, hw_TypeSys const *ts, hw_fnState *fS, hw_List *l)
{
    hw_uint getnext(tid);
    hw_uint getnext(usz);
    hw_uint getnext(len);
    hw_uint getnext(const_idx);

    l->type->vt->_push(l, ts, NULL, mod->data.bytes.data + const_idx, len);

    return at;
}

hw_bcursor *_fn_set_var_push_var(hw_bcursor *at, hw_Module const *mod, hw_TypeSys const *ts, hw_fnState *fS, hw_List *dest)
{

    hw_byte getnext(_src_l_or_p)
    hw_uint getnext(vid);

    hw_uint getnext(tid);
    hw_uint getnext(from);
    hw_uint getnext(len);

    hw_List const *l_from = &z__Arr_getVal(fS->stack.local, vid);

    dest->type->vt->_push(dest, ts, NULL
        , l_from->data + (from * l_from->type->unitsize), len);

    return at;
}


hw_bcursor *_fn_set_var_set(hw_bcursor *at, hw_Module const *mod, hw_TypeSys const *ts, hw_fnState *fS, hw_List *dest)
{
    hw_byte getnext(_src_l_or_p)
    hw_uint getnext(vid);

    hw_uint getnext(tid);
    hw_uint getnext(dest_index);
    hw_uint getnext(src_index);
    hw_uint getnext(len);
    

    hw_List const *l_from = &z__Arr_getVal(fS->stack.local, vid);

    dest->type->vt->_set(
        dest
        , ts
        , NULL
        , dest_index
        , l_from->data + (src_index * l_from->type->unitsize), len);

    return at;
}

enum _Fn_SET {
    _FN_SET_VPUSH_CONST = 1,
    _FN_SET_VPUSH_VAR,
    _FN_SET_VSET_VAR
};

static hw_bcursor * (* _fn_set_var[])(hw_bcursor *at, hw_Module const *mod, hw_TypeSys const *ts, hw_fnState *fS, hw_List *l) = {
    [_FN_SET_VPUSH_CONST] = _fn_set_var_push_const,
    [_FN_SET_VPUSH_VAR] = _fn_set_var_push_var,
    [_FN_SET_VSET_VAR] = _fn_set_var_set,
};

enum {
      OP_NIL = 0x0

    , OP_VAR             = 0x1
    , OP_VAR_DEC         = 0x11
    , OP_VAR_DEC_PCONST
    , OP_VAR_DEC_PVAR
    , OP_VAR_POP

    , OP_VSET            = 0x2
    , OP_VSET_PUSH_CONST = 0x21
    , OP_VSET_PUSH_VAR
    , OP_VSET_SET_VAR

    , OP_VOP        = 0x3
    , OP_VOP_ADD    = 0x31
    , OP_VOP_SUB
    , OP_VOP_MUL
    , OP_VOP_DIV

    , OP_JUMP         = 0x4
    , OP_JUMP_BYTE    = 0x41
    , OP_JUMP_BYTE_IF
};

#define OP_getminor(ins) (ins&0x0f)

enum {
    VTYPE_LOCAL,
    VTYPE_PASSEDREFF
};
*/
/*
hw_fnret hw_exec(hw_State *S, hw_fnState *fS, void ** ret_data)
{    
    hw_Module const *mod = S->main;
    hw_TypeSys const *ts = &S->ts;

    int quit = 0;
    hw_bcursor *at = fS->cursor.at;
    hw_byte truth = 0;
    
    while (!quit) {
        hw_DEBUG_CODE (
            printf("Called ins 0x%02hhx %01hhx:%01hhx\n"
                    , at->raw, at->ins.major, at->ins.minor);
        );
        switch (at->ins.major) {
            ;break; case 0x0: // Undefined
                switch (at->ins.minor) {
                    ;break; case 0x0: next(1); return hw_fnret_END;
                    ;break; case 0x1: next(1); {
                        z__Arr_foreach(i, fS->stack.local) {
                            printf("VID %zu::", i - fS->stack.local.data);
                            print_listmdata(i);

                            printf("\n  Data: ");
                            print_listdata(i);
                            puts("");
                        }
                    }
                    ;break; default: next(1);
                }
            ;break; case OP_VAR: // Var Dec and Undec
                switch (at->ins.minor) { // Only Dec
                    ;break; case OP_getminor(OP_VAR_DEC): next(1); {
                        hw_uint getnext(tid);
                        hw_uint getnext(usz);
                        hw_uint getnext(len);
                        
                        hw_Type const *t = hw_TypeList_get(ts->types, tid);
                        z__Arr_pushInc(&fS->stack.local);
                        hw_List *l = &z__Arr_getTop(fS->stack.local);

                        t->vt->new(l, ts, NULL, tid, len);
                    }

                    ;break; case OP_getminor(OP_VAR_DEC_PCONST): next(1); { //Dec & Set Const
                        hw_uint getnext(tid);
                        hw_uint getnext(usz);
                        hw_uint getnext(len);
                        hw_uint getnext(const_idx);

                        hw_Type const *t = hw_TypeList_get(ts->types, tid);
                        z__Arr_pushInc(&fS->stack.local);
                        hw_List *l = &z__Arr_getTop(fS->stack.local);

                        t->vt->new(l, ts, NULL, tid, len);
                        t->vt->push(l, ts, NULL, mod->data.bytes.data + const_idx, len);
                    }
                    ;break; case OP_getminor(OP_VAR_DEC_PVAR): next(1); { //Dec & Set Const
                        hw_byte getnext(src_l_or_p);
                        hw_uint getnext(src_vid);
                        hw_uint getnext(src_from);
                        hw_uint getnext(len);

                        z__Arr_pushInc(&fS->stack.local);
                        hw_List *l = &z__Arr_getTop(fS->stack.local);
                        hw_List *src = &z__Arr_getVal(fS->stack.raw[src_l_or_p], src_vid);

                        src->type->vt->new(l, ts, NULL, src->type->typeID, len + 2);
                        l->type->vt->push(l, ts, NULL, src->data + (src->type->unitsize) * src_from, len);
                    }
                    ;break; case OP_getminor(OP_VAR_POP): next(1); { // Pop
                        hw_byte getnext(l_or_p);
                        hw_uint getnext(vid);
                        hw_uint getnext(len);

                        hw_List *l = &z__Arr_getVal(fS->stack.raw[l_or_p], vid);
                        l->type->vt->pop(l, ts, NULL, len);
                    }
                    ;break; default: next(1);
                }

            ;break; case OP_VSET: next(1) {  // Var Local Manipulation
                hw_bcursor tmp = at[-1];
                hw_byte getnext(l_or_p);
                hw_uint getnext(vid);
                setc(_fn_set_var[tmp.ins.minor](at, mod, ts, fS, &z__Arr_getVal(fS->stack.raw[l_or_p], vid)));
            }
            ;break; case OP_VOP: // Operator
                next(1) {
                    hw_bcursor op = at[-1];
                    hw_byte getnext(_dest_l_or_p);
                    hw_byte getnext(_src_l_or_p);

                    hw_uint getnext(_dest_vid);
                    hw_uint getnext(_src_vid);


                    hw_List *dest = &z__Arr_getVal(fS->stack.raw[_dest_l_or_p], _dest_vid);
                    hw_List *src = &z__Arr_getVal(fS->stack.raw[_src_l_or_p], _src_vid);

                    printf("\n=================================Helr\n");
                    (*src->type->op.raw[op.ins.minor])(src, ts, hw_make_arg(src, 
                                                                      at        // destidx
                                                                    , at+8      // srcidx
                                                                    , at+16));  // srclen
                    at += sizeof(hw_uint) * 3;
                }

            ;break; case OP_JUMP: // Jump
                switch (at->ins.minor) {
                    ;break; case OP_getminor(OP_JUMP_BYTE): next(1); { // forloop
                        hw_int getnext(by);
                        at += by;
                    }

                    ;break; case OP_getminor(OP_JUMP_BYTE_IF): next(1); {
                        hw_byte getnext(_l_or_p);
                        hw_uint getnext(vid);
                        hw_uint getnext(idx);
                        hw_int  getnext(by);
                        
                    }                    
                }
            ;break; default: next(1);
        }
    }

    return hw_fnret_END;
}

*/
#if 0
void hwn_run(z__Str name, hw_Module *mod, hw_uint fn_index)
{
    hw_State *s = z__New0(*s, 1);
    hw_State_new(s, mod, name);

    hw_TypeSys_setdefault(&s->ts);

    hw_fnState *fS = hw_State_pushStack(s, 8, 8, 8);
    hw_bState_start(&fS->cursor
                   , s->main->fns.data[fn_index].code.bytes.data, 0);

    void *rdata = NULL;
    
/*
    do {
        fS = hw_State_topfnStack(s);
        hw_fnret r = hw_exec(s, fS, &rdata);
        switch (r) {
            break; case hw_fnret_END:
                hw_State_popStack(s);
            break; case hw_fnret_CALL:
                ;
        }
    } while(z__Arr_getUsed(s->fnStates) > 0);
    */
}


int main (int argc, char const *argv[])
{
    /*
    FILE *fp = fopen("test.hwfncode", "w");
    z__fio_ptr_dump(fncode, 1, sizeof fncode, fp);
    fclose(fp);

    z__size usz, len;
    fp = fopen("test.hwfncode", "r");
    void *code = z__fio_ptr_newLoad(&usz, &len, fp);
    fclose(fp);
    
    */

    hw_Module *mod = hw_Module_cons_dummy(z__Str("test", 4), 0);

    hw_Function *fn = hw_Module_getfn_top_reff(mod);
/*
    printf("=> Data Added Index %llu\n", hw_Module_pushData(mod, hw_uint, 10, 32, 0, 1));

    hw_Function_writeByte_stream(fn, hw_byte, OP_VAR_DEC_PCONST);
    hw_Function_writeByte_stream(fn, hw_uint, 02); // TypeID
    hw_Function_writeByte_stream(fn, hw_uint, sizeof(hw_int)); // UnitSize 
    hw_Function_writeByte_stream(fn, hw_uint, 4); // Len
    hw_Function_writeByte_stream(fn, hw_uint, 0); // Const INDEX

    hw_Function_writeByte_stream(fn, hw_byte, 01); //print

    hw_Function_writeByte_stream(fn, hw_byte, OP_VOP_ADD); 
    hw_Function_writeByte_stream(fn, hw_byte, VTYPE_LOCAL); 
    hw_Function_writeByte_stream(fn, hw_byte, VTYPE_LOCAL); 
    hw_Function_writeByte_stream(fn, hw_uint, 00); // destvid
    hw_Function_writeByte_stream(fn, hw_uint, 00); // srcvid
    hw_Function_writeByte_stream(fn, hw_uint, 00); // dest idx
    hw_Function_writeByte_stream(fn, hw_uint, 00); // src idx
    hw_Function_writeByte_stream(fn, hw_uint, 02); // len

    hw_Function_writeByte_stream(fn, hw_byte, OP_JUMP_BYTE_IF);
    

    hw_Function_writeByte_stream(fn, hw_byte, OP_VAR_POP);
    hw_Function_writeByte_stream(fn, hw_byte, VTYPE_LOCAL);
    hw_Function_writeByte_stream(fn, hw_uint, 00);
    hw_Function_writeByte_stream(fn, hw_uint, 01);

    hw_Function_writeByte_stream(fn, hw_byte, 01); //print
    hw_Function_writeByte_stream(fn, hw_byte, 0);

    hwn_run(z__Str("test", 4), mod, 0);

    */
    hw_Module_delete(mod);

    return 0;
}

#endif

