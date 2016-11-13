//
// Created by leewy on 2016/11/1.
//

#ifndef RISCV_SIM_DEBUG_H
#define RISCV_SIM_DEBUG_H
#ifndef VMM_H
#include "vmm.h"
#endif
#ifndef REGISTER_H
#include "register.h"
#endif
#ifndef IOSTREAM
#include <iostream>
#endif
#ifndef STRING
#include <string>
#endif
#ifndef MAP
#include <map>
#endif
#ifndef CSTDIO
#include <cstdio>
#endif
#ifndef CSTDLIB
#include <cstdlib>
#endif
#ifdef DEBUG
#define LOG(x) fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, x)
#define LOGF(fmt, ...) \
    fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define HEX(ptr, num)                                     \
    {                                                     \
        fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);   \
        for (int _i = 0; _i < num; ++_i) {                \
            fprintf(stderr, "%x ", *((char*)(ptr) + _i)); \
        }                                                 \
        fprintf(stderr, "\n");                            \
    }
#define HEXS(s, ptr, num)                                     \
    {                                                         \
        fprintf(stderr, "%s:%d: %s ", __FILE__, __LINE__, s); \
        for (int _i = 0; _i < num; ++_i) {                    \
            fprintf(stderr, "%x ", *((char*)(ptr) + _i));     \
        }                                                     \
        fprintf(stderr, "\n");                                \
    }
#else
#define LOG(x)
#define LOGF(fmt, ...)
#define HEX(ptr, num)
#define HEXS(s, ptr, num)
#endif

class debuger {
    VM& memory;
    RegisterFile* reg;
    unsigned long long step;

    bool has_until;
    bool judge_mem;
    unsigned long long cnt;
    unsigned char op;
    unsigned long long val;

    std::map<std::string, int> reg_map;
    void map_init() {
        char buf[5];
        for (int i = 0; i < 32; ++i) {
            sprintf(buf, "x%d", i);
            reg_map[buf] = i;
            sprintf(buf, "f%d", i);
            reg_map[buf] = i;
        }
        reg_map["zero"] = 0;
        reg_map["ra"] = 1;
        reg_map["sp"] = 2;
        reg_map["gp"] = 3;
        reg_map["tp"] = 4;
        reg_map["t0"] = 5;
        reg_map["t1"] = 6;
        reg_map["t2"] = 7;
        reg_map["s0"] = 8;
        reg_map["s1"] = 9;
        reg_map["a0"] = 10;
        reg_map["a1"] = 11;
        reg_map["a2"] = 12;
        reg_map["a3"] = 13;
        reg_map["a4"] = 14;
        reg_map["a5"] = 15;
        reg_map["a6"] = 16;
        reg_map["a7"] = 17;
        reg_map["s2"] = 18;
        reg_map["s3"] = 19;
        reg_map["s4"] = 20;
        reg_map["s5"] = 21;
        reg_map["s6"] = 22;
        reg_map["s7"] = 23;
        reg_map["s8"] = 24;
        reg_map["s9"] = 25;
        reg_map["s10"] = 26;
        reg_map["s11"] = 27;
        reg_map["t3"] = 28;
        reg_map["t4"] = 29;
        reg_map["t5"] = 30;
        reg_map["t6"] = 31;
        reg_map["fp"] = 8;

        reg_map["ft0"] = 32;
        reg_map["ft1"] = 33;
        reg_map["ft2"] = 34;
        reg_map["ft3"] = 35;
        reg_map["ft4"] = 36;
        reg_map["ft5"] = 37;
        reg_map["ft6"] = 38;
        reg_map["ft7"] = 39;
        reg_map["fs0"] = 40;
        reg_map["fs1"] = 41;
        reg_map["fa0"] = 42;
        reg_map["fa1"] = 43;
        reg_map["fa2"] = 44;
        reg_map["fa3"] = 45;
        reg_map["fa4"] = 46;
        reg_map["fa5"] = 47;
        reg_map["fa6"] = 48;
        reg_map["fa7"] = 49;
        reg_map["fs2"] = 50;
        reg_map["fs3"] = 51;
        reg_map["fs4"] = 52;
        reg_map["fs5"] = 53;
        reg_map["fs6"] = 54;
        reg_map["fs7"] = 55;
        reg_map["fs8"] = 56;
        reg_map["fs9"] = 57;
        reg_map["fs10"] = 58;
        reg_map["fs11"] = 59;
        reg_map["ft8"] = 60;
        reg_map["ft9"] = 61;
        reg_map["ft10"] = 62;
        reg_map["ft11"] = 63;
    }
    unsigned long long getRegister(int val) {
        union {
            unsigned long long dword;
            double db;
        } value;
        if (val >= 32)
            value.db = reg->getFloatRegVal(val - 32);
        else
            value.dword = reg->getIntRegVal(val);
        return value.dword;
    }

   public:
    debuger(VM& _memory, RegisterFile* _reg)
        : memory(_memory), reg(_reg), step(0) {
        map_init();
    }
    void wait() {
        if ((!step && !has_until) || judge()) {
            step = 0;
            has_until = false;
            std::cout << ">> ";
            std::string tmp;
            std::cin >> tmp;
            if (tmp == "h")
                help();
            else if (tmp == "p")
                print();
            else if (tmp == "s")
                sstep();
            else if (tmp == "u")
                until();
            else {
                std::cerr << "Error: Debug input error." << std::endl;
                help();
            }
        }
    }

   private:
    void help() {
        std::cout << "Usage: [h/p/s/u] (arguments)" << std::endl;
        std::cout << "\th:\t Print this message." << std::endl;
        std::cout << "\tp (mem address / reg register): Print this value in "
                     "this memory or reigster (e.g. \"p reg s0\", \"p mem "
                     "10000\")."
                  << std::endl;
        std::cout << "\ts (num): Run num step and halt." << std::endl;
        std::cout << "\tu (condition): Run until the condition is true. (e.g. "
                     "\"u reg pc == 10000\", \"u mem 10000 >= 10000\")"
                  << std::endl;
    }
    void print() {
        std::string tmp;
        std::cin >> tmp;
        if (tmp == "mem") {
            std::cin >> tmp;
            auto value = stoull(tmp, nullptr, 16);
            value = memory[value];
            std::cout << std::ios::hex << value << std::endl;
            std::ios::dec;
        } else if (tmp == "reg") {
            std::cin >> tmp;
            int value = reg_map[tmp];
            auto ans = getRegister(value);
            std::cout << std::ios::hex << value << std::endl;
            std::ios::dec;
        } else {
            help();
        }
    }
    void sstep() {
        std::string tmp;
        std::cin >> tmp;
        auto value = stoull(tmp, nullptr, 16);
        step = value;
    }
    void until() {
        has_until = true;
        std::string tmp;
        std::cin >> tmp;
        if (tmp == "mem")
            judge_mem = true;
        else if (tmp == "reg")
            judge_mem = false;
        else {
            has_until = false;
            return;
        }
        std::cin >> tmp;
        cnt = stoull(tmp, nullptr, 16);
        std::cin >> tmp;
        if (tmp == "==")
            op = 0;
        else if (tmp == "<")
            op = 1;
        else if (tmp == ">")
            op = 2;
        else if (tmp == "<=")
            op = 3;
        else if (tmp == ">=")
            op = 4;
        else if (tmp == "!=")
            op = 5;
        else {
            has_until = false;
            return;
        }
        std::cin >> tmp;
        val = stoull(tmp, nullptr, 16);
    }
    bool judge() {
        union {
            unsigned long long dword;
            double db;
        } value;
        if (!has_until) return false;
        if (judge_mem)
            value.dword = memory[cnt];
        else
            value.dword = getRegister(cnt);
        if (judge_mem || cnt < 32) switch (op) {
                case 0:  // eq
                    return value.dword == val;
                case 1:  // less than
                    return value.dword < val;
                case 2:
                    return value.dword > val;
                case 3:
                    return value.dword <= val;
                case 4:
                    return value.dword >= val;
                case 5:
                    return value.dword != val;
                default:
                    return false;
            }
        else {
            union {
                unsigned long long dword;
                double db;
            } tmp;
            tmp.dword = val;
            switch (op) {
                case 0:  // eq
                    return value.db == tmp.db;
                case 1:  // less than
                    return value.db < tmp.db;
                case 2:
                    return value.db > tmp.db;
                case 3:
                    return value.db <= tmp.db;
                case 4:
                    return value.db >= tmp.db;
                case 5:
                    return value.db != tmp.db;
                default:
                    return false;
            }
        }
    }
};
#endif  // RISCV_SIM_DEBUG_H
