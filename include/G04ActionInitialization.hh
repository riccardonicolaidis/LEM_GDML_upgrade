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
/// \file G04ActionInitialization.hh
/// \brief Definition of the G04ActionInitialization class

#ifndef ACTION_HH
#define ACTION_HH 1

#include "G4VUserActionInitialization.hh"



#include "G04PrimaryGeneratorAction.hh"
#include "G04RunAction.hh"
#include "G04EventAction.hh"
#include "G04DetectorConstruction.hh"
#include "G04TrackingAction.hh"

/// Action initialization class.

class G04ActionInitialization : public G4VUserActionInitialization
{
  public:
    G04ActionInitialization( G04DetectorConstruction* detConstruction);
    virtual ~G04ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;


  private:
    G04DetectorConstruction* fDetConstruction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
