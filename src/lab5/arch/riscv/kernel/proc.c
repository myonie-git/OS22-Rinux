#include "proc.h"
#include "mm.h"
#include "defs.h"
#include "printk.h"
#include "rand.h"
#include "vm.h"
#include "string.h"
#include "elf.h"

extern void __dummy();
extern void _traps();

extern char uapp_start[];
extern char uapp_end[];

struct task_struct* idle; //idle process
struct task_struct* current;//point to the current running process "task current"
struct task_struct* task[NR_TASKS]; //process array, all the processes are saved at here

extern unsigned long swapper_pg_dir[512];
void task_init(){
	//call kalloc to assign a physial page for idle
	idle = (struct task_struct*)(kalloc());
	//set state for task_running
	idle->state = TASK_RUNNING;
	//IDLE do not participate in schedule, set its counter / priority to 0
	idle->counter = 0;
	idle->priority  = 0;
	//set the pid of idle as 0
	idle->pid = 0;
	idle->pgd = swapper_pg_dir;
	idle->thread.sscratch = 0;
	current = idle;
	task[0] = idle;

	for(int i = 1; i < NR_TASKS; i++){
		task[i] = (struct task_struct*)kalloc();
		task[i]->state = TASK_RUNNING;
		task[i]->counter = 0;
		task[i]->priority = rand();
		task[i]->pid = i;
		// Use alloc_page to apply an empty page for U-Mode Stack
		//task[i]->thread_info->kernel_sp

		//将swapper_pg_dir复制到每个进程的页表中
		task[i]->pgd = (pagetable_t)alloc_page();
		//memset(task[i]->pgd, 0x0, PGSIZE);
		for(int j = 0; j < 512; j++){
			task[i]->pgd[j] = swapper_pg_dir[j];
		}
		task[i]->pgd = (pagetable_t)((uint64_t)((uint64_t)task[i]->pgd - PA2VA_OFFSET));
		load_program(task[i]);

		task[i]->thread.ra =  (uint64_t)__dummy; //get the address of __dummy()
		task[i]->thread.sp = (uint64)task[i] + (uint64)PGSIZE;

		/*
		//not elf
		uint64_t u_stack = alloc_page();
		
		uint64_t pgnum = ((uint64_t)(&uapp_end) - (uint64_t)(&uapp_start) - 1) / PGSIZE + 1;
		uint64_t u_app_alloc = alloc_pages(pgnum); 
		memcpy((void*)(u_app_alloc), (void*)(&uapp_start), pgnum * PGSIZE); // copy code
    
		//将uapp以及U-Mode Stack做相应的映射
		create_mapping((uint64_t*)((uint64_t)task[i]->pgd + PA2VA_OFFSET), 0, (uint64)((uint64_t)u_app_alloc - PA2VA_OFFSET), (uint64)uapp_end - (uint64)uapp_start, PERMU | PERMX | PERMR | PERMV | PERMW);
		create_mapping((uint64_t*)((uint64_t)task[i]->pgd + PA2VA_OFFSET), (uint64_t)USER_END - PGSIZE, (uint64)((uint64_t)u_stack - PA2VA_OFFSET), (uint64)PGSIZE, PERMU  | PERMR | PERMV | PERMW);
		
		//set sepc and sscratch
		task[i]->thread.sepc = USER_START;
		task[i]->thread.sstatus = (1 << 5) |  (1 << 18);
		task[i]->thread.sscratch = USER_END;
		*/

		//设置spp，使得sret返回至U-Mode，SPIE：sret后开启中断
		//SUM：S-Mode可以访问user页面
		//create_mapping()
	}
	printk("...proc_init done!\n");
}

static uint64 load_program(struct task_struct* task){
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*)((uint64)&uapp_start);
	
	uint64_t phdr_start = (uint64_t)ehdr + ehdr->e_phoff;
	int phdr_cnt = ehdr->e_phnum;

	Elf64_Phdr* phdr;
	for(int i = 0; i < phdr_cnt; i++){
		phdr = (Elf64_Phdr*)(phdr_start + sizeof(Elf64_Phdr) * i);
		if(phdr->p_type == PT_LOAD){
						// copy the program section to another space
			//mapping the program section with corresponding size and flag
			uint64 pgnum = ((uint64)(phdr->p_vaddr) - (uint64)PGROUNDDOWN(phdr->p_vaddr) + (uint64)phdr->p_memsz - 1) / PGSIZE;
			uint64 u_app_alloc = alloc_pages(pgnum);
			uint64 addr = ((uint64_t)(&uapp_start) + phdr->p_offset);

			memcpy((void*)(u_app_alloc + (phdr->p_vaddr) - PGROUNDDOWN(phdr->p_vaddr)), (void*)(addr), phdr->p_memsz); 

            create_mapping((uint64*)((uint64_t)task->pgd + PA2VA_OFFSET), PGROUNDDOWN(phdr->p_vaddr), (uint64)((uint64)u_app_alloc - PA2VA_OFFSET), (uint64)((uint64)(phdr->p_vaddr) - (uint64)PGROUNDDOWN(phdr->p_vaddr)) + (uint64)phdr->p_memsz, phdr->p_flags << 1 | PERMV | PERMU);

		}
	}

	uint64_t u_stack = alloc_page();
	create_mapping((uint64_t*)((uint64_t)task->pgd + PA2VA_OFFSET), (uint64_t)USER_END - PGSIZE, (uint64)((uint64)u_stack - PA2VA_OFFSET), (uint64)PGSIZE, PERMU | PERMR | PERMV | PERMW);
	//pc for the user program
	task->thread.sepc = ehdr->e_entry;
    task->thread.sstatus = (1 << 18) | (1 << 5);
    task->thread.sscratch = USER_END;
}

void dummy(){
        uint64 MOD = 1000000007;
        uint64 auto_inc_local_var = 0;
        int last_counter = -1;
        while(1){
                if(last_counter == -1 || current->counter != last_counter){
                        last_counter = current->counter;
                        auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
						printk("[PID = %d] is running. COUNTER = %d auto_inc_local_var = %d. Thread space begin at %lx\n", current->pid, current->counter,auto_inc_local_var, current->thread.sp);
                        //printk("[PID = %d] is running. auto_inc_local_var = %d\n",current->pid, auto_inc_local_var);
                }
        }
}

extern void __switch_to(struct task_struct* prev, struct task_struct* next);

void switch_to(struct task_struct* next){
	if(next->pid == current->pid) return;
	else __switch_to(current, next);
}

void do_timer(void){
	if(current->pid == 0){
		schedule();
	}
	else{
		if(!current->counter){
			schedule();
		}
		else if(!--current->counter){
			schedule();
		}
	}
}

void schedule(){
	printk("\nScheduling...\n");
	int tmp = 0;
	int NextIndex = 1;
#ifdef DSJF
	printk("DSJF\n");
	for(int i = 1; i < NR_TASKS; i++){
		if(task[i]->counter != 0){
			if(tmp == 0 || tmp > task[i]->counter){
				tmp = task[i]->counter;
				NextIndex = i;
			}
		}
	}
#elif DPRIORITY
	printk("DPRIORITY\n");
        for(int i = 1; i < NR_TASKS; i++){
                if(task[i]->counter != 0){
                        if(tmp < task[i]->priority){
                                tmp = task[i]->priority;
                                NextIndex = i;
                        }
                }
        }
#else
	printk("None\n");
#endif
	if(tmp != 0){
		printk("Switch to PID = %d, Count = %d, Priority = %d\n", NextIndex, task[NextIndex]->counter, task[NextIndex]->priority);
		switch_to(task[NextIndex]);
	}
	else{
		printk("\nNo thread is running!\n Initializing...\n");
		for(int i = 1; i < NR_TASKS; i++){
			task[i]->counter = rand();
			printk("Set [PID = %d]; PRIORITY = %d; COUNTER = %d\n", task[i]->pid, task[i]->priority, task[i]->counter);
		}
		printk("\nReScheduling...\n");
		schedule();
	}
}
