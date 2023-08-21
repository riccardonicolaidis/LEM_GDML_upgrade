#include "G04EventAction.hh"
#include "G4UImanager.hh"
#include "G04PrimaryGeneratorAction.hh"



G04EventAction::G04EventAction(const G04DetectorConstruction *detector, 
                               const G04PrimaryGeneratorAction *generation,
                               const G04SteeringClass *steering,
                               const G04RunAction *runaction): 

                               fNumberSD(detector -> GetNumberOfSensitiveDetectors()), 
                               fDetector(detector), 
                               fPrimary(generation),
                               fParser(detector->GetParser()),
                               fSteering(steering),
                               fRunAction(runaction),
                               StartingNTuple(runaction->GetIndexStartSensitiveDet())
{
    OutputTextFolder = fSteering -> GetOutputTextFolder();
    G4String sensitiveDetFilename = OutputTextFolder + "_GDML_SD_EventAction.txt";
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


    sensitiveDetFile.close();
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

    G4int NTupleColumnIndex_SD = fRunAction -> GetIndexStartSensitiveDet();

    
    Ed_Silicon_Thin   = 0.;
    Ed_Silicon_Thick  = 0.;
    Ed_Veto           = 0.;
    Ed_Calo           = 0.;


    for(int i = 0; i < 10; i++)
    {
        vEd_Silicon_Thin[i]  = 0.;
        vEd_Silicon_Thick[i] = 0.;
    }

    for(G4int i = 0; i < 1000; i++)
    {
        NTupleData[i] = 0.0;
    }

    // Getting the hits collections for this event
    G4HCofThisEvent* hcofEvent = Event->GetHCofThisEvent();
    for(G4int iHC=0;iHC<hcofEvent->GetNumberOfCollections();++iHC)
    {
        G4VHitsCollection* collection=static_cast<G4VHitsCollection*>(hcofEvent->GetHC(iHC));
        G4int    HC_ID  = collection -> GetColID();
        G4String SDName = collection -> GetSDname();
        G4int    HCSize = collection -> GetSize();

        for(G4int iHit = 0; iHit < HCSize; iHit++)
        {
            HitClass* hit=static_cast<HitClass*>(collection->GetHit(iHit));
            NTupleData[HC_ID]+=hit->GetEdep();
        }
        man -> FillNtupleDColumn(0 , NTupleColumnIndex_SD+HC_ID , NTupleData[HC_ID]);

        if(G4StrUtil::contains(SDName, "Thin"))
        {
            Ed_Silicon_Thin  += NTupleData[HC_ID];
            for(int j = 0; j < NumberPairSilicon; j++)
            {
                if(G4StrUtil::contains(SDName, (G4String) std::to_string(j)))
                    vEd_Silicon_Thin[j] += NTupleData[HC_ID];
            }
        }
        else if(G4StrUtil::contains(SDName, "Thick"))
        {
            Ed_Silicon_Thick += NTupleData[HC_ID];
            for(int j = 0; j < NumberPairSilicon; j++)
            {
                if(G4StrUtil::contains(SDName, (G4String) std::to_string(j)))
                    vEd_Silicon_Thick[j] += NTupleData[HC_ID];
            }
        }
        else if(G4StrUtil::contains(SDName, "Veto"))
            Ed_Veto          += NTupleData[HC_ID];
        else if(G4StrUtil::contains(SDName, "Calo"))
            Ed_Calo          += NTupleData[HC_ID];


        if(NTupleData[HC_ID] > 0.0)
            AtLeastOne = true;
    }


    man -> FillNtupleIColumn(0, 7, Event -> GetEventID());
    man -> FillNtupleIColumn(0, 8, fSteering -> GetJobNumber());

    G4double MCEnergy = fPrimary -> GetMCEnergy();
    G4double EnergySum = Ed_Silicon_Thin + Ed_Silicon_Thick + Ed_Calo + Ed_Veto;


    // Definisco delle soglie per eventi anomali
    G4double E_th_Veto       = 40 * keV;
    G4double E_th_Silicon    = 10 * keV;
    G4double AlarmPercentage = 0.5;

    if(!AtLeastOne)
        return;

    man -> AddNtupleRow(0);


    bool GoodTriggerCondition               = false;
    bool EneregyConfinement_AnomalyDetected = false;


    // 1. Check the number of Thin/Thick detectors over threshold
    // 2. If the number of Thin detectors over threshold greater than 1  = BAD TRIGGER
    // 3. If the number of Thick detectors over threshold greater than 1 = BAD TRIGGER
    // 4. Number of Thin and Thick detectors over threshold is 1 respectively = GOOD TRIGGER
    // 5. Number of Thin and Thick detectors over threshold is 0 = BAD TRIGGER
    // 6. ID of the Thin detector over threshold is different from the ID of the Thick detector over threshold = BAD TRIGGER

    G4int    NumberThinOverThreshold  = 0;
    G4int    NumberThickOverThreshold = 0;
    G4int    IDThinOverThreshold      = -1;
    G4int    IDThickOverThreshold     = -1;

    for(int i = 0; i < NumberPairSilicon; i++)
    {
        if(vEd_Silicon_Thin[i] > E_th_Silicon)
        {
            NumberThinOverThreshold++;
            IDThinOverThreshold = i;
        }
        if(vEd_Silicon_Thick[i] > E_th_Silicon)
        {
            NumberThickOverThreshold++;
            IDThickOverThreshold = i;
        }
    }

    if(NumberThinOverThreshold == 1 && NumberThickOverThreshold == 1)
    {
        if(IDThinOverThreshold == IDThickOverThreshold)
            GoodTriggerCondition = true;
    }

    // Check Energy Confinement Trigger Anomaly
    // 1. Veto Activated (over threshold) = EVENT Discarded / NOT CONFINED
    // 2. Veto NOT Activated (under threshold) = EVENT Accepted / CONFINED - continue the checks
    // 3. Compute the measured energy: SUM Thin + Thick + Calo
    // 4. If the discrepancy between the measured energy and the MC energy is greater than 50% of the MC energy = Anomaly - to be investigated - saving the event

    if(Ed_Veto < E_th_Veto)
    {
        if((MCEnergy - EnergySum) > (AlarmPercentage * MCEnergy))
            EneregyConfinement_AnomalyDetected = true;
    }




    if(GoodTriggerCondition && EneregyConfinement_AnomalyDetected)
    {
        G4cout << "Event " << Event -> GetEventID() << " - Energy Confinement Anomaly Detected" << G4endl;
        for(G4int iHC=0;iHC<hcofEvent->GetNumberOfCollections();++iHC)
        {
            G4VHitsCollection* collection=static_cast<G4VHitsCollection*>(hcofEvent->GetHC(iHC));

            G4int    HCSize = collection -> GetSize();

            for(G4int iHit = 0; iHit < HCSize; iHit++)
            {
                HitClass* hit=static_cast<HitClass*>(collection->GetHit(iHit));
                G4ThreeVector Position = hit -> GetPos();
                G4double      Edep     = hit -> GetEdep();

                man -> FillNtupleDColumn(1,0,Position.getX());
                man -> FillNtupleDColumn(1,1,Position.getY());
                man -> FillNtupleDColumn(1,2,Position.getZ());
                man -> FillNtupleDColumn(1,3,Edep);
                man -> FillNtupleIColumn(1,4,Event -> GetEventID());
                man -> AddNtupleRow(1);
            }
        }

    }


    
        

    return;
}

