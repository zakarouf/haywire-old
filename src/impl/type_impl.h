#ifndef ZAKAROUF_HAYWIRE_IMPL_TYPESYS_H
#define ZAKAROUF_HAYWIRE_IMPL_TYPESYS_H

typedef struct hw_TypeFn hw_TypeFn;
typedef struct hw_TypeSys hw_TypeSys;

void hw_interface_set_for_list(hw_TypeFn *tfn);
void hw_interface_set_for_u64(hw_TypeFn *tfn);
void hw_interface_set_for_i64(hw_TypeFn *tfn);
void hw_interface_set_for_f64(hw_TypeFn *tfn);

void hw_TypeSys_init_default(hw_TypeSys *ts);

#endif

