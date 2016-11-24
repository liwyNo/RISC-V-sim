//
// Created by leewy on 2016/11/10.
//

#include "vmm.h"

unsigned long long VM::operator[](unsigned long long offset) {
    //return block(offset, this);
    unsigned long long cnt;
    store((char*)&cnt, offset, 8);
    return cnt;
}