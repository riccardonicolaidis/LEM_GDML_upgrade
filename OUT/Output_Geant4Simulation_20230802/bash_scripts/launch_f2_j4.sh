#!/bin/bash
cd /data1/home/rnicolai
source setenv_gcc6.26_cc7.sh
cd /data1/home/rnicolai/LEM_GDML_upgrade
pip3 install matplotlib --user
pip3 install numpy --user
pip3 install scipy --user
pip3 install pandas --user
pip3 install seaborn --user


rm -rf /data1/home/rnicolai/LEM_GDML_upgrade/build_f2_j4
mkdir /data1/home/rnicolai/LEM_GDML_upgrade/build_f2_j4
cd /data1/home/rnicolai/LEM_GDML_upgrade/build_f2_j4
cmake .. 
make -j4 
./gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230802/GDML_src/LEM_V6_GAGG_calo-GDML-worldVOL_Parsed.gdml /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230802/Geant_macros/macro_f2_j4.mac /data1/home/rnicolai/LEM_GDML_upgrade/Output_Geant4Simulation_20230802/steering_files/steering_f2_j4.txt
