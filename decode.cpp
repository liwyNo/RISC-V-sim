#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include "debug.h"
#include "register.h"
#include "vmm.h"

using namespace std;

map<string, int> typeIndex;
RegisterFile *reg;
extern VM memory;
int content;  // the integer value of one instruction
bool canjump = false;
void memoryWrite(ULL offset, ULL value, unsigned char bit) {
    ULL ori = memory[offset];
    ULL mask = bit == 8 ? ~0ULL : ((1ULL << (bit * 8)) - 1);
    ori = (ori & ~mask) | (value & mask);
    memory[offset] = ori;
}

// Initialize the map index and register file
void Initialize(ULL startAddr) {
    reg->setPC(startAddr);

    content = 0;
    /*
    R-TYPE:1; I-TYPE:2; S-TYPE:3; SB-TYPE:4; UJ-TYPE:5; U-TYPE:6; SYS-TYPE:7;
     */

    typeIndex.insert(pair<string, int>("0110011", 1));
    typeIndex.insert(pair<string, int>("0010011", 2));
    typeIndex.insert(pair<string, int>("0100011", 3));
    typeIndex.insert(pair<string, int>("0000011", 2));
    typeIndex.insert(pair<string, int>("1100011", 4));
    typeIndex.insert(pair<string, int>("1100111", 2));
    typeIndex.insert(pair<string, int>("1101111", 5));
    typeIndex.insert(pair<string, int>("0010111", 6));
    typeIndex.insert(pair<string, int>("0110111", 6));
    typeIndex.insert(pair<string, int>("0111011", 1));
    typeIndex.insert(pair<string, int>("0011011", 2));
    typeIndex.insert(pair<string, int>("1110011", 7));
    typeIndex.insert(pair<string, int>("0000111", 8));
    typeIndex.insert(pair<string, int>("0100111", 9));
    typeIndex.insert(pair<string, int>("1000011", 10));
    typeIndex.insert(pair<string, int>("1000111", 11));
    typeIndex.insert(pair<string, int>("1001011", 12));
    typeIndex.insert(pair<string, int>("1001111", 13));
    typeIndex.insert(pair<string, int>("1010011", 14));
}

/*
This part finishes the decode part and lists aLL the R-TYPE instructions
 */
void sub(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val - (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void sra(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 63;  // lower 5 bits is used
    LL rdVal = (LL)rs1Val >> rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void add(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val + (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void sll(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 63;
    ULL rdVal = rs1Val << rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void slt(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal;
    if ((LL)rs1Val < (LL)rs2Val)
        rdVal = 1;
    else
        rdVal = 0;
    reg->setIntRegVal(rdVal, rdInt);
}
void sltu(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    if ((ULL)rs1Val < (ULL)rs2Val)
        rdVal = 1;
    else
        rdVal = 0;
    reg->setIntRegVal(rdVal, rdInt);
}
void _xor(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val ^ (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void srl(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 63;
    ULL rdVal = rs1Val >> rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void _or(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val | (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void _and(ULL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal = (LL)rs1Val & (LL)rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void addw(ULL rs1Val, ULL rs2Val, int rdInt) {
    /*
    Overfows are ignored, and the low
    32-bits of the result is sign-extended to 64-bits
    and written to the destination register.
     */
    int src_1 = rs1Val;
    int src_2 = rs2Val;
    int sum = src_1 + src_2;
    LL rdVal = sum;
    reg->setIntRegVal((ULL)sum, rdInt);
}
void sllw(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 31;
    int src = rs1Val;
    LL rdVal = src << rs2Val;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
void srlw(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 31;
    unsigned int src = rs1Val;
    ULL rdVal = src >> rs2Val;
    reg->setIntRegVal(rdVal, rdInt);
}
void subw(ULL rs1Val, ULL rs2Val, int rdInt) {
    int src_1 = rs1Val;
    int src_2 = rs2Val;
    int sum = src_1 - src_2;
    LL rdVal = sum;
    reg->setIntRegVal((ULL)sum, rdInt);
}
void sraw(ULL rs1Val, ULL rs2Val, int rdInt) {
    rs2Val = rs2Val & 31;
    int src = rs1Val;
    LL rdVal = src >> rs2Val;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}

void R_TYPE_funct3_1(string instruction) {
    // aLL R-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends hiere

    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 000:
            sub(rs1Val, rs2Val, rdInt);
            break;
        case 101:
            sra(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void R_TYPE_funct3_2(string instruction) {
    // aLL R-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 000:
            add(rs1Val, rs2Val, rdInt);
            break;
        case 1:
            sll(rs1Val, rs2Val, rdInt);
            break;
        case 10:
            slt(rs1Val, rs2Val, rdInt);
            break;
        case 11:
            sltu(rs1Val, rs2Val, rdInt);
            break;
        case 100:
            _xor(rs1Val, rs2Val, rdInt);
            break;
        case 101:
            srl(rs1Val, rs2Val, rdInt);
            break;
        case 110:
            _or(rs1Val, rs2Val, rdInt);
            break;
        case 111:
            _and(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void R_TYPE_funct3_3(string instruction) {
    // aLL R-TYPE instructions have the same part
    int rs1Val = (content >> 15) & 31;
    int rs2Val = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    rs1Val = reg->getIntRegVal(rs1Val);
    // common part ends here

    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 000:
            subw(rs1Val, rs2Val, rdInt);
            break;
        case 101:
            sraw(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void R_TYPE_funct3_4(string instruction) {
    // aLL R-TYPE instructions have the same part
    int rs1Val = (content >> 15) & 31;
    int rs2Val = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    rs1Val = reg->getIntRegVal(rs1Val);
    // common part ends here

    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 0:
            addw(rs1Val, rs2Val, rdInt);
            break;
        case 101:
            srlw(rs1Val, rs2Val, rdInt);
            break;
        case 1:
            sllw(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "R-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void R_TYPE_funct7_1(string instruction) {
    string funct7 = instruction.substr(0, 7);
    switch (stoi(funct7)) {
        case 100000:
            R_TYPE_funct3_1(instruction);
            break;
        case 0000000:
            R_TYPE_funct3_2(instruction);
            break;
        case 1:
            void M_TYPE_funct3_1(string instruction);
            M_TYPE_funct3_1(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "R-TYPE funct7 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void R_TYPE_funct7_2(string instruction) {
    string funct7 = instruction.substr(0, 7);
    switch (stoi(funct7)) {
        case 100000:
            R_TYPE_funct3_3(instruction);
            break;
        case 0000000:
            R_TYPE_funct3_4(instruction);
            break;
        case 1:
            void M_TYPE_funct3_2(string instruction);
            M_TYPE_funct3_2(instruction);
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "R-TYPE funct7 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void R_TYPE_opcode(string instruction) {
    string opcode = instruction.substr(25, 7);
    switch (stoi(opcode)) {
        case 110011:
            R_TYPE_funct7_1(instruction);
            break;
        case 111011:
            R_TYPE_funct7_2(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void slli(string instruction) {
    LL shamt = (content >> 20) & 63;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val << shamt;
    reg->setIntRegVal(rdVal, rdInt);
}
void srli(string instruction) {
    LL shamt = (content >> 20) & 63;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rdVal = rs1Val >> shamt;
    reg->setIntRegVal(rdVal, rdInt);
}
void srai(string instruction) {
    LL shamt = (content >> 20) & 63;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val >> shamt;
    reg->setIntRegVal(rdVal, rdInt);
}
void addi(string instruction) {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val + immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
void slti(string instruction) {
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
void sltiu(string instruction) {
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
void xori(string instruction) {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val ^ immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
void ori(string instruction) {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val | immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
void andi(string instruction) {
    LL immediateNum = (LL)(content >> 20);
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL rdVal = rs1Val & immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
void lb(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];
    loadData = (loadData << 56) >> 56;  // return value can be sign-extended
    reg->setIntRegVal((ULL)loadData, rdInt);
}
void lh(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];
    loadData = (loadData << 48) >> 48;  // same as the reason mentioned above
    reg->setIntRegVal((ULL)loadData, rdInt);
}
void lw(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);

    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];
    loadData = (loadData << 32) >> 32;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
void ld(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL loadData = (LL)memory[(ULL)(immediateNum + rs1Val)];

    reg->setIntRegVal((ULL)loadData, rdInt);
}
void lbu(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL loadData = memory[(ULL)(immediateNum + rs1Val)];
    loadData = loadData & 255;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
void lhu(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL loadData = memory[(ULL)(immediateNum + rs1Val)];
    loadData = loadData & 65535;
    reg->setIntRegVal((ULL)loadData, rdInt);
}
void lwu(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    ULL loadData = memory[(ULL)(immediateNum + rs1Val)];
    long long mask = (1LL << 63) >> 31;
    loadData = loadData & (~mask);
    reg->setIntRegVal((ULL)loadData, rdInt);
}
void jalr(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    reg->setIntRegVal((ULL)(reg->getPC() + 4), rdInt);
    canjump = true;
    reg->setPC((ULL)(((rs1Val + immediateNum) >> 1) << 1));
}
void addiw(string instruction) {
    int immediateNum = content >> 20;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    int rs1Val = (int)reg->getIntRegVal(rs1Int);
    rs1Val += immediateNum;
    LL rdVal = (LL)rs1Val;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
void slliw(string instruction) {
    int shamt = (content >> 20) & 31;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    int rs1Val = (int)reg->getIntRegVal(rs1Int);
    LL rdVal = (LL)(rs1Val << shamt);
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
void srliw(string instruction) {
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
void sraiw(string instruction) {
    int shamt = (content >> 20) & 31;
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;

    int rs1Val = (int)reg->getIntRegVal(rs1Int);
    rs1Val >>= shamt;
    LL rdVal = (LL)rs1Val;
    reg->setIntRegVal((ULL)rdVal, rdInt);
}
void I_TYPE_funct3_1(string instruction) {
    string funct3 = instruction.substr(17, 3);

    switch (stoi(funct3)) {
        case 1:
            slli(instruction);
            break;
        case 101: {
            string funct7 = instruction.substr(0, 7);
            switch (stoi(funct7)) {
                case 0:
                    srli(instruction);
                    break;
                case 100000:
                    srai(instruction);
                    break;
            }
        } break;
        case 0:
            addi(instruction);
            break;
        case 10:
            slti(instruction);
            break;
        case 11:
            sltiu(instruction);
            break;
        case 100:
            xori(instruction);
            break;
        case 110:
            ori(instruction);
            break;
        case 111:
            andi(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "I-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void I_TYPE_funct3_2(string instruction) {
    string funct3 = instruction.substr(17, 3);

    switch (stoi(funct3)) {
        case 000:
            lb(instruction);
            break;
        case 1:
            lh(instruction);
            break;
        case 10:
            lw(instruction);
            break;
        case 100:
            lbu(instruction);
            break;
        case 101:
            lhu(instruction);
            break;
        case 110:
            lwu(instruction);
            break;
        case 11:
            ld(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "I-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void I_TYPE_funct3_3(string instruction) {
    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 0:
            addiw(instruction);
            break;
        case 1:
            slliw(instruction);
            break;
        case 101: {
            string funct7 = instruction.substr(0, 7);
            switch (stoi(funct7)) {
                case 0:
                    srliw(instruction);
                    break;
                case 100000:
                    sraiw(instruction);
                    break;
            }
        } break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "I-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void I_TYPE_opcode(string instruction) {
    string opcode = instruction.substr(25, 7);
    switch (stoi(opcode)) {
        case 10011:
            I_TYPE_funct3_1(instruction);
            break;
        case 11:
            I_TYPE_funct3_2(instruction);
            break;
        case 11011:
            I_TYPE_funct3_3(instruction);
            break;
        case 1100111:
            jalr(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void sb(string instruction) {
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
void sh(string instruction) {
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
void sw(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >>
                      52;  // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL memoryAddr = immediateNum + rs1Val;
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    long long mask = (1LL << 63) >> 31;
    rs2Val &= (~mask);

    memoryWrite(memoryAddr, rs2Val, 4);
}
void sd(string instruction) {
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
void S_TYPE_funct3(string instruction) {
    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 0:
            sb(instruction);
            break;
        case 1:
            sh(instruction);
            break;
        case 10:
            sw(instruction);
            break;
        case 11:
            sd(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void beq(string instruction) {
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
void bne(string instruction) {
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
void blt(string instruction) {
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
void bge(string instruction) {
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
void bltu(string instruction) {
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
void bgeu(string instruction) {
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
void SB_TYPE_funct3(string instruction) {
    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 0:
            beq(instruction);
            break;
        case 1:
            bne(instruction);
            break;
        case 100:
            blt(instruction);
            break;
        case 101:
            bge(instruction);
            break;
        case 110:
            bltu(instruction);
            break;
        case 111:
            bgeu(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void lui(string instruction) {
    int rdInt = (content >> 7) & 31;
    LL immediateNum =
        (LL)((content >> 12)
             << 12);  // The 32-bit result is sign-extended to 64 bits.

    reg->setIntRegVal((ULL)immediateNum, rdInt);
}
void auipc(string instruction) {
    int rdInt = (content >> 7) & 31;
    LL immediateNum =
        (LL)((content >> 12)
             << 12);  // The 32-bit result is sign-extended to 64 bits.

    ULL rdVal = reg->getPC() + (ULL)immediateNum;
    reg->setIntRegVal(rdVal, rdInt);
}
void U_TYPE_opcode(string instruction) {
    string opcode = instruction.substr(25, 7);
    switch (stoi(opcode)) {
        case 110111:
            lui(instruction);
            break;
        case 10111:
            auipc(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void jal(string instruction) {
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
void ecall() {
// make it can compile first.
#define systemCall
    LL sys_call_num = reg->getIntRegVal(7);
    systemCall((int)sys_call_num);
}
void ebreak() {}
void E_INS(string instruction) {
    string diffPart = instruction.substr(11, 1);
    switch (stoi(diffPart)) {
        case 0:
            ecall();
            break;
        case 1:
            ebreak();
            break;
    }
}
void SYS_INSTRUCTION(string instruction) {
    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 0:
            E_INS(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void MUL(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rax\n\t"
        "imulq %2\n\t"
        "movq %%rax, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "r"(rs1Val), "r"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void MULH(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    __asm__ __volatile__(
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
void MULHSU(LL rs1Val, ULL rs2Val, int rdInt) {
    LL rdVal;
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rax\n\t"
        "imulq %2\n\t"
        "movq %%rax, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "r"(rs1Val), "r"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void MULHU(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    __asm__ __volatile__(
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
void DIV(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal(-1, rdInt);
        return;
    }
    if (((rs1Val ^ 0x8000000000000000LL) == 0) && (rs2Val == -1)) {
        reg->setIntRegVal(rs1Val, rdInt);
        return;
    }
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rdx\n\t"
        "sarq $63, %%rdx\n\t"
        "idivq %2\n\t"
        "movq %%rax, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "r"(rs1Val), "m"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void DIVU(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal(0x7fffffffffffffff, rdInt);
        return;
    }
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rdx\n\t"
        "shrq $63, %%rdx\n\t"
        "divq %2\n\t"
        "movq %%rax, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "r"(rs1Val), "m"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void REM(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal(rs1Val, rdInt);
        return;
    }
    if (((rs1Val ^ 0x8000000000000000LL) == 0) && (rs2Val == -1)) {
        reg->setIntRegVal(0, rdInt);
        return;
    }
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rdx\n\t"
        "sarq $63, %%rdx\n\t"
        "idivq %2\n\t"
        "movq %%rdx, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "r"(rs1Val), "m"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void REMU(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal(rs1Val, rdInt);
        return;
    }
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rdx\n\t"
        "shrq $63, %%rdx\n\t"
        "divq %2\n\t"
        "movq %%rdx, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "r"(rs1Val), "m"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void MULW(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    __asm__ __volatile__(
        "pushq %%rax\n\t"
        "pushq %%rbx\n\t"
        "pushq %%rdx\n\t"
        "movq %1, %%rax\n\t"
        "movq %2, %%rbx\n\t"
        "imull %%ebx, %%eax\n\t"
        "movq %%rax, %0\n\t"
        "popq %%rdx\n\t"
        "popq %%rbx\n\t"
        "popq %%rax\n\t"
        : "=m"(rdVal)
        : "m"(rs1Val), "m"(rs2Val));
    reg->setIntRegVal(rdVal, rdInt);
}
void DIVW(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal(-1, rdInt);
        return;
    }
    int rs1 = rs1Val;
    int rs2 = rs2Val;
    rdVal = (LL)(rs1 / rs2);
    reg->setIntRegVal(rdVal, rdInt);
}
void DIVUW(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal(0x7fffffffffffffff, rdInt);
        return;
    }
    unsigned int rs1 = rs1Val;
    unsigned int rs2 = rs2Val;
    rdVal = (ULL)(rs1 / rs2);
    reg->setIntRegVal(rdVal, rdInt);
}
void REMW(LL rs1Val, LL rs2Val, int rdInt) {
    LL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal((int)rs1Val, rdInt);
        return;
    }
    int rs1 = rs1Val;
    int rs2 = rs2Val;
    rdVal = (LL)(rs1 % rs2);
    reg->setIntRegVal(rdVal, rdInt);
}
void REMUW(ULL rs1Val, ULL rs2Val, int rdInt) {
    ULL rdVal;
    if (rs2Val == 0) {
        reg->setIntRegVal((unsigned int)rs1Val, rdInt);
        return;
    }
    unsigned int rs1 = rs1Val;
    unsigned int rs2 = rs2Val;
    rdVal = (ULL)(rs1 / rs2);
    reg->setIntRegVal(rdVal, rdInt);
}

void M_TYPE_funct3_1(string instruction) {
    // aLL M-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    string funct3 = instruction.substr(17, 3);
    switch (stoi(funct3)) {
        case 000:
            MUL((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 1:
            MULH((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 10:
            MULHSU((LL)rs1Val, rs2Val, rdInt);
            break;
        case 11:
            MULHU(rs1Val, rs2Val, rdInt);
            break;
        case 100:
            DIV((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 101:
            DIVU(rs1Val, rs2Val, rdInt);
            break;
        case 110:
            REM((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 111:
            REMU(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "M-TYPE funct3 error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}

void M_TYPE_funct3_2(string instruction) {
    // aLL M-TYPE instructions have the same part
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rdInt = (content >> 7) & 31;

    ULL rs1Val = reg->getIntRegVal(rs1Int);
    ULL rs2Val = reg->getIntRegVal(rs2Int);
    // common part ends here

    string funct3 = instruction.substr(17, 3);
    int tempInt = stoi(funct3);
    switch (tempInt) {
        case 000:
            MULW((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 100:
            DIVW((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 101:
            DIVUW(rs1Val, rs2Val, rdInt);
            break;
        case 110:
            REMW((LL)rs1Val, (LL)rs2Val, rdInt);
            break;
        case 111:
            REMUW(rs1Val, rs2Val, rdInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void FSQRT_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {}
void FSGNJ_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {}
void FMIN_MAX_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {}
void FCLASS(int rs1Int, int rs2Int, int rdInt, int rmInt) {}
//---------------------lalala-------------------------

void FADD_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    double rs1Val = reg->getFloatRegVal(rs1Int);
    double rs2Val = reg->getFloatRegVal(rs2Int);
    reg->setFloatRegVal(rs1Val + rs2Val, rdInt);
}
void FSUB_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    double rs1Val = reg->getFloatRegVal(rs1Int);
    double rs2Val = reg->getFloatRegVal(rs2Int);
    reg->setFloatRegVal(rs1Val + rs2Val, rdInt);
}
void FMUL_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    double rs1Val = reg->getFloatRegVal(rs1Int);
    double rs2Val = reg->getFloatRegVal(rs2Int);
    reg->setFloatRegVal(rs1Val * rs2Val, rdInt);
}
void FMUL(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    float rs1Val = reg->getFloatRegVal(rs1Int);
    float rs2Val = reg->getFloatRegVal(rs2Int);
    reg->setFloatRegVal(rs1Val * rs2Val, rdInt);
}
void FDIV(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    float rs1Val = reg->getFloatRegVal(rs1Int);
    float rs2Val = reg->getFloatRegVal(rs2Int);
    reg->setFloatRegVal(rs1Val / rs2Val, rdInt);
}
void FDIV_D(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    double rs1Val = reg->getFloatRegVal(rs1Int);
    double rs2Val = reg->getFloatRegVal(rs2Int);
    reg->setFloatRegVal(rs1Val / rs2Val, rdInt);
}
void FCVT_SD(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    float rs1Val = reg->getFloatRegVal(rs1Int);
    reg->setFloatRegVal(rs1Val, rdInt);
}
void FCVT_DS(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    float rs1Val = reg->getFloatRegVal(rs1Int);
    reg->setFloatRegVal(rs1Val, rdInt);
}
void FEQ_LT_LE(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    double rs1Val = reg->getFloatRegVal(rs1Int);
    double rs2Val = reg->getFloatRegVal(rs2Int);
    if (rmInt == 0) {
        if (rs1Val <= rs2Val)
            reg->setIntRegVal(1, rdInt);
        else
            reg->setIntRegVal(0, rdInt);
    } else if (rmInt == 1) {
        if (rs1Val < rs2Val)
            reg->setIntRegVal(1, rdInt);
        else
            reg->setIntRegVal(0, rdInt);
    } else if (rmInt == 2) {
        if (rs1Val == rs2Val)
            reg->setIntRegVal(1, rdInt);
        else
            reg->setIntRegVal(0, rdInt);
    } else {
        cerr << "fcompare ins was wrong!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}
void FCVT_WD_LD(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    double rs1Val = reg->getFloatRegVal(rs1Int);
    if (rs2Int == 0) {
        reg->setIntRegVal((int)rs1Val, rdInt);
    } else if (rs2Int == 1) {
        reg->setIntRegVal((unsigned int)rs1Val, rdInt);
    } else if (rs2Int == 2) {
        reg->setIntRegVal((long long)rs1Val, rdInt);
    } else if (rs2Int == 3) {
        reg->setIntRegVal((unsigned long long)rs1Val, rdInt);
    } else {
        cerr << "fcvt.*.d ins was wrong!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}
void FCVT_DW_DL(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    if (rs2Int == 0) {
        int rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal(rs1Val, rdInt);
    } else if (rs2Int == 1) {
        unsigned int rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal(rs1Val, rdInt);
    } else if (rs2Int == 2) {
        long long rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal(rs1Val, rdInt);
    } else if (rs2Int == 3) {
        unsigned long long rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal(rs1Val, rdInt);
    } else {
        cerr << "fcvt.d.* ins was wrong!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}
void FMV_XD(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    if (rmInt != 0) {
        FCLASS(rs1Int, rs2Int, rdInt, rmInt);
        return;
    }
    double rs1Val = reg->getFloatRegVal(rs1Int);
    reg->setIntRegVal(*((unsigned long long *)(&rs1Val)), rdInt);
}
void FMV_DX(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    unsigned long long rs1Val = reg->getIntRegVal(rs1Int);
    reg->setFloatRegVal(*((double *)(&rs1Val)), rdInt);
}
void FCVT_SW_SL(int rs1Int, int rs2Int, int rdInt, int rmInt) {
    if (rs2Int == 0) {
        int rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal((float)rs1Val, rdInt);
    } else if (rs2Int == 1) {
        unsigned int rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal((float)rs1Val, rdInt);
    } else if (rs2Int == 2) {
        long long rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal((float)rs1Val, rdInt);
    } else if (rs2Int == 3) {
        unsigned long long rs1Val = reg->getIntRegVal(rs1Int);
        reg->setFloatRegVal((float)rs1Val, rdInt);
    } else {
        cerr << "fcvt.s.* ins was wrong!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

/*
the control logic
*/
void FLoad_funct3(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rdInt = (content >> 7) & 31;
    LL immediateNum = (LL)content >> 20;

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);

    string funct3 = instruction.substr(17, 3);
    int tempInt = stoi(funct3);
    if (tempInt == 10) {
        float *loadData;
        //*((unsigned int *)loadData) = mymem.rwmemReadWord(immediateNum +
        // rs1Val);
        *((unsigned int *)loadData) = memory[immediateNum + rs1Val];
        reg->setFloatRegVal(*loadData, rdInt);
    } else if (tempInt == 11) {
        double *LoadData;
        //*((ULL *)LoadData) = mymem.rwmemReadDword(immediateNum + rs1Val);
        *((ULL *)LoadData) = memory[immediateNum + rs1Val];
        reg->setFloatRegVal(*LoadData, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << instruction << endl;
        cerr << "float load funct3 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

void FStore_funct3(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    LL immediateLowerPart = (LL)(content >> 7) & 31;
    LL immediateHigherPart = (LL)((content >> 25) & 127) << 5;
    LL immediateNum = ((immediateHigherPart + immediateLowerPart) << 52) >> 52;
    // get the immediate number and get sign-extended

    LL rs1Val = (LL)reg->getIntRegVal(rs1Int);
    LL memoryAddr = immediateNum + rs1Val;

    string funct3 = instruction.substr(17, 3);
    int tempInt = stoi(funct3);
    if (tempInt == 10) {
        float rs2Val = reg->getFloatRegVal(rs2Int);
        // mymem.rwmemWriteWord(*((unsigned int *)(&rs2Val)), memoryAddr);
        memoryWrite(memoryAddr, rs2Val, 4);
    } else if (tempInt == 11) {
        double rs2val = reg->getFloatRegVal(rs2Int);
        // mymem.rwmemWriteDword(*((ULL *)(&rs2val)), memoryAddr);
        memoryWrite(memoryAddr, rs2val, 8);
    } else {
        cerr << "Error when parsing instruction: " << instruction << endl;
        cerr << "float load funct3 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

void FMadd_funct2(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    string funct2 = instruction.substr(5, 2);
    int tempInt = stoi(funct2);
    if (tempInt == 0) {
        float rs1Val = reg->getFloatRegVal(rs1Int);
        float rs2Val = reg->getFloatRegVal(rs2Int);
        float rs3Val = reg->getFloatRegVal(rs3Int);
        float rdVal = rs1Val * rs2Val + rs3Val;
        reg->setFloatRegVal(rdVal, rdInt);
    } else if (tempInt == 1) {
        double rs1val = reg->getFloatRegVal(rs1Int);
        double rs2val = reg->getFloatRegVal(rs2Int);
        double rs3val = reg->getFloatRegVal(rs3Int);
        double rdval = rs1val * rs2val + rs3val;
        reg->setFloatRegVal(rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << instruction << endl;
        cerr << "fmadd funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

void FMsub_funct2(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    string funct2 = instruction.substr(5, 2);
    int tempInt = stoi(funct2);
    if (tempInt == 0) {
        float rs1Val = reg->getFloatRegVal(rs1Int);
        float rs2Val = reg->getFloatRegVal(rs2Int);
        float rs3Val = reg->getFloatRegVal(rs3Int);
        float rdVal = rs1Val * rs2Val - rs3Val;
        reg->setFloatRegVal(rdVal, rdInt);
    } else if (tempInt == 1) {
        double rs1val = reg->getFloatRegVal(rs1Int);
        double rs2val = reg->getFloatRegVal(rs2Int);
        double rs3val = reg->getFloatRegVal(rs3Int);
        double rdval = rs1val * rs2val - rs3val;
        reg->setFloatRegVal(rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << instruction << endl;
        cerr << "fmsub funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

void FNMsub_funct2(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    string funct2 = instruction.substr(5, 2);
    int tempInt = stoi(funct2);
    if (tempInt == 0) {
        float rs1Val = reg->getFloatRegVal(rs1Int);
        float rs2Val = reg->getFloatRegVal(rs2Int);
        float rs3Val = reg->getFloatRegVal(rs3Int);
        float rdVal = -(rs1Val * rs2Val - rs3Val);
        reg->setFloatRegVal(rdVal, rdInt);
    } else if (tempInt == 1) {
        double rs1val = reg->getFloatRegVal(rs1Int);
        double rs2val = reg->getFloatRegVal(rs2Int);
        double rs3val = reg->getFloatRegVal(rs3Int);
        double rdval = -(rs1val * rs2val - rs3val);
        reg->setFloatRegVal(rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << instruction << endl;
        cerr << "fnmsub funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

void FNMadd_funct2(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;

    string funct2 = instruction.substr(5, 2);
    int tempInt = stoi(funct2);
    if (tempInt == 0) {
        float rs1Val = reg->getFloatRegVal(rs1Int);
        float rs2Val = reg->getFloatRegVal(rs2Int);
        float rs3Val = reg->getFloatRegVal(rs3Int);
        float rdVal = -(rs1Val * rs2Val + rs3Val);
        reg->setFloatRegVal(rdVal, rdInt);
    } else if (tempInt == 1) {
        double rs1val = reg->getFloatRegVal(rs1Int);
        double rs2val = reg->getFloatRegVal(rs2Int);
        double rs3val = reg->getFloatRegVal(rs3Int);
        double rdval = -(rs1val * rs2val + rs3val);
        reg->setFloatRegVal(rdval, rdInt);
    } else {
        cerr << "Error when parsing instruction: " << instruction << endl;
        cerr << "fnmadd funct2 error!" << endl;
        cerr << "Exit!" << endl;
        assert(false);
    }
}

void F_TYPE_funct7(string instruction) {
    int rs1Int = (content >> 15) & 31;
    int rs2Int = (content >> 20) & 31;
    int rs3Int = (content >> 27) & 31;
    int rdInt = (content >> 7) & 31;
    int rmInt = (content >> 12) & 7;

    string funct7 = instruction.substr(0, 7);
    switch (stoi(funct7)) {
        case 0000001:
            FADD_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 101:
            FSUB_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1001:
            FMUL_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1000:
            FMUL(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1100:
            FDIV(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1101:
            FDIV_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 101101:
            FSQRT_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 10001:
            FSGNJ_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 10101:
            FMIN_MAX_D(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 100000:
            FCVT_SD(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 100001:
            FCVT_DS(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1010001:
            FEQ_LT_LE(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1110000:
            FCLASS(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1100001:
            FCVT_WD_LD(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1101001:
            FCVT_DW_DL(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1110001:
            FMV_XD(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1111001:
            FMV_DX(rs1Int, rs2Int, rdInt, rmInt);
            break;
        case 1101000:
            FCVT_SW_SL(rs1Int, rs2Int, rdInt, rmInt);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
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
void getOpcode(string instruction) {
    string opcode = instruction.substr(25, 7);
    /*
    R-TYPE:1; I-TYPE:2; S-TYPE:3; SB-TYPE:4; UJ-TYPE:5; U-TYPE:6; SYS-TYPE:7;
     */

    switch (typeIndex[opcode]) {
        case 1:
            R_TYPE_opcode(instruction);
            break;
        case 2:
            I_TYPE_opcode(instruction);
            break;
        case 3:
            S_TYPE_funct3(instruction);
            break;
        case 4:
            SB_TYPE_funct3(instruction);
            break;
        case 6:
            U_TYPE_opcode(instruction);
            break;
        case 5:
            jal(instruction);
            break;
        case 7:
            SYS_INSTRUCTION(instruction);
            break;
        case 8:
            FLoad_funct3(instruction);
            break;
        case 9:
            FStore_funct3(instruction);
            break;
        case 10:
            FMadd_funct2(instruction);
            break;
        case 11:
            FMsub_funct2(instruction);
            break;
        case 12:
            FNMsub_funct2(instruction);
            break;
        case 13:
            FNMadd_funct2(instruction);
            break;
        case 14:
            F_TYPE_funct7(instruction);
            break;
        default:
            cerr << "Error when parsing instruction: " << instruction << endl;
            cerr << "opcode error!" << endl;
            cerr << "Exit!" << endl;
            assert(false);
            return;
    }
}
void decode(ULL startAddr, bool enable_debug) {
    reg = new RegisterFile;
    Initialize(startAddr);  // Initialize some variables and prepare for the
                            // decode part

    char tempChar[33];
    bool flag = false;
    debuger mydb = debuger(memory, reg);
    while (true) {
        if (enable_debug) mydb.wait();
        memset(tempChar, 0, sizeof(tempChar));

        // cout << "PC: " << reg->getPC() << endl;
        // cout << "PC: hex " << std::hex << reg->getPC() << endl;
        content = memory[reg->getPC()];
        printf("%02x\n", content);
        for (int i = 0; i < 32; ++i)
            tempChar[31 - i] = (content & (1 << i)) == 0 ? '0' : '1';
        tempChar[32] = 0;
        string instruction(tempChar);

        // std::cout << instruction << endl;
        getOpcode(instruction);

        if (canjump)
            canjump = false;
        else
            reg->changePC(4);
    }
}