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
/// \file persistency/gdml/G04/include/G04SensitiveDetector.hh
/// \brief Definition of the G04SensitiveDetector class
//
//
//

#ifndef SENSITIVE_DETECTOR_HH
#define SENSITIVE_DETECTOR_HH 1



#include <vector>

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"


#include "G04HitClass.hh"
#include "G4AnalysisManager.hh"



class G4Step;

/// Sensitive detector to be attached to the GDML geometry

class G04SensitiveDetector : public G4VSensitiveDetector
{
  public:
      G04SensitiveDetector(const G4String&, const G4String&);
     ~G04SensitiveDetector();

      virtual void Initialize(G4HCofThisEvent *hitCollection);
      virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);
      virtual void EndOfEvent(G4HCofThisEvent *hitCollection);

  private:

  HitsCollection *fHitsCollection;

};

#endif

