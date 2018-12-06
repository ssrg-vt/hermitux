#ifndef SYSCALL_REWRITING_H
#define SYSCALL_REWRITING_H

#include <fstream>
#include <elf.h>

#include <stdio.h>
#include <typeinfo>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
#include "PatchCFG.h"
#include "PatchObject.h"
#include "PatchModifier.h"
#include "Immediate.h"

#define SC_INSTR_SIZE 2
#define JMP_INSTR_SIZE 5
#define EXTRA_OW_BYTES (JMP_INSTR_SIZE - SC_INSTR_SIZE)
#define REL_JMP_OPCODE 0xE9
#define HERMITCORE_ROOT "../../hermitux-kernel/"
#define HERMITCORE_BUILD_DIR HERMITCORE_ROOT "/build"
#define NEW_ASM_FILE HERMITCORE_ROOT "/arch/x86/kernel/syscall-veneer.s"
#define HERMIT_EXECUTABLE_PATH HERMITCORE_ROOT "/prefix/x86_64-hermit/extra/tests/hermitux"
#define SYSCALL_PROLOGUE_FUNC "fast_syscall_prologue"
#define SYSCALL_CSV_FILE "./supported_syscalls.csv"

using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace PatchAPI;
using namespace InstructionAPI;
using namespace boost::algorithm;

/* Utils */
void print_block(Block *block);
string get_syscall_asm_func(void);
string get_syscall_asm_func_test(void);
map<int, string>* get_syscall_func_map(void);

class ElfFile
{
  private:
	ifstream efile;
	vector<Elf64_Phdr> prog_headers;

	void set_program_headers(ifstream &efile, Elf64_Ehdr ehdr);

  public:
	ElfFile(char *filename);
	void close_file();
	Elf64_Off get_segment_offset();
	Elf64_Addr get_segment_va();
};

class Syscall
{
	Function *function;
	Instruction::Ptr instruction;
	Address address;
	Block *sc_block;
	Block *next_block;
	int num_bytes_to_overwrite;

	void set_next_block();
	string get_modified_instruction(Instruction::Ptr instr);
	string get_objdump_instruction(string objdump, Address addr);
	int32_t get_displacement();
	void get_value_in_register(Block *curr_block, string reg, Address start_from, vector<Block *> *visited, vector<int> *possible_sc_nums);

  public:
	Syscall(Function *func, Block *block, Instruction::Ptr &instr, uint64_t addr);
	Function *get_function();
	Block *get_sc_block();
	Block *get_next_block();
	Address get_address();
	Instruction::Ptr get_instruction();
	string get_dest_label();
	string get_assembly_to_write(string objdump, map<int, string>* syscall_func_map);
	void overwrite(fstream &binfile, uint64_t offset, uint64_t va);
	vector<int> get_possible_sc_nos(void);
};

/* Instruction Parser */
bool instruction_assigns_to_register(Instruction::Ptr instr, string reg);
string get_size_agnostic_reg_name(RegisterAST::Ptr reg);
bool operand_is_immediate(Operand operand);
bool instruction_is_syscall(Instruction::Ptr instr);
bool instruction_is_mov(Instruction::Ptr instr);
bool instruction_is_self_xor(Instruction::Ptr instr);
uint32_t get_immediate_value(Operand op);

#endif /* SYSCALL_REWRITING_H */
