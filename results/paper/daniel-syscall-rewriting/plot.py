#!/usr/bin/python3

from config import *
from utils import get_results
import matplotlib
import matplotlib.pyplot as plt

# Numbers of iterations of the measurement loop
SRC_LOOP_ITERATIONS=100000

def plot_results():

    matplotlib.rcParams.update({'font.size': 13})

    avg_func, stdev_func = get_results(RES_FILE_FUNC)
    avg_sys, stdev_sys = get_results(RES_FILE_SC)
    avg_fs, stdev_fs = get_results(RES_FILE_FAST_SC)
    avg_lin, stdev_lin = get_results(RES_FILE_LINUX)
    avg_linold, stdev_linold = get_results(RES_FILE_LINUX_OLD)

    means = [avg_lin, avg_linold, avg_sys, avg_fs, avg_func]
    
    # Divide each total loop latency by the number of iteration to get the
    # average latency for a signle syscall
    means = [x/SRC_LOOP_ITERATIONS for x in means]

    # Pierre: as we now show a single syscal laltency, let's remove stdevs
    #stdevs = [stdev_lin, stdev_linold, stdev_sys, stdev_fs, stdev_func]
    ind = [1, 2, 3, 4, 5]
    
    fig, ax = plt.subplots()
    #rects = ax.bar(ind, means, 0.5, yerr=stdevs)
    rects = ax.bar(ind, means, 0.6, color=['skyblue', 'skyblue', 'moccasin',
        'moccasin', 'cornsilk'], 
        edgecolor=['black', 'black', 'black', 'black', 'black'])

    # No need for a title, we can save some space by putting all the info 
    # in the y_label
    ax.set_ylabel('System call latency\n(clock cycles)')
    ax.set_xticks(ind)
    ax.set_xticklabels(("Linux", "Linux\n(unpatched)", "HermiTux\nsyscall",
                        "HermiTux \nfast syscall", "HermitCore\nfunction call"))
    ax.grid()
    ax.set_axisbelow(True)
    
    # let's not show the speedup arrow it's too unclear
    #speedup = ((avg_sys - avg_fs) / avg_sys) * 100
    #speedup_str = "Speedup = {0:.2f}%".format(speedup)
    #ax.text(4, rects[0].get_height(), speedup_str, horizontalalignment='center')
    #ax.annotate(speedup_str, xy=(0.6, 0.44), xytext=(0.6, 0.50), xycoords='axes fraction',
    #            textcoords=('axes fraction'), ha='center',
    #            bbox=dict(lw=2, fc='cyan'),
    #            arrowprops=dict(arrowstyle='-[, widthB=6,lengthB=0.7', lw=1.2))
    autolabel(rects, ax)


    ax.annotate('straight',
        xy=(0, 1), xycoords='data',
        xytext=(-50, 30), textcoords='offset points',
        arrowprops=dict(arrowstyle="->"))

    plt.tight_layout()
    fig.set_size_inches(7,3)
    fig.subplots_adjust(bottom=0.2)
    fig.savefig(FINAL_PLOT_FILE)

    # Uncomment this to get the interactive plot window
    #plt.show()

def autolabel(rects, ax):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., height,
                '%d' % int(height),
                ha='center', va='bottom')


if __name__ == "__main__":
    plot_results()
