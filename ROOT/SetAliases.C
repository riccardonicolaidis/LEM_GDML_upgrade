// C++ include
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

// ROOT include
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TRandom2.h"
#include "TH2D.h"
#include "TF2.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TMath.h"
#include "TList.h"
#include "TKey.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TEntryList.h"
#include "TTreeFormula.h"
#include "TGraph2D.h"
#include "TMath.h"

// Custom include
#include "./TH1DLog.h"
#include "./TH2DLog.h"
#include "./TH2DLogX.h"


using namespace std;

// ######################################################## //
// Macro for
// 1) Setting Aliases
// 2) Defining New Files
// 3) Skimming the TTree
// 4) Plotting preliminary results on Angle resolution
// 5) Generation of .csv files for the analysis with python
// ######################################################## //



void SetAliases(TString filename,
                TString filename_noExt,
                TString destination,
                TString pathConfigurationFile   = "",
                TString ParticleName            = "",
                TString indexParticleFile_str   = "",
                TString AreaGeneration_cm2_str  = "",
                double  E_thr_Thin              = 0.04,
                double  E_thr_Thick             = 0.04,
                double  E_thr_Plastic           = 0.1,
                int     NumberPairsSensors      = 5,
                double  ResSilicon              = 0.01,
                double  ResPlastic              = 0.2)
{
    int indexParticleFile = atoi(indexParticleFile_str.Data());
    double AreaGeneration_cm2 = AreaGeneration_cm2_str.Atof();

    // Print all the input parameters
    cout << "filename = " << filename << endl;
    cout << "filename_noExt = " << filename_noExt << endl;
    cout << "destination = " << destination << endl;
    cout << "pathConfigurationFile = " << pathConfigurationFile << endl;
    cout << "ParticleName = " << ParticleName << endl;
    cout << "indexParticleFile_str = " << indexParticleFile_str << endl;
    cout << "indexParticleFile = " << indexParticleFile << endl;
    cout << "AreaGeneration_cm2_str = " << AreaGeneration_cm2_str << endl; 
    cout << "AreaGeneration_cm2 = " << AreaGeneration_cm2 << endl;
    cout << "E_thr_Thin = " << E_thr_Thin << endl;
    cout << "E_thr_Thick = " << E_thr_Thick << endl;
    cout << "E_thr_Plastic = " << E_thr_Plastic << endl;
    cout << "NumberPairsSensors = " << NumberPairsSensors << endl;
    cout << "ResSilicon = " << ResSilicon << endl;
    cout << "ResPlastic = " << ResPlastic << endl;


    /* ###################################################### */
    /*                    MANAGING FOLDERS                    */
    /* ###################################################### */
    TString destination_Violation = destination + "/EnergyViolation3D";
    gSystem -> mkdir(destination_Violation.Data(), true);



    /* ###################################################### */
    /*            DEFINITION OF THE BRANCHES NAMES            */
    /* ###################################################### */


    TString NameFile         = filename_noExt;
    TString DirName[3]       = {"DirX", "DirY", "DirZ"};
    TString PolarAngle[2]    = {"dirTheta", "dirPhi"};
    TString NewPolarAngle[2] = {"dirThetaNew", "dirPhiNew"};



    /* ###################################################### */
    /*                    DEFINING BRANCHES                   */
    /* ###################################################### */

    int     NumberBranches = 7 + 3 + 2*NumberPairsSensors;
    TString BranchName[NumberBranches];

    int iBranch = 0;
    BranchName[iBranch++] = "RandEnergy";
    BranchName[iBranch++] = "Xgen";
    BranchName[iBranch++] = "Ygen";
    BranchName[iBranch++] = "Zgen";
    int iBranchDir = iBranch;
    BranchName[iBranch++] = "pDirX";
    BranchName[iBranch++] = "pDirY";
    BranchName[iBranch++] = "pDirZ";
    int iBranchEnergies = iBranch;
    BranchName[iBranch++] = "Ed_LV_PlasticVetoBottom";
    BranchName[iBranch++] = "Ed_LV_PlasticVetoTop";
    int iBranchEnergies_Calo = iBranch;
    BranchName[iBranch++] = "Ed_LV_Calo";
    int iStartSensors       = iBranch;
    int iStartSensors_Thin  = iStartSensors;
    int iStartSensors_Thick = iStartSensors + NumberPairsSensors;

    for(int iPair = 0; iPair < NumberPairsSensors; iPair++)
    {
        BranchName[iBranch] = "Ed_LV_SiliconDetector_Thin_" + to_string(iPair);
        BranchName[iBranch+NumberPairsSensors] = "Ed_LV_SiliconDetector_Thick_" + to_string(iPair);
        iBranch++;
    }

    /* ###################################################### */
    /*                    OPENING FILE ROOT                   */
    /* ###################################################### */

    TFile *file = new TFile(filename, "READ");
    TList *list = file -> GetListOfKeys();


    /* ###################################################### */
    /*                   CREATING A NEW FILE                  */
    /* ###################################################### */
    
    /* ------------- CLONING THE TREE STRUCTURE ------------- */
    TFile *file_alias = new TFile(destination +"/" + filename_noExt + "_alias.root", "RECREATE");
    TTree *Edep_old   = (TTree*) (file -> Get("Edep"));
    TTree *Hits       = (TTree*) (file -> Get("Hits"));


    /* ###################################################### */
    /*                        LOG FILES                       */
    /* ###################################################### */

    ofstream Log;
    Log.open(destination+"/"+"Log_"+filename_noExt+".txt");
    Log  << "Setting aliases for " << filename << endl;
    Log  << "NumberBranches = "    << NumberBranches << endl;
    cout << "Setting aliases for " << filename << endl;
    cout << "NumberBranches = "    << NumberBranches << endl;

    for(int i = 0; i < NumberBranches; i++)
    {
        cout << "BranchName[" << i << "] = " << BranchName[i] << endl;
        Log  << "BranchName[" << i << "] = " << BranchName[i] << endl;
    }
    
    Log << "File " << filename << " opened" << endl;
    Log << "File structure:" << endl;
    
    for(int i = 0; i < list -> GetSize(); ++i)
    {
        Log << "Key " << i << " = " << list -> At(i) -> GetName() << endl;
    }

    /* --------------- PRINTING THE STRUCTURE --------------- */
    Log << "TTree structure:" << endl;
    TObjArray *list2 = Edep_old -> GetListOfBranches();
    for(int i = 0; i < list2 -> GetSize(); ++i)
    {
        Log << "Branch " << i << " = " << list2 -> At(i) -> GetName() << endl;
    }
    Log.close();


    /* ###################################################### */
    /*                  CSV FILES FOR PYTHON                  */
    /* ###################################################### */

    ofstream csv;
    csv.open(destination_Violation + "/" + filename_noExt + "_Hits.csv");
    double X, Y, Z, E;
    int ID, JobNumber_Hit;
    Hits -> SetBranchAddress("X", &X);
    Hits -> SetBranchAddress("Y", &Y);
    Hits -> SetBranchAddress("Z", &Z);
    Hits -> SetBranchAddress("E", &E);
    Hits -> SetBranchAddress("ID", &ID);
    Hits -> SetBranchAddress("JobNumber", &JobNumber_Hit);
    for(int i = 0; i < Hits -> GetEntries(); i++)
    {
        Hits -> GetEntry(i);
        csv << ID << "," << JobNumber_Hit << "," << X << "," << Y << "," << Z << "," << E << endl;
        if(i%10000 == 0)
            std::cout << "i = " << i << endl;
    }
    csv.close();


    /* ###################################################### */
    /*               CLONING THE TTREE STRUCTURE              */
    /* ###################################################### */
    /* ----------------- CD TO THE NEW FILE ----------------- */
    file_alias -> cd();
            
    /* ------------- CLONING ONLY THE STRUCTURE ------------- */
    TTree *Edep = Edep_old -> CloneTree(0);

    /* ---------- DEFINING STRINGS FOR THE ALIASES ---------- */
    int NumberEnergySensors = 2*NumberPairsSensors + 2 + 1;

    /* ---------- TOTAL ENERGY DEPOSITED IN SENSORS --------- */
    TString TotThin   = "(";
    TString TotThick  = "(";
    TString gTotThin  = "(";
    TString gTotThick = "(";


    TString TotThin_NoThreshold  = "(";

    for(int i = 0; i < NumberPairsSensors; i++)
    {
        if(i == 0)
        {
            TotThin   += Form("(%s)*(%s > %g)",   BranchName[i + iStartSensors_Thin].Data(),  BranchName[i + iStartSensors_Thin].Data(),  E_thr_Thin);
            TotThick  += Form("(%s)*(%s > %g)",   BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
            gTotThin  += Form("(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thin].Data(),  BranchName[i + iStartSensors_Thin].Data(),  E_thr_Thin);
            gTotThick += Form("(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
            TotThin_NoThreshold  += Form("(%s)",   BranchName[i + iStartSensors_Thin].Data());
        }
        else
        {
            TotThin   += Form("+(%s)*(%s > %g)",   BranchName[i + iStartSensors_Thin].Data(),  BranchName[i + iStartSensors_Thin].Data(),  E_thr_Thin);
            TotThick  += Form("+(%s)*(%s > %g)",   BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
            gTotThin  += Form("+(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thin].Data(),  BranchName[i + iStartSensors_Thin].Data(),  E_thr_Thin);
            gTotThick += Form("+(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
            TotThin_NoThreshold  += Form("+(%s)",   BranchName[i + iStartSensors_Thin].Data());
        }
    }

    TotThin   += ")";
    TotThick  += ")";
    gTotThin  += ")";
    gTotThick += ")";
    TotThin_NoThreshold  += ")";

    TString TotalEnergy   = Form("((%s) + (%s) + (%s)*(%s > %g))", TotThin.Data(), TotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    TString gTotalEnergy  = Form("((%s) + (%s) + (g%s)*(%s > %g))", gTotThin.Data(), gTotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    TString TotThickCalo  = Form("((%s) + (%s)*(%s > %g))", TotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    TString gTotThickCalo = Form("((%s) + (g%s)*(g%s > %g))", gTotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);

    TString PID         = "TMath::Log10((TotalEnergy)*(TotThin))";
    TString gPID        = "TMath::Log10((gTotalEnergy)*(gTotThin))";
    TString PID_noCalo  = Form("TMath::Log10((%s)*(TotThin))", BranchName[iBranchEnergies_Calo].Data());
    TString gPID_noCalo = Form("TMath::Log10((g%s)*(gTotThin))", BranchName[iBranchEnergies_Calo].Data());


    /* ----------------- GAUS VARIABLES RND ----------------- */
    /* ----------------- GAUSSIAN SMEARINGS ----------------- */
    // Put a g in front of the name of the branch to get the smeared value
    for(int i = 0; i < NumberEnergySensors; i++)
    {
        Edep -> SetAlias(Form("wnorm%d", i), "(sin(2 *pi*rndm)*sqrt(-2*log(rndm)))");
        Edep -> SetAlias(Form("g%s", BranchName[i + iBranchEnergies].Data()), Form("(%s)*(1 + wnorm%d * %g)", BranchName[i + iBranchEnergies].Data(), i, ((i + iBranchEnergies)>= iStartSensors) ? ResSilicon : ResPlastic));
        cout << "g" << BranchName[i + iBranchEnergies].Data() << " = " << Form("(%s)*(1 + wnorm%d * %g)", BranchName[i + iBranchEnergies].Data(), i, ((i + iBranchEnergies)>= iStartSensors) ? ResSilicon : ResPlastic) << endl;

    }



    /* ------------------- SETTING ALIASES ------------------ */
    Edep -> SetAlias("TotThin",       TotThin.Data());
    Edep -> SetAlias("TotThin_NoThreshold",       TotThin_NoThreshold.Data());
    Edep -> SetAlias("TotThick",      TotThick.Data());
    Edep -> SetAlias("gTotThin",      gTotThin.Data());
    Edep -> SetAlias("gTotThick",     gTotThick.Data());
    Edep -> SetAlias("TotThickCalo",  TotThickCalo.Data());
    Edep -> SetAlias("gTotThickCalo", gTotThickCalo.Data());
    Edep -> SetAlias("TotalEnergy",   TotalEnergy.Data());
    Edep -> SetAlias("gTotalEnergy",  gTotalEnergy.Data());
    Edep -> SetAlias("PID",           PID.Data());
    Edep -> SetAlias("gPID",          gPID.Data());
    Edep -> SetAlias("PID_noCalo",    PID_noCalo.Data());
    Edep -> SetAlias("gPID_noCalo",   gPID_noCalo.Data());
    Edep -> SetAlias("NCVF",          "(1.0 - (TotalEnergy)/(RandEnergy - Ed_LV_AlTop))");      
    Edep -> SetAlias("PID3", "TMath::Log10((Ed_LV_Calo + (TotThick)) * (TotThick))");

    Edep_old -> SetAlias("TotThin",       TotThin.Data());
    Edep_old -> SetAlias("TotThin_NoThreshold",       TotThin_NoThreshold.Data());
    Edep_old -> SetAlias("TotThick",      TotThick.Data());
    Edep_old -> SetAlias("gTotThin",      gTotThin.Data());
    Edep_old -> SetAlias("gTotThick",     gTotThick.Data());
    Edep_old -> SetAlias("TotThickCalo",  TotThickCalo.Data());
    Edep_old -> SetAlias("gTotThickCalo", gTotThickCalo.Data());
    Edep_old -> SetAlias("TotalEnergy",   TotalEnergy.Data());
    Edep_old -> SetAlias("gTotalEnergy",  gTotalEnergy.Data());    
    Edep_old -> SetAlias("PID",           PID.Data());
    Edep_old -> SetAlias("gPID",          gPID.Data());
    Edep_old -> SetAlias("PID_noCalo",    PID_noCalo.Data());
    Edep_old -> SetAlias("gPID_noCalo",   gPID_noCalo.Data());
    Edep_old -> SetAlias("NCVF",          "(1.0 - (TotalEnergy)/(RandEnergy - Ed_LV_AlTop))");
    Edep_old -> SetAlias("PID3", "TMath::Log10((Ed_LV_Calo + (TotThick)) * (TotThick))");



    cout << "TotThin = " << TotThin << endl;
    cout << "TotThick = " << TotThick << endl;
    cout << "gTotThin = " << gTotThin << endl;
    cout << "gTotThick = " << gTotThick << endl;
    cout << "TotThickCalo = " << TotThickCalo << endl;
    cout << "gTotThickCalo = " << gTotThickCalo << endl;
    cout << "TotalEnergy = " << TotalEnergy << endl;
    cout << "gTotalEnergy = " << gTotalEnergy << endl;
    cout << "PID = " << PID << endl;
    cout << "gPID = " << gPID << endl;
    cout << "PID_noCalo = " << PID_noCalo << endl;
    cout << "gPID_noCalo = " << gPID_noCalo << endl;
    cout << "NCVF = " << "(1.0 - (TotalEnergy)/(RandEnergy - Ed_LV_AlTop))" << endl;


    Edep     -> SetAlias("NormP", "TMath::Sqrt(pDirX*pDirX + pDirY*pDirY + pDirZ*pDirZ)");
    Edep_old -> SetAlias("NormP", "TMath::Sqrt(pDirX*pDirX + pDirY*pDirY + pDirZ*pDirZ)");
    for(int i = 0; i < 3; ++i)
    {
        if(i == 2)
        {
            Edep     -> SetAlias(DirName[i].Data(), Form("(-%s)/NormP", BranchName[i+iBranchDir].Data()));
            Edep_old -> SetAlias(DirName[i].Data(), Form("(-%s)/NormP", BranchName[i+iBranchDir].Data()));
        }
        else
        {
            Edep     -> SetAlias(DirName[i].Data(), Form("%s/NormP", BranchName[i+iBranchDir].Data()));
            Edep_old -> SetAlias(DirName[i].Data(), Form("%s/NormP", BranchName[i+iBranchDir].Data()));
        }
    }

    Edep_old -> SetAlias(PolarAngle[0].Data(), Form("TMath::ACos(%s)", DirName[2].Data()));
    Edep_old -> SetAlias(PolarAngle[1].Data(), Form("TMath::ATan2(%s, %s)", DirName[1].Data(), DirName[0].Data()));
    Edep_old -> SetAlias(NewPolarAngle[1].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Cos(%s)),(TMath::Cos(%s))))",              PolarAngle[0].Data(), PolarAngle[0].Data(), PolarAngle[1].Data() )); // *(180/3.415927))
    Edep_old -> SetAlias(NewPolarAngle[0].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Sin(%s)*TMath::Sin(%s)),TMath::Cos(%s)))", PolarAngle[0].Data(), PolarAngle[1].Data(), NewPolarAngle[1].Data(), PolarAngle[0].Data())); // *(180/3.415927))

    Edep -> SetAlias(PolarAngle[0].Data(), Form("TMath::ACos(%s)", DirName[2].Data()));
    Edep -> SetAlias(PolarAngle[1].Data(), Form("TMath::ATan2(%s, %s)", DirName[1].Data(), DirName[0].Data()));
    Edep -> SetAlias(NewPolarAngle[1].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Cos(%s)),(TMath::Cos(%s))))",              PolarAngle[0].Data(), PolarAngle[0].Data(), PolarAngle[1].Data() )); // *(180/3.415927))
    Edep -> SetAlias(NewPolarAngle[0].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Sin(%s)*TMath::Sin(%s)),TMath::Cos(%s)))", PolarAngle[0].Data(), PolarAngle[1].Data(), NewPolarAngle[1].Data(), PolarAngle[0].Data())); // *(180/3.415927))



    /* ###################################################### */
    /*                 GOOD EVENTS CONDITIONS                 */
    /* ###################################################### */

    TString ConditionPairSilicon[NumberPairsSensors];
    TString ConditionPairSiliconAll;
    TString ConditionVeto;
    TString ConditionGoodEvents;
    TString ConditionGoodEventsSinglePair[NumberPairsSensors];
    TString ConditionNoCalo;
    TString NumberPairsHit;

    for(int i = 0; i< NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors+i].Data() << " & " <<BranchName[iStartSensors+ NumberPairsSensors+i].Data() << endl;
        ConditionPairSilicon[i] = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors_Thin+i].Data(), E_thr_Thin, BranchName[iStartSensors_Thick + i].Data(), E_thr_Thick);
        cout << "ConditionPairSilicon[" << i << "] = " << ConditionPairSilicon[i].Data() << endl;
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s)",    ConditionPairSilicon[i].Data());
            NumberPairsHit          = Form("(1*(%s)", ConditionPairSilicon[i].Data());
        }
        else 
        {
            ConditionPairSiliconAll += Form("||(%s)", ConditionPairSilicon[i].Data());
            NumberPairsHit          += Form("+1*(%s)", ConditionPairSilicon[i].Data());
        }
    }
    ConditionPairSiliconAll += ")";
    NumberPairsHit          += ")";

    ConditionVeto = Form("(%s < %g) && (%s < %g)", BranchName[iBranchEnergies].Data(), E_thr_Plastic, BranchName[iBranchEnergies + 1].Data(), E_thr_Plastic);
    cout << "ConditionVeto = " << ConditionVeto.Data() << endl;
    ConditionGoodEvents = Form("(%s < 2.0) && (%s) && (%s)", NumberPairsHit.Data(), ConditionPairSiliconAll.Data(), ConditionVeto.Data());
    cout << "ConditionGoodEvents = " << ConditionGoodEvents.Data() << endl;
    for(int i = 0; i < NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors_Thin+i].Data() << " & " <<BranchName[iStartSensors_Thick+i].Data() << endl;
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionVeto.Data());
        cout << "ConditionGoodEventsSinglePair[" << i << "] = " << ConditionGoodEventsSinglePair[i].Data() << endl;
    }

    ConditionNoCalo = Form("(%s < %g)", BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    cout << "ConditionNoCalo = " << ConditionNoCalo.Data() << endl;


    /* ###################################################### */
    /*               TTREE FORMULAE FOR SKIMMING              */
    /* ###################################################### */


    TTreeFormula *fConditionGoodEvents_Calo   = new TTreeFormula("fConditionGoodEvents", Form("(%s) && !(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), Edep_old);
    TTreeFormula *fConditionGoodEvents_NoCalo = new TTreeFormula("fConditionGoodEvents", Form("(%s) && (%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), Edep_old);


    TEntryList *SkimmingList   = new TEntryList("SkimmingList", "SkimmingList", Edep_old);
    TEntryList *NCVF_Violation = new TEntryList("NCVF_Violation", "NCVF_Violation", Edep);


    Edep_old -> Draw(">>SkimmingList", Form("(%s)", ConditionGoodEvents.Data()), "entrylist");
    cout << "SkimmingList->GetN() = " << SkimmingList->GetN() << endl;
    





    /* ###################################################### */
    /*                SKIMMING TTREE PRODUCTION               */
    /* ###################################################### */
    Edep_old -> SetEntryList(SkimmingList);
    for(int i = 0; i < SkimmingList -> GetN(); ++i)
    {
        Edep_old -> GetEntry(SkimmingList -> GetEntry(i));
        Edep     -> Fill();
    }

    // Now the TTree Edep is filled
    Edep -> Draw(">>NCVF_Violation", Form("(%s) && (NCVF > 0.01)", ConditionGoodEvents.Data()), "entrylist");


    ofstream csv_NCVF;
    csv_NCVF.open(destination_Violation + "/" + filename_noExt + "_Hits_NCVF.csv");
    int EventID, JobNumber_Edep;
    Edep -> SetBranchAddress("EventID",   &EventID);
    Edep -> SetBranchAddress("JobNumber", &JobNumber_Edep);
 


    int index_Edep = 0;
    int index_Hits = 0;
    for(int i = 0; i < NCVF_Violation -> GetN(); ++i)
    {
        int Entry = NCVF_Violation -> GetEntry(i);
        
        Edep -> GetEntry(Entry);
        
        index_Hits = 0;

        while((EventID != ID) && (JobNumber_Hit != JobNumber_Edep) && (index_Hits < Hits -> GetEntries()) )
        {
            index_Hits++;
            
            Hits -> GetEntry(index_Hits);            
        }

        while((EventID == ID) && (JobNumber_Hit == JobNumber_Edep) && (index_Hits < Hits -> GetEntries()))
        {
            csv_NCVF << ID << "," << JobNumber_Hit << "," << X << "," << Y << "," << Z << "," << E << endl;
            
            index_Hits++;
            
            Hits -> GetEntry(index_Hits);
        }
    }

    csv_NCVF.close();









    /* Files with the computed angles from MonteCarlo Momentum direction */
    double projXAngle, projYAngle;
    double pDirX, pDirY, pDirZ;
    double Norm;
    double nDirX, nDirY, nDirZ;
    double Theta, Phi;

    
    TFile *tProjFile;
    TTree *tProj;
    tProjFile = new TFile(destination+"/"+"angles_"+filename_noExt+".root","RECREATE");
    tProj = new TTree("tProj","tProj");

    tProj -> Branch("ProjX", &projXAngle, "ProjX/D");
    tProj -> Branch("ProjY", &projYAngle, "ProjY/D");
    tProj -> Branch("pDirX", &pDirX, "pDirX/D");
    tProj -> Branch("pDirY", &pDirY, "pDirY/D");
    tProj -> Branch("pDirZ", &pDirZ, "pDirZ/D");
    tProj -> Branch("Theta", &Theta, "Theta/D");
    tProj -> Branch("Phi", &Phi, "Phi/D");


    Edep -> SetBranchAddress(BranchName[iBranchDir].Data(),   &pDirX);
    Edep -> SetBranchAddress(BranchName[iBranchDir+1].Data(), &pDirY);
    Edep -> SetBranchAddress(BranchName[iBranchDir+2].Data(), &pDirZ);

    for(int k = 0; k < (Edep -> GetEntries()); ++k)
    {
        Edep -> GetEntry(k);
        Norm= TMath::Sqrt(pDirX*pDirX + pDirY*pDirY + pDirZ*pDirZ);
        nDirX = -pDirX/Norm;
        nDirY = -pDirY/Norm;
        nDirZ = -pDirZ/Norm;

        Theta = TMath::ACos(nDirZ);
        Phi   = TMath::ATan2(nDirY, nDirX);

        projXAngle = (Theta) * TMath::Cos(Phi) * (180/TMath::Pi());
        projYAngle = (Theta) * TMath::Sin(Phi) * (180/TMath::Pi());

        tProj -> Fill();
    }

    Edep -> AddFriend(tProj);
    tProjFile -> Write();






    /* ###################################################### */
    /*                  PLOTS FOR DIAGNOSTICS                 */
    /* ###################################################### */

    TCanvas     *cAngles;
    TH2D        *hAngles;
    TGraph      *gAngles[NumberPairsSensors];
    TMultiGraph *mgAngles;
    TMultiGraph *mgAnglesProtonsAlpha;
    TCanvas     *CAnglesProtonsAlpha;
    TCanvas     *cAngleGraphs;
    int          ColorsPlot[8] = {kBlack,kBlue,kRed,kBlue,kRed,kBlue,kRed};



    cAngleGraphs = new TCanvas(Form("Directions: %s", filename_noExt.Data()),Form("Directions: %s", filename_noExt.Data()),900,700);
    cAngleGraphs -> cd();

    mgAngles = new TMultiGraph();

    cout << Form("Particle %s", filename_noExt.Data()) << endl;
    cout << "Copy No \t Std X \t Std Y \t # Events " << endl;
    for(int k = 0 ; k < NumberPairsSensors; ++k)
    {
        int idx = (k+1)%NumberPairsSensors;
        // The 0 has to be plotted last

        TH2D *hAngles;
        TH2D *hGenPosition = new TH2D(Form("hGenPosition_%d", idx), Form("hGenPosition_%d", idx), 100, -35, 35, 100, -35, 35);
        if(NameFile.Contains("e-"))
        {
            hAngles = new TH2D(Form("h2DAngles_%d", idx), Form("hAngles_%d", idx), 100, -45, 45, 100, -45, 45);
        }
        else
        {
            hAngles = new TH2D(Form("h2DAngles_%d", idx), Form("hAngles_%d", idx), 100, -45, 45, 100, -45, 45);
        }

        Edep -> Draw(Form("ProjY:ProjX>>h2DAngles_%d", idx), ConditionGoodEventsSinglePair[idx].Data(), "colz");
        TCanvas *cAngles = new TCanvas(Form("cAngles_%d", idx), Form("cAngles_%d", idx), 900, 700);
        hAngles -> Draw("colz");
        hAngles -> SaveAs(destination + "/" + "2DAngHisto_" + Form("%d_", idx)  + filename_noExt + ".root");
        hAngles -> GetXaxis() -> SetTitle("Angle projection X [deg]");
        hAngles -> GetYaxis() -> SetTitle("Angle projection Y [deg]");
        cAngles -> SaveAs(destination+"/"+"2DAngHistoFigure_" + Form("%d_", idx) +filename_noExt + "_" + filename_noExt + ".pdf");
        cAngles -> SaveAs(destination+"/"+"2DAngHistoFigure_" + Form("%d_", idx) +filename_noExt + "_" + filename_noExt + ".png");

        cAngles -> Close();

        Edep -> Draw(Form("Ygen:Xgen>>hGenPosition_%d", idx), ConditionGoodEventsSinglePair[idx].Data(), "colz");
        TCanvas *cGenPosition = new TCanvas(Form("cGenPosition_%d", idx), Form("cGenPosition_%d", idx), 900, 700);
        hGenPosition -> Draw("colz");
        hGenPosition -> GetXaxis() -> SetTitle("X [mm]");
        hGenPosition -> GetYaxis() -> SetTitle("Y [mm]");
        cGenPosition -> SaveAs(destination+"/"+"GenPosition_" + Form("%d_", idx) +filename_noExt + "_" + filename_noExt + ".pdf");
        cGenPosition -> SaveAs(destination+"/"+"GenPosition_" + Form("%d_", idx) +filename_noExt + "_" + filename_noExt + ".png");
        cGenPosition -> Close();
        
        Edep -> Draw("ProjY:ProjX", ConditionGoodEventsSinglePair[idx].Data(), "");

        gAngles[idx] = new TGraph(Edep->GetSelectedRows(), Edep->GetV2(), Edep->GetV1());
        gAngles[idx] -> SetMarkerColor(ColorsPlot[idx]);
        gAngles[idx] -> SetMarkerStyle(8);
        gAngles[idx] -> SetMarkerSize(0.6);

        cout << Form("%d \t%g \t%g", idx, gAngles[idx]-> GetRMS(1), gAngles[idx]-> GetRMS(2)) << " " << Edep->GetSelectedRows()<< endl;
        mgAngles -> Add(gAngles[idx]);
    }

    gPad -> SetGrid();
    mgAngles -> GetXaxis() -> SetTitle("Angle projection X [deg]");
    mgAngles -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

    
    if(NameFile.Contains("e-"))
    {
        mgAngles -> GetXaxis() -> SetRangeUser(-40, 40);
        mgAngles -> GetYaxis() -> SetRangeUser(-40, 40);    
    }
    else
    {
    mgAngles -> GetXaxis() -> SetRangeUser(-40, 40);
    mgAngles -> GetYaxis() -> SetRangeUser(-40, 40);
    }


    mgAngles -> Draw("AP");

    cAngleGraphs -> SaveAs( destination+"/"+"Angles_"+filename_noExt+".pdf");
    cAngleGraphs -> SaveAs( destination+"/"+"Angles_"+filename_noExt+".png");




    // Montecarlo quantities plots
    TCanvas *c2 = new TCanvas("c2", "c2", 4000, 2000);
    c2 -> Divide(4, 2);
    for(int i = 0; i < 7; ++i)
    {
        c2 -> cd(i+1);
        Edep -> Draw(Form("%s", BranchName[i].Data()), "", "");
        gPad -> SetLogy();
        gPad -> SetGrid();
    }
    c2 -> SaveAs(destination+"/" + "Montecarlo_" + filename_noExt+ ".pdf");
    c2 -> SaveAs(destination+"/" + "Montecarlo_" + filename_noExt+ ".png");

    // Energy deposition plots
    TCanvas *c3 = new TCanvas("c3", "c3", 5000, 4000);
    c3 -> Divide(4,3);
    for(int i = 0; i < 12; ++i)
    {
        c3 -> cd(i+1);
        if(i == 0)
        {
            TString ToDraw = Form("%s + %s", BranchName[i + iBranchEnergies].Data(), BranchName[i + iBranchEnergies+1].Data());
            Edep -> Draw(Form("%s",ToDraw.Data()), Form("(%s)>0",ToDraw.Data()) , "");
        }
        else
        {
            Edep -> Draw(Form("%s", BranchName[i + 1 + iBranchEnergies].Data()), "", "");
        }
        gPad -> SetLogy();
        gPad -> SetGrid();
    }
    c3 -> SaveAs(destination+"/" + "Energies_" + filename_noExt+ ".pdf");
    c3 -> SaveAs(destination+"/" + "Energies_" + filename_noExt+ ".png");    




    // Transfer function Edep vs Egen
    // Silicon Thin detector


    Edep_old -> SetEntryList(0);

    TCanvas *c4 = new TCanvas("c4", "c4", 1000, 1000);

    

    Edep_old -> Draw("TotThin:RandEnergy", "(TotThin>0.)", "goff");
    double Egen_min = TMath::MinElement(Edep_old->GetSelectedRows(), Edep_old->GetV2());
    double Egen_max = TMath::MaxElement(Edep_old->GetSelectedRows(), Edep_old->GetV2());
    double Edep_min = TMath::MinElement(Edep_old->GetSelectedRows(), Edep_old->GetV1());
    double Edep_max = TMath::MaxElement(Edep_old->GetSelectedRows(), Edep_old->GetV1());


    TH2DLog *h2D_EdepGen_log = new TH2DLog();

    h2D_EdepGen_log -> SetName("h2D_EdepGen");
    h2D_EdepGen_log -> SetXAxis(Egen_min, Egen_max, 200);
    h2D_EdepGen_log -> SetYAxis(Edep_min, Edep_max, 200);
    h2D_EdepGen_log -> SetTitle("Edep vs Egen");
    h2D_EdepGen_log -> SetXTitle("Egen [MeV]");
    h2D_EdepGen_log -> SetYTitle("Edep [MeV]");

    h2D_EdepGen_log -> GenerateHistogram();

    TH2D *h2D_EdepGen = (TH2D*) h2D_EdepGen_log -> GetHistogram();

    Edep_old -> Draw("TotThin_NoThreshold:RandEnergy>>h2D_EdepGen", "(TotThin>0.)", "colz");

    gPad -> SetLogx();
    gPad -> SetLogy();
    gPad -> SetLogz();
    gPad -> SetGrid();


    c4 -> SaveAs(destination+"/" + "EdepEgen_" + filename_noExt+ ".pdf");
    c4 -> SaveAs(destination+"/" + "EdepEgen_" + filename_noExt+ ".png");
    c4 -> SaveAs(destination+"/" + "EdepEgen_" + filename_noExt+ ".root");



    /* ###################################################### */
    /*                 CONFIGURATION FILE INFO                */
    /* ###################################################### */

    // Read the configuration file
    vector<TString> ParticleName_conf;
    vector<double> Emin_conf;
    vector<double> Emax_conf;
    vector<int> EvNumber_conf;
    vector<int> JobNumber_conf;



    ifstream fileConf;
    fileConf.open(pathConfigurationFile.Data());

    if(fileConf.is_open())
    {
        std::cout << "Reading configuration file " << pathConfigurationFile.Data() << std::endl;
        while(!fileConf.eof())
        {
            string line;
            getline(fileConf, line);
            if(line[0] != '#')
            {
                istringstream iss(line);
                TString ParticleName;
                double Emin;
                double Emax;
                int EvNumber;
                int JobNumber;
                iss >> ParticleName >> Emin >> Emax >> EvNumber >> JobNumber;
                ParticleName_conf.push_back(ParticleName);
                Emin_conf.push_back(Emin);
                Emax_conf.push_back(Emax);
                EvNumber_conf.push_back(EvNumber);
                JobNumber_conf.push_back(JobNumber);
            
                cout << "ParticleName = " << ParticleName << endl;
                cout << "Emin = " << Emin << endl;
                cout << "Emax = " << Emax << endl;
                cout << "EvNumber = " << EvNumber << endl;
                cout << "JobNumber = " << JobNumber << endl;
            }
        }
    }
    else
    {
        std::cout << "Error: configuration file " << pathConfigurationFile.Data() << " not found" << std::endl;
        return;
    }


    TH1DLog *hGeomFactor_Thin = new TH1DLog();
    hGeomFactor_Thin -> SetName(Form("hGeomFactor_Thin_%d", indexParticleFile));
    hGeomFactor_Thin -> SetTitle(Form("Geometric Factor %d", indexParticleFile));
    hGeomFactor_Thin -> SetXTitle("E_{kin} [MeV]");
    hGeomFactor_Thin -> SetYTitle("Geometric Factor [cm^{2} sr]");
    hGeomFactor_Thin -> SetXAxis(Emin_conf[indexParticleFile], Emax_conf[indexParticleFile], 80);
    hGeomFactor_Thin -> GenerateHistogram();
    TH1D* hGeomFactor_Thin_Log = hGeomFactor_Thin -> GetHistogram();

    TH1DLog *hGeomFactor_Test = new TH1DLog();
    hGeomFactor_Test -> SetName(Form("hGeomFactor_Test_%d", indexParticleFile));
    hGeomFactor_Test -> SetTitle(Form("Geometric Factor %d", indexParticleFile));
    hGeomFactor_Test -> SetXTitle("E_{kin} [MeV]");
    hGeomFactor_Test -> SetYTitle("Geometric Factor [cm^{2} sr]");
    hGeomFactor_Test -> SetXAxis(Emin_conf[indexParticleFile], Emax_conf[indexParticleFile], 80);
    hGeomFactor_Test -> GenerateHistogram();
    TH1D* hGeomFactor_Test_Log = hGeomFactor_Test -> GetHistogram();


    Edep_old -> Draw(Form("RandEnergy>>hGeomFactor_Thin_%d", indexParticleFile), "(TotThin_NoThreshold > 0.)", "goff");
    Edep_old -> Draw(Form("RandEnergy>>hGeomFactor_Test_%d", indexParticleFile), "", "goff");


    double dN_dE = (double)EvNumber_conf[indexParticleFile] * (double)JobNumber_conf[indexParticleFile] / (Emax_conf[indexParticleFile] - Emin_conf[indexParticleFile]);

    for(int j = 0; j < hGeomFactor_Thin_Log -> GetNbinsX(); ++j)
    {
        double BinWidth = hGeomFactor_Thin_Log -> GetBinWidth(j+1);
        double Yield = hGeomFactor_Thin_Log -> GetBinContent(j+1);
        double YieldError = hGeomFactor_Thin_Log -> GetBinError(j+1);
        hGeomFactor_Thin_Log -> SetBinContent(j+1, Yield / (dN_dE * BinWidth));
        hGeomFactor_Thin_Log -> SetBinError(j+1, YieldError / (dN_dE * BinWidth));

        Yield = hGeomFactor_Test_Log -> GetBinContent(j+1);
        YieldError = hGeomFactor_Test_Log -> GetBinError(j+1);
        hGeomFactor_Test_Log -> SetBinContent(j+1, Yield / (dN_dE * BinWidth));
        hGeomFactor_Test_Log -> SetBinError(j+1, YieldError / (dN_dE * BinWidth));
    }



    hGeomFactor_Thin_Log -> SetLineColor(kRed);
    hGeomFactor_Thin_Log -> SetLineWidth(2);
    hGeomFactor_Thin_Log -> SetMarkerColor(kRed);
    hGeomFactor_Thin_Log -> Scale(AreaGeneration_cm2 * TMath::Pi());

    hGeomFactor_Test_Log -> SetLineColor(kBlue);
    hGeomFactor_Test_Log -> SetLineWidth(2);
    hGeomFactor_Test_Log -> SetMarkerColor(kBlue);
    hGeomFactor_Test_Log -> Scale(AreaGeneration_cm2 * TMath::Pi());



    TCanvas *cGeomFactor_Thin = new TCanvas("cGeomFactor_Thin", "cGeomFactor_Thin", 900, 700);
    hGeomFactor_Thin_Log -> Draw("E1same");
    
    gPad -> SetLogx();
    gPad -> SetLogy();
    gPad -> SetGrid();

    cGeomFactor_Thin -> SaveAs(destination+"/" + "GeomFactor_Thin_SetAlias_" + filename_noExt+ ".pdf");
    cGeomFactor_Thin -> SaveAs(destination+"/" + "GeomFactor_Thin_SetAlias_" + filename_noExt+ ".png");
    cGeomFactor_Thin -> SaveAs(destination+"/" + "GeomFactor_Thin_SetAlias_" + filename_noExt+ ".root");


    TCanvas *cGeomFactor_Test = new TCanvas("cGeomFactor_Test", "cGeomFactor_Test", 900, 700);
    hGeomFactor_Test_Log -> Draw("E1same");

    gPad -> SetLogx();
    gPad -> SetLogy();
    gPad -> SetGrid();

    cGeomFactor_Test -> SaveAs(destination+"/" + "Test_GeomFactor_SetAlias_" + filename_noExt+ ".pdf");
    




    file_alias -> cd();
    Edep       -> Write();
    file_alias -> Write();
    h2D_EdepGen -> Write();
    hGeomFactor_Thin_Log -> Write();
    file_alias -> Close();
    file       -> Close();
    delete file;

    return;
}