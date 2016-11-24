/*
Here gives the definition of all the registers,
and we choose to use long long to represent the 64 bits
and use int to represent the Program Count.
*/

#ifndef REGISTER_H
#define REGISTER_H

typedef long long LL;
typedef unsigned long long ULL;

struct RegisterFile {
    ULL pc;
    ULL x[32];
    ULL f[32];

   public:
    RegisterFile() {
        memset(x, 0, sizeof(x));
        memset(f, 0, sizeof(f));
        pc = 0;
        x[0] = 0;
        x[2] = 0x00000000f0000000ULL;
    }

    void setPC(ULL value) { pc = value; }
    void changePC(ULL offset) { pc += offset; }

    ULL getPC() { return pc; }

    void setIntRegVal(ULL value, int num) {
        if (num) x[num] = value;
    }
    ULL getIntRegVal(int num) { return x[num]; }

    void setFloatRegVal(double value, int num) { f[num] = value; }
    ULL getFloatRegVal(int num) { return f[num]; }
};
#endif