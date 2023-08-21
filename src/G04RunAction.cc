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
    man -> CreateNtupleDColumn("RandEnergy");       // 0
    man -> CreateNtupleDColumn("Xgen");             // 1
    man -> CreateNtupleDColumn("Ygen");             // 2
    man -> CreateNtupleDColumn("Zgen");             // 3
    man -> CreateNtupleDColumn("pDirX");            // 4
    man -> CreateNtupleDColumn("pDirY");            // 5
    man -> CreateNtupleDColumn("pDirZ");            // 6
    man -> CreateNtupleIColumn("EventID");          // 7
    man -> CreateNtupleIColumn("JobNumber");        // 8

    // Sensitive detectors will start from here

    
    ReportFile << "##############################################" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Name: " << "Edep" << G4endl;
    ReportFile << "##############################################" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "RandEnergy" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Xgen" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Ygen" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Zgen" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "pDirX" << G4endl; 
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "pDirY" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "pDirZ" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "EventID" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "JobNumber" << G4endl;

    IndexStartSensitiveDet = NTupleColumnID; // This is the index where the sensitive detectors will start


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

    
    man ->FinishNtuple(0);
    NTupleID++;
    NTupleColumnID = 0;
    man -> CreateNtuple("Hits", "Hits");
    man -> CreateNtupleDColumn("X");
    man -> CreateNtupleDColumn("Y");
    man -> CreateNtupleDColumn("Z");
    man -> CreateNtupleDColumn("E");
    man -> CreateNtupleIColumn("ID");
    man -> FinishNtuple(1);

    ReportFile << "##############################################" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Name: " << "Hits" << G4endl;
    ReportFile << "##############################################" << G4endl;

    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "X" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Y" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "Z" << G4endl;
    ReportFile << "Ntuple ID: " << NTupleID << " Ntuple Column ID: " << NTupleColumnID++ << " Ntuple Column Name: " << "E" << G4endl;




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
