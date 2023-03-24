#include "defs.h"	
	.section .text.entry
	.align 2
	.globl _traps
_traps:
	#save 32 reg and sepc to stack

	csrr t0, sscratch
	beq t0, x0, Continue_Trap1
	csrw sscratch, sp
	addi sp, t0, 0

	/*
	la t0, current
	#load current.sscratch
	ld t2, 0xa0(t0)
	
	bne t2, x0, Continue_Trap1
	#la t0, current
	##load the sp and sscratch
	ld t1, 0x28(t0)
	#ld t2, 0xa0(t0)
	sd t2, 0x28(t0)
	sd t1, 0xa0(t0)
	*/
Continue_Trap1:

	#addi x10, x0, 100;
	sd x0, -0x08(sp);
	sd x1, -0x10(sp);
	sd x2, -0x18(sp);
	sd x3, -0x20(sp);
	sd x4, -0x28(sp);
    sd x5, -0x30(sp);
    sd x6, -0x38(sp);
    sd x7, -0x40(sp);
    sd x8, -0x48(sp);
    sd x9, -0x50(sp);
    sd x10, -0x58(sp);
    sd x11, -0x60(sp);
    sd x12, -0x68(sp);
    sd x13, -0x70(sp);
    sd x14, -0x78(sp);
    sd x15, -0x80(sp);
    sd x16, -0x88(sp);
    sd x17, -0x90(sp);
    sd x18, -0x98(sp);
    sd x19, -0xa0(sp);
    sd x20, -0xa8(sp);
    sd x21, -0xb0(sp);
    sd x22, -0xb8(sp);
    sd x23, -0xc0(sp);
    sd x24, -0xc8(sp);
    sd x25, -0xd0(sp);
	sd x26, -0xd8(sp);
    sd x27, -0xe0(sp);
    sd x28, -0xe8(sp);
    sd x29, -0xf0(sp);
    sd x30, -0xf8(sp);
    sd x31, -0x100(sp);
	
	csrr t0, sepc;
	sd t0, -0x108(sp);
	csrr t0, sstatus;
	sd t0, -0x110(sp);
	addi sp, sp, -0x118;
	sd x2, 0(sp);

	#save the value of sepc and x1~x32
	#可能需要我们再添加一个sscratch
	#call trap_handler
	#li t0, 0x80000005;
	
	csrr a0, scause;
	csrr a1, sepc;
	add a2, sp, x0;
	call trap_handler;

	#restore sepc and 32 reg(x2 should be restore last) from stack
	ld x2, 0(sp);
	addi sp, sp, 0x118;
    ld x1, -0x10(sp);
    ld x3, -0x20(sp);
    ld x4, -0x28(sp);
    ld x5, -0x30(sp);
    ld x6, -0x38(sp);
    ld x7, -0x40(sp);
    ld x8, -0x48(sp);
    ld x9, -0x50(sp);
    ld x10, -0x58(sp);
    ld x11, -0x60(sp);
    ld x12, -0x68(sp);
    ld x13, -0x70(sp);
    ld x14, -0x78(sp);
    ld x15, -0x80(sp);
    ld x16, -0x88(sp);
    ld x17, -0x90(sp);
    ld x18, -0x98(sp);
    ld x19, -0xa0(sp);
    ld x20, -0xa8(sp);
    ld x21, -0xb0(sp);
    ld x22, -0xb8(sp);
    ld x23, -0xc0(sp);
    ld x24, -0xc8(sp);
    ld x25, -0xd0(sp);
    ld x26, -0xd8(sp);
    ld x27, -0xe0(sp);
    ld x28, -0xe8(sp);
    ld x29, -0xf0(sp);
    ld x30, -0xf8(sp);
    ld x31, -0x100(sp);
	ld t0,  -0x108(sp);
	csrw sepc, t0;
	ld t0,  -0x110(sp);
	csrw sstatus, t0;

	csrr t0, sscratch
	beq t0, x0, Continue_Trap2
	csrw sscratch, sp
	addi sp, t0, 0

Continue_Trap2:

	sret;
	
    .globl __dummy
__dummy:

	csrr t0, sscratch
	csrw sscratch, sp
	add sp, t0, x0
	sret
/*
	la t0, dummy
	csrw sepc, t0
	##change the value of sp and sscratch

	la t0, current
	##load the sp and sscratch
	ld t1, 0x28(t0)
	ld t2, 0xa0(t0)
	sd t2, 0x28(t0)
	sd t1, 0xa0(t0)


    sret
*/
	.globl __switch_to
	
__switch_to:

	#save state prev process
	#save ra, sp, s0-s11
	#thread.ra : 0x28
	#thread.sp : 0x30
	#s[0]:0x38
	#get the address of sp
	
	sd ra, 0x28(a0);
	sd sp, 0x30(a0);
	
	sd s0, 0x38(a0);
	sd s1, 0x40(a0);
	sd s2, 0x48(a0);
	sd s3, 0x50(a0);
	sd s4, 0x58(a0);
	sd s5, 0x60(a0);
	sd s6, 0x68(a0);
	sd s7, 0x70(a0);	
	sd s8, 0x78(a0);
	sd s9, 0x80(a0);
	sd s10, 0x88(a0);
	sd s11, 0x90(a0);

	#save the sepc
	csrr t0, sepc;
	sd t0, 0x98(a0);
	
	csrr t0, sstatus
	sd t0, 0xa0(a0);

	csrr t0, sscratch;
	sd t0, 0xa8(a0);

	#restore state from next process
	ld ra, 0x28(a1);
	ld sp, 0x30(a1);
	ld s0, 0x38(a1);
	ld s1, 0x40(a1);
	ld s2, 0x48(a1);
	ld s3, 0x50(a1);
	ld s4, 0x58(a1);
	ld s5, 0x60(a1);
	ld s6, 0x68(a1);
	ld s7, 0x70(a1);	
	ld s8, 0x78(a1);
	ld s9, 0x80(a1);
	ld s10,0x88(a1);
	ld s11,0x90(a1);

	ld t0, 0x98(a1)
	csrw sepc, t0

	ld t0, 0xa0(a1)
	csrw sstatus, t0

	ld t0, 0xa8(a1)
	csrw sscratch, t0

	#add the switching pgt logic 
	ld t0, 0xb0(a1)

	#li t1, PA2VA_OFFSET
	#sub t0, t0, t1

	srli t0, t0, 12
	li t1, 8 << 60
	or t0, t0, t1

	csrw satp, t0
	#flush memory
	sfence.vma zero, zero
	fence.i

	la t0, current;
	sd a1, 0x00(t0);
	
	ret
