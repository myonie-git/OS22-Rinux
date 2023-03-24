#include "traps.h"
#include "printk.h"
#include "clock.h"
#include "proc.h"
void trap_handler(unsigned long scause, unsigned long spec){
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
	if((scause >> 63) == 1){
		if((scause << 1) >> 1 == 5){
			printk("[S] Supervisor Mode Timer interrupt\n");
			clock_set_next_event();
			do_timer();
		}
	}
}
