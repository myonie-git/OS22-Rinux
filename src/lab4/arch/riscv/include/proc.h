#include "types.h"

#define NR_TASKS (1+4) // use to control the max thread(idle threads + 31 kernel thread)
#define TASK_RUNNING 0 //To simplify the exp, all the thread only has one state

#define PRIORITY_MIN 1
#define PRIORITY_MAX 10

//record  the thread's kernel stack and user stack pointer
struct thread_info{
	uint64 kernel_sp;
	uint64 user_sp;
};

/* thread state section data structure */
struct thread_struct{
	uint64 ra;
	uint64 sp;
	uint64 s[12];
};

/* thread data structure */
struct task_struct{
	struct thread_info* thread_info;
	uint64 state; //tread state
	uint64 counter; //running time remaining 
	uint64 priority;//running priority 1 lowest, 10 highest
	uint64 pid;//thread id
	struct thread_struct thread;
};
//thread initial, create NR_TASKS thread
void task_init();

//called in timer interrupt, use to judge if there is any need to schedule
void do_timer();

//schedule, use to choose next thread to run
void schedule();

//the intrance function of thread switching
void switch_to(struct task_struct* next);


//dummy function: a cycle program, output itself's pid and a inc internal variable
void dummy();
