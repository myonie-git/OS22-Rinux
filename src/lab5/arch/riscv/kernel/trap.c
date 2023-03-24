#include "traps.h"
#include "printk.h"
#include "clock.h"
#include "proc.h"
#include "syscall.h"

extern struct task_struct* current;
void trap_handler(unsigned long scause, unsigned long spec, struct pt_regs *regs){
	//judge the trap type from ecause
	//if ecause == interrupt, judge if it is a timer interrupt
	//if timer interrrupt print the value,and call the clock_set_next_event()
	//other interrupt and traps can be ignored directly
	
	// if interrupt == 1, it is a interrupt
	//if exception code  == 5, it is a superviosr timer interrupt
	//printk("1");
	/*
	else if(scause == 0x00000005){
		printk("3");
	}
	else if(scause == 0x80000005){
		printk("4");
	}
	*/
	//clock_set_next_event();
	if(scause == 0x08){
		//System Call : System Write
		if(regs->x[31 - 17] == 64){
			//a0:1, a1: &buffer, a2:tail
			regs->x[31 - 10] = sys_write(1, (const char*)regs->x[31 - 11], regs->x[31 - 12]);
			//此处需要sepc + 4
			regs->sepc += 4;
		}
		else if(regs->x[31 - 17] == 172){
			regs->x[31 - 10] = sys_getpid();
			//此处需要sepc + 4
			regs->sepc += 4;
		}
	}

	if((scause >> 63) == 1){
		if((scause << 1) >> 1 == 5){
			//printk("[S] Supervisor Mode Timer interrupt\n");
			clock_set_next_event();
			do_timer();
		}
	}
}
