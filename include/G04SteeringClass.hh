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
    G4String OutputMonitoringFolder;
    int UpdateFrequency;
    int JobNumber;
    int FileNumber;


    const G4String OutputTextFolder_KEY       = "OUT_TEXT";
    const G4String OutputMonitoringFolder_KEY = "OUT_MONITORING_FLD";
    const G4String UpdateFrequency_KEY        = "UPDATE_FREQUENCY";
    const G4String JobNumber_KEY              = "NJOB";
    const G4String FileNumber_KEY             = "NFILE";

public:
    G04SteeringClass();
    ~G04SteeringClass();

    void UploadSteeringFile(G4String filename);

    void SetOutputTextFolder      (G4String folder) { OutputTextFolder = folder; };
    void SetOutputMonitoringFolder(G4String folder) { OutputMonitoringFolder = folder; };
    void SetUpdateFrequency       (int freq)        { UpdateFrequency = freq; };
    void SetJobNumber             (int job)         { JobNumber = job; };
    void SetFileNumber            (int file)        { FileNumber = file; };
    
    G4String GetOutputTextFolder()       const { return OutputTextFolder; };
    G4String GetOutputMonitoringFolder() const { return OutputMonitoringFolder; };
    int      GetUpdateFrequency()        const { return UpdateFrequency; };
    int      GetJobNumber()              const { return JobNumber; };
    int      GetFileNumber()             const { return FileNumber; };
};

#endif





