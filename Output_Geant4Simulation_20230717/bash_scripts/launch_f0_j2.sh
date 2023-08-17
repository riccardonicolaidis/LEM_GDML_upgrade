#!/bin/bash
rm -rf /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j2
mkdir /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j2
cd /data1/home/rnicolai/LEM_GDML_upgrade/build_f0_j2
cmake .. 
make -j4 
./gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/GDML_src/LEM_Ibrido_modificaCono-tessellated-worldVOL_Parsed.gdml /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/Geant_macros/macro_f0_j2.mac /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/steering_files/steering_f0_j2.txt
