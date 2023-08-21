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
#include "G04SteeringClass.hh"

using namespace std;

class G04EventAction : public G4UserEventAction
{
public:
        G04EventAction(const G04DetectorConstruction   *detector, 
                       const G04PrimaryGeneratorAction *generator,
                       const G04SteeringClass          *steering,
                       const G04RunAction              *runaction
                       );
        ~G04EventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);

private:
        const G4int                      fNumberSD;
        const G04DetectorConstruction   *fDetector;
        const G04RunAction              *fRunAction;
        const G04PrimaryGeneratorAction *fPrimary;
        const G4GDMLParser              &fParser;
        G4String                         OutputTextFolder;
        const G04SteeringClass          *fSteering;
        G4double                         NTupleData[1000];
        const G4int                      StartingNTuple;

        G4int    NumberPairSilicon = 5;
        G4double vEd_Silicon_Thin[10];
        G4double vEd_Silicon_Thick[10];
        G4double Ed_Silicon_Thin;
        G4double Ed_Silicon_Thick;
        G4double Ed_Veto;
        G4double Ed_Calo;


};




#endif