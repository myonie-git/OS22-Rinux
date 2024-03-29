#ifndef _DEFS_H
#define _DEFS_H

#include "types.h"

#define PHY_START 0X0000000080000000
#define PHY_SIZE 128 * 1024 * 1024
#define PHY_END (PHY_START + PHY_SIZE)

#define PGSIZE 0x1000
#define PGROUNDUP(addr) ((addr+PGSIZE - 1) & (~(PGSIZE - 1)))
#define PGROUNDDOWN(addr) (addr & (~(PGSIZE - 1)))

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
