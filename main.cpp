#include <iostream>
#include <string>
#include <fstream>
#include "elf.h"
#include "debug.h"

void help(){

}
int analyze_argument(int argc, char** argv){
    if(argc == 1) {
        help();
        exit(0);
    }

    //analyze the argument
    int first_args = 1;
    for(; first_args < argc; ++first_args){
        if(std::string(argv[first_args])[0] == '-'){
            if(std::string(argv[first_args]) != "-h") {
                std::cerr << "Invaild argument \"" << argv[first_args] << "\"." << std::endl;
                help();
                exit(-1);
            }
            else {
                help();
                exit(0);
            }
        }
        else
            break;
    }
    return first_args;
}
// Open the elf file
bool elf_open(char * file_name, char *& elf_buffer, size_t & file_length){
    std::ifstream elf_file(file_name, std::ios::in | std::ios::binary);
    LOG(file_name);
    if(elf_file.fail()){
        std::cerr << "Error: Can't not open file \"" << file_name << "\"." << std::endl;
        return false;
    }

    file_length = (size_t) elf_file.gcount();
    if(elf_file.fail()){
        std::cerr << "Error: Can't read the length of file \"" << file_name << "\"." << std::endl;
        return false;
    }
    elf_buffer = new char[file_length + 1];
    elf_file.read(elf_buffer, file_length);
    elf_buffer[file_length] = '\0';
    return true;
}
bool elf_check_ident(char * elf_buffer, int & elf_class){
    elf_ident ident(elf_buffer);
    LOGF("DEBUG - elf_ident:\n");
    LOGF("\tEI_MAG: %x\n", ident.EI_MAG);
    LOGF("\tEI_CLASS: %x\n", ident.EI_CLASS);
    LOGF("\tEI_DATA: %x\n", ident.EI_DATA);
    LOGF("\tEI_VERSION: %x\n", ident.EI_VERSION);
    LOGF("\tEI_OSABI: %x\n", ident.EI_OSABI);
    LOGF("\tEI_ABIVERSION: %x\n", ident.EI_ABIVERSION);
    LOGF("\n");

    if(ident.EI_MAG != EIMAG){
        std::cerr << "Error: File is not an exec file." << std::endl;
        return false;
    }

    if(ident.EI_VERSION != EIVERSION){
        std::cerr << "Error: File is not original version ELF file." << std::endl;
        return false;
    }

    if(ident.EI_DATA != EIDATA_LITTLE){
        std::cerr << "Error: File is not little endianness." << std::endl;
        return false;
    }

    elf_class = ident.EI_CLASS;
    return true;
}
bool elf_check_header(char * elf_buffer, int elf_class){
    elf64_header elf_header(elf_buffer);
    LOGF("DEBUG -- elf_header:");
    HEXS("\te_type:", &elf_header.e_type, 2);
    HEXS("\te_machine:", &elf_header.e_machine, 2);
    HEXS("\te_version:", &elf_header.e_version, 4);
    HEXS("\te_entry:", &elf_header.e_entry, 8);
    HEXS("\te_phoff:", &elf_header.e_phoff, 8);
    HEXS("\te_shoff:", &elf_header.e_shoff, 8);
    HEXS("\te_flag:", &elf_header.e_flag, 4);
    HEXS("\te_ehsize:", &elf_header.e_ehsize, 2);
    HEXS("\te_phentsize:", &elf_header.e_phentsize, 2);
    HEXS("\te_phnum:", &elf_header.e_phnum, 2);
    HEXS("\te_shentsize:", &elf_header.e_shentsize, 2);
    HEXS("\te_shunm:", &elf_header.e_shnum, 2);
    HEXS("\te_shstrndx:", &elf_header.e_shstrndx, 2);

    if(elf_header.e_type != ET_EXEC){
        std::cerr << "Error: File is not an exec file." << std::endl;
        return false;
    }
    if(elf_header.e_machine != EM_riscv){
        std::cerr << "Error: File is not a RISC-V exec file." << std::endl;
        return false;
    }
    if(elf_header.e_version != EV_CURRENT) {
        std::cerr << "Error: File is not current exec version." << std::endl;
        return false;
    }
}
inline void check(bool code){
    if(code == false) {
        exit(-1);
    }
}
int main(int argc, char ** argv){
    //open the elf file
    int first_args = analyze_argument(argc, argv);
    char* file_name = argv[first_args];
    char* elf_buffer;
    size_t file_length;
    int elf_class;

    check(elf_open(file_name, elf_buffer, file_length));
    check(elf_check_ident(elf_buffer, elf_class));
    check(elf_check_header(elf_buffer, elf_class));

    return 0;
}