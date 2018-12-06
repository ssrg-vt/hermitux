#include "syscall_rewriting.hpp"

Syscall::Syscall(Function *func, Block *block, Instruction::Ptr &instr, uint64_t addr)
{
    this->function = func;
    this->instruction = instr;
    this->address = addr;
    this->sc_block = block;
    this->set_next_block();
    this->num_bytes_to_overwrite = 2;
}

Function *Syscall::get_function()
{
    return this->function;
}

Block *Syscall::get_sc_block()
{
    return this->sc_block;
}

Block *Syscall::get_next_block()
{
    return this->next_block;
}

Address Syscall::get_address()
{
    return this->address;
}

Instruction::Ptr Syscall::get_instruction()
{
    return this->instruction;
}

void Syscall::set_next_block()
{
    CodeObject *co = this->sc_block->obj();
    CodeRegion *cr = this->sc_block->region();
    this->next_block = co->findBlockByEntry(cr, this->address + 2);
}

vector<int> Syscall::get_possible_sc_nos(void)
{
    vector<Block *> *visited = new vector<Block *>();
    vector<int> *possible_sc_nums = new vector<int>();
    get_value_in_register(this->sc_block, "ax", this->address, visited, possible_sc_nums);
    visited->~vector();
    return *possible_sc_nums;
}

void Syscall::get_value_in_register(Block *curr_block, string reg, Address start_from, vector<Block *> *visited, vector<int> *possible_sc_nums)
{
    visited->push_back(curr_block);

    Block::Insns insns;
    curr_block->getInsns(insns);

    /* Start searching for assignments in instructions in reverse order from the syscall */
    for (auto i = insns.rbegin(); i != insns.rend(); ++i)
    {
        Instruction::Ptr instr = i->second;
        Address addr = i->first;

        if (addr > start_from)
        {
            continue;
        }

        if (instruction_assigns_to_register(instr, reg))
        {
            if (instruction_is_mov(instr))
            {
                if (instr->readsMemory())
                {
                    // printf("Memory read!\n");
                    // printf("%lx: %s\n", addr, instr->format().c_str());
                    possible_sc_nums->push_back(-1);
                    return;
                }
                Operand sourceop = instr->getOperand(1);
                if (operand_is_immediate(sourceop))
                {
                    possible_sc_nums->push_back(get_immediate_value(sourceop));
                    return;
                }
                else
                {
                    set<RegisterAST::Ptr> readset;
                    instr->getReadSet(readset);
                    if (readset.size() != 1)
                    {
                        printf("%lx: %s\n", addr, instr->format().c_str());
                        cout << "Read set != 1\n";
                        possible_sc_nums->push_back(-2);
                        return;
                    }
                    string rname = get_size_agnostic_reg_name(*(readset.begin()));
                    get_value_in_register(curr_block, rname, addr, visited, possible_sc_nums);
                    return;
                }
            }

            else if (instruction_is_self_xor(instr))
            {
                possible_sc_nums->push_back(0);
                return;
            }

            else
            {
                printf("Unknown operation affecting value\n");
                printf("%lx: %s\n", addr, instr->format().c_str());
                possible_sc_nums->push_back(-3);
                return;
            }
        }
    }

    /*  No assignment of interest has been made in the current block,
        so we traverse through all preceding blocks in a similar manner  */
    const Block::edgelist &incoming = curr_block->sources();
    for (auto j = incoming.begin(); j != incoming.end(); ++j)
    {
        Block *preceding_block = (*j)->src();
        vector<Function *> pbfuncs;
        preceding_block->getFuncs(pbfuncs);
        bool already_visited = any_of(visited->begin(), visited->end(), [preceding_block](Block *b) { return preceding_block == b; });
        bool same_func = any_of(pbfuncs.begin(), pbfuncs.end(), [this](Function *f) { return this->function == f; });
        bool empty_list = possible_sc_nums->empty();
        if (!already_visited && (same_func || empty_list))
        {
            get_value_in_register(preceding_block, reg, preceding_block->last(), visited, possible_sc_nums);
        }
    }
}

string Syscall::get_dest_label()
{
    /* Addres will be unique for each syscall */
    char hexaddr[16];
    sprintf(hexaddr, "%lx", this->address);
    return "syscall_" + string(hexaddr) + "_destination";
}

/*  Dyninst cannot recognise negative values, which causes assembler errors.
    This function modifies the instruction string to replace, for example,
    $ffffffff with $-1.
    It may be used in the future for computing RIP dependent values, which
	are currently not part of the syscall list. */
string Syscall::get_modified_instruction(Instruction::Ptr instr)
{
    string new_instr;
    Operation opn = instr->getOperation();
    string curr_instr = instr->format();

    if (!instr->isValid() || !instr->isLegalInsn())
        cout << hex << this->address << ": " << dec << instr->format() << endl;

    if (!opn.format().compare("cmp"))
    {
        vector<Operand> operands;
        instr->getOperands(operands);
        for (auto i = operands.begin(); i != operands.end(); ++i)
        {
            Operand op = *i;
            Result res = op.getValue()->eval();
            if (res.type == s32)
            {
                int32_t newval = res.convert<int32_t>();
                string newop = to_string(newval);
                int pos = curr_instr.find('$') + 1;
                int len = curr_instr.find(',') - pos;
                new_instr = curr_instr.replace(pos, len, to_string(newval));
            }
            else
            {
                new_instr = curr_instr;
            }
        }
    }
    else
    {
        new_instr = curr_instr;
    }
    new_instr += "\n";
    return new_instr;
}

string Syscall::get_objdump_instruction(string objdump, Address addr)
{
    char addr_label[12];
    sprintf(addr_label, "%lx:", addr);
    long label_start = objdump.find(addr_label);
    long insn_start = objdump.find_first_not_of(" \t", label_start + strlen(addr_label));
    long insn_end = objdump.find_first_of("#<\n", insn_start);
    //printf("addr_label = %s; insn_start = %ld; insn_end = %ld\n", addr_label, insn_start, insn_end);
    return objdump.substr(insn_start, insn_end - insn_start);
}

int32_t Syscall::get_displacement()
{
    FILE *fpipe;
    string cmd = "nm " + string(HERMIT_EXECUTABLE_PATH) + " | grep " + get_dest_label();
    char line[256];
    uint64_t dest_address;
	int32_t displacement;

    if (!(fpipe = (FILE *)popen(cmd.c_str(), "r")))
    {
        perror("Problems with nm pipe");
        exit(-1);
    }

    while (fgets(line, sizeof line, fpipe))
        ;
    pclose(fpipe);

    if (strlen(line) == 0)
    {
        cout << "Error: Destination label not found\n";
        exit(1);
    }

    dest_address = strtol(line, NULL, 16);

	/* For static binaries the syscall invocation address will always be
	 * superior to the destination as the invocation address is in application
	 * code and the desitination in kernel code. Kernel code is mapped
	 * @0x2000000 and application at 0x4000000. So the displacement is always
	 * negative! */
    displacement = (this->address + JMP_INSTR_SIZE) - dest_address;
    return -displacement;
}

string Syscall::get_assembly_to_write(string objdump, map<int, string>* syscall_func_map)
{
    string assembly = "";
    assembly += this->get_dest_label() + ":\n";

    vector<int> possible_sc_nos = this->get_possible_sc_nos();
    bool indeterminable_syscall = possible_sc_nos.size() != 1 || possible_sc_nos[0] < 0;


    if (indeterminable_syscall)
    {
        assembly += "\tcall " SYSCALL_PROLOGUE_FUNC "\n";
    }
    else
    {
        int syscall_no = possible_sc_nos[0];
        auto func_it = syscall_func_map->find(syscall_no);
        
        if (func_it != syscall_func_map->end())
        {
            string syscall_func = func_it->second;
            assembly += "\tmov \%r10,\%rcx \n";
            assembly += "\tcall " + syscall_func + "\n";
        }
        else
        {
            assembly += "\tcall " SYSCALL_PROLOGUE_FUNC "\n";
        }
    }

    Block::Insns instructions;
    this->next_block->getInsns(instructions);

    for (auto k = instructions.begin(); k != instructions.end(); ++k)
    {
        Instruction::Ptr instr = k->second;
        Address addr = k->first;
        assembly += "\t" + this->get_objdump_instruction(objdump, addr) + "\n";
        //assembly += "\t" + get_modified_instruction(instr);
        this->num_bytes_to_overwrite += instr->size();

        if (this->num_bytes_to_overwrite >= EXTRA_OW_BYTES)
        {
            uint32_t w[2];
            char hexaddr[16];
            uint64_t addr = k->first + instr->size();
            w[1] = 0x0;
            w[0] = addr & 0xffffffff;

            /* Return to user application */
            sprintf(hexaddr, "0x%08x", w[0]);
            assembly += "\tpush $" + string(hexaddr) + "\n";
            assembly += "\tret \n";

            break;
        }
    }

    assembly += "\n";
    return assembly;
}

void Syscall::overwrite(fstream &binfile, uint64_t seg_offset, uint64_t seg_va)
{
    int32_t displacement = this->get_displacement(); // No warning?
    uint64_t write_at = seg_offset + (this->address - seg_va);
    char *to_write = new char[this->num_bytes_to_overwrite];
    string padding = "";

    memset(to_write, REL_JMP_OPCODE, 1);
    for (int i = 0; i < 4; i++)
    {
        char foo = (displacement >> (i * 8)) & 0xFF;
        memset(to_write + i + 1, foo, 1);
    }
//    memset(to_write + JMP_INSTR_SIZE, 0x90, this->num_bytes_to_overwrite);

    binfile.seekp(write_at);
    //binfile.write(to_write, this->num_bytes_to_overwrite);
    binfile.write(to_write, JMP_INSTR_SIZE);
}
