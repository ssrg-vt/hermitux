#!/usr/bin/python

import argparse, sys, os, shutil, subprocess

TMP_FOLDER="/tmp/"
CFLAGS_BEFORE_INCLUDES=['-D__KERNEL__']
CFLAGS=['-m64', '-Wall', '-O2', '-mno-red-zone', 
        '-fno-var-tracking-assignments', '-fstrength-reduce', 
        '-fomit-frame-pointer', '-finline-functions', '-ffreestanding',
        '-nostdinc', '-fno-stack-protector', '-mno-sse', '-mno-mmx', 
        '-mno-sse2', '-mno-3dnow', '-mno-avx', '-fno-delete-null-pointer-checks',
        '-falign-jumps=1', '-falign-loops=1', '-mno-80387', '-mno-fp-ret-in-387',
        '-mskip-rax-setup', '-fno-common', '-Wframe-larger-than=1024', 
        '-fno-strict-aliasing', '-fno-asynchronous-unwind-tables', 
        '-fno-strict-overflow', '-maccumulate-outgoing-args', '-g']
INCLUDES=[]

def prepare_and_parse_cmdl(argv):
    parser = argparse.ArgumentParser(description = "Disable syscalls in " + 
        "libhermit.a")

    parser.add_argument("-o", "--output", required=True, 
        help="Output libhermit.a")
    parser.add_argument("-s", "--syscalls", required=True, nargs='+',
        help="List of syscalls to disable")
    parser.add_argument("-b", "--hermit-build", help="Hermit build folder " + 
        "location (defaults to <hermit code>/build)")
    parser.add_argument("-t", "--hermit-toolchain", help="Hermit toolchain " +
            "install prefix (defaults to /opt/hermit)")
    parser.add_argument("-c", "--hermit-code", help= "Hermit source folder",
            required=True)
    parser.add_argument("-p", "--libhermit-prefix", help="Hermit sources " + 
        "install, defaults to <hermit code>/prefix")

    return parser.parse_args()

def er(string):
    sys.stderr.write("ERROR: " + string)

def touch(fname, times=None):
    with open(fname, 'a'):
        os.utime(fname, times)

if __name__ == "__main__":
    args = prepare_and_parse_cmdl(sys.argv)

    OUTPUT = os.path.abspath(args.output)
    SYSCALLS = args.syscalls
    TMP=TMP_FOLDER + "/libhermit-tmp.a"
    SRCS = args.hermit_code
    if not SRCS:
        SRCS = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(INPUT))))
    BUILD = args.hermit_build
    if not BUILD:
        BUILD = SRCS + "/build"
    TOOLCHAIN=args.hermit_toolchain
    if not TOOLCHAIN:
        TOOLCHAIN = "/opt/hermit"
    SRC_PREFIX=args.libhermit_prefix
    if not SRC_PREFIX:
        SRC_PREFIX = SRCS + "/prefix"
    INPUT = SRC_PREFIX + "/x86_64-hermit/lib/libhermit.a"
    archiver = TOOLCHAIN + "/bin/x86_64-hermit-ar"
    
    # Copy the original lib to a temporary file
    shutil.copyfile(INPUT, TMP)

    # Delete the syscalls modules
    for s in SYSCALLS:
        # Assumes the following convention: each syscall is implemented into its
        # own source file, named "syscall_name.c". CMake will compile this as an
        # object file named "syscall_name.c.obj", which is what we need to 
        # remove from the static library
        module = s + ".c.obj"
        cmd = [archiver, "dv", TMP, module]
        try:
            ar_output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            er("Deleting " + module + " from " + TMP + ":")
            er(out)
            sys.exit(-1)
        print "Deleted " + module

    # Recompile arch/x86/kernel/isrs.c
    INCLUDES = ["-I" + BUILD + "/include", "-I" + SRCS + "/include"]
    INCLUDES += ["-I" + SRCS + "/arch/x86/include"]
    INCLUDES += ["-I" + SRCS + "/lwip/src/include"]
    INCLUDES += ["-I" + SRCS + "/drivers"]

    env = []
    for s in SYSCALLS:
        env += ['-DDISABLE_SYS_' + s.upper()]

    cmd = [TOOLCHAIN + "/bin/x86_64-hermit-gcc"]
    cmd += CFLAGS_BEFORE_INCLUDES
    cmd += INCLUDES
    cmd += CFLAGS
    cmd += env
    cmd += ['-o', './isrs.c.obj.tmp']
    cmd += ['-c', SRCS + "/arch/x86/kernel/isrs.c"]
    
    try:
        out = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        er("Compiling: \n")
        print out
        exit(-1)

    # Rename elf sections
    cmd = [TOOLCHAIN + "/bin/x86_64-hermit-objcopy"]
    cmd += ["--rename-section", ".bss=.kbss"]
    cmd += ["--rename-section", ".text=.ktext"]
    cmd += ["--rename-section", ".data=.kdata"]
    cmd += ["isrs.c.obj.tmp", "isrs.c.obj"]
    out = ""
    try:
        out = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        er("objcopy: \n")
        er(format(e.output))
        sys.exit(-1)

    # Remove old isrs.o from the static library
    cmd = [archiver, "dv", TMP, "isrs.c.obj"]
    try:
        ar_output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        er("Deleting isrs.c.obj from " + TMP + ":")
        er(out)
        sys.exit(-1)

    # Add new isrs.o to the static library
    cmd = [archiver, "qv", TMP, "./isrs.c.obj"]
    try:
        ar_output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        er("Adding isrs.c.obj from " + TMP + ":")
        er(out)
        sys.exit(-1)

    # Copy the temporary file to the output location
    shutil.copyfile(TMP, OUTPUT)
