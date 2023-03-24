#include "defs.h"

.extern start_kernel

    .section .text.init
    .globl _start
_start:
    # Set stvec = _traps

    la sp, boot_stack_top;

    call setup_vm

    call relocate

    call mm_init #call mm_init to initialize the memory management 
    
    call setup_vm_final

    call task_init #call the task_init function
    
    la t0, _traps
    csrw stvec, t0
 
    # set sie[STIE] = 1
    la t0, (1<<5)
    csrrs x0, sie, t0   

    # Set first time interrupt
    rdtime t0
    li t1, 10000000
    add  t0, t0, t1
    addi a7, x0, 0
    addi a6, x0, 0
    add  a0, x0, t0
    addi a1, x0, 0
    addi a2, x0, 0
    addi a3, x0, 0
    addi a4, x0, 0
    addi a5, x0, 0
    ecall
    
    #Set sstatus[SIE] = 1
    #csrsi sstatus, (1<<1)    
    # Lab1 Code
    # ------------------
    # - your code here -
    # -----------------
    csrr t0, sstatus
    ori t0, t0, 0x20
    csrw sstatus, t0

    call start_kernel #call the start_kernel function

relocate:
    #set ra = ra + pa2va_offset
    #set sp = sp + pa2va_offset

    li t0, PA2VA_OFFSET
    add ra, ra, t0

    add sp, sp, t0

    #set satp with early_pgtbl;
    #satp.Mode = 8, satp.ASID = 0, PPN = va

    #li t0, (8<<60 | early_pgtbl[VM_START >> 30 & 0x1FF] >> 12)
    la t0, early_pgtbl
    # + 8 * (VM_START >> 30 & 0x1FF)
    srli t0, t0, 12

    li t1, 8 << 60
    or t0, t0, t1

    csrw satp, t0 

    #flush tlb
    sfence.vma zero, zero

    #flush icache 
    fence.i 
    ret

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 * 4 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top:
