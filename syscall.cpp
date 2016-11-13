//
// Created by leewy on 2016/11/13.
//

#include "syscall.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cassert>
#include <iostream>
using ULL = unsigned long long;
void sys_exit(int code) { exit(code); }
ssize_t sys_read(int fd, char* buf, ssize_t n) { read(fd, buf, n); }
ssize_t sys_write(int fd, const char* buf, ssize_t n) { write(fd, buf, n); }
int sys_gettimeofday(timeval* tv) { return gettimeofday(tv, NULL); }
int sys_brk(void* ptr) { return 0; }
ULL systemCall(int sys_code, ULL a0, ULL a1, ULL a2, ULL a3) {
    ULL rval = 0;
    switch (sys_code) {
        case SYS_exit:
            sys_exit(a0);
            break;
        case SYS_read:
            rval = sys_read(a0, (char*)a1, a2);
            break;
        case SYS_write:
            rval = sys_write(a0, (const char*)a1, a2);
            break;
        case SYS_gettimeofday:
            rval = sys_gettimeofday((timeval*)a0);
            break;
        case SYS_brk:
            rval = sys_brk((void*)a0);
            break;
        default:
            std::cerr << "Error: Systemcall error." << std::endl;
            assert(false);
    }
    return rval;
}