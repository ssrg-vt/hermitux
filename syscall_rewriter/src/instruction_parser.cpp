#include "syscall_rewriting.hpp"

bool is_desired_register(RegisterAST::Ptr reg, string reg_name)
{
    string r1 = reg->format();
    if (boost::algorithm::icontains(r1, reg_name))
    {
        return true;
    }
    return false;
}

string get_size_agnostic_reg_name(RegisterAST::Ptr reg)
{
    string r = reg->format();

    if (iends_with(r, "d"))
    {
        return r.substr(0, r.length() - 1);
    }

    else if (istarts_with(r, "r") || istarts_with(r, "e"))
    {
        return r.substr(1, r.length());
    }
}

bool operand_is_immediate(Operand operand)
{
    Expression::Ptr exp = operand.getValue();
    if (dynamic_cast<Immediate *>(exp.get()) != nullptr)
        return true;
    else
        return false;
}

bool instruction_assigns_to_register(Instruction::Ptr instr, string reg)
{
    set<RegisterAST::Ptr> written;
    instr->getWriteSet(written);

    return any_of(written.begin(), written.end(), [reg](RegisterAST::Ptr r) { return is_desired_register(r, reg); });
}

bool instruction_is_syscall(Instruction::Ptr instr)
{
    entryID op = instr->getOperation().getID();
    if (op == e_syscall)
        return true;
    else
        return false;
}

bool instruction_is_mov(Instruction::Ptr instr)
{
    string mnemonic = instr->getOperation().format();
    if (!mnemonic.compare("mov"))
    {
        return true;
    }
    return false;
}

bool instruction_is_self_xor(Instruction::Ptr instr)
{
    //printf("Self XOR check for %lx: %s\n", addr, instr->format().c_str());
    entryID op = instr->getOperation().getID();
    if (op == e_xor)
    {
        string s0 = instr->getOperand(0).format(instr->getArch());
        string s1 = instr->getOperand(1).format(instr->getArch());
        if (s0 == s1)
        {
            return true;
        }
    }
    return false;
}

uint32_t get_immediate_value(Operand op)
{
    Result res = op.getValue()->eval();
    if (res.type == Result_Type::u32 || res.type == Result_Type::s32)
    {
        uint32_t scval = res.convert<uint32_t>();
        return scval;
    }
    else
    {
        printf("Error with result type!\n");
        return 4567;
    }
}