#pragma once

#include "types.h"

enum SYS_NUMBER {
    SYS_GETPID = 172,
    SYS_WRITE = 64
};

size_t sys_write(unsigned int fd, const char* buf, size_t count);

size_t sys_getpid();