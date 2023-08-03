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
/// \file G04DetectorConstruction.cc
/// \brief Implementation of the G04DetectorConstruction class
 

#include "G04DetectorConstruction.hh"
#include "G4AffineTransform.hh"




using namespace std;
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G04DetectorConstruction::G04DetectorConstruction(const G4GDMLParser& parser,G04SteeringClass* steering)
 : G4VUserDetectorConstruction(),
   fParser(parser),
   fSteering(steering)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* G04DetectorConstruction::Construct()
{

  // Retrieve the world volume from the parsed structure
  G4VPhysicalVolume *worldVolume = fParser.GetWorldVolume();
  return worldVolume;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G04DetectorConstruction::ConstructSDandField()
{
  //------------------------------------------------ 
  // Sensitive detectors
  //------------------------------------------------ 

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  

  G4String SiliconDetector = "Silicon";
  G4String PlasticScintillatorDetector = "PlasticScintillator";

   
  ///////////////////////////////////////////////////////////////////////
  //
  // Example how to retrieve Auxiliary Information for sensitive detector
  //

  // Define a text file name to store the auxiliary information
  // If the file exists, it will be overwritten


  OutputTextFolder = fSteering -> GetOutputTextFolder();

  G4String auxfilename        = OutputTextFolder+"_GDML_AuxiliaryInfo.txt";
  G4String materialfilename   = OutputTextFolder+"_GDML_MaterialInfo.txt";
  G4String solidnamesfilename = OutputTextFolder+"_GDML_SolidNames.txt";
  G4String sensitiveDetFilename = OutputTextFolder+"_GDML_SD_DetectorConstruction.txt";
  G4String SiliconPositionFilename = OutputTextFolder+"_GDML_SiliconPosition.txt";
  
  std::ofstream auxfile(auxfilename);
  std::ofstream materialfile(materialfilename);
  std::ofstream solidnamesfile(solidnamesfilename);
  std::ofstream sensitiveDetFile(sensitiveDetFilename);
  std::ofstream SiliconPositionFile(SiliconPositionFilename);

  const G4GDMLAuxMapType* auxmap = fParser.GetAuxMap();
  
  G4cout         << "Found " << auxmap->size() << " volume(s) with auxiliary information." << G4endl << G4endl;
  auxfile        << "Found " << auxmap->size() << " volume(s) with auxiliary information."  << G4endl << G4endl;
  materialfile   << "Found " << auxmap->size() << " volume(s) with auxiliary information." << G4endl << G4endl;
  solidnamesfile << "Found " << auxmap->size() << " volume(s) with auxiliary information." << G4endl << G4endl;


  G4double TotalMass = 0.0;

  for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin(); iter!=auxmap->end(); iter++) 
  {
    G4cout  << "Volume " << ((*iter).first)->GetName() << " has the following list of auxiliary information: " << G4endl;
    auxfile << "Volume " << ((*iter).first)->GetName() << " has the following list of auxiliary information: " << G4endl;

    G4LogicalVolume* logvol = (*iter).first;


    G4Material*      mat    = logvol->GetMaterial();
    G4String         matname= mat->GetName();
    G4double         mass = logvol -> GetMass() / g;
    TotalMass += mass;

    materialfile << "Volume " << ((*iter).first)->GetName() << " Material: " << matname << " Mass: "<< mass << G4endl;

    G4String LogicName = logvol->GetName();
    G4String SolidName = logvol->GetSolid()->GetName();
    G4String SolidType = logvol->GetSolid()->GetEntityType();

    solidnamesfile << LogicName << " " << SolidName << " " << SolidType << G4endl;




    for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin(); vit!=(*iter).second.end(); vit++)
    {
      G4cout  << "\tType: " << (*vit).type << " Value: " << (*vit).value << G4endl;
      auxfile << "\tType: " << (*vit).type << " Value: " << (*vit).value << G4endl;

      if((*vit).type == "Color")
      {
        G4String colorHex = (*vit).value;
        // Convert the color from Hex to RGB. The Hex color (ARGB) is formatted in a string "#********"
        // 2 digits for Alpha
        // 2 digits for Red
        // 2 digits for Green
        // 2 digits for Blue

        G4String alphaHex = colorHex.substr(1,2);
        G4String redHex   = colorHex.substr(3,2);
        G4String greenHex = colorHex.substr(5,2);
        G4String blueHex  = colorHex.substr(7,2);

        G4int alphaDec = std::stoi(alphaHex,nullptr,16);
        G4int redDec   = std::stoi(redHex,nullptr,16);
        G4int greenDec = std::stoi(greenHex,nullptr,16);
        G4int blueDec  = std::stoi(blueHex,nullptr,16);

        G4VisAttributes* visAtt = new G4VisAttributes(G4Colour(redDec/255.0, greenDec/255.0, blueDec/255.0));


        logvol->SetVisAttributes(visAtt);
      }
      else if((*vit).type == "SensDet")
      {
        sensitiveDetFile << iter -> first -> GetName() << " " << vit->type << " " << (*vit).value << G4endl;
      }



    }
  }

  G4cout << G4endl;
  auxfile << G4endl;

  materialfile << G4endl << G4endl;
  materialfile << "################################################################" << G4endl;
  materialfile << "Total Mass of the detector: " << TotalMass << " g" << G4endl;
  materialfile << "################################################################" << G4endl;


  // The same as above, but now we are looking for
  // sensitive detectors setting them for the volumes

  G4bool SensDetSet = false;

  for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin(); iter!=auxmap->end(); iter++) 
  {
    SensDetSet = false;
    G4cout << "Volume " << ((*iter).first)->GetName() << " has the following list of auxiliary information: " << G4endl;
    auxfile << "Volume " << ((*iter).first)->GetName() << " has the following list of auxiliary information: " << G4endl;

    for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin();
         vit!=(*iter).second.end();vit++)
    {
      if ((*vit).type=="SensDet")
      {
        G4cout  << "Attaching sensitive detector " << (*vit).value << " to volume " << ((*iter).first)->GetName() << G4endl;
        auxfile << "Attaching sensitive detector " << (*vit).value << " to volume " << ((*iter).first)->GetName() << G4endl;

        if((((*vit).value == "Silicon") || ((*vit).value == "PlasticScintillator")) && (!SensDetSet))
        {
          SensDetSet = true;
          G4LogicalVolume* logvol = (*iter).first;
          G04SensitiveDetector* DetectorSD = new G04SensitiveDetector(logvol->GetName(), logvol->GetName());
          SDman -> AddNewDetector(DetectorSD);
          SetSensitiveDetector( logvol, DetectorSD); 

        }

      }
    }
  }

  auxfile.close();
  materialfile.close();
  solidnamesfile.close();

}

