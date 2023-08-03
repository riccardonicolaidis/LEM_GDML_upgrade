#! /bin/bash
# This script is used to submit a job to the condor queue

echo "Setting up environment"
echo "sourcing /cvmfs/sft.cern.ch//lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh"
echo
source /cvmfs/sft.cern.ch//lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh
echo "sourcing /data1/home/rnicolai/GEANT/geant4-11.1.0-install/bin/geant4.sh"
source /data1/home/rnicolai/GEANT/geant4-11.1.0-install/bin/geant4.sh
echo

# Installing pip3 packages
pip3.8 install matplotlib --user
pip3.8 install numpy --user
pip3.8 install scipy --user
pip3.8 install pandas --user
pip3.8 install sklearn --user


# source /cvmfs/sft.cern.ch/lcg/contrib/gcc/6.3/x86_64-centos7/setup.sh
# source /cvmfs/geant4.cern.ch/geant4/10.5/x86_64-centos7-gcc63-opt-MT/CMake-setup.sh
# export CXX=`which g++`
# export CC=`which gcc`


source /cvmfs/sft.cern.ch/lcg/views/LCG_geant4ext20220607/x86_64-centos7-gcc8-opt/setup.sh
