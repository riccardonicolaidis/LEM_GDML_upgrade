import os
import sys
import math
import shutil
import subprocess
import time


def Geant4Simulation():
    
    # RUN SETTING
    RunName = "Geant4Simulation"
    Date = time.strftime("%Y%m%d")
    print("RunName: ", RunName)
    print("Date: ", Date)


    # GEANT4 SIMULATION SETTINGS

    gps_particle = ["e-", "proton", "alpha", "gamma", "mu-"]
    gps_ene_type = "Lin"
    gps_ene_gradient = 0
    gps_ene_intercept = 1
    gps_ene_min = [0.08, 2       , 20     , 10e-3  , 10e3]
    gps_ene_max = [20  , 100     , 500    , 10     , 10e3]
    gps_pos_type = "Plane"
    gps_pos_shape = "Circle"
    gps_pos_centre_cm = [0, 0, 11]
    gps_pos_radius_cm = 40
    gps_ang_type = "iso"
    gps_ang_mintheta_deg = 0
    gps_ang_maxtheta_deg = 90
    run_beamOn = 100000


    # Create a directory for the run
    RunDir = "Output_" + RunName + "_" + Date
    

    # Localizza 
    ActualDir = os.getcwd()
    if "PythonMacros" in ActualDir:
        os.chdir("../")
        ActualDir = os.getcwd()

    if "build" in ActualDir:
        os.chdir("../")
        ActualDir = os.getcwd()

    print("ActualDir: ", ActualDir)
    HomeDirectorySimulation = ActualDir
    
    # Path di partenza dove ci sono tutti i file gdml da usare
    start_GDML_path = os.path.join(HomeDirectorySimulation, "GDML_ForSimulation")
    start_macro_path = os.path.join(HomeDirectorySimulation, "macro")
    start_PythonMacros_path = os.path.join(HomeDirectorySimulation, "PythonMacros")
    start_OutputText_path = os.path.join(HomeDirectorySimulation, "OutputText")

    # Create a directory for the run
    if not os.path.exists(RunDir):
        os.makedirs(RunDir)
    else:
        print("Directory already exists")
        # Ask if I want to delete the directory
        input_user = input("Do you want to delete the directory? [y/n]")
        if input_user == "y":
            shutil.rmtree(RunDir)
            os.makedirs(RunDir)
            print("Directory deleted and created: ", RunDir)
        else:
            print("Directory not deleted")
            return
    
    # Mi sposto nella directory di output
    os.chdir(RunDir)
    print("Directory created: ", RunDir)

    # Voglio strutturare la directory come segue:
    # - Dir: GDML Files for history
    # - Dir: Diagnostic text folders and subfolders
    # - Dir: Datasets  and root files

    os.makedirs("GDML_src")
    os.makedirs("Text_output")
    os.makedirs("DST")
    os.makedirs("Geant_macros")

    out_GDML_path = os.path.join(os.getcwd(), "GDML_src")
    out_Text_path = os.path.join(os.getcwd(), "Text_output")
    out_DST_path = os.path.join(os.getcwd(), "DST")
    out_Geant_macros_path = os.path.join(os.getcwd(), "Geant_macros")
    out_Text_paths = []
    out_DST_paths = []

    # Copi i files gdml nella directory di output
    for file in os.listdir(start_GDML_path):
        if file.endswith(".gdml"):
            shutil.copy(os.path.join(start_GDML_path, file), out_GDML_path)
            print("Copied file: ", file)
    
    print("Found ", len(os.listdir(out_GDML_path)), " gdml files")



    for index, file in enumerate(os.listdir(out_GDML_path)):
        os.makedirs(os.path.join(out_Text_path, "GDML_file_"+str(index)))
        out_Text_paths.append(os.path.join(out_Text_path, "GDML_file_"+str(index)))
        os.makedirs(os.path.join(out_DST_path, "GDML_file_"+str(index)))
        out_DST_paths.append(os.path.join(out_DST_path, "GDML_file_"+str(index)))

        # Create the macro file inside the macro directory
        macro_file = open(os.path.join(start_macro_path, "macro_"+str(index)+".mac"), "w")
        macro_file.write("# Date of creation: "+ str(Date) +" UNIX Time :"+ str(time.time) +"\n")
        macro_file.write("/control/verbose 0\n")
        
        macro_file.write("/gps/pos/type " + gps_pos_type + "\n")
        macro_file.write("/gps/pos/shape " + gps_pos_shape + "\n")
        macro_file.write("/gps/pos/centre " + str(gps_pos_centre_cm[0]) + " " + str(gps_pos_centre_cm[1]) + " " + str(gps_pos_centre_cm[2]) + " cm\n")
        macro_file.write("/gps/pos/radius " + str(gps_pos_radius_cm) + " cm\n")        
        macro_file.write("/gps/ang/type " + gps_ang_type + "\n")
        macro_file.write("/gps/ang/mintheta " + str(gps_ang_mintheta_deg) + " deg\n")
        macro_file.write("/gps/ang/maxtheta " + str(gps_ang_maxtheta_deg) + " deg\n")

        for i in range(len(gps_particle)):
            NameOfFileWithPath = os.path.join(out_DST_paths[index], gps_particle[i])
            macro_file.write("/NameOfFile/NameOfFile " + NameOfFileWithPath + "\n")
            macro_file.write("/gps/particle " + gps_particle[i] + "\n")
            macro_file.write("/gps/ene/type " + gps_ene_type + "\n")
            macro_file.write("/gps/ene/gradient " + str(gps_ene_gradient) + "\n")
            macro_file.write("/gps/ene/intercept " + str(gps_ene_intercept) + "\n")
            macro_file.write("/gps/ene/min " + str(gps_ene_min[i]) + " MeV\n")
            macro_file.write("/gps/ene/max " + str(gps_ene_max[i]) + " MeV\n")
            macro_file.write("/run/beamOn " + str(run_beamOn) + "\n")

        macro_file.close()
        shutil.copy(os.path.join(start_macro_path, "macro_"+str(index)+".mac"), out_Geant_macros_path)

    os.chdir(HomeDirectorySimulation)
    os.system("rm -rf build")
    os.system("mkdir build")
    os.chdir("build")
    os.system("cmake ..")
    os.system("make -j4")

    for index, file in enumerate(os.listdir(out_GDML_path)):
        os.system("./gdml_det " + os.path.join(out_GDML_path, file) + " " + os.path.join(out_Geant_macros_path, "macro_"+str(index)+".mac"))
        os.chdir(HomeDirectorySimulation)   
        # Move folder OutputText to the right place
        for textfile in os.listdir(os.path.join(HomeDirectorySimulation, "OutputText")):
            shutil.copy(os.path.join(HomeDirectorySimulation, "OutputText",textfile), out_Text_paths[index])
        os.chdir("build")
    os.chdir(HomeDirectorySimulation)



if __name__ == "__main__":
    Geant4Simulation()




