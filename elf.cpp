//
// Created by leewy on 2016/10/27.
//

#include "elf.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
elf_ident::elf_ident(const char *elf_buffer) {
    memcpy(this, elf_buffer, sizeof(*this));

}

elf32_header::elf32_header(const char *elf_buffer) {
    memcpy(this, elf_buffer, sizeof(*this));
}

elf64_header::elf64_header(const char *elf_buffer) {
    memcpy(this, elf_buffer, sizeof(*this));
}

Elf64_Phdr::Elf64_Phdr(const char *elf_buffer) {
    memcpy(this, elf_buffer, sizeof(*this));
}
