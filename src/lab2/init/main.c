#include "printk.h"
#include "sbi.h"
#include "defs.h"
#include "traps.h"
extern void test();

int start_kernel() {
    unsigned char LASTCLOCKINDEX = 0;
    while(1){
	for(int i = 0;i<10000;i++){
		for(int j = 0;j<10000;j++){
		}
	}	
	printk("kernel is running\n");	
    }
    //test(); // DO NOT DELETE !!!

	return 0;
}
