#!/usr/bin/python3

import angr
import monkeyhex
import sys

# Print to stderr
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def process_bb(node, function_addr, bb_done, cfg, prev_addr, res):

    # Return if we leave the function
    if node.function_address != function_addr:
        eprint("WARNING: @" + str(hex(node.addr)) + " we exited the function "
                "from " + str(hex(prev_addr)))
        eprint("  --> faddr is " + str(hex(node.function_address)) + " was "
                "expecting " + str(hex(function_addr)))
        return

    # Return if we already processed that bb
    if node.addr in bb_done:
        return

    bb_done.append(node.addr)

    # Is this an exit node?
    if(node.has_return):
        res.append(node)
        return

    # Prepare list of nodes to process next. We want to stay in the current
    # function so if we get a call to another, consider the next node from
    # the current function
    next_nodes = []
    jk = node.block.vex.jumpkind
    last_instr_addr = node.block.instruction_addrs[len(node.block.instruction_addrs) - 1]
    if jk == "Ijk_Call" or jk == "Ijk_Sys_syscall":
        next_nodes.append(cfg.model.get_node(last_instr_addr + 4))
    else:
        next_nodes = node.successors

    for s in next_nodes:
        process_bb(s, function_addr, bb_done, cfg, last_instr_addr, res)

def process_exit_node(node):
    cs = node.block.capstone

    # Search for an instruction modifying x30
    for instruction in cs.insns:
        i = instruction.insn
        written_regs = i.regs_access()[1]
        for r in written_regs:
            if(i.reg_name(r) == "x30"):
                return True

    # Ok, we couldnt find an x30 modification. If we only have a successor,
    # check it out. Checking more successors may be a little bit complex
    if len(node.successors) == 1:
        return process_exit_node(node.successors[0])

    return False

def process_svc(addr, cfg):
    n = cfg.model.get_node(addr + 4)
    whitelist = True

    bb_done = []
    exit_nodes = []
    process_bb(n, n.function_address, bb_done, cfg, 0, exit_nodes)

    for n in exit_nodes:
        whitelist = whitelist and process_exit_node(n)

    return whitelist

def find_syscalls(node, addrs_done):
    res = []

    if node.addr in addrs_done:
        return res

    addrs_done.append(node.addr)

    if(node.is_syscall):
        for s in node.predecessors:
            res.append(s.instruction_addrs[len(s.instruction_addrs)-1])

    for s in node.successors:
        res += find_syscalls(s, addrs_done)

    return res


if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Usage: %s <binary>" % sys.argv[0])
        sys.exit(-1)

    p = angr.Project(sys.argv[1], auto_load_libs=False)

    # Start a the BB right after the svc instruction
    cfg = p.analyses.CFGFast()

    addrs_done = []
    syscall_list = find_syscalls(cfg.model.get_node(p.entry), addrs_done)

    num_whitelisted = 0
    for s in syscall_list:
        whitelisted = process_svc(s, cfg)
        if whitelisted:
            num_whitelisted += 1
        print(str(hex(s)) + ": " + str(whitelisted))

    eprint("Whitelisted %d / %d syscalls" % (num_whitelisted, len(syscall_list)))

