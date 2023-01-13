#ifndef ZAKAROUF_HAYWIRE_IO_H
#define ZAKAROUF_HAYWIRE_IO_H

#include "../core/base.h"
#include "var.h"

void hw_describe_var(FILE *fp, hw_Var v);
void hw_describe_var_data(FILE *fp, hw_Var v);

void hw_print_str(const z__Str str);
void hw_print_cstr(const char *str, hw_uint len);
void hw_putc(const char ch);

#define hw_fprint(fp, fmt, ...) z__fprint(fp, fmt ,##__VA_ARGS__)
#define hw_print(fmt, ...) hw_fprint(stdout, fmt ,##__VA_ARGS__)
#define hw_println(fmt, ...) hw_print(fmt "\n" ,##__VA_ARGS__)

#endif
