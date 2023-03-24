#include "print.h"
#include "sbi.h"

void puts(char *s) {
	int i = 0;
 	while(s[i]!='\0'){
		sbi_ecall(0x1, 0x0, s[i], 0, 0, 0, 0, 0);
		i++;
	};	
	return;
	// unimplemented
}

void puti(int x) {
   	if(x<0){
		sbi_ecall(0x1, 0x0, '-', 0, 0, 0, 0, 0);
		x = -x;
	}
	else if(x == 0){
		sbi_ecall(0x1, 0x0, '0', 0, 0, 0, 0, 0);
		return;
	}
	while(x!=0){
		char output[20];
		char cnt = 0;
		while(x != 0){
			output[cnt++] = x % 10 + '0';
			x /= 10;
		}
		while(cnt){
			sbi_ecall(0x1, 0x0, output[--cnt],  0, 0, 0, 0, 0);
		}
	}
	return;
	// unimplemented
}
