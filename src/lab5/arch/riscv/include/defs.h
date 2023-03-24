#ifndef _DEFS_H
#define _DEFS_H


#define OPENSBI_SIZE (0x200000)

#define VM_START (0xffffffe000000000)
#define VM_END   (0xffffffff00000000)
#define VM_SIZE  (VM_END - VM_START)

#define PA2VA_OFFSET (VM_START - PHY_START)

#define PHY_START (0X0000000080000000)
#define PHY_SIZE (128 * 1024 * 1024)
#define PHY_END (PHY_START + PHY_SIZE)

#define PGSIZE (0x1000)
#define PGROUNDUP(addr) ((addr+PGSIZE - 1) & (~(PGSIZE - 1)))
#define PGROUNDDOWN(addr) (addr & (~(PGSIZE - 1)))

#define PERMX (0x08)
#define PERMW (0x04)
#define PERMR (0x02)
#define PERMV (0x01)
#define PERMU (0x10)

#define USER_START (0x0000000000000000) // user space start virtual address
#define USER_END   (0x0000004000000000) // user space end virtual address

#define PGTBL_PHYPPN (0x00FFFFFFFFFFF000)
#define PGTBL_ENTPPN (0x003FFFFFFFFFFC00)

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
