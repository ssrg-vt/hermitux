import subprocess as sp
from statistics import mean, stdev

HERMITUX_BASE = '/home/danchiba/Documents/HermiTux/hermitux/'
HERMIT_LOCAL_INSTALL = HERMITUX_BASE + 'hermitux-kernel/prefix/'
HERMITUX_KERNEL = HERMIT_LOCAL_INSTALL + 'x86_64-hermit/extra/tests/hermitux'

RES_FILE_FUNC = './func_call_results'
RES_FILE_SC = './syscall_results'
RES_FILE_FAST_SC = './fast_sc_results'

HERMIT_CMD = 'HERMIT_VERBOSE=0 HERMIT_ISLE=uhyve HERMIT_TUX=1 HERMIT_KVM=1 ' + \
                  HERMIT_LOCAL_INSTALL + '/bin/proxy ' + HERMITUX_KERNEL

HERMIT_FUNC_CMD = HERMIT_CMD + ' ioctl_unopt func ' + RES_FILE_FUNC
HERMIT_SC_CMD = HERMIT_CMD + ' ioctl syscall ' + RES_FILE_SC
HERMIT_FAST_SC_CMD = HERMIT_CMD + ' ioctl_fast syscall ' + RES_FILE_FAST_SC

ITERATIONS = 100



def main():
    clear_result_files()
    run_benchmarks()
    print_results()

def clear_result_files():
    open(RES_FILE_FUNC, 'w').close()
    open(RES_FILE_SC, 'w').close()
    open(RES_FILE_FAST_SC, 'w').close()
    
    
def run_benchmarks():
    for i in range(0, ITERATIONS):
        sp.Popen(HERMIT_FUNC_CMD, shell=True)
        sp.Popen(HERMIT_SC_CMD, shell=True)
        sp.Popen(HERMIT_FAST_SC_CMD, shell=True)


def print_results():
    avg, stdev = get_results(RES_FILE_FUNC)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('Function call', avg, stdev))
    avg, stdev = get_results(RES_FILE_SC)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('System call', avg, stdev))
    avg, stdev = get_results(RES_FILE_FAST_SC)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('Fast system call', avg, stdev))


def get_results(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    results = list(map(int, lines))
    return mean(results), stdev(results)

if __name__ == "__main__":
    main()
    
