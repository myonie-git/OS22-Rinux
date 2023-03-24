#include "printk.h"
#include "sbi.h"
#include "defs.h"
#include "traps.h"
#include "proc.h"

extern void test();

int start_kernel() {
    while(1){
	for(int i = 0;i<10000;i++){
		for(int j = 0;j<10000;j++){
		}
	}	
	printk("kernel is running\n");	
    }
    return 0;
}
