import os
import sys
import math
import shutil
import subprocess
import time


def Geant4Simulation():
    
    # ######################################################## #
    #                       RUN SETTINGS                       #
    # ######################################################## #
    
    RunName = "Geant4Simulation"
    Date = time.strftime("%Y%m%d")
    print("RunName: ", RunName)
    print("Date: ", Date)

    N_jobs = 10
    N_evjob = 10000000

    # Run directory for the run
    RunDir = "Output_" + RunName + "_" + Date


    # ######################################################## #
    #                      GEANT4 SETTINGS                     #
    # ######################################################## #

    gps_particle = ["e-", "proton", "alpha"]
    gps_ene_min = [0.08, 2       , 10]
    gps_ene_max = [20  , 100     , 500]


    gps_ene_type = "Lin"
    gps_ene_gradient = 0
    gps_ene_intercept = 1
    gps_pos_type = "Plane"
    gps_pos_shape = "Circle"
    gps_pos_centre_cm = [0, 0, 12]
    gps_pos_radius_cm = 50
    gps_ang_type = "iso"
    gps_ang_mintheta_deg = 0
    gps_ang_maxtheta_deg = 90
    run_beamOn = N_evjob


    

    # ######################################################## #
    #               Find location inside project               #
    # ######################################################## #
    
    ActualDir = os.getcwd()
    if "PythonMacros" in ActualDir:
        os.chdir("../")
        ActualDir = os.getcwd()

    if "build" in ActualDir:
        os.chdir("../")
        ActualDir = os.getcwd()

    print("ActualDir: ", ActualDir)
    HomeDirectorySimulation = ActualDir
    
    # ######################################################## #
    #                       PROJECT PATHS                      #
    # ######################################################## #
    
    proj_paths = {
        "GDML": os.path.join(HomeDirectorySimulation, "GDML_ForSimulation"),
        "macro": os.path.join(HomeDirectorySimulation, "macro"),
        "PythonMacros": os.path.join(HomeDirectorySimulation, "PythonMacros"),
        "OutputText": os.path.join(HomeDirectorySimulation, "OutputText")
    }
    

    # ######################################################## #
    #               DIRECTORY CREATION FOR OUTPUT              #
    # ######################################################## #

    if not os.path.exists(RunDir):
        os.makedirs(RunDir)
    else:
        print("Directory already exists")
        input_user = input("Do you want to delete the directory? [y/n]")
        if input_user == "y":
            shutil.rmtree(RunDir)
            os.makedirs(RunDir)
            print("Directory deleted and created: ", RunDir)
        else:
            print("Directory not deleted")
            return    
    os.chdir(RunDir)
    print("Directory created: ", RunDir)
    RunDir_global = os.getcwd()
    
    proj_paths.update({"RunDir": RunDir_global})
    
    
    # ######################################################## #
    #               CONFIGURATION FILE GENERATION              #
    # ######################################################## #
    
    config_file = open(os.path.join(RunDir_global,"config.txt"), "w")

    for index in range(len(gps_particle)):
        config_file.write(gps_particle[index]+ " ")
        config_file.write(str(gps_ene_min[index]) + " ")
        config_file.write(str(gps_ene_max[index]) + " ")
        config_file.write("\n")
    config_file.close()

    # Voglio strutturare la directory come segue:
    # - Dir: GDML Files for history
    # - Dir: Diagnostic text folders and subfolders
    # - Dir: Datasets  and root files

    os.makedirs("GDML_src")
    os.makedirs("Text_output")
    os.makedirs("DST")
    os.makedirs("Geant_macros")    
    
    out_paths = {
        "GDML": os.path.join(os.getcwd(), "GDML_src"),
        "Text_output": os.path.join(os.getcwd(), "Text_output"),
        "DST": os.path.join(os.getcwd(), "DST"),
        "Geant_macros": os.path.join(os.getcwd(), "Geant_macros")
    }
    
    out_Text_paths = []
    out_DST_paths = []

    # Copi i files gdml nella directory di output
    for file in os.listdir(proj_paths["GDML"]):
        if file.endswith(".gdml"):
            shutil.copy(os.path.join(proj_paths["GDML"], file), out_paths["GDML"])
            print("Copied file: ", file)
    
    print("Found ", len(os.listdir(out_paths["GDML"])), " gdml files")

    report_file = open(os.path.join(proj_paths["RunDir"], "report.txt"), "w")

    for index, file in enumerate(os.listdir(out_paths["GDML"])):
        os.makedirs(os.path.join(out_paths["Text_output"], "GDML_file_"+str(index)))
        out_Text_paths.append(os.path.join(out_paths["Text_output"], "GDML_file_"+str(index)))
        os.makedirs(os.path.join(out_paths["DST"], "GDML_file_"+str(index)))
        out_DST_paths.append(os.path.join(out_paths["DST"], "GDML_file_"+str(index)))

        # Create the macro file inside the macro directory
        # Check if the file already exists

        
        for n in range(N_jobs):    
            
            macro_file_name = os.path.join(out_paths["Geant_macros"], "macro_f"+str(index)+"_j"+str(n)+".mac")
            steering_file_name = os.path.join(out_paths["Geant_macros"], "steering_f"+str(index)+"_j"+str(n)+".txt")
        
            report_file.write("File "+ str(index) + " Job "+ str(n) + "\n")
            report_file.write(file + "\n")
            report_file.write("Macro file: " + macro_file_name + "\n")
            report_file.write("Steering file: " + steering_file_name + "\n")
        
            macro_file = open(macro_file_name, "w")
            steering_file = open(steering_file_name, "w")
            steering_file.write("OUT_TEXT " + os.path.join(out_paths["Text_output"], "GDML_file_"+str(index))+"\n")
            steering_file.close()
            
            
            macro_file.write("# Date of creation: "+ str(Date) +" UNIX Time :"+ str(time.time()) +"\n")
            macro_file.write("# File GDML : "+ str(file) +"\n\n\n")
            macro_file.write("/control/verbose 0\n")
            macro_file.write("/run/verbose 1\n")
            macro_file.write("/event/verbose 0\n\n\n")
            macro_file.write("/run/initialize\n\n\n")
            macro_file.write("/random/SetSeed " + str(n) + str(n) + "\n\n\n")
            
            macro_file.write("/gps/pos/type " + gps_pos_type + "\n")
            macro_file.write("/gps/pos/shape " + gps_pos_shape + "\n")
            macro_file.write("/gps/pos/centre " + str(gps_pos_centre_cm[0]) + " " + str(gps_pos_centre_cm[1]) + " " + str(gps_pos_centre_cm[2]) + " cm\n")
            macro_file.write("/gps/pos/radius " + str(gps_pos_radius_cm) + " cm\n")        
            macro_file.write("/gps/ang/type " + gps_ang_type + "\n")
            macro_file.write("/gps/ang/mintheta " + str(gps_ang_mintheta_deg) + " deg\n")
            macro_file.write("/gps/ang/maxtheta " + str(gps_ang_maxtheta_deg) + " deg\n\n\n\n")
            
            for i in range(len(gps_particle)):
                NameOfFileWithPath = os.path.join(out_DST_paths[index], gps_particle[i], "_j"+str(n))
                report_file.write("Particle: " + gps_particle[i] + "\n")
                report_file.write("Energy: " + str(gps_ene_min[i]) + " - " + str(gps_ene_max[i]) + " MeV\n")
                report_file.write("Output file: " + NameOfFileWithPath + "\n")
                macro_file.write("/NameOfFile/NameOfFile " + NameOfFileWithPath + "\n")
                macro_file.write("/gps/particle " + gps_particle[i] + "\n")
                macro_file.write("/gps/ene/type " + gps_ene_type + "\n")
                macro_file.write("/gps/ene/gradient " + str(gps_ene_gradient) + "\n")
                macro_file.write("/gps/ene/intercept " + str(gps_ene_intercept) + "\n")
                macro_file.write("/gps/ene/min " + str(gps_ene_min[i]) + " MeV\n")
                macro_file.write("/gps/ene/max " + str(gps_ene_max[i]) + " MeV\n")
                macro_file.write("/run/beamOn " + str(run_beamOn) + "\n")
            report_file.write("\n\n")
            macro_file.close()
            
            condor_file_name = os.path.join(out_paths["Geant_macros"], "condor_f"+str(index)+"_j"+str(n)+".sub")
            
            condor_file = open(condor_file_name, "w")
            # Write a condor configuration file for submitting job
            condor_file.write("universe = vanilla\n")
            condor_file.write("getenv = True\n")
            condor_file.write("executable = " + os.path.join(HomeDirectorySimulation, "build", "gdml_det") + "\n")
            Arguments = [macro_file_name, steering_file_name]
            condor_file.write("arguments = " + " ".join(Arguments) + "\n")
    
    
    report_file.close()        
            
    os.chdir(HomeDirectorySimulation)
    os.system("rm -rf build")
    os.system("mkdir build")
    os.chdir("build")
    os.system("cmake ..")
    os.system("make -j4")

    CondorSubmtFiles = []
    for files in os.listdir(out_paths["Geant_macros"]):
        if files.endswith(".sub"):
            CondorSubmtFiles.append(files)

    for files in CondorSubmtFiles:
        os.system("condor_submit " + files)
        #sleep(0.1)
        os.system("sleep 0.1")    




if __name__ == "__main__":
    Geant4Simulation()




