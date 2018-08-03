ITERATIONS = 1000
REPETITIONS = 100000

BENCHMARKS = ["null", "open", "read", "write"]

LINUX_CMD_PREFIX = './prog -N ' + str(REPETITIONS) + ' ' 
HERMIT_CMD_PREFIX = 'make test ARGS="-N ' + str(REPETITIONS) + ' ' 
HERMIT_FAST_CMD_PREFIX = 'make test_fast ARGS="-N ' + str(REPETITIONS) + ' ' 

LINUX_FILE_PREFIX = 'results/linux_'
LINUX_OLD_FILE_PREFIX = 'results/linux_old'
HERMIT_FILE_PREFIX = 'results/hermit_'
HERMIT_FAST_FILE_PREFIX = 'results/hermit_fast_'

PLOT_FILE_SUFFIX = '_plot.pdf'
