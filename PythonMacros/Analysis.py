import os
import shutil
import sys
import time
import subprocess
import argparse

from LatexDocumentClass import LatexDocumentClass
from utils import *

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
    
    
    # ######################################################## #
    #            MINI CHECK FOR THE INPUT DIRECTORY            #
    # ######################################################## #

    DirectoryCheck = 0
    for dir in dirs:
        if os.path.isdir(os.path.join(global_input_dir, dir)):
            print('Directory: {}'.format(dir))
            if dir == 'GDML_src':
                DirectoryCheck += 1
            elif dir == 'Geant_macros':
                DirectoryCheck += 1
            elif dir == 'Text_output':
                DirectoryCheck += 1
            elif dir == 'Analysis_output':
                DirectoryCheck += 1
            elif dir == 'DST':
                DirectoryCheck += 1    
    
    if DirectoryCheck < 5:
        print('The input directory is not correct. Please check the input directory.')
        sys.exit()
    else:
        print('The input directory is correct.')
        print('Continue...')
        

    # ######################################################## #
    #                REPOSITORY DIRECTORY CHECK                #
    # ######################################################## #
    
    if 'LEM_GDML_upgrade' in actual_dir:
        while not os.getcwd().endswith('LEM_GDML_upgrade'):
            os.chdir('..')    
        project_dir = os.getcwd()
    else:
        project_dir = input('Please insert the project directory: ')
    print('Project directory: {}'.format(project_dir))
    
    # ######################################################## #
    #                     PROJECT PATH TREE                    #
    # ######################################################## #
    
    proj_paths = {
        "GDML": os.path.join(project_dir, "GDML"),
        "ROOT": os.path.join(project_dir, "ROOT"),
        "PythonMacros": os.path.join(project_dir, "PythonMacros"),
        "root" : project_dir
    }
    
    # ######################################################## #
    #                     OUTPUT PATH TREE                     #
    # ######################################################## #

    out_paths = {
        "GDML_src" : os.path.join(global_input_dir, "GDML_src"),
        "DST" : os.path.join(global_input_dir, "DST"),
        "Geant_macros" : os.path.join(global_input_dir, "Geant_macros"),
        "Text_output" : os.path.join(global_input_dir, "Text_output"),
        "Analysis_output" : os.path.join(global_input_dir, "Analysis_output"),
        "root" : global_input_dir
    }

    
    Number_of_files = len(os.listdir(out_paths["GDML_src"]))
    # The script, inside DST, automatically generates folders for each GDML file:
    # GDML_file_0
    # GDML_file_1
    # ...
    # GDML_file_N where N is the number of files in the GDML_ForSimulation directory, copied in the GDML_src
    
    if not os.path.exists(out_paths["Analysis_output"]):
        os.makedirs(out_paths["Analysis_output"])
    else:
        input_answer = input('The Analysis_output directory already exists' + '\n' + 'Do you want to overwrite it? [y/n]')
        if input_answer == 'y':
            shutil.rmtree(out_paths["Analysis_output"])
            os.makedirs(out_paths["Analysis_output"])
        else: 
            sys.exit()
        
        
    # ######################################################## #
    #            RETRIEVE FILE NAMES FROM config.txt           #
    # ######################################################## #
    
    # Retrieve the names of the files from the config.txt file
    config_file_name = os.path.join(out_paths["root"], "config.txt")
    config_file = open(config_file_name, "r")
    
    ParticleNames = []
    gps_ene_min = []
    gps_ene_max = []
    StandardNamesRoot = []
    StandardNamesRootAlias = []
    
    for line in config_file:
        ParticleNames.append(line.split()[0])
        gps_ene_min.append(float(line.split()[1]))
        gps_ene_max.append(float(line.split()[2]))
        StandardNamesRoot.append(ParticleNames[-1]+"_t0.root")
        StandardNamesRootAlias.append(ParticleNames[-1]+"_t0_alias.root")
        
    
    
        
    # Create subdirectories for each GDML file
    for i in range(Number_of_files):
        os.makedirs(os.path.join(out_paths["Analysis_output"], 'GDML_file_{}'.format(i)))

        
    for dir in os.listdir(out_paths["DST"]):
        pattern = r"/GDML_file_(\d+)"
        match = re.search(pattern, os.path.join(out_paths["DST"], dir))
        if match is not None:
            number = int(match.group(1))
        else:
            number = -1
    
    
        # ######################################################## #
        #                      SETTING ALIASES                     #
        # ######################################################## #
        
        Command = ''
        ROOT_MacroName = os.path.join(out_paths["ROOT"], 'SetAliases.C')
        ArgVect = []

        for index, file in enumerate(StandardNamesRoot):
                print("Processing file: {}".format(file))
                # Usare questa parte come prototipo per runnare uno script root da python
                
                ArgVect = []
                ArgVect.append(os.path.join(out_paths["DST"], dir ,file))
                ArgVect.append(file.replace(".root", ""))
                ArgVect.append(os.path.join(out_paths["Analysis_output"], 'GDML_file_{}'.format(number)))
                
                Command = ROOT_CMD(ROOT_MacroName, ArgVect)
                print(Command)
                os.system(Command)
                
        PrefixRootFiles = os.path.join(out_paths["DST"],'GDML_file_{}'.format(number))
        PrefixRootFilesAlias = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number))

        Command = ''
        ROOT_MacroName = os.path.join(out_paths["ROOT"], 'GeometricFactor.C')+ "("
        ArgVect = []
        
        for index, file in enumerate(StandardNamesRootAlias):
            ArgVect.append(os.path.join(PrefixRootFilesAlias, file))
            ArgVect.append(file.replace(".root", ""))
        for index in range(3):
            ArgVect.append(gps_ene_min[index])
            ArgVect.append(gps_ene_max[index])
        ArgVect.append(PrefixRootFilesAlias)
        print(Command)
        os.system(Command)
        
        
        #########################
        # Latex Report Creation #
        #########################
                    
        # Directory creation
        os.makedirs(os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number), 'LatexReport'))
        LatexReportDir = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number), 'LatexReport')
        
        # Creazione del report
        Report = LatexDocumentClass()
        
        #Settings
        Report.SetNameOfTheDocument('Simulation Report')
        Report.SetTitle('GEANT4 Simulation Report')
        Report.SetAuthor("Riccardo Nicolaidis")
        Report.SetEmail("riccardo.nicolaidis@unitn.it")
        Report.SetOutputDirectory(LatexReportDir)
        
        # Names of the particles
        ParticleNames = ['electrons', 'protons', 'alphas']
        # Get the images directory
        PDF_images_dir = os.path.join(global_input_dir, 'Analysis_output', 'GDML_file_{}'.format(number))
        
        for index, file in enumerate(StandardNamesRoot):
            
            
            Report.BeginSlide("Montecarlo quantities for {}".format(ParticleNames[index]))
            for mcfile in os.listdir(PDF_images_dir):
                print(mcfile)
                if ("Montecarlo" in mcfile) and (file.replace(".root", "") in mcfile):
                    Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "MC quantities", 0.8)
                    print("Inserted figure: {}".format(mcfile))
            Report.EndSlide()
            
            
            Report.BeginSlide("Energies distribution for {}".format(ParticleNames[index]))
            for mcfile in os.listdir(PDF_images_dir):
                print(mcfile)
                if ("Energies" in mcfile) and (file.replace(".root", "") in mcfile):
                    Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Detected energies", 0.8)
                    print("Inserted figure: {}".format(mcfile))
            Report.EndSlide()
            
            
            Report.BeginSlide("Angles distribution accepted for {}".format(ParticleNames[index]))                
            for mcfile in os.listdir(PDF_images_dir):
                print(mcfile)
                if ("Angles" in mcfile) and (file.replace(".root", "") in mcfile):
                    Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Angles distribution", 0.8)
                    print("Inserted figure: {}".format(mcfile))
            Report.EndSlide()
            
            
            name_histo2D = []
            name_PositionGen = []
            
            
            for imgHisto2D in os.listdir(PDF_images_dir):
                if  ("2DAngHistoFigure_" in imgHisto2D) and (file.replace(".root", "") in imgHisto2D):
                    name_histo2D.append(imgHisto2D)
                if  ("GenPosition_" in imgHisto2D) and (file.replace(".root", "") in imgHisto2D):
                    name_PositionGen.append(imgHisto2D)
            
            
            for index_img, name_img  in enumerate(name_histo2D):
                for j in range(len(name_histo2D)):
                    if ("2DAngHistoFigure_{}".format(j) ) in name_img:
                        Report.BeginSlide("Angles distribution accepted for {}".format(ParticleNames[index]))
                        Report.InsertFigure(os.path.join(PDF_images_dir, name_img), "Angles distribution", 0.8)
                        Report.EndSlide()
                
                        
            for index_img, name_img  in enumerate(name_PositionGen):
                for j in range(len(name_PositionGen)):
                    if ("GenPosition_{}".format(j) ) in name_img:
                        Report.BeginSlide("Generation Position distribution accepted for {}".format(ParticleNames[index]))
                        Report.InsertFigure(os.path.join(PDF_images_dir, name_img), "Generation Position", 0.8)
                        Report.EndSlide()
            
            
            Report.BeginSlide("Geometric factors for {}".format(ParticleNames[index]))
            for mcfile in os.listdir(PDF_images_dir):
                print(mcfile)
                if ("Acceptances" in mcfile) and (file.replace(".root", "") in mcfile):
                    Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Geometric factors", 0.8)
                    print("Inserted figure: {}".format(mcfile))                
            Report.EndSlide()
            
            
            Report.BeginSlide("Geometric factors for {}".format(ParticleNames[index]))
            for mcfile in os.listdir(PDF_images_dir):
                print(mcfile)
                if "ChannelGeomFactor" in mcfile and file.replace(".root", "") in mcfile:
                    Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Geometric factors", 0.8)
                    print("Inserted figure: {}".format(mcfile))
            Report.EndSlide()
            
            
        Report.BeginSlide("Geometric factors for all particles")
        for mcfile in os.listdir(PDF_images_dir):
            if "GeomAll" in mcfile:
                Report.InsertFigure(os.path.join(PDF_images_dir, mcfile), "Geometric factors", 0.8)                         
        Report.EndSlide()
        Report.Compile()
        
            
                                    
if __name__ == "__main__":
    
    ############################
    # Command line parsing     #
    ############################
    
    
    parser = argparse.ArgumentParser(description='Script for the analysis of the simulation data of the Geant4 simulation for the Low Energy Module LEM')
    parser.add_argument('-i', '--input-dir', type=str, help='Input directory of the simulation data')

    args = parser.parse_args()
    actual_dir = os.getcwd()
    
    os.chdir(args.input_dir)
    input_dir = os.getcwd()
    os.chdir(actual_dir)
        
    
    Analysis(input_dir)
