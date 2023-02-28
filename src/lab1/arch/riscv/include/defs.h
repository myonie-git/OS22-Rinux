#ifndef _DEFS_H
#define _DEFS_H

#include "types.h"

#define csr_read(csr,val)                       \
({                                          \
    register uint64 __v;                    \
    asm volatile("csrr %[__v]," #csr        \
     		   :[__v] "=r" (__v)        \
    		   :                        \
                   : "memory");             \
     val = __v;                             \
})                                          \

#define csr_write(csr, val)                         \
({                                                  \
    uint64 __v = (uint64)(val);                     \
    asm volatile ("csrw " #csr ", %0"               \
                    : : "r" (__v)                   \
                    : "memory");                    \
})

#endif
