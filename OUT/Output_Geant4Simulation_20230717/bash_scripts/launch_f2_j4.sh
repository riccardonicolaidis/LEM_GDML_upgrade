#!/bin/bash
rm -rf /data1/home/rnicolai/LEM_GDML_upgrade/build_f2_j4
mkdir /data1/home/rnicolai/LEM_GDML_upgrade/build_f2_j4
cd /data1/home/rnicolai/LEM_GDML_upgrade/build_f2_j4
cmake .. 
make -j4 
./gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/GDML_src/LEM_V6_GAGG_calo-GDML-worldVOL_Parsed.gdml /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/Geant_macros/macro_f2_j4.mac /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230717/steering_files/steering_f2_j4.txt
