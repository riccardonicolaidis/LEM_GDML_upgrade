import os
import sys
import math
import shutil
import subprocess
import time


def Geant4Simulation():
    
    # RUN SETTING
    RunName = "Geant4Simulation_PID"
    Date = time.strftime("%Y%m%d")
    print("RunName: ", RunName)
    print("Date: ", Date)


    # GEANT4 SIMULATION SETTINGS

    gps_particle = ["e-", "proton", "alpha", "e+"]
    gps_ene_min = [0.01, 1, 3, 0.01]
    gps_ene_max = [20, 200, 1000, 20]

    
    SaveRandomSeed = False    

    gps_ene_type = "Lin"
    gps_ene_gradient = 0
    gps_ene_intercept = 1
    #gps_position_cm = [0,0, 12]
    gps_pos_type = "Plane"
    gps_pos_shape = "Circle"
    gps_pos_centre_cm = [0, 0, 11.24]
    gps_pos_radius_cm = 0.3
    gps_ang_type = "iso"
    #gps_direction = [1, 1, -3.6]
    gps_ang_mintheta_deg = 0
    gps_ang_maxtheta_deg = 30
    run_beamOn = 2000000


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
    RunDir_global = os.getcwd()

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
        # Check if the file already exists
        if os.path.exists(os.path.join(start_macro_path, "macro_"+str(index)+".mac")):
            os.remove(os.path.join(start_macro_path, "macro_"+str(index)+".mac"))
        
            
        macro_file = open(os.path.join(start_macro_path, "macro_"+str(index)+".mac"), "w")
        macro_file.write("# Date of creation: "+ str(Date) +" UNIX Time :"+ str(time.time) +"\n")
        macro_file.write("/control/verbose 0\n")
        
        macro_file.write("/gps/pos/type " + gps_pos_type + "\n")
        macro_file.write("/gps/pos/shape " + gps_pos_shape + "\n")
        macro_file.write("/gps/pos/centre " + str(gps_pos_centre_cm[0]) + " " + str(gps_pos_centre_cm[1]) + " " + str(gps_pos_centre_cm[2]) + " cm\n")
        macro_file.write("/gps/pos/radius " + str(gps_pos_radius_cm) + " cm\n")        
        
        #macro_file.write("/gps/position " + str(gps_position_cm[0]) + " " + str(gps_position_cm[1]) + " " + str(gps_position_cm[2]) + " cm\n")
        macro_file.write("/gps/ang/type " + gps_ang_type + "\n")
        macro_file.write("/gps/ang/mintheta " + str(gps_ang_mintheta_deg) + " deg\n")
        macro_file.write("/gps/ang/maxtheta " + str(gps_ang_maxtheta_deg) + " deg\n")
        #macro_file.write("/gps/direction " + str(gps_direction[0]) + " " + str(gps_direction[1]) + " " + str(gps_direction[2]) + "\n")

        for i in range(len(gps_particle)):
            NameOfSettingFile = "setting_file_"+str(gps_particle[i])+".mac"
            
            settings_file = open(os.path.join(out_Geant_macros_path, NameOfSettingFile), "w")
            settings_file.write("# Date of creation: "+ str(Date) +" UNIX Time :"+ str(time.time) +"\n")
            settings_file.write("/control/verbose 0\n")
            settings_file.write("/gps/pos/type " + gps_pos_type + "\n")
            settings_file.write("/gps/pos/shape " + gps_pos_shape + "\n")
            settings_file.write("/gps/pos/centre " + str(gps_pos_centre_cm[0]) + " " + str(gps_pos_centre_cm[1]) + " " + str(gps_pos_centre_cm[2]) + " cm\n")
            settings_file.write("/gps/pos/radius " + str(gps_pos_radius_cm) + " cm\n")        
        
            
            #settings_file.write("/gps/position " + str(gps_position_cm[0]) + " " + str(gps_position_cm[1]) + " " + str(gps_position_cm[2]) + " cm\n")
            settings_file.write("/gps/ang/type " + gps_ang_type + "\n")
            settings_file.write("/gps/ang/mintheta " + str(gps_ang_mintheta_deg) + " deg\n")
            settings_file.write("/gps/ang/maxtheta " + str(gps_ang_maxtheta_deg) + " deg\n")
            
            
            NameOfFileWithPath = os.path.join(out_DST_paths[index], gps_particle[i])
            macro_file.write("/NameOfFile/NameOfFile " + NameOfFileWithPath + "\n")
            macro_file.write("/gps/particle " + gps_particle[i] + "\n")
            macro_file.write("/gps/ene/type " + gps_ene_type + "\n")
            macro_file.write("/gps/ene/gradient " + str(gps_ene_gradient) + "\n")
            macro_file.write("/gps/ene/intercept " + str(gps_ene_intercept) + "\n")
            macro_file.write("/gps/ene/min " + str(gps_ene_min[i]) + " MeV\n")
            macro_file.write("/gps/ene/max " + str(gps_ene_max[i]) + " MeV\n")
            
            settings_file.write("/gps/particle " + gps_particle[i] + "\n")
            settings_file.write("/gps/ene/type " + gps_ene_type + "\n")
            settings_file.write("/gps/ene/gradient " + str(gps_ene_gradient) + "\n")
            settings_file.write("/gps/ene/intercept " + str(gps_ene_intercept) + "\n")
            settings_file.write("/gps/ene/min " + str(gps_ene_min[i]) + " MeV\n")
            settings_file.write("/gps/ene/max " + str(gps_ene_max[i]) + " MeV\n")
            
            
            if SaveRandomSeed:
                macro_file.write("/random/setSavingFlag 1\n")
                settings_file.write("/random/setSavingFlag 1\n")
                #macro_file.write("/random/saveEachEventFlag 1\n")
                macro_file.write("/random/setDirectoryName " + os.path.join(out_Text_paths[index], "rnd_seed_"+gps_particle[i]) + "\n")
                #settings_file.write("/random/setSavingFlag 1\n")
                #settings_file.write("/random/setDirectoryName " + os.path.join(out_Text_paths[index], "rnd_seed_"+gps_particle[i]) + "\n")
                #settings_file.write("/random/resetEngineFromEachEvent 1\n")
            macro_file.write("/run/beamOn " + str(int(run_beamOn)) + "\n")
            settings_file.close()

        macro_file.close() 
        shutil.copy(os.path.join(start_macro_path, "macro_"+str(index)+".mac"), out_Geant_macros_path)

    # Define the FileNames within the DST folder
    for i, dst_dir in enumerate(out_DST_paths):
        fname = open(os.path.join(dst_dir, "FileNames.txt"), "w")
        fenergy = open(os.path.join(dst_dir, "EnergyGen.txt"), "w")
        for j, particle in enumerate(gps_particle):
            fname.write(particle + "_t0.root\n")
            fenergy.write(str(gps_ene_min[j]) + " " + str(gps_ene_max[j]) + "\n")
        fname.close()
        fenergy.close()

    os.chdir(HomeDirectorySimulation)
    os.system("rm -rf build")
    os.system("mkdir build")
    os.chdir("build")
    os.system("cmake ..")
    os.system("make -j4")

    # Open a file in the out_GDML_path directory
    
    report_file = open(os.path.join(RunDir_global, "report.txt"), "w")

    for index, file in enumerate(os.listdir(out_GDML_path)):
        os.system("./gdml_det " + os.path.join(out_GDML_path, file) + " " + os.path.join(out_Geant_macros_path, "macro_"+str(index)+".mac"))
        report_file.write("GDML file " + str(index) + ' : ' + file + "\n")
        os.chdir(HomeDirectorySimulation)   
        # Move folder OutputText to the right place
        for textfile in os.listdir(os.path.join(HomeDirectorySimulation, "OutputText")):
            shutil.copy(os.path.join(HomeDirectorySimulation, "OutputText",textfile), out_Text_paths[index])
        os.chdir("build")
    os.chdir(HomeDirectorySimulation)
    report_file.close()



if __name__ == "__main__":
    Geant4Simulation()




