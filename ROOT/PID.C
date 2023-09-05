#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "TString.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGraph.h"
#include "./TH1DLog.h"
#include "./TH2DLog.h"
#include "./TH2DLogX.h"
#include "TLine.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TList.h"
#include "TObjArray.h"
#include "TTreeFormula.h"
#include "TEllipse.h"
#include "TObjString.h"


using namespace std;

int PID(
        int     NFiles,
        TString pathFileNames = "",
        TString pathFileEnergies = "",
        TString destination = "",
        double  Emin_plots = 0.01,
        double  Emax_plots = 300.,
        double  E_thr_Thin = 0.04,
        double  E_thr_Thick = 0.04,
        double  E_thr_Plastic = 0.05,
        TString PathSiliconPositionFile = "",
        int     NumberPairsSensors = 5,
        double  ResSilicon = 0.01,
        double  ResPlastic = 0.05,
        double  NCVF_threshold = 0.1
)
{

    // Open the file with name pathFileNames.txt
    ifstream fileNames(pathFileNames.Data());
    
    

    vector<TString> FileNames;
    vector<TString> FileNames_noExt;
    vector<TString> FileNames_noPath;
    vector<double>  Emins;
    vector<double>  Emaxs;
    FileNames.resize(NFiles);
    FileNames_noExt.resize(NFiles);
    FileNames_noPath.resize(NFiles);
    Emins.resize(NFiles);
    Emaxs.resize(NFiles);



    string textline;

    if (fileNames.is_open())
    {
        for(int i = 0; i < NFiles; i++)
        {
            getline(fileNames, textline);
            FileNames[i]        = textline;
            FileNames_noExt[i]  = FileNames[i];
            FileNames_noExt[i].ReplaceAll(".root", "");
            FileNames_noPath[i] =  FileNames_noExt[i](FileNames_noExt[i].Last('/')+1, FileNames_noExt[i].Length());

            cout << "FileNames[" << i << "] = " << FileNames[i] << endl;
            cout << "FileNames_noExt[" << i << "] = " << FileNames_noExt[i] << endl;
            cout << "FileNames_noExt[" << i << "] = " << FileNames_noExt[i] << endl;
            cout << "FileNames_noPath[" << i << "] = " << FileNames_noPath[i] << endl;

        }
    }
    else
    {
        cout << "Unable to open file" << endl;
        return 1;
    }

    /* ###################################################### */
    /*                     PARTICLE NAMES                     */
    /* ###################################################### */

    vector<TString> ParticleNames;
    ParticleNames.resize(NFiles);

    for(int i = 0; i < NFiles; i++)
    {
        string CompleteFileName = FileNames_noExt[i].Data();
        string ParticleName     = CompleteFileName.substr(CompleteFileName.find_last_of("/")+1);
        ParticleNames[i] = ParticleName;
        ParticleNames[i].ReplaceAll("_t0_alias", "");
    }
        


    /* ###################################################### */
    /*                  ENERGIES INFORMATION                  */
    /* ###################################################### */

    // Open the file with name pathFileEnergies.txt
    ifstream fileEnergies(pathFileEnergies.Data());

    if (fileEnergies.is_open())
    {
        for(int i = 0; i < NFiles; i++)
        {
            fileEnergies >> Emins[i];
            fileEnergies >> Emaxs[i];
        }
    }
    else
    {
        cout << "Unable to open file" << endl;
        return 1;
    }


    /* ###################################################### */
    /*                SILICON DETECTOR POSITION               */
    /* ###################################################### */

    // Open the file with name PathSiliconPositionFile.txt
    ifstream fileSiliconPosition(PathSiliconPositionFile.Data());
    // The format of data is the following:
    // NAME X/double Y/double Z/double Radius/double
    // The names contain information on the detector
    // LV_SiliconDetector_Thin_2
    //     - Thin/Thick
    //     - 1/2/3/4/5

    vector<TString> SiliconDetectorNames;
    vector<double>  SiliconDetectorX;
    vector<double>  SiliconDetectorY;
    vector<double>  SiliconDetectorZ;
    vector<double>  SiliconDetectorRadius;
    vector<int>     SiliconDetectorNumber;
    vector<int>     SiliconDetectorThickness;

    double zMeanThin = 0.;
    double zMeanThick = 0.;

    // Convention:
    // 0 = Thin
    // 1 = Thick
    

    string textlineSiliconPosition;

    if (fileSiliconPosition.is_open())
    {
        while(getline(fileSiliconPosition, textlineSiliconPosition))
        {
            // Remove the newline character at the end of the line
            // Split the line using the space as delimiter
            // I used the TObjectArray returned by the Tokenize method
            // There may be better ways to do it...

            TString lineTString = textlineSiliconPosition;
            TObjArray *lineTStringSplitted = lineTString.Tokenize(" ");

            TIter next(lineTStringSplitted);
            vector<TString> lineSplitted;
            TObjString *token;

            while ((token=static_cast<TObjString*>(next())))
            {
                lineSplitted.push_back(token->GetString());
                cout << "token = " << token->GetString() << endl;
            }

            int ThinThick = lineSplitted[0].Atoi();
            int number    = lineSplitted[1].Atoi();
            double x      = lineSplitted[2].Atof();
            double y      = lineSplitted[3].Atof();
            double z      = lineSplitted[4].Atof();
            double radius = lineSplitted[5].Atof();

            // Find the mean z position of the silicon detectors
            if(ThinThick == 0)
            {
                zMeanThin += z/NumberPairsSensors;
            }
            else if(ThinThick == 1)
            {
                zMeanThick += z/NumberPairsSensors;
            }
            else
            {
                cout << "Error in the name of the silicon detector" << endl;
                return 1;
            }

            // Find the number of the detector
            cout << "number = " << number << endl;
            SiliconDetectorX.push_back(x);
            SiliconDetectorY.push_back(y);
            SiliconDetectorZ.push_back(z);
            SiliconDetectorRadius.push_back(radius);
            SiliconDetectorNumber.push_back(number);
            SiliconDetectorThickness.push_back(ThinThick);
        }
    }
    else
    {
        cout << "Unable to open file" << endl;
        return 1;
    }

    fileSiliconPosition.close();
    



    /* ###################################################### */
    /*                      PLOT SETTINGS                     */
    /* ###################################################### */

    double Xmin = Emin_plots;
    double Xmax = Emax_plots;
    double Ymin = -3.2;
    double Ymax = 3.2;
    double Nbins_X = 300;
    double Nbins_Y = 300;

    vector<TFile*> file;
    vector<TTree*> Edep;
    file.resize(NFiles);
    Edep.resize(NFiles);

    for(int i = 0; i < NFiles; i++)
    {
        file[i] = new TFile(FileNames[i], "READ");
        Edep[i] = (TTree*)file[i]->Get("Edep");

        // Get List Of Aliases
        TList *listOfAliases = Edep[i] -> GetListOfAliases();
        for(int j = 0; j < listOfAliases -> GetEntries(); ++j)
        {
            TString AliasName = listOfAliases -> At(j) -> GetName();
            TString AliasDefinition = listOfAliases -> At(j) -> GetTitle();
            cout << "AliasName = " << AliasName << endl;
            cout << "AliasDefinition = " << AliasDefinition << endl;
        }
    }



    TString AliasETot = "ETot";

    TString DirName[3]       = {"DirX", "DirY", "DirZ"};
    TString PolarAngle[2]    = {"dirTheta", "dirPhi"};
    TString NewPolarAngle[2] = {"dirThetaNew", "dirPhiNew"};


    int NumberBranches = 7 + 3 + 2*NumberPairsSensors;
    std::cout << "NumberBranches = " << NumberBranches << endl;


    vector<TString> BranchName;
    BranchName.resize(NumberBranches);

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
        BranchName[iBranch]                    = "Ed_LV_SiliconDetector_Thin_" + to_string(iPair);
        BranchName[iBranch+NumberPairsSensors] = "Ed_LV_SiliconDetector_Thick_" + to_string(iPair);
        iBranch++;
    }
    
    for(int i = 0; i < NumberBranches; i++)
    {
        std::cout << "BranchName[" << i << "] = " << BranchName[i] << endl;
    }



    vector<TString> ConditionPairSilicon;
    ConditionPairSilicon.resize(NumberPairsSensors);
    TString ConditionPairSiliconAll;
    TString ConditionDrilledVeto;
    TString ConditionGoodEvents;
    vector<TString> ConditionGoodEventsSinglePair;
    ConditionGoodEventsSinglePair.resize(NumberPairsSensors);
    TString ConditionNoCalo;
    TString NumberPairsHit;


    ConditionDrilledVeto = Form("(%s < %g) && (%s < %g)", BranchName[iBranchEnergies].Data(), E_thr_Plastic, BranchName[iBranchEnergies + 1].Data(), E_thr_Plastic);

    // Defining the conditions for tagged as good events
    for(int i = 0; i< NumberPairsSensors; ++i)
    {
        ConditionPairSilicon[i]          = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionDrilledVeto.Data());
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
            NumberPairsHit          = Form("(1*((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
        else 
        {
            ConditionPairSiliconAll += Form("|| ((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
            NumberPairsHit          += Form(" + 1*((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
    }
    NumberPairsHit      += ")";
    ConditionGoodEvents  = Form("(%s < 2.0) && (%s) && (%s)", NumberPairsHit.Data(), ConditionPairSiliconAll.Data(), ConditionDrilledVeto.Data());
    ConditionNoCalo      = Form("(%s < %g)", BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);




    TFile *fileOut = new TFile(destination +"/Plots.root", "RECREATE");

    TString destination_PID          = destination + "/PID_plots";
    TString destination_DeadMaterial = destination + "/DeadMaterial_plots";

    gSystem -> mkdir(destination_PID.Data(), true);
    gSystem -> mkdir(destination_DeadMaterial.Data(), true);

    
    vector<TH2DLogX*> hPID_log;
    vector<TH2D*>     hPID;
    vector<TString>   hPID_XTitle;
    vector<TString>   hPID_YTitle;
    vector<TString>   DrawRule;
    vector<TString>   Condition;
    vector<TString>   SaveAsPath;
    vector<TCanvas*>  cPID;

    int N_Histos = 14;

    hPID_log.resize(N_Histos);
    hPID.resize(N_Histos);
    hPID_XTitle.resize(N_Histos);
    hPID_YTitle.resize(N_Histos);
    DrawRule.resize(N_Histos);
    Condition.resize(N_Histos);
    SaveAsPath.resize(N_Histos);
    cPID.resize(N_Histos);

    int IndexHisto = 0;

    /* ###################################################### */
    /*             TEMPLATE FOR HISTOGRAM DRAWING             */
    /* ###################################################### */

    // hPID_Name[IndexHisto]: Assigned later in the loop
    // hPID_Title[IndexHisto] = "";
    // hPID_XTitle[IndexHisto] = "";
    // hPID_YTitle[IndexHisto] = "";
    // DrawRule[IndexHisto] = "";
    // Condition[IndexHisto] = "";
    // SaveAsPath[IndexHisto] = "";


    // Histo 0
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick + TotThin + Ed_LV_Calo)";
    Condition[IndexHisto]   = ConditionGoodEvents.Data();
    SaveAsPath[IndexHisto]  = destination_PID + "/PID";

    // Histo 1
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "gPID:(gTotThick + gTotThin + gEd_LV_Calo)";
    Condition[IndexHisto]   = ConditionGoodEvents.Data();
    SaveAsPath[IndexHisto]  = destination_PID + "/gPID";

    // Histo 2
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick + TotThin)";
    Condition[IndexHisto]   = Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_NoCalo";

    // Histo 3
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "gPID:(gTotThick + gTotThin)";
    Condition[IndexHisto]   = Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/gPID_NoCalo";

    // Histo 4
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} MC [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(RandEnergy - Ed_LV_AlTop)";
    Condition[IndexHisto]   = ConditionGoodEvents.Data();
    SaveAsPath[IndexHisto]  = destination_PID + "/PID2";

    // Histo 5
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} MC [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "gPID:(RandEnergy - Ed_LV_AlTop)";
    Condition[IndexHisto]   = ConditionGoodEvents.Data();
    SaveAsPath[IndexHisto]  = destination_PID + "/gPID2";

    // Histo 6
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} MC [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(RandEnergy - Ed_LV_AlTop)";
    Condition[IndexHisto]   = Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/PID2_NoCalo";

    // Histo 7
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} MC [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "gPID:(RandEnergy - Ed_LV_AlTop)";
    Condition[IndexHisto]   = Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/gPID2_NoCalo";


    // Histo 8
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick + TotThin + Ed_LV_Calo)";
    Condition[IndexHisto]   = Form("(%s)&&(NCVF > %g)", ConditionGoodEvents.Data(), NCVF_threshold);
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_NCVF_NOK";

    // Histo 9
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick + TotThin + Ed_LV_Calo)";
    Condition[IndexHisto]   = Form("(%s)&&(NCVF > %g) && (%s)", ConditionGoodEvents.Data(), NCVF_threshold, ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_NCVF_NOK_NoCalo";

    // Histo 10
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick + TotThin + Ed_LV_Calo)";
    Condition[IndexHisto]   = Form("(%s)&&(NCVF < %g)", ConditionGoodEvents.Data(), NCVF_threshold);
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_NCVF_OK";

    // Histo 11
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{tot} Reco [MeV]";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick + TotThin + Ed_LV_Calo)";
    Condition[IndexHisto]   = Form("(%s)&&(NCVF < %g) && (%s)", ConditionGoodEvents.Data(), NCVF_threshold, ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_NCVF_OK_NoCalo";
    
    // Histo 12
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{Thick} / E_{Thin}";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick / TotThin)";
    Condition[IndexHisto]   = Form("(%s) && (%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_ThickThin_NoCalo";

    // Histo 13
    IndexHisto++;
    hPID_XTitle[IndexHisto] = "E_{Thick} / E_{Thin}";
    hPID_YTitle[IndexHisto] = "PID";
    DrawRule[IndexHisto]    = "PID:(TotThick / TotThin)";
    Condition[IndexHisto]   = Form("(%s)", ConditionGoodEvents.Data());
    SaveAsPath[IndexHisto]  = destination_PID + "/PID_ThickThin";



    // Loop 
    for(int i = 0; i < N_Histos; ++i)
    {
        cout << "Drawing Histo " << i << endl;
        hPID_log[i] = new TH2DLogX();
        hPID_log[i] -> SetName(Form("hPID_%d", i));
        hPID_log[i] -> SetTitle(Form("hPID_%d", i));
        hPID_log[i] -> SetXTitle(hPID_XTitle[i].Data());
        hPID_log[i] -> SetYTitle(hPID_YTitle[i].Data());
        hPID_log[i] -> SetXAxis(Xmin,Xmax, Nbins_X);
        if(i >= 12)
        {
            hPID_log[i] -> SetXAxis(0.001,1000, Nbins_X);
        }
        hPID_log[i] -> SetYAxis(Ymin,Ymax, Nbins_Y);
        hPID_log[i] -> GenerateHistogram();
        hPID[i] = (TH2D*)hPID_log[i] -> GetHistogram();
        
        
        for(int j = 0; j < NFiles; ++j)
        {
            if(j == 0)
            {
                Edep[j] -> Draw(Form("%s>>hPID_%d", DrawRule[i].Data(), i), Condition[i].Data(), "goff");
            }
            else
            {
                Edep[j] -> Draw(Form("%s>>+hPID_%d", DrawRule[i].Data(), i), Condition[i].Data(), "goff");
            }
        }
        //hPID_log[i] -> SaveAs(SaveAsPath[i].Data());
    }

    for(int i = 0; i < N_Histos; ++i)
    {
        cPID[i] = new TCanvas(Form("cPID_%d", i), Form("cPID_%d", i), 1000, 1000);
        cPID[i] -> cd();
        hPID[i] -> Draw("colz");
        gPad -> SetLogx();
        gPad -> SetLogz();
        gPad -> SetGridx();
        gPad -> SetGridy();


        TString PathPDF = SaveAsPath[i] + ".pdf";
        TString PathPNG = SaveAsPath[i] + ".png";
        cPID[i] -> SaveAs(PathPDF.Data());
        cPID[i] -> SaveAs(PathPNG.Data());
    }

    cout << "Done: Saved all the plots" << endl;

    vector<TObjArray*> ListOfBranches;
    ListOfBranches.resize(NFiles);

    cout << "Reading Files" << endl;
    for(int i = 0; i < NFiles; ++i)
    {

        // NCVF Map
        // Xgen, Ygen, Zgen
        // pDirX, pDirY, pDirZ
        cout << "Reading File " << i << endl;
        cout << "zMeanThick = " << zMeanThick << endl;
        double H = zMeanThin;

        vector<double> vXimpact, vYimpact, vNCVF;
        vector<bool> CaloTriggered;

        double bXgen, bYgen, bZgen, bpDirX, bpDirY, bpDirZ, bNCVF;
        Edep[i] -> SetBranchAddress("Xgen", &bXgen);
        Edep[i] -> SetBranchAddress("Ygen", &bYgen);
        Edep[i] -> SetBranchAddress("Zgen", &bZgen);
        Edep[i] -> SetBranchAddress("pDirX", &bpDirX);
        Edep[i] -> SetBranchAddress("pDirY", &bpDirY);
        Edep[i] -> SetBranchAddress("pDirZ", &bpDirZ);
        TTreeFormula *fNCVF = new TTreeFormula("fNCVF", Edep[i]->GetAlias("NCVF"), Edep[i]);
        TTreeFormula *fConditionGoodEvents_Calo = new TTreeFormula("fConditionGoodEvents", Form("(%s) && !(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), Edep[i]);
        TTreeFormula *fConditionGoodEvents_NoCalo = new TTreeFormula("fConditionGoodEvents", Form("(%s) && (%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), Edep[i]);

        cout << "Edep[i]->GetEntries() = " << Edep[i]->GetEntries() << endl;

        for(int j = 0; j < Edep[i]->GetEntries(); ++j)
        {
            Edep[i] -> GetEntry(j);
            double Ximpact = bXgen + (H - bZgen) * bpDirX / bpDirZ;
            double Yimpact = bYgen + (H - bZgen) * bpDirY / bpDirZ;
            double NCVF = fNCVF -> EvalInstance();

            if(fConditionGoodEvents_Calo -> EvalInstance() == 1)
            {
                vXimpact.push_back(Ximpact);
                vYimpact.push_back(Yimpact);
                vNCVF.push_back(NCVF);
                CaloTriggered.push_back(true);
            }
            else if(fConditionGoodEvents_NoCalo -> EvalInstance() == 1)
            {
                vXimpact.push_back(Ximpact);
                vYimpact.push_back(Yimpact);
                vNCVF.push_back(NCVF);
                CaloTriggered.push_back(false);
            }
        }

        // Now I want to bin the NCVF map in a 2D histogram (TGraph2D)
        // I want to bin it in 2D because I want to plot it in 2D

        double Ximpact_max = *max_element(vXimpact.begin(), vXimpact.end());
        double Ximpact_min = *min_element(vXimpact.begin(), vXimpact.end());
        double Yimpact_max = *max_element(vYimpact.begin(), vYimpact.end());
        double Yimpact_min = *min_element(vYimpact.begin(), vYimpact.end());

        int Nbins_Ximpact = 80;
        int Nbins_Yimpact = 80;

        TH2D *hNCVF_Calo = new TH2D("hNCVF_Calo", "hNCVF_Calo", Nbins_Ximpact, Ximpact_min, Ximpact_max, Nbins_Yimpact, Yimpact_min, Yimpact_max);
        TH2D *hNCVF_NoCalo = new TH2D("hNCVF_NoCalo", "hNCVF_NoCalo", Nbins_Ximpact, Ximpact_min, Ximpact_max, Nbins_Yimpact, Yimpact_min, Yimpact_max);
        // Now loop on the bins and fill the histogram setting the NCVF as the mean of the values found in the bin.
        // If there are no values in the bin, set the NCVF to -1

        vector< vector< vector<double> > > vNCVF_bin_Calo;
        vector< vector< vector<double> > > vNCVF_bin_NoCalo;

        vNCVF_bin_Calo.resize(Nbins_Ximpact+2);
        vNCVF_bin_NoCalo.resize(Nbins_Ximpact+2);
        for(int j = 0; j < vNCVF_bin_Calo.size(); ++j)
        {
            vNCVF_bin_Calo[j].resize(Nbins_Yimpact+2);
            vNCVF_bin_NoCalo[j].resize(Nbins_Yimpact+2);
            for(int k = 0; k < vNCVF_bin_Calo[j].size(); ++k)
            {
                //cout << "resizing vNCVF_bin[" << j << "][" << k << "]" << endl;
                vNCVF_bin_Calo[j][k].clear();
                vNCVF_bin_NoCalo[j][k].clear();
            }
        }

        for(int j = 0; j < vXimpact.size(); ++j)
        {
            //cout << "j = " << j << endl;
            int binX = hNCVF_Calo -> GetXaxis() -> FindBin(vXimpact[j]);
            int binY = hNCVF_Calo -> GetYaxis() -> FindBin(vYimpact[j]);
            cout << "binX = " << binX << ", binY = " << binY << endl;
            
            if(CaloTriggered[j])
                vNCVF_bin_Calo[binX][binY].push_back(vNCVF[j]);
            else
                vNCVF_bin_NoCalo[binX][binY].push_back(vNCVF[j]);
        }

        for(int j = 0; j < Nbins_Ximpact; ++j)
        {
            for(int k = 0; k < Nbins_Yimpact; ++k)
            {
                //cout << "j = " << j << ", k = " << k << endl;
                if(vNCVF_bin_Calo[j][k].size() == 0)
                {
                    hNCVF_Calo -> SetBinContent(j, k, -1);
                }
                else
                {
                    double NCVF_mean_Calo = 0;
                    int NCVF_mean_Calo_counter = 0;
                    for(int l = 0; l < vNCVF_bin_Calo[j][k].size(); ++l)
                    {
                        if(vNCVF_bin_Calo[j][k][l] != -1)
                        {
                            NCVF_mean_Calo += vNCVF_bin_Calo[j][k][l];
                            NCVF_mean_Calo_counter++;
                        }
                    }
                    NCVF_mean_Calo /= NCVF_mean_Calo_counter;
                    hNCVF_Calo -> SetBinContent(j, k, NCVF_mean_Calo);
                }

                if(vNCVF_bin_NoCalo[j][k].size() == 0)
                {
                    hNCVF_NoCalo -> SetBinContent(j, k, -1);
                }
                else
                {
                    double NCVF_mean_NoCalo = 0;
                    int NCVF_mean_NoCalo_counter = 0;
                    for(int l = 0; l < vNCVF_bin_NoCalo[j][k].size(); ++l)
                    {
                        if(vNCVF_bin_NoCalo[j][k][l] != -1)
                        {
                            NCVF_mean_NoCalo += vNCVF_bin_NoCalo[j][k][l];
                            NCVF_mean_NoCalo_counter++;
                        }
                    }
                    NCVF_mean_NoCalo /= NCVF_mean_NoCalo_counter;
                    hNCVF_NoCalo -> SetBinContent(j, k, NCVF_mean_NoCalo);
                }
            }
        }



        TCanvas *cNCVF = new TCanvas("cNCVF", "cNCVF", 2000, 1000);
        cNCVF -> Divide(2, 1);
        cNCVF -> cd(1);
        hNCVF_Calo -> Draw("colz");
        hNCVF_Calo -> SetStats(0);

        vector<TEllipse*> vEllipse;
        vEllipse.resize(4*NumberPairsSensors);

        // vector<double> SiliconDetectorX;
        // vector<double> SiliconDetectorY;
        // vector<double> SiliconDetectorZ;
        // vector<double> SiliconDetectorRadius;
        // vector<int> SiliconDetectorNumber;
        // vector<int> SiliconDetectorThickness;


        for(int j = 0; j < 2*NumberPairsSensors; ++j)
        {
            vEllipse[j] = new TEllipse(SiliconDetectorX[j], SiliconDetectorY[j], SiliconDetectorRadius[j], SiliconDetectorRadius[j]);
            vEllipse[j] -> SetFillStyle(0);
            if(SiliconDetectorThickness[j] == 1)
            {
                vEllipse[j] -> SetLineColor(kRed);
                vEllipse[j] -> SetLineWidth(2);
                vEllipse[j] -> SetLineStyle(2);
            }
            else
            {
                vEllipse[j] -> SetLineColor(kYellow);
                vEllipse[j] -> SetLineWidth(2);
                vEllipse[j] -> SetLineStyle(3);
            }
            vEllipse[j] -> Draw("same");
        }
            





        cNCVF -> cd(2);
        hNCVF_NoCalo -> Draw("colz");
        hNCVF_NoCalo -> SetStats(0);
        // Draw again 
        for(int j = 0; j < 2*NumberPairsSensors; ++j)
        {
            vEllipse[j+2*NumberPairsSensors] = new TEllipse(SiliconDetectorX[j], SiliconDetectorY[j], SiliconDetectorRadius[j], SiliconDetectorRadius[j]);
            vEllipse[j+2*NumberPairsSensors] -> SetFillStyle(0);
            if(SiliconDetectorThickness[j] == 1)
            {
                vEllipse[j+2*NumberPairsSensors] -> SetLineColor(kRed);
                vEllipse[j+2*NumberPairsSensors] -> SetLineWidth(2);
                vEllipse[j+2*NumberPairsSensors] -> SetLineStyle(2);
            }
            else
            {
                vEllipse[j+2*NumberPairsSensors] -> SetLineColor(kYellow);
                vEllipse[j+2*NumberPairsSensors] -> SetLineWidth(2);
                vEllipse[j+2*NumberPairsSensors] -> SetLineStyle(3);
            }        
            vEllipse[j+2*NumberPairsSensors] -> Draw("same");
        }


        cNCVF -> SaveAs(destination_DeadMaterial + "/NCVF_" + FileNames_noPath[i] + ".pdf");
        cNCVF -> SaveAs(destination_DeadMaterial + "/NCVF_" + FileNames_noPath[i] + ".png");
        cNCVF -> SaveAs(destination_DeadMaterial + "/NCVF_" + FileNames_noPath[i] + ".root");



        ListOfBranches[i] = Edep[i] -> GetListOfBranches();
        // Get the size of this list
        int NBranchesFromFile = ListOfBranches[i] -> GetEntries();
        int NPlots = NBranchesFromFile - NumberBranches; // I want to plot all the branches not enumerated in the Branch Names before

        vector<TString> BranchesToPlot;
        BranchesToPlot.resize(NPlots);
        vector<TCanvas*> c;
        c.resize(NPlots);

        int indexBranch = 0;


        for(int j = 0; j < NBranchesFromFile; ++j)
        {
            TString CurrentBranch = ListOfBranches[i] -> At(j) -> GetName();
            cout << CurrentBranch.Data() << endl;
            // Loop over BranchName[NumberBranches] and check if CurrentBranch is in the list
            // If not push it back in BranchesToPlot
            // Do not use the == because it is too strict

            bool IsInList = false;
            for(int k = 0; k < NumberBranches; ++k)
            {
                if( (CurrentBranch.Contains(BranchName[k].Data())) || BranchName[k].Contains(CurrentBranch.Data()) )
                {
                    IsInList = true;
                }
            }
            if(!IsInList)
            {
                BranchesToPlot[indexBranch++] = CurrentBranch;
                cout << "Pushing back " << CurrentBranch.Data() << endl;
            }
        }

        cout << "Number of branches to plot " << BranchesToPlot.size() << endl;
        cout << "Number of branches in the file " << NBranchesFromFile << endl;
        cout << "Number of branches in the list " << ListOfBranches[i] -> GetEntries() << endl;
        cout << "File name " << FileNames_noPath[i].Data() << endl;
        for(int j = 0; j < NPlots; ++j)
        {
            cout << "Plotting " << BranchesToPlot[j].Data() << endl;
            c[j] = new TCanvas(Form("c_%d_%d", i, j), Form("c_%d_%d", i, j), 1000, 1000);
            TString ConditionPlot = Form("(%s)&&(NCVF > %g)", ConditionGoodEvents.Data(), NCVF_threshold);
            Edep[i] -> Draw(Form("%s", BranchesToPlot[j].Data()), ConditionPlot.Data(), "");
            gPad -> SetGridx();
            gPad -> SetGridy();
            gPad -> SetLogy();
            c[j] -> SaveAs(Form("%s/%s_%s.pdf", destination_DeadMaterial.Data(), FileNames_noPath[i].Data(),BranchesToPlot[j].Data()));
            c[j] -> SaveAs(Form("%s/%s_%s.png", destination_DeadMaterial.Data(), FileNames_noPath[i].Data(),BranchesToPlot[j].Data()));

        }
    }







    // TH2DLogX *hPID_log[4];
    // TH2D *hPID[4];

    // TH2DLogX *hPID2_log[4];
    // TH2D *hPID2[4];

    // Index 0: PID Thin + Thick + Calo
    // Index 1: PID gThin + gThick + gCalo
    // Index 2: PID Thin + Thick NO Calo
    // Index 3: PID gThin + gThick NO Calo

    // for(int i = 0; i < 4; ++i)
    // {
    //     hPID_log[i] = new TH2DLogX();
    //     hPID_log[i] -> SetName(Form("hPID_%d", i));
    //     hPID_log[i] -> SetTitle(Form("hPID_%d", i));
    //     hPID_log[i] -> SetXTitle("E_{tot} [MeV]");
    //     hPID_log[i] -> SetYTitle("PID");
    //     hPID_log[i] -> SetXAxis(Xmin, Xmax, 300);
    //     hPID_log[i] -> SetYAxis(-3., +3.2, 300);
    //     hPID_log[i] -> GenerateHistogram();
    //     hPID[i] = (TH2D*) hPID_log[i] -> GetHistogram();

    //     hPID2_log[i] = new TH2DLogX();
    //     hPID2_log[i] -> SetName(Form("hPID2_%d", i));
    //     hPID2_log[i] -> SetTitle(Form("hPID2_%d", i));
    //     hPID2_log[i] -> SetXTitle("E_{tot} [MeV]");
    //     hPID2_log[i] -> SetYTitle("PID");
    //     hPID2_log[i] -> SetXAxis(Xmin, Xmax, 300);
    //     hPID2_log[i] -> SetYAxis(-3., +3.2, 300);
    //     hPID2_log[i] -> GenerateHistogram();
    //     hPID2[i] = (TH2D*) hPID2_log[i] -> GetHistogram();
    // }


    // for(int i = 0; i < NFiles; ++i)
    // {
    //     if(i == 0)
    //     {
    //         Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Calo)>>hPID_0", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("gPID:(gTotThick + gTotThin + gEd_LV_Calo)>>hPID_1", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("PID:(TotThick + TotThin)>>hPID_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
    //         Edep[i] -> Draw("gPID:(gTotThick + gTotThin)>>hPID_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
        
    //         Edep[i] -> Draw("PID:RandEnergy>>hPID2_0", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("gPID:RandEnergy>>hPID2_1", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("PID:RandEnergy>>hPID2_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
    //         Edep[i] -> Draw("gPID:RandEnergy>>hPID2_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
        
    //     }
    //     else
    //     {
    //         Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Calo)>>+hPID_0", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("gPID:(gTotThick + gTotThin + gEd_LV_Calo)>>+hPID_1", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("PID:(TotThick + TotThin)>>+hPID_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
    //         Edep[i] -> Draw("gPID:(gTotThick + gTotThin)>>+hPID_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
        
    //         Edep[i] -> Draw("PID:RandEnergy>>+hPID2_0", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("gPID:RandEnergy>>+hPID2_1", ConditionGoodEvents.Data(), "colz");
    //         Edep[i] -> Draw("PID:RandEnergy>>+hPID2_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
    //         Edep[i] -> Draw("gPID:RandEnergy>>+hPID2_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");

    //     }
    // }



    // TH2DLogX *hPID_single;
    // TCanvas *cPID_single;
    // TH2D *hPIDlog_single;
    
    // cout << "Plotting Single PID" << endl;
    // for(int i = 0; i < 10; ++i)
    // {
    //     cout << "########################" << endl;
    // }

    



    // for(int i = 0; i < NFiles; ++i)
    // {
    //     hPID_single = new TH2DLogX();
    //     hPID_single -> SetName("hPID_single");
    //     hPID_single -> SetTitle(ParticleNames[i].Data());
    //     hPID_single -> SetXTitle("E_{tot} [MeV]");
    //     hPID_single -> SetYTitle("PID");
    //     hPID_single -> SetXAxis(Emins[i], Emaxs[i], 200);
    //     hPID_single -> SetYAxis(-3., +3.2, 200);
    //     hPID_single -> GenerateHistogram();

    //     hPIDlog_single = (TH2D*) hPID_single -> GetHistogram();
        
    //     Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Calo)>>hPID_single", ConditionGoodEvents.Data(), "colz");
    //     cPID_single = new TCanvas("cPID_single", "cPID_single", 600, 600);

    //     hPIDlog_single -> Draw("colz");
    //     gPad -> SetLogx();
    //     gPad -> SetLogz();
    //     gPad -> SetGrid();
    //     cPID_single -> SaveAs(destination_PID + Form("/PID_single_%s.pdf", ParticleNames[i].Data()));
    //     delete hPID_single;
    //     delete cPID_single;
    //     delete hPIDlog_single;
    // }
    // for(int i = 0; i < 10; ++i)
    // {
    //     cout << "########################" << endl;
    // }

    vector<TGraph*> grPID;
    grPID.resize(NFiles);

    TMultiGraph *mgrPID = new TMultiGraph("mgrPID", "mgrPID");
    TLegend *legPID = new TLegend(0.1, 0.7, 0.4, 0.9);
    for(int i = 0; i < NFiles; ++i)
    {
        Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Calo)", Form("(%s) && (!(%s))", ConditionGoodEvents.Data(), ConditionGoodEventsSinglePair[0].Data()), "colz"); // Form("(%s) && (TotThick > 0.0) && (TotThin > 0.0)",ConditionGoodEvents.Data())
        grPID[i] = new TGraph(Edep[i] -> GetSelectedRows(), Edep[i] -> GetV2(), Edep[i] -> GetV1());
        //grPID[i] -> Print();
        grPID[i] -> SetName(ParticleNames[i].Data());
        grPID[i] -> SetMarkerStyle(20);
        grPID[i] -> SetMarkerSize(0.5);
        grPID[i] -> SetMarkerColor(i + 1);
        grPID[i] -> SetLineColor(i + 1);
        grPID[i] -> SetLineWidth(2);
        mgrPID -> Add(grPID[i]);
        legPID -> AddEntry(grPID[i], ParticleNames[i].Data(), "lp");
    }

    TCanvas *cgraphPID = new TCanvas("cgPID", "cgPID", 600, 600);
    mgrPID -> Draw("ap");
    mgrPID -> GetXaxis() -> SetTitle("E_{tot}^{reco} [MeV]");
    mgrPID -> GetYaxis() -> SetTitle("PID");
    mgrPID -> SetTitle("PID only lateral");
    gPad -> SetLogx();
    gPad -> SetGrid();
    legPID -> Draw();
    cgraphPID -> SaveAs(destination_PID + "/graph_PID_lateral.pdf");
    cgraphPID -> SaveAs(destination_PID + "/graph_PID_lateral.png");    
    delete cgraphPID;


    mgrPID = new TMultiGraph("mgrPID", "mgrPID");
    legPID = new TLegend(0.1, 0.7, 0.4, 0.9);
    for(int i = 0; i < NFiles; ++i)
    {
        Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Calo)", ConditionGoodEventsSinglePair[0].Data(), "colz"); // Form("(%s) && (TotThick > 0.0) && (TotThin > 0.0)",ConditionGoodEvents.Data())
        grPID[i] = new TGraph(Edep[i] -> GetSelectedRows(), Edep[i] -> GetV2(), Edep[i] -> GetV1());
        //grPID[i] -> Print();
        grPID[i] -> SetName(ParticleNames[i].Data());
        grPID[i] -> SetMarkerStyle(20);
        grPID[i] -> SetMarkerSize(0.5);
        grPID[i] -> SetMarkerColor(i + 1);
        grPID[i] -> SetLineColor(i + 1);
        grPID[i] -> SetLineWidth(2);
        mgrPID -> Add(grPID[i]);
        legPID -> AddEntry(grPID[i], ParticleNames[i].Data(), "lp");
    }

    cgraphPID = new TCanvas("cgPID", "cgPID", 600, 600);
    mgrPID -> Draw("ap");
    mgrPID -> GetXaxis() -> SetTitle("E_{tot}^{reco} [MeV]");
    mgrPID -> GetYaxis() -> SetTitle("PID");
    mgrPID -> SetTitle("PID only single pair center");
    gPad -> SetLogx();
    gPad -> SetGrid();
    legPID -> Draw();
    cgraphPID -> SaveAs(destination_PID + "/graph_PID_center.pdf");
    cgraphPID -> SaveAs(destination_PID + "/graph_PID_center.png");
    delete cgraphPID;






    // cPID = new TCanvas("cPID", "cPID", 600, 600);
    // hPID[0] -> Draw("colz");
    // hPID[0] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cPID -> SaveAs(destination_PID + "/PID.pdf");
    // cPID -> SaveAs(destination_PID + "/PID.root");
    // cgPID = new TCanvas("cgPID", "cgPID", 600, 600);
    // hPID[1] -> Draw("colz");
    // hPID[1] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cgPID -> SaveAs(destination_PID + "/gPID.pdf");
    // cgPID -> SaveAs(destination_PID + "/gPID.root");
    // cPID_NoCalo = new TCanvas("cPID_NoCalo", "cPID_NoCalo", 600, 600);
    // hPID[2] -> Draw("colz");
    // hPID[2] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cPID_NoCalo -> SaveAs(destination_PID + "/PID_NoCalo.pdf");
    // cPID_NoCalo -> SaveAs(destination_PID + "/PID_NoCalo.root");

    // cout << "Plotting PID2" << endl << endl << endl;
    // cgPID_NoCalo = new TCanvas("cgPID_NoCalo", "cgPID_NoCalo", 600, 600);
    // cout << hPID[3] -> GetEntries() << endl;
    // hPID[3] -> Draw("colz");
    // hPID[3] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cgPID_NoCalo -> SaveAs(destination_PID + "/gPID_NoCalo.pdf");
    // cgPID_NoCalo -> SaveAs(destination_PID + "/gPID_NoCalo.root");

    // cPID2 = new TCanvas("cPID2", "cPID2", 600, 600);
    // hPID2[0] -> Draw("colz");
    // hPID2[0] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cPID2 -> SaveAs(destination_PID + "/PID2.pdf");
    
    // cgPID2 = new TCanvas("cgPID2", "cgPID2", 600, 600);
    // hPID2[1] -> Draw("colz");
    // hPID2[1] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cgPID2 -> SaveAs(destination_PID + "/gPID2.pdf");

    // cPID2_NoCalo = new TCanvas("cPID2_NoCalo", "cPID2_NoCalo", 600, 600);
    // hPID2[2] -> Draw("colz");
    // hPID2[2] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cPID2_NoCalo -> SaveAs(destination_PID + "/PID2_NoCalo.pdf");

    // cgPID2_NoCalo = new TCanvas("cgPID2_NoCalo", "cgPID2_NoCalo", 600, 600);
    // hPID2[3] -> Draw("colz");
    // hPID2[3] -> SetStats(0);
    // gPad -> SetLogx();
    // gPad -> SetLogz();
    // gPad -> SetGridx();
    // gPad -> SetGridy();
    // cgPID2_NoCalo -> SaveAs(destination_PID + "/gPID2_NoCalo.pdf");


    TCanvas *cDisentangle = new TCanvas("cDisentangle", "cDisentangle", 600, 600);
    TH2D *hDisentangle = new TH2D("hDisentangle", "hDisentangle", 300, -2.5, 4., 300, -2.5, 4.);

    for(int i = 0; i < NFiles; ++i)
    {

        TString ConditionGoodEvents_PID2 = Form("(%s) && (Ed_LV_Calo > %g)", ConditionGoodEvents.Data(), E_thr_Plastic);
        if(i == 0)
        {
            Edep[i] -> Draw("PID:(TMath::Log10(TotThick * (TotThick + Ed_LV_Calo)))>>hDisentangle", ConditionGoodEvents_PID2.Data(), "colz");
        }
        else
        {
            Edep[i] -> Draw("PID:(TMath::Log10(TotThick * (TotThick + Ed_LV_Calo)))>>+hDisentangle", ConditionGoodEvents_PID2.Data(), "colz");
        }
    }
    hDisentangle -> SetStats(0);
    hDisentangle -> SetXTitle("log_{10} ( E_{Thick} * (E_{Thick} + E_{Calo})");
    hDisentangle -> SetYTitle("PID");
    gPad -> SetLogz();
    gPad -> SetGrid();
    cDisentangle -> SaveAs(destination + "/Disentangle.pdf");
    cDisentangle -> SaveAs(destination + "/Disentangle.root");


    TH2DLog *hDeltaE_E_log = new TH2DLog();
    TH2D *hDeltaE_E;

    hDeltaE_E_log -> SetName("hDeltaE_E_log");
    hDeltaE_E_log -> SetTitle("#Delta E - E diagram");
    hDeltaE_E_log -> SetXTitle("E_{tot} [MeV]");
    hDeltaE_E_log -> SetYTitle("#Delta E [MeV]");
    hDeltaE_E_log -> SetXAxis(1e-2, 200, 600);
    hDeltaE_E_log -> SetYAxis(1e-2, 200, 600);
    hDeltaE_E_log -> GenerateHistogram();
    hDeltaE_E = hDeltaE_E_log -> GetHistogram();

    for(int i = 0; i < NFiles; ++i)
    {
        if(i == 0)
        {
            Edep[i] -> Draw("TotThin:TotThick>>hDeltaE_E_log", ConditionGoodEvents.Data(), "colz");
        }
        else
        {
            Edep[i] -> Draw("TotThin:TotThick>>+hDeltaE_E_log", ConditionGoodEvents.Data(), "colz");
        }
    }

    TCanvas *cDeltaE_E = new TCanvas("cDeltaE_E", "cDeltaE_E", 800, 600);
    hDeltaE_E -> Draw("colz");
    hDeltaE_E -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogy();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cDeltaE_E -> SaveAs(destination_PID + "/DeltaE_E.pdf");



    vector<TH1DLog*> hYield;
    vector<TH1D*> hYield_Log;
    hYield.resize(NFiles);
    hYield_Log.resize(NFiles);


    vector<TH1DLog*> hGen;
    vector<TH1D*> hGen_Log;
    hGen.resize(NFiles);
    hGen_Log.resize(NFiles);


    for(int i =0 ; i < NFiles; ++i)
    {
        hYield[i] = new TH1DLog();
        hYield[i] -> SetName(Form("hYield_%d", i));
        hYield[i] -> SetTitle(Form("Yield %d", i));
        hYield[i] -> SetXTitle("E_{kin} [MeV]");
        hYield[i] -> SetYTitle("Yield [a.u.]");
        hYield[i] -> SetXAxis(Emins[i], Emaxs[i], 200);
        hYield[i] -> GenerateHistogram();
        hYield_Log[i] = hYield[i] -> GetHistogram();

        hGen[i] = new TH1DLog();
        hGen[i] -> SetName(Form("hGen_%d", i));
        hGen[i] -> SetTitle(Form("Gen %d", i));
        hGen[i] -> SetXTitle("E_{kin} [MeV]");
        hGen[i] -> SetYTitle("Gen [a.u.]");
        hGen[i] -> SetXAxis(Emins[i], Emaxs[i], 200);
        hGen[i] -> GenerateHistogram();
        hGen_Log[i] = hGen[i] -> GetHistogram();

        Edep[i] -> Draw(Form("%s>>hGen_%d", BranchName[0].Data(), i),"", "goff");
        Edep[i] -> Draw(Form("%s>>hYield_%d", BranchName[0].Data(), i), ConditionGoodEvents.Data(), "goff");
    

        hYield_Log[i] -> Sumw2();
        hGen_Log[i] -> Sumw2();

        hYield_Log[i] -> Divide(hGen_Log[i]);

        TCanvas *cYield = new TCanvas(Form("cYield_%d", i), Form("cYield_%d", i), 800, 600);
        hYield_Log[i] -> Draw();
        hYield_Log[i] -> SetStats(0);
        gPad -> SetLogx();
        gPad -> SetGridx();
        gPad -> SetGridy();
        gPad -> SetLogy();
        cYield -> SaveAs(destination_PID + Form("/Yield_%s.pdf", ParticleNames[i].Data()));
    
    }


    TH2DLog *correlation = new TH2DLog();
    correlation -> SetName("hcorr");
    correlation -> SetTitle("Energy MC Vs Reconstructed");
    correlation -> SetTitle("E_{kin}^{MC} [MeV]");
    correlation -> SetYTitle("E_{kin}^{Reco} [MeV]");
    correlation -> SetXAxis(Emin_plots, Emax_plots, 200);
    correlation -> SetYAxis(Emin_plots, Emax_plots, 200);
    correlation -> GenerateHistogram();
    TH2D *correlation_log = correlation -> GetHistogram();

    for(int i = 0; i < NFiles; ++i)
    {
        if(i == 0)
        {
            Edep[i] -> Draw("TotalEnergy:RandEnergy>>hcorr",ConditionGoodEvents.Data(), "colz");
        }
        else
        {
            Edep[i] -> Draw("TotalEnergy:RandEnergy>>+hcorr",ConditionGoodEvents.Data(), "colz");
        }
    }

    TCanvas *cCorrelation = new TCanvas("cCorrelation", "cCorrelation", 800, 800);
    correlation_log -> Draw("colz");
    correlation_log -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogy();
    gPad -> SetLogz();

    TLine *line = new TLine(Emin_plots, Emin_plots, Emax_plots, Emax_plots);
    line -> SetLineColor(kRed);
    //line -> Draw("same");

    gPad -> SetGridx();
    gPad -> SetGridy();
    cCorrelation -> SaveAs(destination_PID + "/Correlation.pdf");




    TH2DLogX *hCorrection = new TH2DLogX();
    hCorrection -> SetName("hCorrection");
    hCorrection -> SetTitle("Energy MC Vs Reconstructed");
    hCorrection -> SetYTitle("E_{kin}^{MC} - E_{kin}^{Reco} [MeV]");
    hCorrection -> SetXTitle("E_{kin}^{MC} [MeV]");
    hCorrection -> SetXAxis(Emin_plots, Emax_plots, 200);
    hCorrection -> SetYAxis(0., 10, 200);

    hCorrection -> GenerateHistogram();
    TH2D *hCorrection_log = hCorrection -> GetHistogram();

    for(int i = 0; i < NFiles; ++i)
    {
        if(i == 0)
        {
            Edep[i] -> Draw("(RandEnergy-TotalEnergy):RandEnergy>>hCorrection",ConditionGoodEvents.Data(), "colz");
        }
        else
        {
            Edep[i] -> Draw("(RandEnergy-TotalEnergy):RandEnergy>>+hCorrection",ConditionGoodEvents.Data(), "colz");
        }
    }

    TCanvas *cCorrection = new TCanvas("cCorrection", "cCorrection", 800, 800);
    hCorrection_log -> Draw("colz");
    hCorrection_log -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGrid();

    cCorrection -> SaveAs(destination_PID + "/Correction.pdf");

    fileOut ->  Write();
    fileOut -> Close();


    

    return 0;
}
