#! /bin/bash
cd /data1/home/rnicolai
cd /data1/home/rnicolai/LEM_GDML_upgrade
rm -rf /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j1
mkdir /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j1
cd /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j1
cmake .. 
make -j10 
chmod -R 777 /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j1
echo "Version of Geant"
geant4-config --version
echo "Prefix"
geant4-config --prefix
echo  
echo  
echo  
echo  
echo  
echo  
echo  
echo  
echo  
echo  
./gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230804_test/GDML_src/LEM_Ibrido_modificaCono-tessellated-worldVOL_Parsed_DummyMaterials.gdml /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230804_test/Geant_macros/macro_f1_j1.mac /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230804_test/steering_files/steering_f1_j1.txt
