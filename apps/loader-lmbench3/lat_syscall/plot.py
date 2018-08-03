#!/usr/bin/python3

from config import *
from statistics import mean, stdev
import matplotlib
import matplotlib.pyplot as plt

def main():
    for bench in BENCHMARKS:
        fname = LINUX_FILE_PREFIX + bench
        lin_avg, lin_stdev = get_results(fname)

        fname = HERMIT_FILE_PREFIX + bench
        hermit_avg, hermit_stdev = get_results(fname)

        fname = HERMIT_FAST_FILE_PREFIX + bench
        hfast_avg, hfast_stdev = get_results(fname)

        means = [lin_avg, hermit_avg, hfast_avg]
        stdevs = [lin_stdev, hermit_stdev, hfast_stdev]

        ind = [1, 2, 3]
    
        fig, ax = plt.subplots()
        rects = ax.bar(ind, means, 0.5, yerr=stdevs)

        ax.set_xticks(ind)
        ax.set_xticklabels(("Linux", "HermiTux\nsyscall", "HermiTux \nfast syscall"))
        ax.grid()
        ax.set_axisbelow(True)

        autolabel(rects, ax)

        hermit_speedup = (hermit_avg - hfast_avg) / hermit_avg * 100
        speedup_str = "Speedup = {0:.2f}%".format(hermit_speedup)
        ax.text(4, rects[0].get_height(), speedup_str, horizontalalignment='center')
        ax.annotate(speedup_str, xy=(0.6, 0.44), xytext=(0.6, 0.50), xycoords='axes fraction',
                    textcoords=('axes fraction'), ha='center',
                    bbox=dict(lw=2, fc='cyan'),
                    arrowprops=dict(arrowstyle='-[, widthB=6,lengthB=0.7', lw=1.2))


        plt.tight_layout()
        fig.set_size_inches(7,4.5)
        fig.savefig('results/' + bench + PLOT_FILE_SUFFIX)


def autolabel(rects, ax):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., height,
                '%d' % int(height),
                ha='center', va='bottom')


def get_results(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    results = list(map(float, lines))
    return mean(results), stdev(results)


if __name__ == '__main__':
    main()
