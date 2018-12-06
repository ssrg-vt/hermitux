#!/usr/bin/python3

from config import *
from utils import get_results
from plot import plot_results

# LINUX_CMD = './eval ' + RES_FILE_LINUX
LINUX_CMD = './eval ' + RES_FILE_LINUX_OLD
HERMIT_FUNC_CMD = HERMITCORE_CMD + ' ' + HERMIT_EXECUTABLES_DIR + '/syscall_eval ' + RES_FILE_FUNC
HERMIT_SC_CMD = HERMITUX_CMD + ' eval ' + RES_FILE_SC
HERMIT_FAST_SC_CMD = HERMITUX_CMD + ' eval_fast ' + RES_FILE_FAST_SC

def main():
    clear_result_files()
    run_benchmarks()
    print_results()
    # plot_results()

def clear_result_files():
    open(RES_FILE_FUNC, 'w').close()
    open(RES_FILE_SC, 'w').close()
    open(RES_FILE_FAST_SC, 'w').close()
    # open(RES_FILE_LINUX, 'w').close()
    open(RES_FILE_LINUX_OLD, 'w').close()


def run_benchmarks():
    for i in range(0, ITERATIONS):
        proc1 = sp.Popen(HERMIT_FUNC_CMD, shell=True)
        proc1.wait()
    
    for i in range(0, ITERATIONS):
        proc2 = sp.Popen(HERMIT_SC_CMD, shell=True)
        proc2.wait()
    
    for i in range(0, ITERATIONS):
        proc3 = sp.Popen(HERMIT_FAST_SC_CMD, shell=True)
        proc3.wait()

    for i in range(0, ITERATIONS):
        proc4 = sp.Popen(LINUX_CMD, shell=True)
        proc4.wait()


def print_results():
    # avg_lin, stdev_lin = get_results(RES_FILE_LINUX)
    avg_lin, stdev_lin = get_results(RES_FILE_LINUX_OLD)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('Linux', avg_lin, stdev_lin))
    avg_func, stdev_func = get_results(RES_FILE_FUNC)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('Function call', avg_func, stdev_func))
    avg_sys, stdev_sys = get_results(RES_FILE_SC)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('System call', avg_sys, stdev_sys))
    avg_fs, stdev_fs = get_results(RES_FILE_FAST_SC)
    print("{0:16}: Average = {1:10.2f}, Std Deviation = {2:9.2f}".format('Fast system call', avg_fs, stdev_fs))

    speedup = ((avg_sys - avg_fs) / avg_sys) * 100
    print("Speedup = {0:.2f}%".format(speedup))


if __name__ == "__main__":
    main()
