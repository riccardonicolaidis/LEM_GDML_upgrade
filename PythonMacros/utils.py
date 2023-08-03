import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import sys
import os
import re
import shutil


def ROOT_CMD(MacroName, args):
    '''
    MacroName: string, name of the macro (better if it is the global path)
    args: vector of strings or numbers (if numbers they are converted to strings)
    '''
    
    cmd = 'root -l -b -q '
    cmd += "'" + MacroName + '('
    Nargs = len(args)
    for i in range(Nargs):
        IsString = isinstance(args[i], str)
        if IsString:
            if i == Nargs-1:
                cmd += '\"' + str(args[i]) + '\"'
            else:
                cmd += '\"' + str(args[i]) + '\"' + ','
        else:
            if i == Nargs-1:
                cmd += str(args[i])
            else:
                cmd += str(args[i]) + ','
    cmd += ")'"
    return cmd
    
if __name__ == '__main__':
    vect = ["Prova_Frossi", 100000]

    Name = '/home/riccardo/Documenti/GeantProjects/LEM_GDML_upgrade/ROOT/Test.C'
    cmd = ROOT_CMD(Name, vect)
    print(cmd)
    os.system(cmd)