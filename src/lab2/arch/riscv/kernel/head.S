.extern start_kernel

    .section .text.init
    .globl _start
_start:
    # Set stvec = _traps
    
    la sp, boot_stack_top;   

    la t0, _traps
    csrw stvec, t0
 
    # set sie[STIE] = 1
    la t0, (1<<5)
    csrrs x0, sie, t0    

    # Set first time interrupt
    rdtime t0
    li t1, 1000000
    add  t0, t0, t1
    addi a7, x0, 0
    addi a6, x0, 0
    add  a0, x0, t1
    addi a1, x0, 0
    addi a2, x0, 0
    addi a3, x0, 0
    addi a4, x0, 0
    addi a5, x0, 0
    ecall
    


    #Set sstatus[SIE] = 1
    csrsi sstatus, (1<<1)    
    # Lab1 Code
    # ------------------
    # - your code here -
    # -----------------

    #la sp, boot_stack_top # set sp to the top of the stack
    call start_kernel #call the start_kernel function

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top:
