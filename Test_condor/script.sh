#!/bin/bash
echo "##############################################"
echo "##############################################"
echo "..ooOO00OOoo.. Eseguo il file script.sh ..ooOO00OOoo.."
echo "##############################################"
echo "##############################################"


cd /data1/home/rnicolai
source setenv_gcc6.26_cc7.sh
cd /data1/home/rnicolai/LEM_GDML_upgrade


pip3 install matplotlib --user
pip3 install numpy --user
pip3 install scipy --user
pip3 install pandas --user
pip3 install sklearn --user

python3.6 /data1/home/rnicolai/LEM_GDML_upgrade/Test_condor/test_py.py