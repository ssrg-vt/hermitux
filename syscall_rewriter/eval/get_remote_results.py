#!/usr/bin/python3

from config import *
import plot

REMOTE = 'whitewhale'
REMOTE_HERMITUX_BASE = '/home/danchiba/hermitux'
REMOTE_PATH = '/home/danchiba/hermitux/syscall-rewriter/eval/results/*'

def main():
    cmd = 'scp -r ' + REMOTE + ':' + REMOTE_PATH +  ' ' + RESULTS_DIR
    proc = sp.Popen(cmd, shell=True)
    proc.wait()
    
    plot.plot_results()


if __name__ == '__main__':
    main()
