import os
import sys
import re
import argparse



GAGGdefinition = '''

<materials>
  <element name="Gd" formula="Gd" Z="64.">
    <atom value="157.25"/>
      </element>
  <element name="Al" formula="Al" Z="13.">
    <atom value="26.981539"/>
      </element>
  <element name="Ga" formula="Ga" Z="31.">
    <atom value="69.723"/>
      </element>
  <element name="O" formula="O" Z="8.">
    <atom value="16.00"/>
      </element>


  <material name="GAGG" formula="Gd3Al2Ga3O12">
    <D value="6.63" unit="g/cm3"/>  
    <composite n="3" ref="Gd"/>
    <composite n="2" ref="Al"/>
    <composite n="3" ref="Ga"/>
    <composite n="12" ref="O"/>
  </material>
  
</materials>

'''


def Build_MaterialRefLine(Material):
    lineMat = "\t\t\t<materialref ref=\""+ Material +"\"/>\n"
    return lineMat

def Build_VolumeNameLine(VolumeName):
    lineName = "\t\t\t<volume name=\""+ VolumeName +"\">\n"
    return lineName

def Build_VolumeNameRefLine(VolumeNameRef):
    lineNameRef = "\t\t\t<volumeref ref=\""+ VolumeNameRef +"\"/>\n"
    return lineNameRef

def RGBA_to_Hex(RGBA_array):
    HexString = "#"
    if len(RGBA_array) == 4:
        for i in range(4):
            HexString += "{:02x}".format(int(RGBA_array[i]*255))
        return HexString
    else:
        return RGBA_to_Hex([1., 0., 0., 1.])

def Build_AuxiliaryLine_Color(RGBA_array):
    Color = RGBA_to_Hex(RGBA_array)
    lineCol = "\t\t\t<auxiliary auxtype=\"Color\" auxvalue=\""+ Color +"\"/>\n"
    return lineCol


def Build_AuxiliaryLine_SensDet():
    lineSensDet = "\t\t\t<auxiliary auxtype=\"SensDet\" auxvalue=\"Calorimeter\"/>\n"
    return lineSensDet



def GDML_Retrieve_Names(input_file, N_characters_to_remove):
    N_characters_to_remove = int(N_characters_to_remove)
    
    Keys_No_Mod = ["Assembly", "worldVOL"]
    
    RelativePath = os.path.dirname(input_file)
    FileName_GDML = os.path.basename(input_file)
    ActualPath = os.getcwd()
    os.chdir(RelativePath)
    AbsolutePath = os.getcwd()
    os.chdir(ActualPath)
    
    print("RelativePath: ", RelativePath)
    print("AbsolutePath: ", AbsolutePath)
    
    FileName_VolName = FileName_GDML.replace(".gdml", "_VolName.txt")
    FileName_VolNameRef = FileName_GDML.replace(".gdml", "_VolNameRef.txt")
    
    
    f_GDML = open(os.path.join(AbsolutePath,FileName_GDML), "r")
    
    VolumeName_List = []
    VolumeNameRef_List = []
    
    

    for line in f_GDML:
        if "volume name=" in line:
            VolumeName_List.append(line.split('"')[1])
        if "volumeref ref=" in line:
            VolumeNameRef_List.append(line.split('"')[1])
    
    
    f_GDML.close()
    
    maxLen_VolumeName = max([len(VolumeName) for VolumeName in VolumeName_List])
    maxLen_VolumeNameRef = max([len(VolumeNameRef) for VolumeNameRef in VolumeNameRef_List])
    
    
    # Check if files already exist
    if os.path.isfile(os.path.join(AbsolutePath,FileName_VolName)) and os.path.isfile(os.path.join(AbsolutePath,FileName_VolNameRef)):
        print("Files already exist")
        Cancel = ''
        while Cancel not in ['y','n']:
            Cancel = input("Do you want to overwrite them? [y/n] ")
            Cancel = Cancel.lower()
        if Cancel == 'n':
            print("Exiting...")
            sys.exit()
        elif Cancel == 'y':
            print("Overwriting files...")
    
    f_VolumeName = open(os.path.join(AbsolutePath,FileName_VolName), "w")
    f_VolumeNameRef = open(os.path.join(AbsolutePath,FileName_VolNameRef), "w")
    
    for VolumeName in VolumeName_List:
        VolumeName_2 = VolumeName
        
        SkipMod = False
        for Key_No_Mod in Keys_No_Mod:
            if Key_No_Mod in VolumeName:
                SkipMod = True
                break        
        
        if not SkipMod:
            if N_characters_to_remove > 0:
                VolumeName_2 = VolumeName_2[:-N_characters_to_remove]
            
            VolumeName_2 = VolumeName_2.replace("__","_")
            # If ends with _ remove it
            # This is a FreeCAD issue
            if VolumeName_2[-1] == "_":
                VolumeName_2 = VolumeName_2[:-1]
                
            if VolumeName_2.endswith("Thin") or VolumeName_2.endswith("Thick"):
                VolumeName_2 = VolumeName_2 + "_0"
    
        f_VolumeName.write(VolumeName + " "*(maxLen_VolumeName + 5 - len(VolumeName)) + VolumeName_2 + "\n")
    
    for VolumeNameRef in VolumeNameRef_List:
        VolumeNameRef_2 = VolumeNameRef
        SkipMod = False
        
        for Key_No_Mod in Keys_No_Mod:
            if Key_No_Mod in VolumeNameRef:
                SkipMod = True
                break
        
        if not SkipMod:
            if N_characters_to_remove > 0:
                VolumeNameRef_2 = VolumeNameRef_2[:-N_characters_to_remove]
            VolumeNameRef_2 = VolumeNameRef_2.replace("__","_")
            # If ends with _ remove it
            # This is a FreeCAD issue
            if VolumeNameRef_2[-1] == "_":
                VolumeNameRef_2 = VolumeNameRef_2[:-1]
            
            if VolumeNameRef_2.endswith("Thin") or VolumeNameRef_2.endswith("Thick"):
                VolumeNameRef_2 = VolumeNameRef_2 + "_0"
        
        f_VolumeNameRef.write(VolumeNameRef + " "*(maxLen_VolumeNameRef + 5 - len(VolumeNameRef)) + VolumeNameRef_2 + "\n")




def Get_Name_Of_File(input_file):
    input_file_check = False
    
    # Check if input file is a file or is a directory
    if os.path.isfile(input_file):
        input_file_check = True
    else:
        input_file_check = False
        Directory = input_file
    
    if not input_file_check:
        # Search a .gdml file without the word Parsed inside
        for file in os.listdir(Directory):
            if file.endswith(".gdml") and "Parsed" not in file:
                input_file = os.path.join(Directory, file)
                input_file_check = True
                break
        
        answer = ''
        while answer not in ['y','n']:
            answer = input("The found file is: " + input_file + "\n Is it ok? [y/n]  ").lower()

        if answer == 'n':
            # List all the .gdml files with a number menu
            print("List of .gdml files:")
            ListOfFiles = []
            for file in os.listdir(Directory):
                if file.endswith(".gdml"):
                    ListOfFiles.append(file)
            for i in range(len(ListOfFiles)):
                print("[" + str(i) + "]  --> " + ListOfFiles[i])

            # Ask for the number
            answer = ''
            while not (answer.isdigit() and  int(answer) < len(ListOfFiles)):
                answer = input("Select the number of the file: ")
            
            input_file = os.path.join(Directory, ListOfFiles[int(answer)])  
            
        
    return input_file




def GDML_Parser(input_file, dummy_materials, all_sensitive_det):
    
    
    
    
    
    
    RelativePath = os.path.dirname(input_file)
    FileName_GDML = os.path.basename(input_file)
    ActualPath = os.getcwd()
    os.chdir(RelativePath)
    AbsolutePath = os.getcwd()
    os.chdir(ActualPath)
    
    print("RelativePath: ", RelativePath)
    print("AbsolutePath: ", AbsolutePath)
    
    # Read Names from GDML File
    FileName_VolName = FileName_GDML.replace(".gdml", "_VolName.txt")
    FileName_VolNameRef = FileName_GDML.replace(".gdml", "_VolNameRef.txt")
    
    VolumeName_List_gdml = []
    VolumeNameRef_List_gdml = []
    
    f_GDML = open(os.path.join(AbsolutePath,FileName_GDML), "r")
    
    for line in f_GDML:
        if "volume name=" in line:
            VolumeName_List_gdml.append(line.split('"')[1])
        if "volumeref ref=" in line:
            VolumeNameRef_List_gdml.append(line.split('"')[1])
    f_GDML.close()

    # Read Names from VolName File
    VolumeName_List_txt = []
    VolumeNameRef_List_txt = []
    
    VolumeName_List_new = []
    VolumeNameRef_List_new = []
    
    f_VolumeName = open(os.path.join(AbsolutePath,FileName_VolName), "r")
    f_VolumeNameRef = open(os.path.join(AbsolutePath,FileName_VolNameRef), "r")
    
    for line in f_VolumeName:
        # How many spaces are there?
        n_spaces = sum([1 for char in line if char == ' '])
        line = line.replace(" "*n_spaces, " ").replace("\n","")
        print(line)
        line_split = line.split(" ")
        VolumeName_List_txt.append(line_split[0])
        VolumeName_List_new.append(line_split[1])
        print(line_split[0]+' '+line_split[1])
        
    for line in f_VolumeNameRef:
        # How many spaces are there?
        n_spaces = sum([1 for char in line if char == ' '])
        line = line.replace(" "*n_spaces, " ").replace("\n","")
        print(line)
        line_split = line.split(" ")
        VolumeNameRef_List_txt.append(line_split[0])
        VolumeNameRef_List_new.append(line_split[1])
        print(line_split[0]+' '+line_split[1])

    f_VolumeName.close()
    f_VolumeNameRef.close()
    
    # ######################################################## #
    #                          PARSING                         #
    # ######################################################## #
    
    
    MaterialCorrespondence = (("Silicon", "G4_Si", 1),
                              ("Al", "G4_Al", 0),
                              ("Bakelite", "G4_BAKELITE", 0),
                              ("Plastic", "G4_PLASTIC_SC_VINYLTOLUENE", 1))
    
    
    
    
    ColorCorrespondence = (("Calo",     (0.2, 0.2, 1., 1.)),
                           ("Silicon",  (1, 1, 1, 1.)),
                           ("Bakelite", (0.2, 1., 0.2, 0.8)),
                           ("Veto",     (1., 0.8, 0.6, 0.7)),
                           ("Screw",    (1., 1., 0., 1.)),
                           ("Cover",    (1., 0., 1., 0.7)),
                           ("Frame",    (0., 1, 1, 0.7)),
                           ("Al",       (0.5, 0.5, 0.5, 0.6)))
    
    
    f_GDML = open(os.path.join(AbsolutePath,FileName_GDML), "r")
    
    FileGDML_Parsed = os.path.join(AbsolutePath,FileName_GDML)
    if dummy_materials:
        FileGDML_Parsed = FileGDML_Parsed.replace(".gdml", "_B_DummyMaterials_Parsed.gdml")
    else:
        FileGDML_Parsed = FileGDML_Parsed.replace(".gdml", "_A_Parsed.gdml")   
    
    f_GDML_Parsed = open(os.path.join(AbsolutePath,FileGDML_Parsed), "w")
    
     
    SetSensDet = False
    for line in f_GDML:
        
        if "<materials/>" in line:
            print("Adding GAGG definition")
            f_GDML_Parsed.write(GAGGdefinition)
            f_GDML_Parsed.write("\n")
            #f_GDML_Parsed.write(line)        
        
        if "volume name=" in line:
            print("Volume Name")
            NameLen = 0
            for i in range(len(VolumeName_List_txt)):
                if VolumeName_List_txt[i] in line:
                    if len(VolumeName_List_txt[i]) > NameLen:
                        NameLen = len(VolumeName_List_txt[i])
                        lineName = Build_VolumeNameLine(VolumeName_List_new[i])
            print(lineName)
            print(VolumeName_List_new[i])
            f_GDML_Parsed.write(lineName)
            
        elif "volumeref ref=" in line:
            print("Volume Name Ref")
            NameLen = 0
            for i in range(len(VolumeNameRef_List_txt)):
                if VolumeNameRef_List_txt[i] in line:
                    if len(VolumeNameRef_List_txt[i]) > NameLen:
                        NameLen = len(VolumeNameRef_List_txt[i])
                        lineName = Build_VolumeNameRefLine(VolumeNameRef_List_new[i])
            print(VolumeNameRef_List_new[i])
            print(lineName)
            f_GDML_Parsed.write(lineName)
        elif "materialref" in line:
            if "LV_Calo" in lineName:
                SetSensDet = True
                if "GAGG" in FileName_GDML:
                    lineMat = Build_MaterialRefLine("GAGG")
                else:
                    lineMat = Build_MaterialRefLine("G4_PLASTIC_SC_VINYLTOLUENE")
            else:
                FoundCorrespondence = False
                for MatTuple in MaterialCorrespondence:
                    if MatTuple[0] in lineName:
                        SetSensDet = MatTuple[2]
                        lineMat = Build_MaterialRefLine(MatTuple[1])
                        FoundCorrespondence = True
                        break
                if not FoundCorrespondence:
                    lineMat = Build_MaterialRefLine("G4_Galactic")
                    
            if (not SetSensDet) and dummy_materials:
                lineMat = Build_MaterialRefLine("G4_Galactic")
                
            f_GDML_Parsed.write(lineMat)
        
        elif "auxiliary auxtype=\"Color\"" in line:
            for ColorTuple in ColorCorrespondence:
                if ColorTuple[0] in lineName:
                    lineCol = Build_AuxiliaryLine_Color(ColorTuple[1])
                    f_GDML_Parsed.write(lineCol)
                    break
                
            if SetSensDet or all_sensitive_det:
                lineSensDet = Build_AuxiliaryLine_SensDet()
                f_GDML_Parsed.write(lineSensDet)
                SetSensDet = False
        
        else:
            for i in range(len(VolumeName_List_txt)):
                if VolumeName_List_txt[i] in line:
                    #line = line.replace(VolumeName_List_txt[i], VolumeName_List_new[i])
                    break
            f_GDML_Parsed.write(line)
            
    f_GDML.close()
            
                    
    
    
    
    
    

if __name__ == "__main__":
    argparser = argparse.ArgumentParser( description = "GDML Parser utility")
    
    argparser.add_argument( "-i", "--input", help = "Input GDML file", required = False )
    argparser.add_argument( "-s", "--step-of-process", help = "Step of process: 0 or 1", required = False )
    argparser.add_argument( "-d", "--dummy-materials", help = "Dummy materials: If inserted all the Non-SensDet will be assigned to G4_Galactic", action="store_true", required = False)
    argparser.add_argument( "-a", "--all-sensitive", help = "All sensitive: If inserted all the volumes will be assigned to G4_Galactic", action="store_true", required = False)
    argparser.add_argument( "-r", "--remove-n-characters", help = "Remove N characters from the end of the VolumeName", required = False, default = 0)
    
    
    args = argparser.parse_args()
    
    
    input_file = Get_Name_Of_File(args.input)
    
    if args.step_of_process == "0":
        print("Step 0")
        GDML_Retrieve_Names(input_file, args.remove_n_characters)
    elif args.step_of_process == "1":
        print("Step 1")
        GDML_Parser(input_file, args.dummy_materials, args.all_sensitive)