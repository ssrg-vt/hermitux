#include "syscall_rewriting.hpp"

string get_syscall_asm_func()
{
    string assembly = "";
    assembly += SYSCALL_PROLOGUE_FUNC ":\n";

    /* Save all registers on the stack */
    // assembly += "\tpush \%r15\n";
    // assembly += "\tpush \%r14\n";
    // assembly += "\tpush \%r13\n";
    // assembly += "\tpush \%r12\n";
    // assembly += "\tpush \%r11\n";
    // assembly += "\tpush \%rcx\n";
    // assembly += "\tpush \%rbx\n";

    /* Now we push the reigsters that are used to pass arguments */
    assembly += "\tpush \%rax\n";
    assembly += "\tpush \%rdi\n";
    assembly += "\tpush \%rsi\n";
    assembly += "\tpush \%rdx\n";
    assembly += "\tpush \%r10\n";
    assembly += "\tpush \%r8\n";
    assembly += "\tpush \%r9\n";

    /*  Set the first argument to the handler to be a pointer to struct
        fast_fs_state which we just pushed onto the stack */
    assembly += "\tmov \%rsp,\%rdi\n";
    assembly += "\tcall fast_syscall_handler\n";
    
    /* Restore all the registers in the order they were pushed */
    assembly += "\tpop \%r9\n";
    assembly += "\tpop \%r8\n";
    assembly += "\tpop \%r10\n";
    assembly += "\tpop \%rdx\n";
    assembly += "\tpop \%rsi\n";
    assembly += "\tpop \%rdi\n";
    assembly += "\tpop \%rax\n";

    // assembly += "\tpop \%rbx\n";
    // assembly += "\tpop \%rcx\n";
    // assembly += "\tpop \%r11\n";
    // assembly += "\tpop \%r12\n";
    // assembly += "\tpop \%r13\n";
    // assembly += "\tpop \%r14\n";
    // assembly += "\tpop \%r15\n";
    
    assembly += "\tret\n\n";
    return assembly;
}

#define EXTRA_PUSHES 0
#define EXTRA_CALLS 0
string get_syscall_asm_func_test()
{
    string assembly = "";
    assembly += SYSCALL_PROLOGUE_FUNC ":\n";

    /* Save all registers on the stack */
    for (int i = 0; i < EXTRA_PUSHES; i++)
    {
	    assembly += "push \%r15\n";
	    assembly += "push \%r14\n";
	    assembly += "push \%r13\n";
	    assembly += "push \%r12\n";
	    assembly += "push \%r11\n";
	    assembly += "push \%rcx\n";
	    assembly += "push \%rbx\n";
    }

    for (int i = 0; i < EXTRA_CALLS; i++)
    {
	    assembly += "call dummy_asm_func\n";
    }
    
    /* Now we push the reigsters that are used to pass arguments */
    assembly += "push \%rax\n";
    assembly += "push \%rdi\n";
    assembly += "push \%rsi\n";
    assembly += "push \%rdx\n";
    assembly += "push \%r10\n";
    assembly += "push \%r8\n";
    assembly += "push \%r9\n";

    /*  Set the first argument to the handler to be a pointer to struct
        fast_fs_state which we just pushed onto the stack */
    assembly += "mov \%rsp,\%rdi\n";
    assembly += "call fast_syscall_handler\n";
    
    /* Restore all the registers in the order they were pushed */
    assembly += "pop \%r9\n";
    assembly += "pop \%r8\n";
    assembly += "pop \%r10\n";
    assembly += "pop \%rdx\n";
    assembly += "pop \%rsi\n";
    assembly += "pop \%rdi\n";
    assembly += "pop \%rax\n";

    for(int i = 0; i < EXTRA_PUSHES; i++)
    {
	    assembly += "pop \%rbx\n";
	    assembly += "pop \%rcx\n";
	    assembly += "pop \%r11\n";
	    assembly += "pop \%r12\n";
	    assembly += "pop \%r13\n";
	    assembly += "pop \%r14\n";
	    assembly += "pop \%r15\n";
    }
    assembly += "ret\n\n";
    return assembly;
}

map<int, string>* get_syscall_func_map(void)
{
    map<int, string>* sfm = new map<int, string>();
    ifstream syscall_file(SYSCALL_CSV_FILE, ifstream::in);
    if(!syscall_file)
    {
        printf("Supported system calls file could not be opened\n");
        exit(-1);
    }
    
    int sc_num;
    string sc_func;
    while (syscall_file >> sc_num >> sc_func)
    {
        sfm->insert(pair<int, string>(sc_num, sc_func));
    }

    return sfm;
}

void print_block(Block *block)
{
	Block::Insns instructions;
	block->getInsns(instructions);

	for (auto k = instructions.begin(); k != instructions.end(); ++k)
	{
		Instruction::Ptr instr = k->second;
		Address addr = k->first;

		cout << hex << addr << ": " << instr->format() << endl;
	}
	cout << dec;
}
