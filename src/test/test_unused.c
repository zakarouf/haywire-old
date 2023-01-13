#if 0
/**
 * add2:
 * 
 * add 2 variables on, stack index 1 2 into 3
 */
TEST(add2) {
    hw_State hw;
    hw_State_new(&hw, hw_TypeSys_init_default);
    hw_Thread *t = hw_State_push_thread(&hw);
    hw_Module *m = hw_State_push_mod(&hw);

    hw_Module_new(m);
    hw_Func *f = hw_Module_add_blank_fn(m, z__Str("main", 4));
    f->var_count = 3;
    f->arg_count = 0;
    f->mut_count = 0;

    hw_Type const *type = hw_TypeSys_getreff(&hw.ts, z__Str("uint", 4));
    hw_assert_type(type);

    hw_uint data_offset = 0;
    data_offset = hw_Func_emit_data(f, (hw_uint[]){type->typeID, 10}, sizeof(hw_uint) * 2);
    hw_Func_emit_instruction(f, (hw_codeChunk){ .op_code = hw_OpCode(CTOR_COPY_DATA), .A = 2+1, .x32 = data_offset});

    data_offset = hw_Func_emit_data(f, (hw_uint[]){type->typeID, 11}, sizeof(hw_uint) * 2);
    hw_Func_emit_instruction(f, (hw_codeChunk){ .op_code = hw_OpCode(CTOR_COPY_DATA), .A = 2+2, .x32 = data_offset});

    data_offset = hw_Func_emit_data(f, (hw_uint[]){type->typeID, 15}, sizeof(hw_uint) * 2);
    hw_Func_emit_instruction(f, (hw_codeChunk){ .op_code = hw_OpCode(CTOR_COPY_DATA), .A = 2+3, .x32 = data_offset});

    hw_Func_emit_instruction(f, (hw_codeChunk){ .op_code = hw_OpCode(TYPEFN_0), .A = 3 + 2, .B = 1 + 2, .C = 2 + 2});
    hw_Func_emit_instruction(f, (hw_codeChunk){ .op_code = hw_OpCode(RET) });

    z__Arr_foreach(i, f->code) {
        printf("%llu | op = %u | A = %u | B = %u | C = %u | x32 = %u\n", i->raw, i->op_code, i->A, i->B, i->C, i->x32);
    }

    _Static_assert(sizeof(hw_codeChunk) == 8, "");
    char const *err = hw_debug_sanity_hw_Thread(t);
    hw_assert(err == NULL, "Error: %s", err);

    hw_FnStateArr_push(&t->fn_states, f, &t->stack);
    hw_vm_run(t);
    
    hw_uint result = t->stack.data[5].val._u64;
    z__test_assert(result == 10 + 11, "result is %llu", result);
    z__test_done();
}
#endif


#if 0
TEST(basic) {
    _Static_assert(sizeof(hw_Var) == 8 * 2, "Size of hw_Var should be 16 bytes"); 
    hw_TypeSys ts;
    type_system_init(&ts);    
    hw_Var a = hw_Var_new_from_tstr(&ts, z__Str("list", 4))
         , b = hw_Var_new_from_tstr(&ts, z__Str("int", 3))
         , c = hw_Var_new_from_tstr(&ts, z__Str("int", 3));

    hw_uint len = 16;
    hw_Var_call(&a, core.ctor, &len);

    b.val._i64 = 1;
    c.val._i64 = 2;

    hw_describe_var_data(stdout, b); puts("");
    hw_describe_var_data(stdout, c); puts("");
    hw_describe_var_data(stdout, a); puts("");

    hw_Var_call(&a, unq.list.push, a, b);
    hw_Var_call(&a, unq.list.push, a, c);
    hw_Var_call(&a, unq.list.push, a, a);

    hw_describe_var_data(stdout, b); puts("");
    hw_describe_var_data(stdout, c); puts("");
    hw_describe_var_data(stdout, a); puts("");

    z__test_assert(1, "");
    z__test_done();
}

/*
static void runf(hw_Func *f, hw_TypeSys *ts, hw_VarList *stack)
{
    hw_byte *b = f->code.bytes.data;
    do {
        switch (*b) {
        break; case hw_OpCode_RET:
            return;
        break; case hw_OpCode_PUSH:
            return;
        break; case hw_OpCode_ADD: {
            hw_uint dest = *z__bytes_get(f->data.bytes.data, hw_uint, 0);
            hw_uint res = *z__bytes_get(f->data.bytes.data, hw_uint, 8);
            hw_Var *dest_var = stack->data + dest;
            hw_Var *res_var = stack->data + res;
            hw_Var_call(dest_var, unq.number.add, *res_var);
        }
        break;
        }
        b+= 1;
    } while(1);
}
*/

static void fprint_disassembleChunk(FILE *fp, struct hw_codeChunk chunk, hw_byte const *data, hw_uint data_size)
{
    #define chunk_case(OpCode, OpCode_name) break; case hw_OpCode(OpCode): fprintf(fp, "%-16s - %-8llu", OpCode_name, chunk.data_index); break;
    switch (chunk.op_code) {
        chunk_case(RET, "RETURN");
        chunk_case(ADD, "ADD");
        chunk_case(PUSH, "PUSH");
        default: fprintf(fp, "Unknown Instruction (%hhu) - %16llu", chunk.op_code, chunk.data_index);
    }
}

static void fprint_disassembleFunc(FILE *fp, hw_Func const *f)
{
    for (size_t i = 0; i < f->code.lenUsed; i++) {
        fprintf(fp, "  %04zu | ", i); fprint_disassembleChunk(fp, f->code.data[i], f->const_data.data, f->const_data.lenUsed); fputc('\n', fp);
    }
}

TEST(byte_code) {

    hw_Module mod;
    hw_Module_new(&mod, 0, z__Str("main", 4));

    hw_Func *f = hw_Module_add_blank_fn(&mod, z__Str("main", 4));
    
//    hw_Func_emit_code_byteStream(f, (hw_byte []){hw_OpCode(PUSH)}, sizeof(hw_byte));
//    hw_Func_emit_code_byteStream(f, (hw_uint []){0x00}, sizeof(hw_uint));

    hw_Func_emit_instruction(f, hw_OpCode(ADD), 0x0 * 0);
    hw_Func_emit_instruction(f, hw_OpCode(RET), 0x2 * 8);

    fprint_disassembleFunc(stdout, f);

    hw_TypeSys ts;
    type_system_init(&ts);

    hw_VarList v;
    z__Arr_new(&v, 3);

    z__Arr_pushInc(&v);
    z__Arr_pushInc(&v);

    v.data[0] = hw_Var_new_from_tstr(&ts, z__Str("int", 3));
    v.data[1] = hw_Var_new_from_tstr(&ts, z__Str("int", 3));
    v.data[2] = hw_Var_new_from_tstr(&ts, z__Str("list", 4));

    hw_Var_call(v.data + 0, core.ctor, (hw_uint []){24});
    hw_Var_call(v.data + 1, core.ctor, (hw_uint []){2});


    z__test_assert(1, "");
    z__test_done();
}

TEST(token) {

    #define cstr(c) z__Str(c, sizeof(c))
    #define testtoken\
        cstr(\
          "fn main(args) {\n"\
          "    let x = 23;\n"\
          "    let y = x * 2;\n"\
          "    y += x;\n"\
          "    if x != 2 and y != x { return; }\n"\
          "}\n"\
        )

    hw_debug_print_source_tokens(testtoken);
    
    z__test_assert(1, "");
    z__test_done();
}


#endif

#if 0
void int_bytecode_add(hw_uint x, hw_uint y, hw_uint *result)
{
    *result = x + y;
}

void int_bytecode_addlist(hw_List const *list, hw_uint const *val_indexes, hw_uint const how_many, hw_List *result_list, hw_uint const from)
{
    hw_uint result = list->data_as._u64[val_indexes[0]];
    for (hw_uint i = 1; i < how_many; i++) {
        result += list->data_as._u64[val_indexes[i]];
    }
    result_list->data_as._u64[from] = result;
}

hw_Status int_add(hw_int *x, hw_int y)
{
    *x += y;
    return hw_Status_OK();
}

hw_Status int_addlist(hw_List *self, hw_uint index, hw_uint with_index, hw_List const *with)
{
    self->data_as._i64[index] += with->data_as._i64[with_index];
    return hw_Status_OK();
}


TEST(basic) {
    
    _Static_assert(sizeof(hw_List) == 8 * 4, "");
    hw_TypeSys ts;
    hw_TypeSys_new(&ts);
    hw_TypeFn tfn;

    void hw_TypeFn_get_for_int(hw_TypeFn *t);
    hw_TypeFn_get_for_int(&tfn);

    hw_TypeSys_addtype(&ts, "int", 3, sizeof(hw_uint), &tfn);
    hw_Type *type = (hw_Type *) hw_TypeSys_getreff(&ts, z__Str("int", 3));

    hw_assert(type, "");

    hw_List l, l2; 
    hw_List_new_from_tstr(&l, 16, &ts, z__Str("int", 3));
    hw_List_new_from_tstr(&l2, 16, &ts, z__Str("int", 3));
    hw_assert_list(&l);
    hw_assert_list(&l2);
    
    hw_List_call(&l, core.push, 3, hw_make_datalist((hw_uint[]){1, 2, 3}, (hw_uint[]){3}));
    hw_List_call(&l2, core.push, 3, hw_make_datalist((hw_uint[]){10, 20, 30}, (hw_uint[]){3}));
    
    type->typeID = hw_TYPEID(i64);
    hw_describe_list(stdout, &l); puts("");
    hw_describe_list_data(stdout, &l); puts("");

    hw_describe_list(stdout, &l2); puts("");
    hw_describe_list_data(stdout, &l2); puts("");


    z__test_assert(1, "");
    z__test_done(
        hw_List_call(&l, core.del);
        hw_List_call(&l2, core.del);
    );
}

static void type_system_init(hw_TypeSys *ts)
{
    hw_TypeSys_new(ts);
    hw_TypeFn tfn;

    hw_TypeFn_get_for_int(&tfn);
    hw_TypeSys_addtype(ts, "int", 3, sizeof(hw_uint), &tfn);
    hw_Type *type_int = (hw_Type *) hw_TypeSys_getreff(ts, z__Str("int", 3));
    type_int->typeID = hw_TYPEID(i64);

    hw_assert(type_int, "Type Not Added");
    
    memset(&tfn, 0, sizeof(tfn));
    hw_TypeFn_get_for_list(&tfn);
    hw_TypeSys_addtype(ts, "list", 4, sizeof(hw_List), &tfn);
    hw_Type *type_list = (hw_Type *) hw_TypeSys_getreff(ts, z__Str("list", 4));
    type_int->typeID = hw_TYPEID(list);

    hw_assert(type_list, "Type Not Added");
}

static void list_list_pushsoft(hw_List *list, hw_List elem)
{
    if(list->lenUsed >= list->len) {
        hw_List_call(list, core.expand, list->lenUsed);
    }
    list->data_as._list[list->lenUsed] = elem;
    list->lenUsed += 1;
}

struct hw_Var {
    union {
        hw_uint     _u64;
        hw_int      _i64;
        hw_float    _f64;
        hw_byte     _byte;
        
        
    };
    hw_Type *type;
};

TEST(list) {
    hw_TypeSys ts;
    type_system_init(&ts);

    hw_List list_int, list_list; 
    hw_List_new_from_tstr(&list_int, 16, &ts, z__Str("int", 3));
    hw_List_new_from_tstr(&list_list, 16, &ts, z__Str("list", 4));
    hw_assert_list(&list_int);
    hw_assert_list(&list_list);

    hw_List_call(&list_int, core.push, 3, hw_make_datalist((hw_uint[]){1, 2, 3}, (hw_uint[]){3}));

    list_list_pushsoft(&list_list, list_int);

    hw_List_new_from_tstr(&list_int, 16, &ts, z__Str("int", 3));
    hw_List_call(&list_int, core.push, 3, hw_make_datalist((hw_uint[]){10, 20, 30}, (hw_uint[]){3}));
   
    hw_describe_list_data(stdout, &list_list);

    z__test_assert(1, "");
    z__test_done();
}
#endif
/*
hw_Status hw_emit_at_instruction(hw_CompilerState *cs)
{
    t = hw_Scanner_scan_skipws(&s);
    if(hw_Token_isNot(t, SYMBOL)) { return hw_Status_ERR(); }

    if(cmp("ctor_data")) {
        t = hw_Scanner_scan_skipws(&s);
        hw_assert_token_expect(t, SYMBOL);

        hw_uint *val_reff = NULL;
        hw_uint val = cs->fn_state.var_index.lenUsed;
        z__HashStr_getreff(&cs->fn_state.var_index, z__Str((char *)t.start, t.len), &val_reff);

        if(val_reff) {
            hw_Func_emit_instruction(fn, (hw_codeChunk){ .op_code = hw_OpCode(DTOR), .A = *val_reff});
            val = *val_reff;
        } else {
            z__HashStr_set(&cs->fn_state.var_index, z__Str((char *)t.start, t.len), val);
        }

        t = hw_Scanner_scan_skipws(&s);

        if(hw_Token_is(t, NUMBER)) {
            hw_uint number = strtoll(t.start, NULL, 10);
            hw_uint data_index = hw_Func_emit_data(fn, &number, sizeof(number));
            hw_Func_emit_instruction(fn, (hw_codeChunk){ .op_code = hw_OpCode(CTOR_COPY_DATA), .A = val, .x32 = data_index});
        } else if(hw_Token_is(t, FLOAT)) {
            hw_float fl = strtod(t.start, NULL);
            hw_uint data_index = hw_Func_emit_data(fn, &fl, sizeof(fl));
            hw_Func_emit_instruction(fn, (hw_codeChunk){ .op_code = hw_OpCode(CTOR_COPY_DATA), .A = val, .x32 = data_index});
        } else {
            hw_exit_with_msg("Wrong Literal %s", t.start);
        }
    }

    return hw_Status_OK();

}

hw_uint hw_compile_func(hw_CompilerState *cs)
{
    t = hw_Scanner_scan_skipws(&s);
    hw_uint deep = 0;

    while(!hw_Scanner_isAtEnd(&s)) {
        switch (t.type) {
            #define tok(name) break; case hw_TokenType(name)
            tok(AT):
                if(hw_Status_isERR(hw_emit_at_instruction(cs))) {
                    goto _L_error_return;
                }
            break;
            tok(BRACE_RIGHT): if(deep == 0) { hw_Func_emit_instruction(fn, (hw_codeChunk){.op_code = hw_OpCode(RET)}); }
            break;
            tok(BRACE_LEFT): deep += 1;
            break;
            default:
                goto _L_unknown_token;
            break;
        }
        t = hw_Scanner_scan_skipws(&s);
    }
    goto _L_return;

    _L_unknown_token:
    hw_print_cstr("Unknown token: [", sizeof("Unknown token: "));
    hw_print_str(hw_debug_get_token_name(t));
    hw_print_cstr("] `", 3);  hw_print_cstr(t.start, t.len); hw_print_cstr("`", 1);

    _L_error_return:
    hw_println("Error: {%lld} {%s}", s.line, s.current);

    _L_return:
    return 0;
}
*/


