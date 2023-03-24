#include "syscall.h"
#include "types.h"
#include "proc.h"
#include "vm.h"

extern struct task_struct* current;

size_t sys_write(unsigned int fd, const char* buf, size_t count){
    uint64 index = 0;
    while(buf[index] != '\0' && index < count){
        putc(buf[index++]);
    }
    return index;
}

size_t sys_getpid(){
    return current->pid;
}