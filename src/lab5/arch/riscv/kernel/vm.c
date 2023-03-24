#include "vm.h"
#include "defs.h"
#include "mm.h"
#include "types.h"
#include "string.h"
#include "printk.h"

extern char _stext[];
extern char _etext[];
extern char _srodata[];
extern char _erodata[];
extern char _sdata[];
extern char _ekernel[];

/* early_pgtbl: use to the 1GB map for setup_vm */
unsigned long early_pgtbl[512] __attribute__ ((__aligned__(0x1000)));

void setup_vm(void){
	
	early_pgtbl[PHY_START >> 30 & 0x1FF] = (PERMR | PERMV | PERMW | PERMX) | (PHY_START & PGTBL_PHYPPN) >> 2;//PERMR | PERMV | PERMW | PERMV | (PHY_START & PGTBL_BIT) >> 2;
	early_pgtbl[VM_START >> 30 & 0x1FF] = early_pgtbl[PHY_START >> 30 & 0x1FF];
	
	
	//printk("...setup_vm!");

	return;
	//1GB map, no need to use mulitlevel page
	//va:64bit
	//high bit | 9 bit | 30 bit |
	//high bit can be ignored, 9 bit => early_pgtbl's index
	//30bit in page offset 30 = 9 + 9 +12
}

unsigned long swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));
void setup_vm_final(void){
	memset(swapper_pg_dir, 0x0, PGSIZE);
	
	
	//early_pgtbl[PHY_START >> 30 & 0x1FF] = (PERMR | PERMV | PERMW | PERMX) | (PHY_START & PGTBL_PHYPPN) >> 2;//PERMR | PERMV | PERMW | PERMV | (PHY_START & PGTBL_BIT) >> 2;
	//early_pgtbl[VM_START >> 30 & 0x1FF] = early_pgtbl[PHY_START >> 30 & 0x1FF];
	

	//No OpenSBI mapping required
	
	//mapping kernel text X|-|R|V
	//mapping kernel rodata -|-|R|V
	///mapping other memory -|W|R|V
	create_mapping(swapper_pg_dir, (uint64)_stext, (uint64)(_stext - PA2VA_OFFSET), (uint64)_etext - (uint64)_stext, PERMX | PERMR | PERMV); //.text
	//create_mapping(); //rodata
	//create_mapping(); //other(data + bss)

	create_mapping(swapper_pg_dir, (uint64)_srodata, (uint64)(_srodata - PA2VA_OFFSET), (uint64)_erodata - (uint64)_srodata, PERMR | PERMV);

	create_mapping(swapper_pg_dir, (uint64)_sdata, (uint64)(_sdata - PA2VA_OFFSET), (uint64)VM_START + (uint64)PHY_SIZE - (uint64)_sdata, PERMW | PERMR | PERMV);

	//set satp with swapper_pg_dir
	uint64 SATP;
	SATP = (uint64)(((uint64)&swapper_pg_dir[0] - (uint64)PA2VA_OFFSET) >> 12 | (uint64)8 << 60);
	csr_write(satp, SATP);
	asm volatile("sfence.vma zero, zero");
	printk("...setup_vm_final done!\n");
	/*
	unsigned long int*p = 0xffffffe000202000; 
	unsigned long int raw = *p;
	*p = raw;
	*/
	return;
}

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm){

	
	//use kalloc() to get a page as the page table entry

	//judge the table exists or not
	//first: find the page table 
	//root table:
	//early_pgtbl[PHY_START >> 30 & 0x1FF] = 0x0F | (PHY_START & 0x00FFFFFFFFFFF000) >> 2;

	//int index = va >> 30 & 0x1FF;
	//uint64 tmp_va = va;
	uint64 max_address = va + sz;
	uint64 tmp_va3 = va >> 12 & 0x1FF;
	uint64 tmp_va2 = va >> 21 & 0x1FF;
	uint64 tmp_va1 = va >> 30 & 0x1FF;
	uint64 *sec_pgtbl;
	uint64 *thr_pgtbl;
	uint64 CleanBit3 = 0;
	uint64 CleanBit2 = 0;
	uint64 CleanBit1 = 0;

	//getting the page
	//uint64 test = PA2VA_OFFSET + (uint64)((pgtbl[tmp_va1] & PGTBL_ENTPPN) << 2);
	//test += (uint64)((pgtbl[tmp_va1] & PGTBL_ENTPPN)) << 2;
	//test += (uint64)PA2VA_OFFSET;  
	
	for(; va < max_address; va += PGSIZE, pa += PGSIZE){
		if(CleanBit1 == 0 || tmp_va1 != (uint64)(va >> 30 & 0x1FF)){
			//load the first page first
			CleanBit1 = 1;
			tmp_va1 = va >> 30 & 0x1FF;
			if(!(pgtbl[tmp_va1] & 0x1)){//if the page table not exist, create a table
				uint64 tmp = kalloc() - PA2VA_OFFSET;//create a new page => virtual address
				pgtbl[tmp_va1] = PERMV | ((tmp & PGTBL_PHYPPN) >> 2);
				memset((uint64 *)((uint64)tmp + PA2VA_OFFSET), 0x0, PGSIZE);
			}
			CleanBit2 = 0;
		}
		
		sec_pgtbl = (uint64 *)(PA2VA_OFFSET + (uint64)((pgtbl[tmp_va1] & PGTBL_ENTPPN) << 2)); 		
		if(CleanBit2 == 0 || tmp_va2 != (uint64)(va >> 21 & 0x1FF)){
			CleanBit2 = 1;
			//load the secondary page
			tmp_va2 = va >> 21 & 0x1FF;
			if(!(sec_pgtbl[tmp_va2] & 0x1)){
				uint64 tmp = kalloc() - PA2VA_OFFSET;
				sec_pgtbl[tmp_va2] =  PERMV | (tmp & PGTBL_PHYPPN) >> 2;
				memset((uint64 *)((uint64)tmp + PA2VA_OFFSET), 0x0, PGSIZE);
			}
			CleanBit3 = 0;
		}

		thr_pgtbl = (uint64 *)(PA2VA_OFFSET + (uint64)((sec_pgtbl[tmp_va2] & PGTBL_ENTPPN) << 2));	
		if(CleanBit3 == 0 || tmp_va3 != (uint64)(va >> 12 & 0x1FF)){
			tmp_va3 = va >> 12 & 0x1FF;			
			if(!(thr_pgtbl[tmp_va3] & 0x1)){
				//uint64 tmp = kalloc()  - PA2VA_OFFSET;
				thr_pgtbl[tmp_va3] = perm | (pa & PGTBL_PHYPPN) >> 2;
				//memset((uint64 *)tmp, 0x0, PGSIZE);
			}
		}
	}

	printk("...Creating Mapping Done\n");

/*


			if(!(pgtbl[tmp_va1] & 0x1)){//if the page table not exist, create a table
				uint64 tmp = kalloc() - PA2VA_OFFSET;//create a new page => virtual address
				pgtbl[tmp_va1] = PERMR | PERMV | PERMW | PERMX | ((tmp & PGTBL_PHYPPN) >> 2);
				memset((uint64 *)tmp, 0x0, PGSIZE);
			}
			sec_pgtbl = (uint64 *)(PA2VA_OFFSET + (uint64)((pgtbl[tmp_va1] & PGTBL_ENTPPN) << 2)); 
			//sec_pgtbl += PA2VA_OFFSET; //get the physical address
			if(!(sec_pgtbl[tmp_va2] & 0x1)){
				uint64 tmp = kalloc() - PA2VA_OFFSET;
				sec_pgtbl[tmp_va2] = PERMR | PERMV | PERMW | PERMX | (tmp & PGTBL_PHYPPN) >> 2;
				memset((uint64 *)tmp, 0x0, PGSIZE);
			}
			thr_pgtbl = (uint64 *)(PA2VA_OFFSET + (uint64)((sec_pgtbl[tmp_va2] & PGTBL_ENTPPN) << 2));
			if(!(thr_pgtbl[tmp_va3] & 0x1)){
				uint64 tmp = kalloc()  - PA2VA_OFFSET;
				thr_pgtbl[tmp_va3] = perm | (tmp & PGTBL_PHYPPN) >> 2;
				memset((uint64 *)tmp, 0x0, PGSIZE);
			}

	printk("1");
*/
	/*
	for(;va < max_address; va+= PGSIZE, pa += PGSIZE){
		if(tmp_va1 != va >> 21 & 0x1FF){
			if(tmp_va2 != va >> 30 & 0x1FF){
				//the root page is change
				tmp_va != va >> 30 & 0x1FF;
				//load the root page


			}
		}
		uint64 index = va >> 30 & 0x1FF;
		if(!(pgtbl[index] & 0x1)){//if the page table not exist, create a table
		//next page table not exists
		uint64 tmp = kalloc();
		//bind the entry
	}
	pgtbl[va >> 30 & 0x1FF] = perm | (pa & 0x00FFFFFFFFFFF000) >> 2;


	}
*/
	
}
