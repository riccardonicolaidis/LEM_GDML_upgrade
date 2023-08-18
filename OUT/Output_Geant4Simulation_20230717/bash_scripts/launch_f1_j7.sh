#!/bin/bash
rm -rf /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j7
mkdir /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j7
cd /data1/home/rnicolai/LEM_GDML_upgrade/build_f1_j7
cmake .. 
make -j4 
./gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/GDML_src/LEM_V5b_Silici_allineati-GDML-worldVOL_Parsed.gdml /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/Geant_macros/macro_f1_j7.mac /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/steering_files/steering_f1_j7.txt
