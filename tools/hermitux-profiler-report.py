#!/usr/bin/python

# The code to extract dwarf information from an elf binary is adapted from
# the Pyelftools examples

import os, sys, argparse, operator, subprocess

DEFAULT_REPORT_FILE="hermitux-prof.txt"
HERMITUX_APP_LOAD=0x400000

if __name__ == "__main__":
    report_file = DEFAULT_REPORT_FILE
    total_samples = 0
    read_total_samples = 0

    parser = argparse.ArgumentParser(description="Produce a report based on " +
            "a hermitux profiler sample file")
    parser.add_argument("--input", help="profiler sample file", 
            dest="input_file")
    parser.add_argument("--max", help="max amount of (sorted) LoC to dispay",
            dest="max_loc", type=int, default=10)
    args = parser.parse_args()

    if args.input_file:
        report_file = args.input_file

    if not report_file:
        print("Usage:\n%s <report file>", sys.argv[0])
        sys.exit(-1)

    with open(report_file) as f:
        for i, l in enumerate(f):
            pass
        read_total_samples = i + 1 - 2

    data = {}
    with open(report_file, "r") as f_report:
        binary = f_report.readline().replace("\n", "")
        kernel = f_report.readline().replace("\n", "")

        for line in f_report:
            total_samples += 1
            addr = int(line.replace("\n", ""), 16)

            if addr in data.keys():
                data[addr] += 1
            else:
                data[addr] = 1

        # Sort based on occurences
        sorted_data = sorted(data.items(), key=operator.itemgetter(1))
        displayed = 0
        for item in sorted_data[::-1]:
            pct = (item[1]*100) / read_total_samples
            target_binary = kernel
            if item[0] >= HERMITUX_APP_LOAD:
                target_binary = binary

            addr2line_cmd = ['addr2line', '-a', str(hex(item[0])), '-e',
                    target_binary, '-p', '-f']
            addr2line_output = ""
            try:
                addr2line_output = subprocess.check_output(addr2line_cmd,
                        stderr=subprocess.STDOUT)
            except Exception as e:
                sys.exit("Could not run addr2line: {}".format(e))

            print str(pct) + "% (" + str(item[1]) + ") " + addr2line_output.replace("\n", "")
            displayed += 1
            if displayed == args.max_loc:
                break

