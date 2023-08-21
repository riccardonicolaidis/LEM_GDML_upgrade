#include "G04EventAction.hh"
#include "G4UImanager.hh"
#include "G04PrimaryGeneratorAction.hh"



G04EventAction::G04EventAction(G4int NumberSD, 
                               const G04DetectorConstruction *detector, 
                               G04PrimaryGeneratorAction *generation,
                               G04SteeringClass *steering): 
                               fNumberSD(NumberSD), 
                               fDetector(detector), 
                               fPrimary(generation),
                               fParser(detector->GetParser()),
                               fSteering(steering)
{
    OutputTextFolder = fSteering -> GetOutputTextFolder();
    G4String sensitiveDetFilename = OutputTextFolder + "_GDML_SD_EventAction.txt";
    std::ofstream sensitiveDetFile(sensitiveDetFilename);


    for(int i = 0; i < 10; ++i)
    {
        G4cout << "###############" << G4endl;
    }

    G4cout << sensitiveDetFilename << G4endl;

    for(int i = 0; i < 100; ++i)
    {
        G4cout << "###############" << G4endl;
    }
    
    const G4GDMLAuxMapType* auxmap = fParser.GetAuxMap();
    for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin(); iter!=auxmap->end(); iter++) 
    {
        for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin(); vit!=(*iter).second.end(); vit++)
        {
            if((*vit).type == "SensDet")
            {
                sensitiveDetFile << iter -> first -> GetName() << " " << vit->type << " " << (*vit).value << G4endl;
            }
        }
    }
}

G04EventAction::~G04EventAction()
{
}

void G04EventAction::BeginOfEventAction(const G4Event*)
{
}



void G04EventAction::EndOfEventAction(const G4Event *Event)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4bool AtLeastOne = false;

    for(G4int i = 0; i < 1000; i++)
    {
        NTupleData[i] = 0.0;
    }

    G4double Ed_Silicon_Thin = 0.;
    G4double Ed_Silicon_Thick = 0.;
    G4double Ed_Veto = 0.;
    G4double Ed_Plastic =0.;

    G4double MeasEnergy = 0.0;


    G4int NTupleColumnIndex = StartingNTuple;
    G4int HitsCollectionIndex = 0;
    const G4GDMLAuxMapType* auxmap = fParser.GetAuxMap();
    for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin(); iter!=auxmap->end(); iter++) 
    {
        for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin(); vit!=(*iter).second.end(); vit++)
        {
            if((*vit).type == "SensDet")
            {
                G4VHitsCollection *HC = Event -> GetHCofThisEvent() -> GetHC(HitsCollectionIndex++);
                for(G4int iH = 0; iH < HC -> GetSize(); iH++)
                {
                    HitClass* hit=static_cast<HitClass*>(HC->GetHit(iH));
                    NTupleData[NTupleColumnIndex]+=hit->GetEdep();
                    MeasEnergy += hit->GetEdep(); 
                }
            
                man -> FillNtupleDColumn(0, NTupleColumnIndex, NTupleData[NTupleColumnIndex]);
                

                if(NTupleData[NTupleColumnIndex] > 0.0)
                {
                    AtLeastOne = true;
                }

                G4String SDName = iter -> first -> GetName();
                //G4cout << SDName << G4endl;
                if(G4StrUtil::contains(SDName,"Thin"))
                {
                    Ed_Silicon_Thin += NTupleData[NTupleColumnIndex];
                }
                else if(G4StrUtil::contains(SDName,"Thick"))
                {
                    Ed_Silicon_Thick += NTupleData[NTupleColumnIndex];
                }
                else if(G4StrUtil::contains(SDName,"Calo"))
                {
                    Ed_Plastic = NTupleData[NTupleColumnIndex];
                }
                else if(G4StrUtil::contains(SDName,"Veto"))
                {
                    Ed_Veto += NTupleData[NTupleColumnIndex];
                }


                ++NTupleColumnIndex;
            }
        }
    }
    man -> FillNtupleIColumn(0, NTupleColumnIndex, Event -> GetEventID());

    G4double MCEnergy = fPrimary -> GetMCEnergy();
    G4double EnergySum = Ed_Silicon_Thin + Ed_Silicon_Thick + Ed_Plastic + Ed_Veto;

    G4UImanager* UImanager = G4UImanager::GetUIpointer();    

    if(AtLeastOne)
    {
        if((MCEnergy-MeasEnergy) >= 0.5* MCEnergy)
        {
            if(Ed_Veto == 0. && Ed_Plastic == 0. && Ed_Silicon_Thick > 0.04 && Ed_Silicon_Thin > 0.04)
            {
                //UImanager -> ApplyCommand("/random/saveThisEvent");
                // Get Event Number
                // G4cout << "Problem: " << Event -> GetEventID() << G4endl;
                // G4cout << "MCEnergy: " << MCEnergy << G4endl;
                // G4cout << "EnergySum: " << EnergySum << G4endl;
                // G4cout << "MeasEnergy: " << MeasEnergy << G4endl;

                G4cout << Event -> GetEventID() << "\t" << MCEnergy << "\t" << EnergySum << "\t" << MeasEnergy << "\t" << Ed_Silicon_Thin << "\t" << Ed_Silicon_Thick << "\t" << Ed_Plastic << "\t" << Ed_Veto << G4endl;
            }

        }
        man -> AddNtupleRow(0);
        
    }

}

