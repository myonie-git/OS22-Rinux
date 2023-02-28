#include "print.h"
#include "sbi.h"
#include "defs.h"

extern void test();

int start_kernel() {
    int val = 0;
    /*
    puts("sscratch : ");
    csr_read(sscratch, val);
    puti(val);
    puts("\n");
    val = 1;
    csr_write(sscratch, val);
    puts("Overwrite the value of sscratch : ");
    puti(val);
    puts("\nsscratch : ");
    csr_read(sscratch, val);
    puti(val);
    puts("\n");
    */
    puti(2022);
    puts(" Hello RISC-V 3190105197\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
