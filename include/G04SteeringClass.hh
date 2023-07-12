#ifndef G04_STEERING_CLASS_HH
#define G04_STEERING_CLASS_HH


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"


class G04SteeringClass
{
private:
    G4String OutputTextFolder;

public:
    G04SteeringClass();
    ~G04SteeringClass();

    void SetOutputTextFolder(G4String folder) { OutputTextFolder = folder; };
    G4String GetOutputTextFolder() { return OutputTextFolder; };

};

#endif





