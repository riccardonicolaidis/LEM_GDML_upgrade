#ifndef TRACKING_HH
#define TRACKING_HH

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4HadronicProcessType.hh"
#include "G4SystemOfUnits.hh"


#include "G4AnalysisManager.hh"

class G04DetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G04TrackingAction : public G4UserTrackingAction {

  public:  
    G04TrackingAction(G04DetectorConstruction*);
   ~G04TrackingAction();
   
    virtual void  PreUserTrackingAction(const G4Track*);   
    virtual void PostUserTrackingAction(const G4Track*);
    
  private:
    G04DetectorConstruction* fDetector;
};




#endif