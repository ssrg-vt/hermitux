#!/usr/bin/python3

import angr
import monkeyhex
import sys

def process_bb(node, function_addr, bb_done, cfg, prev_addr, res):

    # Return if we leave the function
    if node.function_address != function_addr:
        print("@" + str(hex(node.addr)) + " FUCK we exited the function from " + str(hex(prev_addr)))
        print("faddr is " + str(hex(node.function_address)) + " was expecting " + str(hex(function_addr)))
        return

    # Return if we already processed that bb
    if node.addr in bb_done:
        return

    bb_done.append(node.addr)

    if(node.has_return):
        res.append(node)
        return

    next_nodes = []
    jk = node.block.vex.jumpkind
    last_instr_addr = node.block.instruction_addrs[len(node.block.instruction_addrs) - 1]
    if jk == "Ijk_Call" or jk == "Ijk_Sys_syscall":
        next_nodes.append(cfg.model.get_node(last_instr_addr + 4))
    else:
        next_nodes = node.successors

    # recursive call on sucessors
    for s in next_nodes:
        process_bb(s, function_addr, bb_done, cfg, last_instr_addr, res)

def process_exit_node(node):
    print("Exit node: " + str(hex(node.addr)) + ":")
    node.block.pp()
    return 

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("Usage: %s <binary> <svc addr>" % sys.argv[0])
        sys.exit(-1)

    binary_path = sys.argv[1]
    svc_addr = int(sys.argv[2], 16)

    print("Loading %s, checking svc @0x%x\n" % (binary_path, svc_addr))

    p = angr.Project(sys.argv[1])

    # Start a the BB right after the svc instruction
    cfg = p.analyses.CFGFast()
    n = cfg.model.get_node(svc_addr + 4)

    bb_done = []
    exit_nodes = []
    process_bb(n, n.function_address, bb_done, cfg, 0, exit_nodes)

    for n in exit_nodes:
        process_exit_node(n)
