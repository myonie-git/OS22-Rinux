	.section .text.entry
	.align 2
	.globl _traps
_traps:
	#save 32 reg and sepc to stack
	addi x10, x0, 100;
	sd x1, -0x08(sp);
	sd x3, -0x10(sp);
        sd x4, -0x18(sp);
        sd x5, -0x20(sp);
        sd x6, -0x28(sp);
        sd x7, -0x30(sp);
        sd x8, -0x38(sp);
        sd x9, -0x40(sp);
        sd x10, -0x48(sp);
        sd x11, -0x50(sp);
        sd x12, -0x58(sp);
        sd x13, -0x60(sp);
        sd x14, -0x68(sp);
        sd x15, -0x70(sp);
        sd x16, -0x78(sp);
        sd x17, -0x80(sp);
        sd x18, -0x88(sp);
        sd x19, -0x90(sp);
        sd x20, -0x98(sp);
        sd x21, -0xa0(sp);
        sd x22, -0xa8(sp);
        sd x23, -0xb0(sp);
        sd x24, -0xb8(sp);
        sd x25, -0xc0(sp);
        sd x26, -0xc8(sp);
        sd x27, -0xd0(sp);
        sd x28, -0xd8(sp);
        sd x29, -0xe0(sp);
        sd x30, -0xe8(sp);
        sd x31, -0xf0(sp);
	csrr t0, sepc;
	sd t0, -0xf8(sp);
	addi sp, sp, -0x100;
	sd x2, 0(sp);
		
	#call trap_handler
	#li t0, 0x80000005;
	csrr a0, scause;
	csrr a1, sepc;
	call trap_handler;


	#restore sepc and 32 reg(x2 should be restore last) from stack
	ld x2, 0(sp);
	addi sp, sp, 0x100;
        ld x1, -0x08(sp);
        ld x3, -0x10(sp);
        ld x4, -0x18(sp);
        ld x5, -0x20(sp);
        ld x6, -0x28(sp);
        ld x7, -0x30(sp);
        ld x8, -0x38(sp);
        ld x9, -0x40(sp);
        ld x10, -0x48(sp);
        ld x11, -0x50(sp);
        ld x12, -0x58(sp);
        ld x13, -0x60(sp);
        ld x14, -0x68(sp);
        ld x15, -0x70(sp);
        ld x16, -0x78(sp);
        ld x17, -0x80(sp);
        ld x18, -0x88(sp);
        ld x19, -0x90(sp);
        ld x20, -0x98(sp);
        ld x21, -0xa0(sp);
        ld x22, -0xa8(sp);
        ld x23, -0xb0(sp);
        ld x24, -0xb8(sp);
        ld x25, -0xc0(sp);
        ld x26, -0xc8(sp);
        ld x27, -0xd0(sp);
        ld x28, -0xd8(sp);
        ld x29, -0xe0(sp);
        ld x30, -0xe8(sp);
        ld x31, -0xf0(sp);
	ld t0,  -0xf8(sp);
	csrw sepc, t0;

	#return from trap
	sret;
	
        .globl __dummy
__dummy:
	la t0, dummy
	csrw sepc, t0	
        sret

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

	la t0, current;
	sd a1, 0x00(t0);
	
	ret
