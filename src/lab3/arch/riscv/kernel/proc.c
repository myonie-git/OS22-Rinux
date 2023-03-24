#include "proc.h"
#include "mm.h"
#include "defs.h"
#include "printk.h"
#include "rand.h"

extern void __dummy();
extern void _traps();
struct task_struct* idle; //idle process
struct task_struct* current;//point to the current running process "task current"
struct task_struct* task[NR_TASKS]; //process array, all the processes are saved at here

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
	current = idle;
	task[0] = idle;

	for(int i = 1; i < NR_TASKS; i++){
		task[i] = (struct task_struct*)kalloc();
		task[i]->state = TASK_RUNNING;
		task[i]->counter = 0;
		task[i]->priority = rand();
		task[i]->pid = i;
		task[i]->thread.ra =  &__dummy; //get the address of __dummy()
		task[i]->thread.sp = (uint64)task[i] + (uint64)PGSIZE;
	}
	printk("...proc_init done!\n");
}

void dummy(){
        uint64 MOD = 1000000007;
        uint64 auto_inc_local_var = 0;
        int last_counter = -1;
        while(1){
                if(last_counter == -1 || current->counter != last_counter){
                        last_counter = current->counter;
                        auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
                        printk("[PID = %d] is running. auto_inc_local_var = %d\n",current->pid, auto_inc_local_var);
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
