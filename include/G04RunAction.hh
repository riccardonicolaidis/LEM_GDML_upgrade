#ifndef RUN_HH
#define RUN_HH


#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4GDMLParser.hh"


#include "G04DetectorConstruction.hh"
#include "G04PrimaryGeneratorAction.hh"
#include "G4AnalysisManager.hh"


class G04DetectorConstruction;
class G04PrimaryGeneratorAction;

class G04RunAction : public G4UserRunAction
{
public:
    G04RunAction(G04DetectorConstruction*, G04PrimaryGeneratorAction*);
    ~G04RunAction();

    virtual void BeginOfRunAction(const G4Run* );
    virtual void EndOfRunAction(const G4Run* );

private:
    G04DetectorConstruction *fDetector;
    G04PrimaryGeneratorAction *fPrimary;
    G4GenericMessenger *fMessenger;
    G4String TotalFileName;
    G4String TotalFileNameFinal;

    const G4GDMLParser &fParser;

};

#endif