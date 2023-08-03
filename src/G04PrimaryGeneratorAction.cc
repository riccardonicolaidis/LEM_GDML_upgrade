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
/// \file persistency/gdml/G04/src/G04PrimaryGeneratorAction.cc
/// \brief Implementation of the G04PrimaryGeneratorAction class
//
//
//
//

#include "G04PrimaryGeneratorAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G04PrimaryGeneratorAction::G04PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(), 
 fParticleTable(0),
 fGeneralParticleSource(0),
 fMessenger(0)
{

  
  fGeneralParticleSource = new G4GeneralParticleSource();

  fParticleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  fGeneralParticleSource->SetParticleDefinition(fParticleTable->FindParticle(particleName="geantino"));
  ParticleNumber = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G04PrimaryGeneratorAction::~G04PrimaryGeneratorAction()
{
  delete fGeneralParticleSource;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G04PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{


  G4AnalysisManager* man = G4AnalysisManager::Instance();

  fGeneralParticleSource->GeneratePrimaryVertex(anEvent);
  G4ThreeVector pos = fGeneralParticleSource->GetParticlePosition();
  G4ThreeVector momentum = fGeneralParticleSource -> GetParticleMomentumDirection();

  man -> FillNtupleDColumn(0,0, fGeneralParticleSource->GetParticleEnergy());
  MCEnergy = fGeneralParticleSource->GetParticleEnergy();
  man -> FillNtupleDColumn(0,1, pos.getX());
  man -> FillNtupleDColumn(0,2, pos.getY());
  man -> FillNtupleDColumn(0,3, pos.getZ());
  man -> FillNtupleDColumn(0,4, momentum.getX());
  man -> FillNtupleDColumn(0,5, momentum.getY());
  man -> FillNtupleDColumn(0,6, momentum.getZ());






   



  ++ParticleNumber;
  if(ParticleNumber % 1000 == 0)
  {
    G4cout << "Event N: " << ParticleNumber << G4endl;
  }
}
