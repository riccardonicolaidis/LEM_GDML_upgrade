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
    
    
    IsTest = False
    RunName = "Geant4Simulation_Isotopes"
    
    
    Date = time.strftime("%Y%m%d")
    print("RunName: ", RunName)
    print("Date: ", Date)


    N_ev = 500000

    # Run directory for the run
    if IsTest:
        RunDir = "Output_" + RunName + "_" + Date + "_test"
    else:
        RunDir = "Output_" + RunName + "_" + Date


    # ######################################################## #
    #                      GEANT4 SETTINGS                     #
    # ######################################################## #

    # gps_particle = ["e-", "proton", "alpha", "triton", "deuteron", "He3"]
    # gps_ene_min  = [0.08, 1       , 10,      1 ,       1,          10]
    # gps_ene_max  = [11  , 100     , 400,     100,      100,        10]

    gps_particle = ["e-", "proton", "alpha"]
    gps_ene_min  = [0.08, 1       , 10]
    gps_ene_max  = [13  , 130     , 400]


    gps_ene_type = "Lin"
    gps_ene_gradient = 0
    gps_ene_intercept = 1
    gps_pos_type = "Plane"
    gps_pos_shape = "Circle"
    gps_pos_centre_cm = [0, 0, 10]
    
    if IsTest:
        gps_pos_radius_cm = 0.01
    else:
        gps_pos_radius_cm = 10
    gps_ang_type = "iso"
    gps_ang_mintheta_deg = 0
    
    if IsTest:
        gps_ang_maxtheta_deg = 0.1
    else:
        gps_ang_maxtheta_deg = 90
    
    run_beamOn = N_ev


    

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
    
    
    
    
    # Compiling the G4 executable
    if os.path.exists(os.path.join(HomeDirectorySimulation, "build")):
        shutil.rmtree(os.path.join(HomeDirectorySimulation, "build"))
    os.makedirs(os.path.join(HomeDirectorySimulation, "build"))
    os.chdir(os.path.join(HomeDirectorySimulation, "build"))
    os.system("cmake ../")
    os.system("make -j10")
    os.chdir(HomeDirectorySimulation)
    
    ExecutablePath = os.path.join(HomeDirectorySimulation, "build", "gdml_det")
    
    # ######################################################## #
    #                       PROJECT PATHS                      #
    # ######################################################## #
    
    proj_paths = {
        "GDML": os.path.join(HomeDirectorySimulation, "GDML_ForSimulation"),
        "macro": os.path.join(HomeDirectorySimulation, "macro"),
        "PythonMacros": os.path.join(HomeDirectorySimulation, "PythonMacros"),
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
    os.makedirs("steering_files")
    os.makedirs("log")
    os.makedirs("Progress_monitoring")
    
    out_paths = {
        "GDML": os.path.join(os.getcwd(), "GDML_src"),
        "Text_output": os.path.join(os.getcwd(), "Text_output"),
        "DST": os.path.join(os.getcwd(), "DST"),
        "Geant_macros": os.path.join(os.getcwd(), "Geant_macros"),
        "log": os.path.join(os.getcwd(), "log"),
        "steering_files": os.path.join(os.getcwd(), "steering_files"),
        "Progress_monitoring": os.path.join(os.getcwd(), "Progress_monitoring")
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
        macro_file_name = os.path.join(out_paths["Geant_macros"], "macro_f"+str(index)+"_ju0"+".mac")
        steering_file_name = os.path.join(out_paths["steering_files"], "steering_f"+str(index)+"_ju0"+".txt")
        bash_script_name = os.path.join(out_paths["bash_scripts"], "launch_f"+str(index)+"_ju0"+".sh")
    
    
        report_file.write("File "+ str(index))
        report_file.write(file + "\n")
        report_file.write("Macro file: " + macro_file_name + "\n")
        report_file.write("Steering file: " + steering_file_name + "\n")
    
        macro_file = open(macro_file_name, "w")
        steering_file = open(steering_file_name, "w")
        steering_file.write("OUT_TEXT " + os.path.join(out_paths["Text_output"],"GDML_file_"+str(index),"OutText_f"+str(index)+"_ju0") + "\n")
        steering_file.write("OUT_MONITORING_FLD " + os.path.join(out_paths["Progress_monitoring"]) + "\n")
        steering_file.write("NJOB " + str(0) + "\n")
        steering_file.write("NFILE " + str(index) + "\n")
        steering_file.write("UPDATE_FREQUENCY " + str(10000) + "\n")
        steering_file.close()
        
        
        macro_file.write("# Date of creation: "+ str(Date) +" UNIX Time :"+ str(time.time()) +"\n")
        macro_file.write("# File GDML : "+ str(file) +"\n\n\n")
        macro_file.write("/control/verbose 0\n")
        macro_file.write("/run/verbose 1\n")
        macro_file.write("/event/verbose 0\n\n\n")
        macro_file.write("/run/initialize\n\n\n")
        macro_file.write("/random/setSeeds " + str(0) + " " + str(0) + "\n\n\n")
        
        macro_file.write("/gps/pos/type " + gps_pos_type + "\n")
        macro_file.write("/gps/pos/shape " + gps_pos_shape + "\n")
        macro_file.write("/gps/pos/centre " + str(gps_pos_centre_cm[0]) + " " + str(gps_pos_centre_cm[1]) + " " + str(gps_pos_centre_cm[2]) + " cm\n")
        macro_file.write("/gps/pos/radius " + str(gps_pos_radius_cm) + " cm\n")        
        macro_file.write("/gps/ang/type " + gps_ang_type + "\n")
        macro_file.write("/gps/ang/mintheta " + str(gps_ang_mintheta_deg) + " deg\n")
        macro_file.write("/gps/ang/maxtheta " + str(gps_ang_maxtheta_deg) + " deg\n\n\n\n")
        
        for i in range(len(gps_particle)):
            NameOfFileWithPath = os.path.join(out_DST_paths[index], gps_particle[i] +"_ju0")
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
        
        
        bash_script = open(bash_script_name, "w")
        bash_script.write("#! /bin/bash\n")
        bash_script.write('cd /data1/home/rnicolai\n')
        bash_script.write('cd /data1/home/rnicolai/LEM_GDML_upgrade\n')
        
        
        # Write bash code in the file for trying to launch the executable untill the executions is successful
        
        TextToWrite = '''
MAX_ATTEMPTS=10
ATTEMPT=1
while [ $ATTEMPT -le $MAX_ATTEMPTS ]; do
    # Esegui il comando qui
'''
        TextToWrite += ExecutablePath + " " + os.path.join(out_paths["GDML"], file) + " " + macro_file_name + " " + steering_file_name + "\n\n"
        TextToWrite += '''    if [ $? -eq 0 ]; then
        echo "Comando eseguito correttamente alla tentativo $ATTEMPT"
        break
    else
        echo "Errore nell'esecuzione del comando alla tentativo $ATTEMPT. Riprovo..."
        ATTEMPT=$((ATTEMPT + 1))
'''
        TextToWrite += "head -n 10 " + os.path.join(out_paths["GDML"], file) + "\n"
        TextToWrite += '''
        
        numero_casuale=$((RANDOM%8 + 3))
        sleep $numero_casuale  # Attendi un po' prima del prossimo tentativo
    fi
done

if [ $ATTEMPT -gt $MAX_ATTEMPTS ]; then
    echo "Il comando non è stato eseguito correttamente dopo $MAX_ATTEMPTS tentativi."
fi

'''            
        bash_script.write(TextToWrite)
        bash_script.close()
    report_file.close()        
    # Change permission chmod 777 to Output Directory
    os.system("chmod -R 777 " + proj_paths["RunDir"])
    print("Changed permission to 777 for directory: \n"+ proj_paths["RunDir"])
    os.system("sleep 1")

    BashScripts = []
    for files in os.listdir(out_paths["bash_scripts"]):
        if files.endswith(".sh"):
            BashScripts.append(os.path.join(out_paths["bash_scripts"],files))
            print("Added file: ", files)
            print("Path: ", os.path.join(out_paths["condor_scripts"],files))

    for files in BashScripts:
        os.system("bash " + files)
        #sleep(0.1)
        #os.system("sleep 0.001")    




if __name__ == "__main__":
    Geant4Simulation()



