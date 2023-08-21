cd 
ls
cd /data1/home/rnicolai/
source .bashrc
cd LEM_GDML_upgrade/PythonMacros/
python3.6 -i ../OUT/Output_Geant4Simulation_20230814/ -R
python3.6 AnalysisPID.py -i ../OUT/Output_Geant4Simulation_20230814/ -R
pip install matplotlib --user
pip3 install matplotlib --user
python3.6 AnalysisPID.py -i ../OUT/Output_Geant4Simulation_20230814/ -R
pip3 install pandas --user
python3.6 AnalysisPID.py -i ../OUT/Output_Geant4Simulation_20230814/ -R
chmod 777 -R ../OUT
python3.6 AnalysisPID.py -i ../OUT/Output_Geant4Simulation_20230814/ -R
kill %1
exit
