import os
import sys
import math
import shutil
import subprocess




def main():
    GDML_Files = []
    Macro_Files = []
    GDML_Files.append("/home/riccardo/Documenti/GeantProjects/LEM_GDML_upgrade/GdmlFiles/Definitivi_spero/Versione_1_Mirion/LEM_Mirion_Tessellated-worldVOL_Parsed.gdml")
    GDML_Files.append("/home/riccardo/Documenti/GeantProjects/LEM_GDML_upgrade/GdmlFiles/Definitivi_spero/Versione_2_Ibrida/LEM_Ibrido_mirion_Ametek_Tessellated-worldVOL_Parsed.gdml")


    os.chdir("../")
    os.system("rm -rf build")
    os.system("mkdir build")
    os.chdir("build")
    os.system("cmake ..")
    os.system("make -j4")
    os.chdir("../")

    os.chdir("macro")

    i = 0
    for gdml in GDML_Files:
        # Crea file nuovo se non esiste
        if not os.path.exists("Prova_"+str(i)+".mac"):
            os.system("touch Prova_"+str(i)+".mac")
        else:
            os.system("rm Prova_"+str(i)+".mac")
            os.system("touch Prova_"+str(i)+".mac")

        f1 = open("Prova_"+str(i)+".mac", "w")
        Macro_Files.append("Prova_"+str(i)+".mac")
        f2 = open("g04.mac", "r")

        for line in f2:
            if "/NameOfFile/NameOfFile" in line:
                f1.write("/NameOfFile/NameOfFile Prova_"+str(i)+ " \n")
            else:
                f1.write(line)
        f1.close()
        f2.close()
        i = i + 1

    os.chdir("../build")            
    os.system("cmake ..")
    os.system("make -j4")

    for i in range(len(Macro_Files)):
        os.system("./gdml_det "+ GDML_Files[i] + " " +Macro_Files[i])

    print(Macro_Files)
    print(GDML_Files)

if __name__ == "__main__":
    main()
