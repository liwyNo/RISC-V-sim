//
// Created by leewy on 2016/11/6.
//

#ifndef RISCV_SIM_VMM_H
#define RISCV_SIM_VMM_H
#include <cstring>
#include <unordered_map>
// PAGE_SIZE = 4M
#define PAGE_BIT 22
#define PAGE_SIZE (1ULL << PAGE_BIT)
#define PAGE_MASK (PAGE_SIZE - 1)
#define TAG_MASK (~PAGE_MASK)
struct block;
struct VM {
    unsigned long long TLB[3];
    unsigned char* TLBB[3];
    std::unordered_map<unsigned long long, unsigned char*> vm_pool;
    block operator[](unsigned long long offset);
    VM(){
        unsigned char* tmp = vm_pool[0] = new unsigned char[PAGE_SIZE];
        for(int i = 0; i < 3; ++i){
            TLB[i] = 0;
            TLBB[i] = tmp;
        }
    }
    bool load(const char* src, unsigned long long des, unsigned long long len) {
        while (1) {
            if (((des + len) & TAG_MASK) != (des & TAG_MASK)) {
                auto a_len = PAGE_SIZE - (des & PAGE_MASK);
                auto base = exist(des & TAG_MASK);
                memcpy(base + (des & PAGE_MASK), src, a_len);
                src += a_len;
                des += a_len;
                len -= a_len;
                continue;
            } else {
                auto base = exist(des & TAG_MASK);
                memcpy(base + (des & PAGE_MASK), src, len);
                break;
            }
        }
        return true;
    }
    bool store(char* src, unsigned long long des, unsigned long long len) {
        while (1) {
            if (((des + len) & TAG_MASK) != (des & TAG_MASK)) {
                auto a_len = PAGE_SIZE - (des & PAGE_MASK);
                auto base = exist(des & TAG_MASK);
                memcpy(src, base + (des & PAGE_MASK), a_len);
                src += a_len;
                des += a_len;
                len -= a_len;
                continue;
            } else {
                auto base = exist(des & TAG_MASK);
                memcpy(src, base + (des & PAGE_MASK), len);
                break;
            }
        }
        return true;
    }
    bool clean(unsigned long long des, unsigned long long len) {
        while (1) {
            if ((des + len) >= ((des & TAG_MASK) + PAGE_SIZE)) {
                auto a_len = PAGE_SIZE - (des & PAGE_MASK);
                auto base = exist(des & TAG_MASK);
                memset(base + (des & PAGE_MASK), 0, a_len);
                des += a_len;
                len -= a_len;
                continue;
            } else {
                auto base = exist(des & TAG_MASK);
                memset(base + (des & PAGE_MASK), 0, len);
                break;
            }
        }
        return true;
    }

    unsigned char * exist(unsigned long long tag) {
        for(int i = 0; i < 3; ++i)
            if(TLB[i] == tag)
                return TLBB[i];
        if (vm_pool.find(tag) == vm_pool.end())
            vm_pool[tag] = new unsigned char[PAGE_SIZE];
        for(int i = 0; i < 2; ++i) {
            TLB[i + 1] = TLB[i];
            TLBB[i + 1] = TLBB[i];
        }
        TLB[0] = tag;
        TLBB[0] = vm_pool[tag];
        return TLBB[0];
    }
};
struct block {
    unsigned long long offset;
    unsigned long long content;
    VM* vm;
    block(unsigned long long _offset, VM* _vm) : offset(_offset), vm(_vm) {}
    operator unsigned long long() {
        vm->store((char*)&content, offset, 8);
        return content;
    }
    block& operator=(const unsigned long long content) {
        this->content = content;
        vm->load((char*)&(this->content), offset, 8);
        return *this;
    }
};

#endif  // RISCV_SIM_VMM_H
