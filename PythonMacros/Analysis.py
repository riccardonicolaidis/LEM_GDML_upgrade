import os
import shutil
import sys
import time
import subprocess
import argparse

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import ROOT as rt
import uproot as up

import re


def Analysis(input_dir):
    actual_dir = os.getcwd()
    os.chdir(input_dir)
    global_input_dir = os.getcwd()
    os.chdir(actual_dir)
    dirs = os.listdir(global_input_dir)
    
    
    # Check if the directory structure is correct
    DirectoryCheck = False
    
    for dir in dirs:
        if os.path.isdir(os.path.join(global_input_dir, dir)):
            print('Directory: {}'.format(dir))
            if dir == 'GDML_src':
                DirectoryCheck = True
            elif dir == 'Geant_macros':
                DirectoryCheck = True
            elif dir == 'Text_output':
                DirectoryCheck = True
            elif dir == 'Analysis_output':
                DirectoryCheck = True
            elif dir == 'DST':
                DirectoryCheck = True    
            else:
                DirectoryCheck = False
                break
    
    if DirectoryCheck == False:
        print('The input directory is not correct. Please check the input directory.')
        sys.exit()
    else:
        print('The input directory is correct.')
        print('Continue...')
        
    # Retrieve the project directory
    # Retrieve the root macros directory
    # Retrieve the python macros directory
    # 
    # The script assumes that the project directory is the parent directory of the input directory
    
    if 'LEM_GDML_upgrade' in actual_dir:
        while not os.getcwd().endswith('LEM_GDML_upgrade'):
            os.chdir('..')    
        project_dir = os.getcwd()
    
    print('Project directory: {}'.format(project_dir))
    
    for dirs in os.listdir(project_dir):
        if dirs == 'PythonMacros':
            python_dir = os.path.join(project_dir, dirs)
            print('Python directory: {}'.format(python_dir))
        elif dirs == 'ROOT':
            root_dir = os.path.join(project_dir, dirs)
            print('ROOT macros directory: {}'.format(root_dir))

    # Retreive the subfolders of the input directory
    for dirs in os.listdir(global_input_dir):
        if dirs == 'DST':
            dst_dir = os.path.join(global_input_dir, dirs)
            print('DST directory: {}'.format(dst_dir))
        elif dirs == 'GDML_src':
            gdml_src_dir = os.path.join(global_input_dir, dirs)
            print('GDML_src directory: {}'.format(gdml_src_dir))
    
    Number_of_files = len(os.listdir(dst_dir))
    # The script, inside DST, automatically generates folders for each GDML file:
    # GDML_file_0
    # GDML_file_1
    # ...
    # GDML_file_N where N is the number of files in the GDML_ForSimulation directory, copied in the GDML_src
    
    if not os.path.exists(os.path.join(global_input_dir, 'Analysis_output')):
        os.makedirs(os.path.join(global_input_dir, 'Analysis_output'))
    else:
        input_answer = input('The Analysis_output directory already exists' + '\n' + 'Do you want to overwrite it? [y/n]')
        if input_answer == 'y':
            shutil.rmtree(os.path.join(global_input_dir, 'Analysis_output'))
            os.makedirs(os.path.join(global_input_dir, 'Analysis_output'))
        else: 
            sys.exit()
        
        
    # Create subdirectories for each GDML file
    for i in range(Number_of_files):
        os.makedirs(os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(i)))

    for root, dirs, files in os.walk(dst_dir):
        for file in files:
            print("Processing file: {}".format(file))

            pattern = r"/GDML_file_(\d+)"
            
            match = re.search(pattern, root)
            number = int(match.group(1))

            # Usare questa parte come prototipo per runnare uno script root da python
            Command = 'root -l -b -q ' 
            Command += "'"+os.path.join(root_dir, 'SetAliases.C')+ "(\""
            Command += os.path.join(root, file)+'\",\"'
            Command += file.replace(".root", "")+'\",\"'
            Command += os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number))+'\")'+"'"
            
            print(Command)
            
            os.system(Command)
        
if __name__ == "__main__":
    
    
    
    
    parser = argparse.ArgumentParser(description='Script for the analysis of the simulation data of the Geant4 simulation for the Low Energy Module LEM')
    parser.add_argument('-i', '--input-dir', type=str, help='Input directory of the simulation data')

    args = parser.parse_args()
    actual_dir = os.getcwd()
    
    os.chdir(args.input_dir)
    input_dir = os.getcwd()
    os.chdir(actual_dir)
        
    
    Analysis(input_dir)
