import os
import sys
import re
import matplotlib.pyplot as plt
import numpy as np


def SimplePlot():
    """Plot a simple function."""
    Path = "/data1/home/rnicolai/LEM_GDML_upgrade/Test_condor"
    
    print("Now I am in the directory: ", os.getcwd())
    print("I am going to change directory to: ", Path)
    #os.chdir(Path)
    x = np.linspace(0, 2*np.pi, 100)
    y = np.sin(x)
    plt.plot(x, y)
    #plt.show()
    plt.savefig("/data1/home/rnicolai/LEM_GDML_upgrade/Test_condor/SimplePlot.png")



if __name__ == '__main__':
    SimplePlot()