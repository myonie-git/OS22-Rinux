#pragma once
#include "proc.h"
void trap_handler(unsigned long scause, unsigned long spec, struct pt_regs *regs);

extern unsigned char TIMECLOCKHAPPENED;

