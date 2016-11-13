//
// Created by leewy on 2016/11/10.
//

#include "vmm.h"
block VM::operator[](unsigned long long offset) { return block(offset, this); }