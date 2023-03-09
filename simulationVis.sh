cd ./build
cmake ..
make
rm -f ../OutputText/g04.out
time ./gdml_det LowEnergyVolume_WorldVolume.gdml
cd ..