#! /bin/bash
cd /data1/home/rnicolai
cd /data1/home/rnicolai/LEM_GDML_upgrade
rm -rf /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j3
mkdir /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j3
cd /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j3
cmake .. 
make -j4 
chmod -R 777 /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j3
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
./gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230802_test/GDML_src/LEM_Ibrido_modificaCono-tessellated-worldVOL_Parsed.gdml /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230802_test/Geant_macros/macro_f0_j3.mac /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230802_test/steering_files/steering_f0_j3.txt
