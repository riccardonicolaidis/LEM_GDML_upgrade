#include "G04RunAction.hh"


G04RunAction::G04RunAction(G04DetectorConstruction* detector, 
                           G04PrimaryGeneratorAction* primary,
                           G04SteeringClass *steering):
                           G4UserRunAction(),  
                           fDetector(detector), 
                           fPrimary(primary), 
                           fParser(detector->GetParser()),
                           fSteering(steering)
{

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man ->SetVerboseLevel(1);

    fMessenger = new G4GenericMessenger(this, "/NameOfFile/","Name of the file to save data");
    fMessenger -> DeclareProperty("NameOfFile", TotalFileName, "Name of the file to save data");


    OutputTextFolder = fSteering -> GetOutputTextFolder();
    G4String ReportFileName = OutputTextFolder+"_NTupleInfo.txt";
    std::ofstream ReportFile(ReportFileName);
    G4int NTupleID = 0;
    G4int NTupleColumnID = 0;

    // Ntuple particle generator
    man -> CreateNtuple("Edep","Edep");
    ReportFile << "##############################################" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Name: " << "Edep" << G4endl;
    ReportFile << "##############################################" << G4endl;
    man -> CreateNtupleDColumn("RandEnergy");       // 0
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "RandEnergy" << G4endl;
    man -> CreateNtupleDColumn("Xgen");             // 1
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Xgen" << G4endl;
    man -> CreateNtupleDColumn("Ygen");             // 2
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Ygen" << G4endl;
    man -> CreateNtupleDColumn("Zgen");             // 3 
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Zgen" << G4endl;
    man -> CreateNtupleDColumn("pDirX");            // 4
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "pDirX" << G4endl; 
    man -> CreateNtupleDColumn("pDirY");            // 5 
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "pDirY" << G4endl;
    man -> CreateNtupleDColumn("pDirZ");            // 6
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "pDirZ" << G4endl;

    G4String sensitiveDetFilename = OutputTextFolder+"_GDML_SD_RunAction.txt";
    std::ofstream sensitiveDetFile(sensitiveDetFilename);


    const G4GDMLAuxMapType* auxmap = fParser.GetAuxMap();
    G4bool SensDetSet = false;
    for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin(); iter!=auxmap->end(); iter++) 
    {
        SensDetSet = false;
        for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin(); vit!=(*iter).second.end(); vit++)
        {
            if(((*vit).type == "SensDet") && (!SensDetSet))
            {
                SensDetSet = true;
                sensitiveDetFile << iter -> first -> GetName() << " " << vit->type << " " << (*vit).value << G4endl;

                G4String ColumnName = "Ed_" +(iter -> first -> GetName());
                man -> CreateNtupleDColumn(ColumnName);
                ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << ColumnName << G4endl;
            }
        }
    }

    man -> CreateNtupleIColumn("EventID");          // 7

    man ->FinishNtuple();

    ReportFile.close();
    sensitiveDetFile.close();

}


G04RunAction::~G04RunAction()
{

}

void G04RunAction::BeginOfRunAction(const G4Run* )
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();    
    TotalFileNameFinal = TotalFileName + ".root";
    man -> OpenFile(TotalFileNameFinal);
}


void G04RunAction::EndOfRunAction(const G4Run* )
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man -> Write();
    man -> CloseFile(TotalFileNameFinal);
    fPrimary -> ResetParticleNumber();
}
