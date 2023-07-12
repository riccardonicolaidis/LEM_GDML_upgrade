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

from utils import *

import re




def Analysis(input_dir):
    
    NFiles = 4
    Emin_plots = 0.08
    Emax_plots = 400.0
    E_thr_Thin = 0.04
    E_thr_Thick = 0.04
    E_thr_Plastic = 0.1
    
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
    else:
        project_dir = ''
        for root, dirs, files in os.walk(actual_dir):
            for dir in dirs:
                if dir == 'LEM_GDML_upgrade':
                    os.chdir(os.path.join(root, dir))
                    project_dir = os.getcwd()
                    break
    
    print('Project directory: {}'.format(project_dir))
    
    python_dir = ''
    root_dir = ''
    for dirs in os.listdir(project_dir):
        if dirs == 'PythonMacros':
            python_dir = os.path.join(project_dir, dirs)
            print('Python directory: {}'.format(python_dir))
        elif dirs == 'ROOT':
            root_dir = os.path.join(project_dir, dirs)
            print('ROOT macros directory: {}'.format(root_dir))

    # Retreive the subfolders of the input directory
    dst_dir = ''
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


    
    IsThereRootFiles = False
    for root, dirs, files in os.walk(dst_dir):
        if 'GDML_file' in root:
            for file in files:
                if file.endswith(".root"):
                    IsThereRootFiles = True
                    break

    print('File root are OK')

    for root, dirs, files in os.walk(dst_dir):
        
        for dir in dirs:
            pattern = r"/GDML_file_(\d+)"
            match = re.search(pattern, os.path.join(root, dir))
            if match is not None:
                number = int(match.group(1))
            else:
                number = -1
            
            
            Command = ''
            ROOT_MacroName = os.path.join(root_dir, 'SetAliases.C')
            ArgVect = []

            PrefixRootFiles = os.path.join(dst_dir,'GDML_file_{}'.format(number))
            PrefixRootFilesAlias = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number))
            

            ListOfRootFiles = []
            # open file FileNames.txt
            with open(os.path.join(PrefixRootFiles, 'FileNames.txt'), 'r') as f:
                for line in f:
                    ListOfRootFiles.append(line.rstrip('\n'))
                    print(line.rstrip('\n'))
            
            fNamesAlias = open(os.path.join(PrefixRootFilesAlias, 'FileNames.txt'), 'w')

            for rootFileName in ListOfRootFiles:
                if rootFileName.endswith(".root"):
                    aliasName = rootFileName.replace(".root", "_alias.root")
                    fNamesAlias.write(os.path.join(PrefixRootFilesAlias, aliasName) + '\n')
            fNamesAlias.close()
            

            
            
            
            for index, file in enumerate(ListOfRootFiles):
                print("Processing file: {}".format(file))
                ArgVect = []
                ArgVect.append(os.path.join(root, dir ,file))
                ArgVect.append(file.replace(".root", ""))
                ArgVect.append(os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number)))
                ArgVect.append(E_thr_Thin)
                ArgVect.append(E_thr_Thick)
                ArgVect.append(E_thr_Plastic)
                Command = ROOT_CMD(ROOT_MacroName, ArgVect)
                print(Command)
                os.system(Command)
                
            
            
            
            
            
            Command = ''
            ROOT_MacroName = os.path.join(root_dir, 'PID.C')
            ArgVect = []
            ArgVect.append(NFiles)
            ArgVect.append(os.path.join(PrefixRootFilesAlias, 'FileNames.txt'))
            ArgVect.append(os.path.join(PrefixRootFiles, 'EnergyGen.txt'))
            ArgVect.append(PrefixRootFilesAlias)
            ArgVect.append(Emin_plots)
            ArgVect.append(Emax_plots)
            ArgVect.append(E_thr_Thin)
            ArgVect.append(E_thr_Thick)
            ArgVect.append(E_thr_Plastic)
            Command = ROOT_CMD(ROOT_MacroName, ArgVect)            
            print(Command)
            os.system(Command)
            
        # Macro root for PID is the following
            #int PID(
            # TString filename_e,
            # TString filename_e_noExt,
            # TString filename_p,
            # TString filename_p_noExt,
            # TString filename_alpha,
            # TString filename_alpha_noExt,
            # double Emin_e = 0.0,
            # double Emax_e = 0.0,
            # double Emin_p = 0.0,
            # double Emax_p = 0.0,
            # double Emin_alpha = 0.0,
            # double Emax_alpha = 0.0,
            # TString destination,
            # int NumberPairsSensors = 5,
            # double ResSilicon = 0.03,
            # double ResPlastic = 0.03,
            # double E_thr_Thin = 0.04,
            # double E_thr_Thick = 0.04,
            # double E_thr_Plastic = 0.04
            # )
            
            
            
                
        
        
if __name__ == "__main__":
    
    
    
    
    parser = argparse.ArgumentParser(description='Script for the analysis of the simulation data of the Geant4 simulation for the Low Energy Module LEM')
    parser.add_argument('-i', '--input-dir', type=str, help='Input directory of the simulation data')

    args = parser.parse_args()
    actual_dir = os.getcwd()
    
    os.chdir(args.input_dir)
    input_dir = os.getcwd()
    os.chdir(actual_dir)
        
    
    Analysis(input_dir)
