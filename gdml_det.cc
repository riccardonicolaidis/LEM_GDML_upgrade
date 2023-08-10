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
/// \file persistency/gdml/G04/gdml_det.cc
/// \brief Main program of the persistency/gdml/G04 example
//
//
//
//
// --------------------------------------------------------------
//      GEANT 4 - gdml_det
//
// --------------------------------------------------------------

#include <vector>
#include <string>
#include <sstream>
#include <fstream>


#include "G04ActionInitialization.hh"
#include "G04DetectorConstruction.hh"

#include "G4RunManagerFactory.hh"

#include "FTFP_BERT.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4GDMLParser.hh"

#include "G4SystemOfUnits.hh"
#include "G04SteeringClass.hh"



int main(int argc,char **argv)
{
   G4cout << G4endl;
   G4cout << "Usage: gdml_det <intput_gdml_file:mandatory>"
          << G4endl;
   G4cout << G4endl;

   if (argc<2)
   {
      G4cout << "Error! Mandatory input file is not specified!" << G4endl;
      G4cout << G4endl;
      return -1;
   }

   // Detect interactive mode (if only one argument) and define UI session
   //

  G04SteeringClass* steering = new G04SteeringClass();
   G4UIExecutive* ui = 0;
   if ( argc == 3 || argc == 2 ) {
     ui = new G4UIExecutive(argc, argv);
     if( argc == 3)
     {
        steering -> UploadSteeringFile(argv[2]);
     }
     else
     {
        steering -> UploadSteeringFile("../macro/default.steering");
     }
   }
   else
   {
      steering -> UploadSteeringFile(argv[3]);
   }

   G4GDMLParser parser;
   parser.Read(argv[1]);




   auto* runManager = G4RunManagerFactory::CreateRunManager();
   runManager->SetNumberOfThreads(1);

   G04DetectorConstruction* detector = new G04DetectorConstruction(parser, steering);


   runManager->SetUserInitialization(detector);
   runManager->SetUserInitialization(new FTFP_BERT);

   // User action initialization
   runManager->SetUserInitialization(new G04ActionInitialization(detector, steering));
   runManager->Initialize();

   // Initialize visualization
   G4VisManager* visManager = new G4VisExecutive;
   visManager->Initialize();

   // Get the pointer to the User Interface manager
   G4UImanager* UImanager = G4UImanager::GetUIpointer();

   // Process macro or start UI session
   if ( ! ui )   // batch mode
   {
     G4String command = "/control/execute ";
     G4String fileName = argv[2];
     UImanager->ApplyCommand(command+fileName);
   }
   else           // interactive mode
   {
     UImanager->ApplyCommand("/control/execute init_vis.mac");
     ui->SessionStart();
     delete ui;
   }

   delete visManager;
   delete runManager;
}
