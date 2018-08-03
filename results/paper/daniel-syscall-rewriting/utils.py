from config import *

def enable_entry_measurement():
    enable_entry_measurement_macro(EVAL_HEADER_FILE)
    enable_entry_measurement_macro(SYSCALL_HEADER_FILE)

    compile_hermitux_kernel()
    compile_eval()


def disable_entry_measurement():
    disable_entry_measurement_macro(EVAL_HEADER_FILE)
    disable_entry_measurement_macro(SYSCALL_HEADER_FILE)

    compile_hermitux_kernel()
    compile_eval()


def enable_entry_measurement_macro(filename):
    macro = "#define " + MEASURE_ENTRY_MACRO_NAME
    with open(filename, 'r') as f:
        headerlines = f.readlines()

    for i, line in enumerate(headerlines):
        if macro in line:
            headerlines[i] = line.lstrip('/')

    with open(filename, 'w') as f:
        f.write(''.join(headerlines))


def disable_entry_measurement_macro(filename):
    macro = "#define " + MEASURE_ENTRY_MACRO_NAME
    commented_macro = '//' + macro
    with open(filename, 'r') as f:
        header = f.read()

    if commented_macro in header:
        return

    header = header.replace(macro, commented_macro)
    with open(filename, 'w') as f:
        f.write(header)


def compile_hermitux_kernel():
    cmd1 = "make clean -C " + HERMITUX_BUILD_DIR
    cmd2 = "make -j$(nproc) hermit-bootstrap-install -C " + HERMITUX_BUILD_DIR
    cmd3 = "make -j$(nproc) install -C " + HERMITUX_BUILD_DIR
    
    proc1 = sp.Popen(cmd1, shell=True)
    proc1.wait()

    proc2 = sp.Popen(cmd2, shell=True)
    proc2.wait()

    proc3 = sp.Popen(cmd3, shell=True)
    proc3.wait()


def compile_eval():
    cmd = 'make -j$(nproc) -C' + EVAL_DIR

    proc = sp.Popen(cmd, shell=True)
    proc.wait()


def get_results(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    results = list(map(int, lines))
    return mean(results), stdev(results)
