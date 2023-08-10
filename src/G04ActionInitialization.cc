//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file G04ActionInitialization.cc
/// \brief Implementation of the G04ActionInitialization class

#include "G04ActionInitialization.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G04ActionInitialization::G04ActionInitialization(G04DetectorConstruction* detConstruction,
                                                 G04SteeringClass* steering)
 : G4VUserActionInitialization(), fDetConstruction(detConstruction), fSteeringClass(steering)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G04ActionInitialization::~G04ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G04ActionInitialization::BuildForMaster() const
{
  //  SetUserAction(new G04RunAction);
  //G04RunAction* runAction = new G04RunAction(fDetConstruction, 0);
  //SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G04ActionInitialization::Build() const
{

  G04PrimaryGeneratorAction* primary = new G04PrimaryGeneratorAction(fSteeringClass);
  SetUserAction(primary);

  G04RunAction* runAction = new G04RunAction(fDetConstruction, primary, fSteeringClass);
  SetUserAction(runAction);

  G04EventAction* eventAction = new G04EventAction(fDetConstruction->GetNumberOfSensitiveDetectors(), fDetConstruction, primary, fSteeringClass);
  SetUserAction(eventAction);

  G04TrackingAction* trackingAction = new G04TrackingAction(fDetConstruction);
  SetUserAction(trackingAction);

  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
