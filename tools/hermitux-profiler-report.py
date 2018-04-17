#!/usr/bin/python

# The code to extract dwarf information from an elf binary is adapted from
# the Pyelftools examples

import os, sys, argparse, operator
from elftools.elf.elffile import ELFFile
from elftools.dwarf.descriptions import describe_form_class

file_blacklist = ['crti.s', 'crtn.s']

DEFAULT_REPORT_FILE="hermitux-prof.txt"
HERMITUX_APP_LOAD=0x400000

def decode_funcname(dwarfinfo, address):
    for CU in dwarfinfo.iter_CUs():
        for DIE in CU.iter_DIEs():
            try:
                if DIE.tag == 'DW_TAG_subprogram':
                    lowpc = DIE.attributes['DW_AT_low_pc'].value

                    highpc_attr = DIE.attributes['DW_AT_high_pc']
                    highpc_attr_class = describe_form_class(highpc_attr.form)
                    if highpc_attr_class == 'address':
                        highpc = highpc_attr.value
                    elif highpc_attr_class == 'constant':
                        highpc = lowpc + highpc_attr.value
                    else:
                        print('Error: invalid DW_AT_high_pc class:',
                              highpc_attr_class)
                        continue

                    if lowpc <= address <= highpc:
                        return DIE.attributes['DW_AT_name'].value
            except KeyError:
                continue
    return None

def decode_file_line(dwarfinfo, address):
    for CU in dwarfinfo.iter_CUs():
        lineprog = dwarfinfo.line_program_for_CU(CU)
        prevstate = None
        for entry in lineprog.get_entries():
            if entry.state is None or entry.state.end_sequence:
                continue
            if prevstate and prevstate.address <= address < entry.state.address:
                filename = lineprog['file_entry'][prevstate.file - 1].name
                line = prevstate.line
                if filename not in file_blacklist:
                    return filename, line
            prevstate = entry.state
    return None, None

if __name__ == "__main__":
    report_file = DEFAULT_REPORT_FILE
    samples = {}
    total_samples = 0

    parser = argparse.ArgumentParser(description="Produce a report based on " +
            "a hermitux profiler sample file")
    parser.add_argument("--input", help="profiler sample file", 
            dest="input_file")
    parser.add_argument("--max", help="max amount of (sorted) LoC to dispay",
            dest="max_loc", type=int, default=0)
    parser.add_argument("--sort-by-func", help="use functions rather than " +
            "addresses as the unit of display", dest="sort_by_func",
            action="store_true")
    args = parser.parse_args()

    if args.input_file:
        report_file = args.input_file

    if not report_file:
        print("Usage:\n%s <report file>", sys.argv[0])
        sys.exit(-1)

    with open(report_file, "r") as f_report:
        binary = f_report.readline().replace("\n", "")
        kernel = f_report.readline().replace("\n", "")

        f_binary = open(binary, "rb")
        f_kernel = open(kernel, "rb")
        elf_binary = ELFFile(f_binary)
        elf_kernel = ELFFile(f_kernel)

        if not elf_binary.has_dwarf_info():
            print("Warning: %s has no dwarf info" % binary)
        if not elf_kernel.has_dwarf_info():
            print("Warning: %s has no darf info" % kernel)

        dwarf_binary = elf_binary.get_dwarf_info()
        dwarf_kernel = elf_kernel.get_dwarf_info()

        for line in f_report:
            total_samples += 1
            addr = int(line.replace("\n", ""), 16)

            if addr in samples.keys():
                samples[addr]["occurences"] += 1
            else:
                if addr < HERMITUX_APP_LOAD:
                    # Kernel code
                    funcname = decode_funcname(dwarf_kernel, addr)
                    filename, line = decode_file_line(dwarf_kernel, addr)
                else:
                    # Application code
                    funcname = decode_funcname(dwarf_binary, addr)
                    filename, line = decode_file_line(dwarf_binary, addr)
                
                samples.update({addr : {"funcname" : funcname, "filename" : 
                    filename,  "line" : line, "occurences" : 1}})
       
        print "hermitux-profiler: read {} samples".format(total_samples)

        if not args.sort_by_func:
            print "{0:<10} {1:>8} {2:>4} {3:<12} {4:<10}".format("Address",
                    "samples", "%", "file:line", "function")
            print "------------------------------------------------------"
            printed = 0
            for s in reversed(sorted(samples.iteritems(), key=lambda (x, y): y["occurences"])):
                print "{0:<10} {1:>8} {2:>4} {3:<12} {4:<10}".format("0x" + format(s[0], 
                    "x"), s[1]["occurences"], str((s[1]["occurences"] * 100) / total_samples) + "%",
                    s[1]["filename"] + ":" + str(s[1]["line"]), s[1]["funcname"])
                printed += 1
                if args.max_loc and printed == args.max_loc:
                    break

        else:
            sorted_func = {}
            for addr, sample in samples.iteritems():
                identifier = sample["filename"] + ":" + sample["funcname"]
                if identifier not in sorted_func:
                    sorted_func.update({identifier : sample["occurences"]})
                else:
                    sorted_func[identifier] += sample["occurences"]
            
            print "{0:<20} {1:>8} {2:>8}".format("file:function", "samples", "%")
            print "------------------------------------------------------"
            for s in reversed(sorted(sorted_func.items(), key=operator.itemgetter(1))):
                print "{0:<20} {1:>8} {2:>8}".format(s[0], str(s[1]),
                        str((s[1] * 100)/total_samples) + "%")

        f_binary.close()
        f_kernel.close()

