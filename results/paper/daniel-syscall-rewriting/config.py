import subprocess as sp
import os
from statistics import mean, stdev

HERMITUX_BASE = '../../'

EVAL_DIR = HERMITUX_BASE + '/syscall-rewriter/eval'

HERMIT_LOCAL_INSTALL = HERMITUX_BASE + '/hermitux-kernel/prefix/'
HERMIT_EXECUTABLES_DIR = HERMIT_LOCAL_INSTALL + '/x86_64-hermit/extra/tests/'
HERMITUX_KERNEL = HERMIT_EXECUTABLES_DIR + '/hermitux'
HERMITUX_KERNEL_SRC_DIR = HERMITUX_BASE + '/hermitux-kernel/'
HERMITUX_BUILD_DIR = HERMITUX_KERNEL_SRC_DIR + '/build'

EVAL_HEADER_FILE = EVAL_DIR + '/eval.h'
SYSCALL_HEADER_FILE = HERMITUX_KERNEL_SRC_DIR + '/include/hermit/syscall.h'
MEASURE_ENTRY_MACRO_NAME = 'MEASURE_SYSCALL_ENTRY'

RESULTS_DIR = './results/'

RES_FILE_FUNC = RESULTS_DIR + 'func_call_results'
RES_FILE_SC = RESULTS_DIR + 'syscall_results'
RES_FILE_FAST_SC = RESULTS_DIR + 'fast_sc_results'
RES_FILE_LINUX = RESULTS_DIR + 'linux_results'
RES_FILE_LINUX_OLD = RESULTS_DIR + 'linux_old_results'

FUNC_START_FILE = RESULTS_DIR + 'func_start_times'
REG_SC_START_FILE = RESULTS_DIR + 'reg_sc_start_times'
FAST_SC_START_FILE = RESULTS_DIR + 'fast_sc_start_times'
LINUX_START_FILE = RESULTS_DIR + 'linux_start_times'
SC_ENTERED_FILE = RESULTS_DIR + 'sc_entered_times'

FINAL_PLOT_FILE = './SyscallPerf.pdf'

HERMITUX_CMD = 'HERMIT_VERBOSE=0 HERMIT_ISLE=uhyve HERMIT_TUX=1 HERMIT_KVM=1 ' + \
                  HERMIT_LOCAL_INSTALL + '/bin/proxy ' + HERMITUX_KERNEL

HERMITCORE_CMD = 'HERMIT_VERBOSE=0 HERMIT_ISLE=uhyve HERMIT_KVM=1 ' + \
                  HERMIT_LOCAL_INSTALL + '/bin/proxy '

# HERMIT_FUNC_CMD = HERMIT_CMD + ' ioctl_func ' + RES_FILE_FUNC
# HERMIT_SC_CMD = HERMIT_CMD + ' ioctl_sys ' + RES_FILE_SC
# HERMIT_FAST_SC_CMD = HERMIT_CMD + ' ioctl_sys_fast ' + RES_FILE_FAST_SC

ITERATIONS = 10
