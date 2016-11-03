//
// Created by leewy on 2016/11/1.
//

#ifndef RISCV_SIM_DEBUG_H
#define RISCV_SIM_DEBUG_H

#ifdef DEBUG
#ifndef CSTDIO
#define <cstdio>
#endif
#define LOG(x) fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, x)
#define LOGF(fmt, ...) fprintf(stderr, "%s:%d: "fmt"\n", __FILE__, __LINE__, __VA_ARGS__)
#define HEX(ptr, num) {fprintf(stderr, "%s:%d: ");for(int _i = 0; _i < num; ++_i){fprintf(stderr,"%x ", ((char*)ptr)[i];}fprintf(stderr,"\n")}
#define HEXS(s, ptr, num) {fprintf(stderr, "%s:%d: %s ", s);for(int _i = 0; _i < num; ++_i){fprintf(stderr,"%x ", ((char*)ptr)[i];}fprintf(stderr,"\n")}
#else
#define LOG(x)
#define LOGF(fmt, ...)
#define HEX(ptr, num)
#define HEXS(s, ptr, num)
#endif

#endif //RISCV_SIM_DEBUG_H
