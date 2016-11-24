#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include "debug.h"
#include "register.h"
#include "syscall.h"
#include "vmm.h"
using namespace std;
#define MASK(x) ((1ULL << x) - 1)
#define SUBINT(x,y,z) (((x) >> (y)) & MASK((z)))
long long ins_counter = 0;
RegisterFile *reg;
extern VM memory;
bool is_exit = false;
int exit_code = 0;
int content;  // the integer value of one instruction
bool canjump = false;

union float_32{
    float fl;
    unsigned int ui;
};
union double_64{
    double db;
    unsigned long long dword;
};
inline void memoryWrite(ULL offset, ULL value, unsigned char bit) {
    ULL ori = memory[offset];
    ULL mask = bit == 8 ? ~0ULL : ((1ULL << (bit * 8)) - 1);
    ori = (ori & ~mask) | (value & mask);
    memory.load((char*)&ori, offset, 8);
}

// Initialize the map index and register file
inline void Initialize(ULL startAddr) {
    reg->setPC(startAddr);
    content = 0;
}

/*
This part finishes the decode part and lists aLL the R-TYPE instructions
 */
inline void sub(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val - (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void sra(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 63;  // lower 5 bits is used
    LL rdVal = (LL)rs1Val >> rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void add(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val + (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void sll(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 63;
    ULL rdVal = rs1Val << rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void slt(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal;
    if ((LL)rs1Val < (LL)rs2Val)
        rdVal = 1;
    else
        rdVal = 0;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void sltu(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    if ((ULL)rs1Val < (ULL)rs2Val)
        rdVal = 1;
    else
        rdVal = 0;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void _xor(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val ^ (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void srl(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 63;
    ULL rdVal = rs1Val >> rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void _or(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val | (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void _and(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val & (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void addw(ULL rs1Val, ULL rs2Val, int rdInt) {
    /*
    Overfows are ignored, and the low
    32-bits of the result is sign-extended to 64-bits
    and written to the destination register.
     */
    int src_1 = rs1Val;
    int src_2 = rs2Val;
    int sum = src_1 + src_2;
    LL rdVal = sum;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void sllw(ULL rs1Val, ULL rs2Val, int rdInt) {
    int shamt = rs2Val & 31;
    int src = rs1Val;
    src <<= shamt;
    LL rdVal = src;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void srlw(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 31;
    unsigned int src = rs1Val;
    ULL rdVal = src >> rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void subw(ULL rs1Val, ULL rs2Val, int rdInt) {
    int src_1 = rs1Val;
    int src_2 = rs2Val;
    int sum = src_1 - src_2;
    LL rdVal = sum;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void sraw(ULL rs1Val, ULL rs2Val, int rdInt) {
    int shamt = rs2Val & 31;
    int src = rs1Val;
    src >>= shamt;
    LL rdVal = src;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}

inline void R_TYPE_funct3_1() {
    // aLL R-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends hiere

    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            sub(rs1Val, rs2Val, rdInt);
            break;
        case 0B101:
            sra(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void R_TYPE_funct3_2() {
    // aLL R-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            add(rs1Val, rs2Val, rdInt);
            break;
        case 1:
            sll(rs1Val, rs2Val, rdInt);
            break;
        case 0B10:
            slt(rs1Val, rs2Val, rdInt);
            break;
        case 0B11:
            sltu(rs1Val, rs2Val, rdInt);
            break;
        case 0B100:
            _xor(rs1Val, rs2Val, rdInt);
            break;
        case 0B101:
            srl(rs1Val, rs2Val, rdInt);
            break;
        case 0B110:
            _or(rs1Val, rs2Val, rdInt);
            break;
        case 0B111:
            _and(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void R_TYPE_funct3_3() {
    // aLL R-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            subw(rs1Val, rs2Val, rdInt);
            break;
        case 0B101:
            sraw(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void R_TYPE_funct3_4() {
    // aLL R-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            addw(rs1Val, rs2Val, rdInt);
            break;
        case 0B101:
            srlw(rs1Val, rs2Val, rdInt);
            break;
        case 0B1:
            sllw(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void R_TYPE_funct7_1() {
    int funct7 = SUBINT(content, 25, 7);
    switch (funct7) {
        case 0B100000:
            R_TYPE_funct3_1();
            break;
        case 0B000000:
            R_TYPE_funct3_2();
            break;
        case 1:
            void M_TYPE_funct3_1();
            M_TYPE_funct3_1();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE funct7 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void R_TYPE_funct7_2() {
    int funct7 = SUBINT(content, 25, 7);
    switch (funct7) {
        case 0B100000:
            R_TYPE_funct3_3();
            break;
        case 0B0000000:
            R_TYPE_funct3_4();
            break;
        case 1:
            void M_TYPE_funct3_2();
            M_TYPE_funct3_2();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE funct7 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void R_TYPE_opcode() {
    int opcode = SUBINT(content, 0, 7);
    switch (opcode) {
        case 0B110011:
            R_TYPE_funct7_1();
            break;
        case 0B111011:
            R_TYPE_funct7_2();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "R-TYPE opcode error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End R-TYPE decode
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of I-TYPE and lists aLL the I-TYPE
instructions
 */
inline void slli() {
    LL shamt = (content >> 20) & 63;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val << shamt;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void srli() {
    LL shamt = (content >> 20) & 63;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rdVal = rs1Val >> shamt;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void srai() {
    LL shamt = (content >> 20) & 63;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val >> shamt;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void addi() {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val + immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void slti() {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    int rdVal = 0;
    if (rs1Val < immediateNum) rdVal = 1;
    reg->setIntRegVal(rdVal, rdInt);
}
// the immediate is first sign-extended to XLEN bits then treated as an unsigned
// number
inline void sltiu() {
    ULL immediateNum = (ULL)((LL)(content >> 20));  // the immediate is first
    // sign-extended to XLEN
    // bits then treated as an
    // unsigned number
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    int rdVal = 0;
    if (rs1Val < immediateNum) rdVal = 1;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void xori() {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val ^ immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void ori() {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val | immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void andi() {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val & immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void lb() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];
    loadData = (loadData << 56) >> 56;  // return value can be sign-extended
    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void lh() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];
    loadData = (loadData << 48) >> 48;  // same as the reason mentioned above
    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void lw() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);

    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];
    loadData = (loadData << 32) >> 32;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void ld() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];

    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void lbu() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL loadData = memory[(ULL)(immediateNum + rs1Val)];
    loadData = loadData & 255;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void lhu() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL loadData = memory[(ULL)(immediateNum + rs1Val)];
    loadData = loadData & 65535;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void lwu() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL loadData = memory[(ULL)(immediateNum + rs1Val)];
    long long mask = (1LL << 32) - 1;
    loadData = loadData & mask;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
inline void jalr() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    reg->setIntRegVal((ULL)(reg->getPC() + 4), rdInt);
    canjump = true;
    reg->setPC((ULL)(((rs1Val + immediateNum) >> 1) << 1));
}
inline void addiw() {
    int immediateNum = content >> 20;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    int rs1Val = (int)reg->getIntRegVal(rs1Int);
    rs1Val += immediateNum;
    LL rdVal = (LL)rs1Val;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void slliw() {
    int shamt = (content >> 20) & 31;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    int rs1Val = (int)reg->getIntRegVal(rs1Int);
    LL rdVal = (LL)(rs1Val << shamt);
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void srliw() {
    /*
    operate on 32-bit values and produce signed 32-bit results.
     */
    int shamt = (content >> 20) & 31;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    unsigned int rs1Val = (unsigned int)reg->getIntRegVal(rs1Int);
    rs1Val >>= shamt;
    LL rdVal = (LL)((int)rs1Val);
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void sraiw() {
    int shamt = (content >> 20) & 31;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    int rs1Val = (int)reg->getIntRegVal(rs1Int);
    rs1Val >>= shamt;
    LL rdVal = (LL)rs1Val;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
inline void I_TYPE_funct3_1() {
    int funct3 = SUBINT(content, 12, 3);

    switch (funct3) {
        case 1:
            slli();
            break;
        case 0B101: {
            int funct7 = SUBINT(content, 26, 6);
            switch (funct7) {
                case 0:
                    srli();
                    break;
                case 0B10000:
                    srai();
                    break;
            }
        } break;
        case 0:
            addi();
            break;
        case 0B10:
            slti();
            break;
        case 0B11:
            sltiu();
            break;
        case 0B100:
            xori();
            break;
        case 0B110:
            ori();
            break;
        case 0B111:
            andi();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "I-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void I_TYPE_funct3_2() {
    int funct3 = SUBINT(content, 12, 3);

    switch (funct3) {
        case 000:
            lb();
            break;
        case 0B1:
            lh();
            break;
        case 0B10:
            lw();
            break;
        case 0B100:
            lbu();
            break;
        case 0B101:
            lhu();
            break;
        case 0B110:
            lwu();
            break;
        case 0B11:
            ld();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "I-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void I_TYPE_funct3_3() {
    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            addiw();
            break;
        case 0B1:
            slliw();
            break;
        case 0B101: {
            int funct7 = SUBINT(content, 26, 6);
            switch (funct7) {
                case 0:
                    srliw();
                    break;
                case 0B10000:
                    sraiw();
                    break;
            }
        } break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "I-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
inline void I_TYPE_opcode() {
    int opcode = SUBINT(content, 0, 7);
    switch (opcode) {
        case 0B10011:
            I_TYPE_funct3_1();
            break;
        case 0B11:
            I_TYPE_funct3_2();
            break;
        case 0B11011:
            I_TYPE_funct3_3();
            break;
        case 0B1100111:
            jalr();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "I-TYPE opcode error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End I-TYPE decode
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of S-TYPE and lists aLL the S-TYPE
instructions
 */
inline void sb() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >>
                                                                         52;  // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL memoryAddr = immediateNum + rs1Val;
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    rs2Val &= 255;

    memoryWrite((ULL)memoryAddr, rs2Val, 1);
}
inline void sh() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >>
                                                                         52;  // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL memoryAddr = immediateNum + rs1Val;
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    rs2Val &= 65535;

    memoryWrite((ULL)memoryAddr, rs2Val, 2);
}
inline void sw() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >>
                                                                         52;  // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL memoryAddr = immediateNum + rs1Val;
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    long long mask = (1LL << 32) - 1;
    rs2Val &= mask;

    memoryWrite(memoryAddr, rs2Val, 4);
}
inline void sd() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >>
                                                                         52;  // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL memoryAddr = immediateNum + rs1Val;
    ULL rs2Val = reg->getIntRegVal(rs2Int);

    memoryWrite(memoryAddr, rs2Val, 8);
}
inline void S_TYPE_funct3() {
    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            sb();
            break;
        case 1:
            sh();
            break;
        case 0B10:
            sw();
            break;
        case 0B11:
            sd();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "S-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End S-TYPE decode
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of SB-TYPE and lists aLL the SB-TYPE
instructions
 */
inline void beq() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    if (rs1Val == rs2Val) {
        ULL immediateNum_1 = ((content >> 31) & 1) << 11;
        ULL immediateNum_2 = ((content >> 7) & 1) << 10;
        ULL immediateNum_3 = ((content >> 25) & 63) << 4;
        ULL immediateNum_4 = ((content >> 8) & 15);
        LL immediateNum =
                (immediateNum_1 + immediateNum_2 + immediateNum_3 + immediateNum_4)
                        << 1;
        immediateNum = (immediateNum << 51) >> 51;

        canjump = true;
        reg->changePC(immediateNum);
    }
}
inline void bne() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    if (rs1Val != rs2Val) {
        ULL immediateNum_1 = ((content >> 31) & 1) << 11;
        ULL immediateNum_2 = ((content >> 7) & 1) << 10;
        ULL immediateNum_3 = ((content >> 25) & 63) << 4;
        ULL immediateNum_4 = ((content >> 8) & 15);
        LL immediateNum =
                (immediateNum_1 + immediateNum_2 + immediateNum_3 + immediateNum_4)
                        << 1;
        immediateNum = (immediateNum << 51) >> 51;

        canjump = true;
        reg->changePC(immediateNum);
    }
}
inline void blt() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    if ((LL)rs1Val < (LL)rs2Val) {
        ULL immediateNum_1 = ((content >> 31) & 1) << 11;
        ULL immediateNum_2 = ((content >> 7) & 1) << 10;
        ULL immediateNum_3 = ((content >> 25) & 63) << 4;
        ULL immediateNum_4 = ((content >> 8) & 15);
        LL immediateNum =
                (immediateNum_1 + immediateNum_2 + immediateNum_3 + immediateNum_4)
                        << 1;
        immediateNum = (immediateNum << 51) >> 51;

        canjump = true;
        reg->changePC(immediateNum);
    }
}
inline void bge() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    if ((LL)rs1Val >= (LL)rs2Val) {
        ULL immediateNum_1 = ((content >> 31) & 1) << 11;
        ULL immediateNum_2 = ((content >> 7) & 1) << 10;
        ULL immediateNum_3 = ((content >> 25) & 63) << 4;
        ULL immediateNum_4 = ((content >> 8) & 15);
        LL immediateNum =
                (immediateNum_1 + immediateNum_2 + immediateNum_3 + immediateNum_4)
                        << 1;
        immediateNum = (immediateNum << 51) >> 51;

        canjump = true;
        reg->changePC(immediateNum);
    }
}
inline void bltu() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    if (rs1Val < rs2Val) {
        ULL immediateNum_1 = ((content >> 31) & 1) << 11;
        ULL immediateNum_2 = ((content >> 7) & 1) << 10;
        ULL immediateNum_3 = ((content >> 25) & 63) << 4;
        ULL immediateNum_4 = ((content >> 8) & 15);
        LL immediateNum = (LL)(immediateNum_1 + immediateNum_2 +
                               immediateNum_3 + immediateNum_4)
                << 1;
        immediateNum = (immediateNum << 51) >> 51;

        canjump = true;
        reg->changePC(immediateNum);
    }
}
inline void bgeu() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    if (rs1Val >= rs2Val) {
        ULL immediateNum_1 = ((content >> 31) & 1) << 11;
        ULL immediateNum_2 = ((content >> 7) & 1) << 10;
        ULL immediateNum_3 = ((content >> 25) & 63) << 4;
        ULL immediateNum_4 = ((content >> 8) & 15);
        LL immediateNum =
                (immediateNum_1 + immediateNum_2 + immediateNum_3 + immediateNum_4)
                        << 1;
        immediateNum = (immediateNum << 51) >> 51;

        canjump = true;
        reg->changePC(immediateNum);
    }
}
inline void SB_TYPE_funct3() {
    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            beq();
            break;
        case 1:
            bne();
            break;
        case 0B100:
            blt();
            break;
        case 0B101:
            bge();
            break;
        case 0B110:
            bltu();
            break;
        case 0B111:
            bgeu();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "SB-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End SB-TYPE decode
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of SB-TYPE and lists aLL the SB-TYPE
instructions
 */
inline void lui() {
    int rdInt = (content >> 7) & 31;
    LL immediateNum =
            (LL)((content >> 12)
                    << 12);  // The 32-bit result is sign-extended to 64 bits.

    reg->setIntRegVal((ULL)immediateNum, rdInt);
}
inline void auipc() {
    int rdInt = (content >> 7) & 31;
    LL immediateNum =
            (LL)((content >> 12)
                    << 12);  // The 32-bit result is sign-extended to 64 bits.

    ULL rdVal = reg->getPC() + (ULL)immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
inline void U_TYPE_opcode() {
    int opcode = SUBINT(content, 0, 7);
    switch (opcode) {
        case 0B110111:
            lui();
            break;
        case 0B10111:
            auipc();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "U-TYPE opcode error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End SB-TYPE decode
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of UJ-TYPE and lists aLL the UJ-TYPE
instructions
 */
inline void jal() {
    int rdInt = (content >> 7) & 31;
    LL immediateNum_1 = ((content >> 31) & 1) << 19;
    LL immediateNum_2 = ((content >> 12) & 255) << 11;
    LL immediateNum_3 = ((content >> 20) & 1) << 10;
    LL immediateNum_4 = ((content >> 21) & 1023);
    LL immediateNum =
            (immediateNum_1 + immediateNum_2 + immediateNum_3 + immediateNum_4)
                    << 1;
    immediateNum = (immediateNum << 43) >> 43;

    reg->setIntRegVal((ULL)reg->getPC() + 4, rdInt);

    canjump = true;
    reg->changePC((ULL)immediateNum);
}
/*
End UJ-TYPE decode
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part parses some system instructions
 */
inline void ecall() {
    // make it can compile first.
    LL sys_call_num = reg->getIntRegVal(17);
    ULL a0 = reg->getIntRegVal(10);
    ULL a1 = reg->getIntRegVal(11);
    ULL a2 = reg->getIntRegVal(12);
    ULL a3 = reg->getIntRegVal(13);
    ULL rval = systemCall((int)sys_call_num, a0, a1, a2, a3);
    reg->setIntRegVal(rval, 10);
}
inline void ebreak() {assert(false);}
inline void E_INS() {
    int diffPart = SUBINT(content, 20, 1);
    switch (diffPart) {
        case 0:
            ecall();
            break;
        case 1:
            ebreak();
            break;
    }
}
inline void SYS_INSTRUCTION() {
    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 0:
            E_INS();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "SYS-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End of decoding system instructions
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of M-TYPE and lists aLL the M-TYPE
instructions
 */
inline void MUL(LL rs1Val, LL rs2Val, int rdInt) {
    reg->setIntRegVal(rs1Val * rs2Val, rdInt);
}
inline void MULH(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    __asm__(
    "pushq %%rax\n\t"
            "pushq %%rdx\n\t"
            "movq %1, %%rax\n\t"
            "imulq %2\n\t"
            "movq %%rdx, %0\n\t"
            "popq %%rdx\n\t"
            "popq %%rax\n\t"
    : "=m"(rdVal)
    : "r"(rs1Val), "r"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
inline void MULHSU(LL rs1Val, ULL rs2Val, int rdInt) {
    __int128 rs1;
    unsigned __int128 rs2;
    unsigned __int128 rd;
    rd = rs1 * rs2;
    rd >>= 64;
    reg->setFloatRegVal(rd, rdInt);
//    ULL rdVal;
//    ULL sign = rs1Val & (1ULL << 63);
//    rs1Val &= MASK(63);
//    __asm__ (
//    "pushq %%rax\n\t"
//            "pushq %%rdx\n\t"
//            "movq %1, %%rax\n\t"
//            "mulq %2\n\t"
//            "movq %%rdx, %0\n\t"
//            "popq %%rdx\n\t"
//            "popq %%rax\n\t"
//    : "=m"(rdVal)
//    : "r"(rs1Val), "r"(rs2Val));
//    rdVal &= MASK(63);
//    rdVal |= sign;
//    reg->setIntRegVal(rdVal, rdInt);
}
inline void MULHU(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    __asm__(
    "pushq %%rax\n\t"
            "pushq %%rdx\n\t"
            "movq %1, %%rax\n\t"
            "mulq %2\n\t"
            "movq %%rdx, %0\n\t"
            "popq %%rdx\n\t"
            "popq %%rax\n\t"
    : "=m"(rdVal)
    : "r"(rs1Val), "r"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
inline void DIV(LL rs1Val, LL rs2Val, int rdInt) {
    assert(rs2Val);
    reg->setIntRegVal(rs1Val / rs2Val, rdInt);
}
inline void DIVU(ULL rs1Val, ULL rs2Val, int rdInt) {
    assert(rs2Val);
    reg->setIntRegVal(rs1Val / rs2Val, rdInt);
}
inline void REM(LL rs1Val, LL rs2Val, int rdInt) {
    assert(rs2Val);
    reg->setIntRegVal(rs1Val % rs2Val, rdInt);
}
inline void REMU(ULL rs1Val, ULL rs2Val, int rdInt) {
    assert(rs2Val);
    reg->setIntRegVal(rs1Val % rs2Val, rdInt);
}
inline void MULW(LL rs1Val, LL rs2Val, int rdInt) {
    int rs1 = rs1Val;
    int rs2 = rs2Val;
    reg->setIntRegVal((LL)(rs1 * rs2), rdInt);
}
inline void DIVW(LL rs1Val, LL rs2Val, int rdInt) {
    assert(rs2Val);
    int rs1 = rs1Val;
    int rs2 = rs2Val;
    reg->setIntRegVal((LL)(rs1 / rs2), rdInt);
}
inline void DIVUW(ULL rs1Val, ULL rs2Val, int rdInt) {
    assert(rs2Val);
    unsigned int rs1 = rs1Val;
    unsigned int rs2 = rs2Val;
    reg->setIntRegVal(rs1 / rs2, rdInt);
}
inline void REMW(LL rs1Val, LL rs2Val, int rdInt) {
    assert(rs2Val);
    int rs1 = rs1Val;
    int rs2 = rs2Val;
    reg->setIntRegVal((LL)(rs1 % rs2), rdInt);
}
inline void REMUW(ULL rs1Val, ULL rs2Val, int rdInt) {
    assert(rs2Val);
    unsigned int rs1 = rs1Val;
    unsigned int rs2 = rs2Val;
    reg->setIntRegVal(rs1 % rs2, rdInt);
}

void M_TYPE_funct3_1() {
    // aLL M-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 000:
            MUL((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B1:
            MULH((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B10:
            MULHSU((LL)rs1Val, rs2Val, rdInt);
            break;
        case 0B11:
            MULHU(rs1Val, rs2Val, rdInt);
            break;
        case 0B100:
            DIV((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B101:
            DIVU(rs1Val, rs2Val, rdInt);
            break;
        case 0B110:
            REM((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B111:
            REMU(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "M-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}

void M_TYPE_funct3_2() {
    // aLL M-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    int funct3 = SUBINT(content, 12, 3);
    switch (funct3) {
        case 000:
            MULW((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B100:
            DIVW((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B101:
            DIVUW(rs1Val, rs2Val, rdInt);
            break;
        case 0B110:
            REMW((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 0B111:
            REMUW(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "M-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End of decoding M-TPYE instructions
 */

//===============This is a lovely dividing line===========
//=======================================================

/*
This part finishes the decode part of FandD-TYPE and lists aLL the FandD-TYPE
instructions
 */
//---------------------lalala-------------------------
inline void FSQRT_D(int rs1Int, int rs2Int, int rdInt, int rmInt) { assert(false); }
inline void FSGNJ_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL rs1Val, rs2Val, rdVal;
    rs1Val = reg->getFloatRegVal(rs1Int);
    rs2Val = reg->getFloatRegVal(rs2Int);
    rdVal = rs1Val & (~(1ULL << 63));
    ULL mid_var = (rs2Val >> 63) << 63;
    rdVal |= mid_var;
    reg->setFloatRegVal(rdVal, rdInt);
}
inline void FSGNJN_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL rs1Val, rs2Val, rdVal;
    rs1Val = reg->getFloatRegVal(rs1Int);
    rs2Val = reg->getFloatRegVal(rs2Int);
    rdVal = rs1Val & (~(1ULL << 63));
    ULL mid_var = ((~rs2Val) >> 63) << 63;
    rdVal |= mid_var;
    reg->setFloatRegVal(rdVal, rdInt);
}
inline void FSGNJX_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL rs1Val, rs2Val, rdVal;
    rs1Val = reg->getFloatRegVal(rs1Int);
    rs2Val = reg->getFloatRegVal(rs2Int);
    rdVal = rs1Val & (~(1ULL << 63));
    ULL mid_var = ((rs2Val ^ rs1Val) >> 63) << 63;
    rdVal |= mid_var;
    reg->setFloatRegVal(rdVal, rdInt);
}
inline void FSGNJ_D_funct3(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    int funct3 = rmInt;
    switch (funct3) {
        case 0:
            FSGNJ_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1:
            FSGNJN_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 2:
            FSGNJX_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        default:
            cerr << "FSGNJN_D ins was wrong!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
    }
}
inline void FMIN_MAX_D(int rs1Int, int rs2Int, int rdInt, int rmInt) { assert(false); }
inline void FCLASS(int rs1Int, int rs2Int, int rdInt, int rmInt) { assert(false); }
//---------------------lalala-------------------------

inline void FADD_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(double *)&temp1;
    ULL temp2 = reg->getFloatRegVal(rs2Int);
    double rs2Val = *(double *)&temp2;
    double result = rs1Val+rs2Val;
    reg->setFloatRegVal(*(ULL *)&result, rdInt);
}
inline void FSUB_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(double *)&temp1;
    ULL temp2 = reg->getFloatRegVal(rs2Int);
    double rs2Val = *(double *)&temp2;
    double result = rs1Val-rs2Val;
    reg->setFloatRegVal(*(ULL *)&result, rdInt);
}
inline void FMUL_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(double *)&temp1;
    ULL temp2 = reg->getFloatRegVal(rs2Int);
    double rs2Val = *(double *)&temp2;
    double result = rs1Val*rs2Val;
    reg->setFloatRegVal(*(ULL *)&result, rdInt);
}
inline void FMUL(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    unsigned int temp1 = reg->getFloatRegVal(rs1Int);
    unsigned int temp2 = reg->getFloatRegVal(rs2Int);
    float rs1Val = *(float *)&temp1;
    float rs2Val = *(float *)&temp2;
    float result = rs1Val*rs2Val;
    reg->setFloatRegVal(*(unsigned int *)&result, rdInt);
}
inline void FDIV(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    unsigned int temp1 = reg->getFloatRegVal(rs1Int);
    unsigned int temp2 = reg->getFloatRegVal(rs2Int);
    float rs1Val = *(float *)&temp1;
    float rs2Val = *(float *)&temp2;
    float result = rs1Val/rs2Val;
    reg->setFloatRegVal(*(unsigned int *)&result, rdInt);
}
inline void FDIV_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(double *)&temp1;
    ULL temp2 = reg->getFloatRegVal(rs2Int);
    double rs2Val = *(double *)&temp2;
    double result = rs1Val/rs2Val;
    reg->setFloatRegVal(*(ULL *)&result, rdInt);
}
inline void FCVT_SD(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    float rs1Val = *(double *)&temp1;
    reg->setFloatRegVal(*(unsigned int *)&rs1Val, rdInt);
}
inline void FCVT_DS(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    unsigned int temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(float *)&temp1;
    reg->setFloatRegVal(*(ULL *)&rs1Val, rdInt);
}
inline void FEQ_LT_LE(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(double *)&temp1;
    ULL temp2 = reg->getFloatRegVal(rs2Int);
    double rs2Val = *(double *)&temp2;
    if(rmInt == 0)
    {
        if(rs1Val <= rs2Val)
            reg->setIntRegVal(1, rdInt);
        else
            reg->setIntRegVal(0, rdInt);
    }
    else if(rmInt == 1)
    {
        if(rs1Val < rs2Val)
            reg->setIntRegVal(1, rdInt);
        else
            reg->setIntRegVal(0, rdInt);
    }
    else if(rmInt == 2)
    {
        if(rs1Val == rs2Val)
            reg->setIntRegVal(1, rdInt);
        else
            reg->setIntRegVal(0, rdInt);
    }
    else
    {
        cout << "fcompare ins was wrong!" << endl;
        cout << "Exit!" << endl;
        assert(false);
    }
}
inline void FCVT_WD_LD(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    ULL temp1 = reg->getFloatRegVal(rs1Int);
    double rs1Val = *(double *)&temp1;
    if(rs2Int == 0)
    {
        reg->setIntRegVal((int)rs1Val, rdInt);
    }
    else if(rs2Int == 1)
    {
        reg->setIntRegVal((unsigned int)rs1Val, rdInt);
    }
    else if(rs2Int == 2)
    {
        reg->setIntRegVal((long long)rs1Val, rdInt);
    }
    else if(rs2Int == 3)
    {
        reg->setIntRegVal((unsigned long long)rs1Val, rdInt);
    }
    else
    {
        cout << "fcvt.*.d ins was wrong!" << endl;
        cout << "Exit!" << endl;
        assert(false);
    }
}
inline void FCVT_DW_DL(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    if(rs2Int == 0)
    {
        int temp = reg->getIntRegVal(rs1Int);
        double rs1Val = temp;
        reg->setFloatRegVal(*(ULL *)&rs1Val, rdInt);
    }
    else if(rs2Int == 1)
    {
        unsigned int temp = reg->getIntRegVal(rs1Int);
        double rs1Val = temp;
        reg->setFloatRegVal(*(ULL *)&rs1Val, rdInt);
    }
    else if(rs2Int == 2)
    {
        long long temp = reg->getIntRegVal(rs1Int);
        double rs1Val = temp;
        reg->setFloatRegVal(*(ULL *)&rs1Val, rdInt);
    }
    else if(rs2Int == 3)
    {
        ULL temp = reg->getIntRegVal(rs1Int);
        double rs1Val = temp;
        reg->setFloatRegVal(*(ULL *)&rs1Val, rdInt);
    }
    else
    {
        cout << "fcvt.d.* ins was wrong!" << endl;
        cout << "Exit!" << endl;
        assert(false);
    }
}
inline void FMV_XD(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    if(rmInt != 0)
    {
        FCLASS(rs1Int, rs2Int, rdInt, rmInt);
        return;
    }
    reg->setIntRegVal(reg->getFloatRegVal(rs1Int),rdInt);
}
inline void FMV_DX(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    reg->setFloatRegVal(reg->getIntRegVal(rs1Int),rdInt);
}
inline void FCVT_SW_SL(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    if(rs2Int == 0)
    {
        int temp = reg->getIntRegVal(rs1Int);
        float rs1Val = temp;
        reg->setFloatRegVal(*(unsigned int *)&rs1Val, rdInt);
    }
    else if(rs2Int == 1)
    {
        unsigned int temp = reg->getIntRegVal(rs1Int);
        float rs1Val = temp;
        reg->setFloatRegVal(*(unsigned int *)&rs1Val, rdInt);
    }
    else if(rs2Int == 2)
    {
        long long temp = reg->getIntRegVal(rs1Int);
        float rs1Val = temp;
        reg->setFloatRegVal(*(unsigned int *)&rs1Val, rdInt);
    }
    else if(rs2Int == 3)
    {
        ULL temp = reg->getIntRegVal(rs1Int);
        float rs1Val = temp;
        reg->setFloatRegVal(*(unsigned int *)&rs1Val, rdInt);
    }
    else
    {
        cout << "fcvt.s.* ins was wrong!" << endl;
        cout << "Exit!" << endl;
        assert(false);
    }
}

// inline void FCVT_SL_SW(int rs1Int, int rs2Int, int rdInt, int rmInt) {
//     if (rs2Int == 0) {
//         float rs1Val = reg->getFloatRegVal(rs1Int);
//         reg->setIntRegVal((int)rs1Val, rdInt);
//     } else if (rs2Int == 1) {
//         float rs1Val = reg->getFloatRegVal(rs1Int);
//         reg->setIntRegVal((unsigned int)rs1Val, rdInt);
//     } else if (rs2Int == 2) {
//         float rs1Val = reg->getFloatRegVal(rs1Int);
//         reg->setIntRegVal((long long)rs1Val, rdInt);
//     } else if (rs2Int == 3) {
//         float rs1Val = reg->getFloatRegVal(rs1Int);
//         reg->setIntRegVal((unsigned long long)rs1Val, rdInt);
//     } else {
//         cerr << "fcvt.*.s ins was wrong!" << endl;
//         cerr << "Exit!" << endl;
//         assert(false);
//     }
// }
/*
the control logic
*/
inline void FLoad_funct3() {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);

    int funct3 = SUBINT(content, 12, 3);
    if (funct3 == 0B10) {
        unsigned int loadData;
        loadData = (unsigned int)(memory[immediateNum + rs1Val]);
        reg->setFloatRegVal(loadData, rdInt);
    } else if (funct3 == 0B11) {
        ULL loadData;
        loadData = memory[immediateNum + rs1Val];
        reg->setFloatRegVal(loadData, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << hex << content << endl;
        cerr << "float load funct3 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

inline void FStore_funct3() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >> 52;
    // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL memoryAddr = immediateNum + rs1Val;

    int funct3 = SUBINT(content, 12, 3);
    if (funct3 == 0B10) {
        unsigned int rs2Val = reg->getFloatRegVal(rs2Int);
        // mymem.rwmemWriteWord(*((unsigned int *)(&rs2Val)), memoryAddr);
        memoryWrite(memoryAddr, rs2Val, 4);
    } else if (funct3 == 0B11) {
        ULL rs2Val = reg->getFloatRegVal(rs2Int);
        // mymem.rwmemWriteDword(*((ULL *)(&rs2val)), memoryAddr);
        memoryWrite(memoryAddr, rs2Val, 8);
    } else {
        cerr << "Error when parsing instruction: " << hex << content << endl;
        cerr << "float load funct3 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

inline void FMadd_funct2() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    int funct2 = SUBINT(content, 25, 2);
    if (funct2 == 0) {
        float_32 rs1Val, rs2Val, rs3Val;
        rs1Val.ui = reg->getFloatRegVal(rs1Int);
        rs2Val.ui = reg->getFloatRegVal(rs2Int);
        rs3Val.ui = reg->getFloatRegVal(rs3Int);
        float rdVal = rs1Val.fl * rs2Val.fl + rs3Val.fl;
        reg->setFloatRegVal(*(unsigned int*)&rdVal, rdInt);
    } else if (funct2 == 1) {
        double_64 rs1val, rs2val, rs3val;
        rs1val.dword = reg->getFloatRegVal(rs1Int);
        rs2val.dword = reg->getFloatRegVal(rs2Int);
        rs3val.dword = reg->getFloatRegVal(rs3Int);
        double rdval = rs1val.db * rs2val.db + rs3val.db;
        reg->setFloatRegVal(*(ULL*)&rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << hex << content << endl;
        cerr << "fmadd funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

inline void FMsub_funct2() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    int funct2 = SUBINT(content, 25, 2);
    if (funct2 == 0) {
        float_32 rs1Val, rs2Val, rs3Val;
        rs1Val.ui = reg->getFloatRegVal(rs1Int);
        rs2Val.ui = reg->getFloatRegVal(rs2Int);
        rs3Val.ui = reg->getFloatRegVal(rs3Int);
        float rdVal = rs1Val.fl * rs2Val.fl - rs3Val.fl;
        reg->setFloatRegVal(*(unsigned int*)&rdVal, rdInt);
    } else if (funct2 == 1) {
        double_64 rs1val, rs2val, rs3val;
        rs1val.dword = reg->getFloatRegVal(rs1Int);
        rs2val.dword = reg->getFloatRegVal(rs2Int);
        rs3val.dword = reg->getFloatRegVal(rs3Int);
        double rdval = rs1val.db * rs2val.db - rs3val.db;
        reg->setFloatRegVal(*(ULL*)&rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << hex << content << endl;
        cerr << "fmsub funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

inline void FNMsub_funct2() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    int funct2 = SUBINT(content, 25, 2);
    if (funct2 == 0) {
        float_32 rs1Val, rs2Val, rs3Val;
        rs1Val.ui = reg->getFloatRegVal(rs1Int);
        rs2Val.ui = reg->getFloatRegVal(rs2Int);
        rs3Val.ui = reg->getFloatRegVal(rs3Int);
        float rdVal = -(rs1Val.fl * rs2Val.fl - rs3Val.fl);
        reg->setFloatRegVal(*(unsigned int*)&rdVal, rdInt);
    } else if (funct2 == 1) {
        double_64 rs1val, rs2val, rs3val;
        rs1val.dword = reg->getFloatRegVal(rs1Int);
        rs2val.dword = reg->getFloatRegVal(rs2Int);
        rs3val.dword = reg->getFloatRegVal(rs3Int);
        double rdval = -(rs1val.db * rs2val.db - rs3val.db);
        reg->setFloatRegVal(*(ULL*)&rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << hex << content << endl;
        cerr << "fnmsub funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

inline void FNMadd_funct2() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    int funct2 = SUBINT(content, 25, 2);
    if (funct2 == 0) {
        float_32 rs1Val, rs2Val, rs3Val;
        rs1Val.ui = reg->getFloatRegVal(rs1Int);
        rs2Val.ui = reg->getFloatRegVal(rs2Int);
        rs3Val.ui = reg->getFloatRegVal(rs3Int);
        float rdVal = -(rs1Val.fl * rs2Val.fl + rs3Val.fl);
        reg->setFloatRegVal(*(unsigned int*)&rdVal, rdInt);
    } else if (funct2 == 1) {
        double_64 rs1val, rs2val, rs3val;
        rs1val.dword = reg->getFloatRegVal(rs1Int);
        rs2val.dword = reg->getFloatRegVal(rs2Int);
        rs3val.dword = reg->getFloatRegVal(rs3Int);
        double rdval = -(rs1val.db * rs2val.db + rs3val.db);
        reg->setFloatRegVal(*(ULL*)&rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << hex << content << endl;
        cerr << "fnmadd funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

inline void F_TYPE_funct7() {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;
    int rmInt = (content >> 12) & 7;

    int funct7 = SUBINT(content, 25, 7);
    switch (funct7) {
        case 0B0000001:
            FADD_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B101:
            FSUB_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1001:
            FMUL_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1000:
            FMUL(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1100:
            FDIV(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1101:
            FDIV_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B101101:
            FSQRT_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B10001:
            FSGNJ_D_funct3(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B10101:
            FMIN_MAX_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B100000:
            FCVT_SD(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B100001:
            FCVT_DS(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1010001:
            FEQ_LT_LE(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1110000:
            FCLASS(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1100001:
            FCVT_WD_LD(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1101001:
            FCVT_DW_DL(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1110001:
            FMV_XD(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1111001:
            FMV_DX(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 0B1101000:
            FCVT_SW_SL(rs1Int, rs2Int, rdInt, rmInt);
            break;
//        case 0B1100000:
//            FCVT_SL_SW(rs1Int, rs2Int, rdInt, rmInt);
//            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "F-TYPE funct7 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
/*
End of decoding FandD-TPYE instructions
 */

// to get the type of the instruction
inline void getOpcode() {
    int opcode = SUBINT(content, 0, 7);
    /*
    R-TYPE:1; I-TYPE:2; S-TYPE:3; SB-TYPE:4; UJ-TYPE:5; U-TYPE:6; SYS-TYPE:7;
     */
    switch (opcode) {
        case 0B0110011: case 0B0111011:
            R_TYPE_opcode();
            break;
        case 0B0010011: case 0B0000011: case 0B1100111: case 0B0011011:
            I_TYPE_opcode();
            break;
        case 0B0100011:
            S_TYPE_funct3();
            break;
        case 0B1100011:
            SB_TYPE_funct3();
            break;
        case 0B0010111: case 0B0110111:
            U_TYPE_opcode();
            break;
        case 0B1101111:
            jal();
            break;
        case 0B1110011:
            SYS_INSTRUCTION();
            break;
        case 0B0000111:
            FLoad_funct3();
            break;
        case 0B0100111:
            FStore_funct3();
            break;
        case 0B1000011:
            FMadd_funct2();
            break;
        case 0B1000111:
            FMsub_funct2();
            break;
        case 0B1001011:
            FNMsub_funct2();
            break;
        case 0B1001111:
            FNMadd_funct2();
            break;
        case 0B1010011:
            F_TYPE_funct7();
            break;
        default:
            cerr << "Error when parsing instruction: " << hex << content << endl;
            cerr << "opcode error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
int decode(ULL startAddr, bool enable_debug) {
    reg = new RegisterFile;
    Initialize(startAddr);  // Initialize some variables and prepare for the
    // decode part
    bool flag = false;
    debuger mydb = debuger(memory, reg);
    while (!is_exit) {
        if (enable_debug){
            mydb.wait();
            ins_counter++;
        }

        // cout << "PC: " << reg->getPC() << endl;
        //cerr << "PC: hex " << std::hex << reg->getPC() << endl;
        content = memory[reg->getPC()];
        // std::cout << hex << content << endl;
        getOpcode();

        if (canjump)
            canjump = false;
        else
            reg->changePC(4);
    }
    return exit_code;
}
