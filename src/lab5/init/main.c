#include "printk.h"
#include "sbi.h"
#include "defs.h"
#include "traps.h"
#include "proc.h"
#include "vm.h"

extern void test();

int start_kernel() {
	schedule();
    while(1){
		printk("kernel is running\n");	
    }
    return 0;
}
