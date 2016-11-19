//
// Created by leewy on 2016/11/13.
//

#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include "vmm.h"
extern unsigned long long sbrk_point;
extern VM memory;
extern long long ins_counter;
using ULL = unsigned long long;
void sys_exit(int code) { 
	printf("Total instruction: %lld", ins_counter);
	exit(code); 
}
ssize_t sys_read(int fd, ULL buf, ssize_t n) {
    char* p_buf = new char[n + 1];
    auto rval = read(fd, p_buf, n);
    memory.load(p_buf, buf, n);
    delete[] p_buf;
    return rval;
}
ssize_t sys_write(int fd, ULL buf, ssize_t n) {
    char* p_buf = new char[n + 1];
    memory.store(p_buf, buf, n);
    auto rval = write(fd, p_buf, n);
    delete[] p_buf;
    return rval;
}
int sys_gettimeofday(ULL offset) {
    timeval tv;
    auto rval = gettimeofday(&tv, NULL);
    memory.load((const char*)(&tv), offset, sizeof(timeval));
    return rval;
}
ULL sys_brk(ULL ptr) {
    if (ptr == 0)
        return sbrk_point;
    else
        return ptr;
}
int sys_close(int fd) { return close(fd); }
int sys_fstat(int fd, ULL buf) {
    struct stat st;
    auto rval = fstat(fd, &st);
    memory.load((const char*)(&st), buf, sizeof(struct stat));
    return rval;
}
ssize_t sys_lseek(int fd, size_t ptr, int whence) {
    return lseek(fd, ptr, whence);
}
ULL systemCall(int sys_code, ULL a0, ULL a1, ULL a2, ULL a3) {
    ULL rval = 0;
    switch (sys_code) {
        case SYS_exit:
            sys_exit(a0);
            break;
        case SYS_read:
            rval = sys_read(a0, a1, a2);
            break;
        case SYS_write:
            rval = sys_write(a0, a1, a2);
            break;
        case SYS_gettimeofday:
            rval = sys_gettimeofday(a0);
            break;
        case SYS_brk:
            rval = sys_brk(a0);
            break;
        case SYS_close:
            rval = sys_close(a0);
            break;
        case SYS_fstat:
            rval = sys_fstat(a0, a1);
            break;
        case SYS_lseek:
            rval = sys_lseek(a0, a1, a2);
            break;
        default:
            std::cerr << "Error: Systemcall error." << sys_code << std::endl;
            assert(false);
    }
    return rval;
}