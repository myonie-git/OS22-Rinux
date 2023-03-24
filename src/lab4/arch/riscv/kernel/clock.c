#include "sbi.h"
#include "clock.h"

unsigned long TIMECLOCK = 10000000;
unsigned long get_cycles(){
	//use the inter asm, get time reg(mtime reg) value and return
	unsigned long long  _tmp;
	register uint64 __v;
	__asm__ volatile(
		"rdtime %[__v]\n"
		:[__v] "=r" (__v)
		:
		: "memory");
	_tmp = __v;
	return _tmp;
}

void clock_set_next_event(){
	// next the time of the next timer interrupt
	unsigned long next = get_cycles() + TIMECLOCK;
	sbi_ecall(0x0, 0x0, next, 0, 0, 0, 0, 0);
	//use sbi_ecall to finish the set of the next timer interrupt
	
	return;
}
