#include <elf.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "syscall_rewriting.hpp"

using namespace std;

void ElfFile::set_program_headers(ifstream &efile, Elf64_Ehdr ehdr)
{
    Elf64_Phdr phdr;
    int start = ehdr.e_phoff;
    int i;

    efile.seekg(start);

    if (ehdr.e_phentsize != sizeof(phdr))
        cout << "Oops\n";

    for (i = 0; i < ehdr.e_phnum; i++)
    {
        efile.read((char *)&phdr, ehdr.e_phentsize);
        this->prog_headers.push_back(phdr);
    }
}

ElfFile::ElfFile(char *filename)
{
    char *buf;
    Elf64_Ehdr ehdr;
    vector<Elf64_Phdr> prog_headers;

    efile.open(filename, ios::binary);
    if (!efile.is_open())
    {
        printf("File could not be opened\n");
        exit(-1);
    }

    efile.read((char *)&ehdr, sizeof(ehdr));
    set_program_headers(efile, ehdr);

    //cout << ehdr.e_phnum << " segments\n";
}

void ElfFile::close_file()
{
    this->efile.close();
}

Elf64_Off ElfFile::get_segment_offset()
{
    for (auto i = prog_headers.begin(); i != prog_headers.end(); ++i)
    {
        Elf64_Phdr phdr = *i;
        if (phdr.p_type == PT_LOAD && phdr.p_flags & PF_X)
        {
            return phdr.p_offset;
        }
    }
}

Elf64_Addr ElfFile::get_segment_va()
{
    for (auto i = prog_headers.begin(); i != prog_headers.end(); ++i)
    {
        Elf64_Phdr phdr = *i;
        if (phdr.p_type == PT_LOAD && phdr.p_flags & PF_X)
        {
            return phdr.p_vaddr;
        }
    }
}

/*
class ElfFile
{
    ifstream efile;
    vector<Elf64_Phdr> prog_headers;

    void set_program_headers(ifstream &efile, Elf64_Ehdr ehdr)
    {
        Elf64_Phdr phdr;
        int start = ehdr.e_phoff;
        int i;

        efile.seekg(start);

        if (ehdr.e_phentsize != sizeof(phdr))
            cout << "Oops\n";

        for (i = 0; i < ehdr.e_phnum; i++)
        {
            efile.read((char *)&phdr, ehdr.e_phentsize);
            this->prog_headers.push_back(phdr);
        }
    }

  public:
    ElfFile(char *filename)
    {
        char *buf;
        Elf64_Ehdr ehdr;
        vector<Elf64_Phdr> prog_headers;

        efile.open(filename, ios::binary);
        if (!efile.is_open())
        {
            printf("File could not be opened\n");
            exit(-1);
        }

        efile.read((char *)&ehdr, sizeof(ehdr));
        set_program_headers(efile, ehdr);

        cout << ehdr.e_phnum << endl;
    }

    ~ElfFile()
    {
        this->efile.close();
    }

    Elf64_Off get_segment_offset(const char *filename)
    {
        for (auto i = prog_headers.begin(); i != prog_headers.end(); ++i)
        {
            Elf64_Phdr phdr = *i;
            if (phdr.p_type == PT_LOAD && phdr.p_flags & PF_X)
            {
                return phdr.p_offset;
            }
        }
    }

    Elf64_Addr get_segment_va()
    {
        for (auto i = prog_headers.begin(); i != prog_headers.end(); ++i)
        {
            Elf64_Phdr phdr = *i;
            if (phdr.p_type == PT_LOAD && phdr.p_flags & PF_X)
            {
                return phdr.p_vaddr;
            }
        }
    }
};
*/

// int main(int argc, char *argv[])
// {
//     get_segment_offset(argv[1]);
//     return 0;
// }
