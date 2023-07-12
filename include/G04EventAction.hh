#ifndef EVENT_HH
#define EVENT_HH


#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


#include "G04RunAction.hh"
#include "G04HitClass.hh"
#include "G04DetectorConstruction.hh"



class G04EventAction : public G4UserEventAction
{
public:
        G04EventAction(G4int NumberSD, const G04DetectorConstruction *detector, G04PrimaryGeneratorAction *generator);
        ~G04EventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);

private:
        const G4int fNumberSD;
        const G04DetectorConstruction *fDetector;
        G04PrimaryGeneratorAction *fPrimary;
        const G4GDMLParser &fParser;

        G4double NTupleData[1000];
        const G4int StartingNTuple = 7;

};




#endif