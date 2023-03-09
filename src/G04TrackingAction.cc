#include "G04TrackingAction.hh"

G04TrackingAction::G04TrackingAction(G04DetectorConstruction* det)
:G4UserTrackingAction(), fDetector(det)
{}

G04TrackingAction::~G04TrackingAction()
{}


void G04TrackingAction::PreUserTrackingAction(const G4Track* track)
{}


void G04TrackingAction::PostUserTrackingAction(const G4Track* )
{}