#include "G04EventAction.hh"


G04EventAction::G04EventAction(G4int NumberSD, const G04DetectorConstruction *detector): fNumberSD(NumberSD), fDetector(detector), fParser(detector->GetParser())
{
    G4String sensitiveDetFilename = "../OutputText/GDML_SD_EventAction.txt";
    std::ofstream sensitiveDetFile(sensitiveDetFilename);

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
                }
            
                man -> FillNtupleDColumn(0, NTupleColumnIndex, NTupleData[NTupleColumnIndex]);
                ++NTupleColumnIndex;

                if(NTupleData[NTupleColumnIndex-1] > 0.0)
                {
                    AtLeastOne = true;
                }
            }
        }
    }


    if(AtLeastOne)
    {
        man -> AddNtupleRow(0);
    }

}

