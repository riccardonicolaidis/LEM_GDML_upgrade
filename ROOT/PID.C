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


using namespace std;

int PID(
        int NFiles,
        TString pathFileNames = "",
        TString pathFileEnergies = "",
        TString destination = "",
        double Emin_plots = 0.01,
        double Emax_plots = 300.,
        double E_thr_Thin = 0.04,
        double E_thr_Thick = 0.04,
        double E_thr_Plastic = 0.05,
        int NumberPairsSensors = 5,
        double ResSilicon = 0.01,
        double ResPlastic = 0.05
)
{
    // Open the file with name pathFileNames.txt
    ifstream fileNames(pathFileNames.Data());
    
    
    TString FileNames[NFiles];
    TString FileNames_noExt[NFiles];
    double Emins[NFiles];
    double Emaxs[NFiles];

    string textline;

    if (fileNames.is_open())
    {
        for(int i = 0; i < NFiles; i++)
        {
            getline(fileNames, textline);
            FileNames[i] = textline;
            cout << "FileNames[" << i << "] = " << FileNames[i] << endl;
            FileNames_noExt[i] = FileNames[i];
            cout << "FileNames_noExt[" << i << "] = " << FileNames_noExt[i] << endl;
            FileNames_noExt[i].ReplaceAll(".root", "");
            cout << "FileNames_noExt[" << i << "] = " << FileNames_noExt[i] << endl;
        }
    }
    else
    {
        cout << "Unable to open file" << endl;
        return 0;
    }


    TString ParticleNames[NFiles];

    for(int i = 0; i < NFiles; i++)
    {
        string CompleteFileName = FileNames_noExt[i].Data();
        string ParticleName = CompleteFileName.substr(CompleteFileName.find_last_of("/")+1);
        ParticleNames[i] = ParticleName;
        ParticleNames[i].ReplaceAll("_t0_alias", "");
    }
        


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
        return 0;
    }

    double Xmin = Emin_plots;
    double Xmax = Emax_plots;


    TFile *file[NFiles];
    TTree *Edep[NFiles];

    for(int i = 0; i < NFiles; i++)
    {
        file[i] = new TFile(FileNames[i], "READ");
        Edep[i] = (TTree*)file[i]->Get("Edep");
    }



    TString AliasETot = "ETot";

    TString DirName[3] = {"DirX", "DirY", "DirZ"};
    TString PolarAngle[2] = {"dirTheta", "dirPhi"};
    TString NewPolarAngle[2] = {"dirThetaNew", "dirPhiNew"};


    int NumberBranches = 7 + 3 + 2*NumberPairsSensors;
    std::cout << "NumberBranches = " << NumberBranches << endl;


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
    BranchName[iBranch++] = "Ed_LV_Veto_bottom";
    BranchName[iBranch++] = "Ed_LV_Veto_top";
    int iBranchEnergies_Calo = iBranch;
    BranchName[iBranch++] = "Ed_LV_Plastic_calo";

    int iStartSensors = iBranch;
    int iStartSensors_Thin = iStartSensors;
    int iStartSensors_Thick = iStartSensors + NumberPairsSensors;

    for(int iPair = 0; iPair < NumberPairsSensors; iPair++)
    {
        BranchName[iBranch] = "Ed_LV_Silicon_Thin_" + to_string(iPair);
        BranchName[iBranch+NumberPairsSensors] = "Ed_LV_Silicon_Thick_" + to_string(iPair);
        iBranch++;
    }

    for(int i = 0; i < NumberBranches; i++)
    {
        std::cout << "BranchName[" << i << "] = " << BranchName[i] << endl;
    }



    TString ConditionPairSilicon[NumberPairsSensors];
    TString ConditionPairSiliconAll;
    TString ConditionDrilledVeto;
    TString ConditionGoodEvents;
    TString ConditionGoodEventsSinglePair[NumberPairsSensors];
    TString ConditionNoCalo;
    TString NumberPairsHit = "(";

    for(int i = 0; i< NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors+i].Data() << " & " <<BranchName[iStartSensors+ NumberPairsSensors+i].Data() << endl;
        ConditionPairSilicon[i] = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        cout << "ConditionPairSilicon[" << i << "] = " << ConditionPairSilicon[i].Data() << endl;
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
            NumberPairsHit += Form("1*((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
        else 
        {
            ConditionPairSiliconAll += Form("|| ((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
            NumberPairsHit += Form(" + 1*((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
    }
    NumberPairsHit += ")";

    ConditionDrilledVeto = Form("(%s < %g) && (%s < %g)", BranchName[iBranchEnergies].Data(), E_thr_Plastic, BranchName[iBranchEnergies + 1].Data(), E_thr_Plastic);

    ConditionGoodEvents = Form("(%s < 2.0) && (%s) && (%s)", NumberPairsHit.Data(), ConditionPairSiliconAll.Data(), ConditionDrilledVeto.Data());
    for(int i = 0; i < NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors_Thin+i].Data() << " & " <<BranchName[iStartSensors_Thick+i].Data() << endl;
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionDrilledVeto.Data());
    }

    ConditionNoCalo = Form("(%s < %g)", BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);

    TFile *fileOut = new TFile(destination +"/Plots.root", "RECREATE");


    TCanvas *cPID, *cgPID, *cPID_NoCalo, *cgPID_NoCalo;
    TCanvas *cPID2 , *cgPID2, *cPID2_NoCalo, *cgPID2_NoCalo;

    


    TH2DLogX *hPID_log[4];
    TH2D *hPID[4];

    TH2DLogX *hPID2_log[4];
    TH2D *hPID2[4];

    // Index 0: PID Thin + Thick + Calo
    // Index 1: PID gThin + gThick + gCalo
    // Index 2: PID Thin + Thick NO Calo
    // Index 3: PID gThin + gThick NO Calo

    for(int i = 0; i < NFiles; ++i)
    {
        hPID_log[i] = new TH2DLogX();
        hPID_log[i] -> SetName(Form("hPID_%d", i));
        hPID_log[i] -> SetTitle(Form("hPID_%d", i));
        hPID_log[i] -> SetXTitle("E_{tot} [MeV]");
        hPID_log[i] -> SetYTitle("PID");
        hPID_log[i] -> SetXAxis(Xmin, Xmax, 300);
        hPID_log[i] -> SetYAxis(-3., +3.2, 300);
        hPID_log[i] -> GenerateHistogram();
        hPID[i] = hPID_log[i] -> GetHistogram();

        hPID2_log[i] = new TH2DLogX();
        hPID2_log[i] -> SetName(Form("hPID2_%d", i));
        hPID2_log[i] -> SetTitle(Form("hPID2_%d", i));
        hPID2_log[i] -> SetXTitle("E_{tot} [MeV]");
        hPID2_log[i] -> SetYTitle("PID");
        hPID2_log[i] -> SetXAxis(Xmin, Xmax, 300);
        hPID2_log[i] -> SetYAxis(-3., +3.2, 300);
        hPID2_log[i] -> GenerateHistogram();
        hPID2[i] = hPID2_log[i] -> GetHistogram();
    }


    for(int i = 0; i < NFiles; ++i)
    {
        if(i == 0)
        {
            Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Plastic_calo)>>hPID_0", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("gPID:(gTotThick + gTotThin + gEd_LV_Plastic_calo)>>hPID_1", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("PID:(TotThick + TotThin)>>hPID_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
            Edep[i] -> Draw("gPID:(gTotThick + gTotThin)>>hPID_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
        
            Edep[i] -> Draw("PID:RandEnergy>>hPID2_0", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("gPID:RandEnergy>>hPID2_1", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("PID:RandEnergy>>hPID2_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
            Edep[i] -> Draw("gPID:RandEnergy>>hPID2_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
        
        }
        else
        {
            Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Plastic_calo)>>+hPID_0", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("gPID:(gTotThick + gTotThin + gEd_LV_Plastic_calo)>>+hPID_1", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("PID:(TotThick + TotThin)>>+hPID_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
            Edep[i] -> Draw("gPID:(gTotThick + gTotThin)>>+hPID_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
        
            Edep[i] -> Draw("PID:RandEnergy>>+hPID2_0", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("gPID:RandEnergy>>+hPID2_1", ConditionGoodEvents.Data(), "colz");
            Edep[i] -> Draw("PID:RandEnergy>>+hPID2_2", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");
            Edep[i] -> Draw("gPID:RandEnergy>>+hPID2_3", Form("(%s)&&(%s)", ConditionGoodEvents.Data(), ConditionNoCalo.Data()), "colz");

        }
    }

    TString destination_PID = destination + "/PID_plots";
    gSystem -> mkdir(destination_PID.Data(), true);


    TH2DLogX *hPID_single;
    TCanvas *cPID_single;
    TH2D *hPIDlog_single;
    
    cout << "Plotting Single PID" << endl;
    for(int i = 0; i < 10; ++i)
    {
        cout << "########################" << endl;
    }

    



    for(int i = 0; i < NFiles; ++i)
    {
        hPID_single = new TH2DLogX();
        hPID_single -> SetName("hPID_single");
        hPID_single -> SetTitle(ParticleNames[i].Data());
        hPID_single -> SetXTitle("E_{tot} [MeV]");
        hPID_single -> SetYTitle("PID");
        hPID_single -> SetXAxis(Emins[i], Emaxs[i], 200);
        hPID_single -> SetYAxis(-3., +3.2, 200);
        hPID_single -> GenerateHistogram();

        hPIDlog_single = hPID_single -> GetHistogram();
        
        Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Plastic_calo)>>hPID_single", ConditionGoodEvents.Data(), "colz");
        cPID_single = new TCanvas("cPID_single", "cPID_single", 600, 600);

        hPIDlog_single -> Draw("colz");
        gPad -> SetLogx();
        gPad -> SetLogz();
        gPad -> SetGrid();
        cPID_single -> SaveAs(destination_PID + Form("/PID_single_%s.pdf", ParticleNames[i].Data()));
        delete hPID_single;
        delete cPID_single;
        delete hPIDlog_single;
    }
    for(int i = 0; i < 10; ++i)
    {
        cout << "########################" << endl;
    }

    TGraph *grPID[NFiles];
    TMultiGraph *mgrPID = new TMultiGraph("mgrPID", "mgrPID");
    TLegend *legPID = new TLegend(0.1, 0.7, 0.4, 0.9);
    for(int i = 0; i < NFiles; ++i)
    {
        Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Plastic_calo)", Form("(%s) && (!(%s))", ConditionGoodEvents.Data(), ConditionGoodEventsSinglePair[0].Data()), "colz"); // Form("(%s) && (TotThick > 0.0) && (TotThin > 0.0)",ConditionGoodEvents.Data())
        grPID[i] = new TGraph(Edep[i] -> GetSelectedRows(), Edep[i] -> GetV2(), Edep[i] -> GetV1());
        grPID[i] -> Print();
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
    delete cgraphPID;


    mgrPID = new TMultiGraph("mgrPID", "mgrPID");
    legPID = new TLegend(0.1, 0.7, 0.4, 0.9);
    for(int i = 0; i < NFiles; ++i)
    {
        Edep[i] -> Draw("PID:(TotThick + TotThin + Ed_LV_Plastic_calo)", ConditionGoodEventsSinglePair[0].Data(), "colz"); // Form("(%s) && (TotThick > 0.0) && (TotThin > 0.0)",ConditionGoodEvents.Data())
        grPID[i] = new TGraph(Edep[i] -> GetSelectedRows(), Edep[i] -> GetV2(), Edep[i] -> GetV1());
        grPID[i] -> Print();
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
    delete cgraphPID;






    cPID = new TCanvas("cPID", "cPID", 600, 600);
    hPID[0] -> Draw("colz");
    hPID[0] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cPID -> SaveAs(destination_PID + "/PID.pdf");
    cPID -> SaveAs(destination_PID + "/PID.root");
    cgPID = new TCanvas("cgPID", "cgPID", 600, 600);
    hPID[1] -> Draw("colz");
    hPID[1] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cgPID -> SaveAs(destination_PID + "/gPID.pdf");
    cgPID -> SaveAs(destination_PID + "/gPID.root");
    cPID_NoCalo = new TCanvas("cPID_NoCalo", "cPID_NoCalo", 600, 600);
    hPID[2] -> Draw("colz");
    hPID[2] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cPID_NoCalo -> SaveAs(destination_PID + "/PID_NoCalo.pdf");
    cPID_NoCalo -> SaveAs(destination_PID + "/PID_NoCalo.root");
    cgPID_NoCalo = new TCanvas("cgPID_NoCalo", "cgPID_NoCalo", 600, 600);
    hPID[3] -> Draw("colz");
    hPID[3] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cgPID_NoCalo -> SaveAs(destination_PID + "/gPID_NoCalo.pdf");
    cgPID_NoCalo -> SaveAs(destination_PID + "/gPID_NoCalo.root");

    cPID2 = new TCanvas("cPID2", "cPID2", 600, 600);
    hPID2[0] -> Draw("colz");
    hPID2[0] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cPID2 -> SaveAs(destination_PID + "/PID2.pdf");
    
    cgPID2 = new TCanvas("cgPID2", "cgPID2", 600, 600);
    hPID2[1] -> Draw("colz");
    hPID2[1] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cgPID2 -> SaveAs(destination_PID + "/gPID2.pdf");

    cPID2_NoCalo = new TCanvas("cPID2_NoCalo", "cPID2_NoCalo", 600, 600);
    hPID2[2] -> Draw("colz");
    hPID2[2] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cPID2_NoCalo -> SaveAs(destination_PID + "/PID2_NoCalo.pdf");

    cgPID2_NoCalo = new TCanvas("cgPID2_NoCalo", "cgPID2_NoCalo", 600, 600);
    hPID2[3] -> Draw("colz");
    hPID2[3] -> SetStats(0);
    gPad -> SetLogx();
    gPad -> SetLogz();
    gPad -> SetGridx();
    gPad -> SetGridy();
    cgPID2_NoCalo -> SaveAs(destination_PID + "/gPID2_NoCalo.pdf");


    TCanvas *cDisentangle = new TCanvas("cDisentangle", "cDisentangle", 600, 600);
    TH2D *hDisentangle = new TH2D("hDisentangle", "hDisentangle", 300, -2.5, 4., 300, -2.5, 4.);

    for(int i = 0; i < NFiles; ++i)
    {

        TString ConditionGoodEvents_PID2 = Form("(%s) && (Ed_LV_Plastic_calo > %g)", ConditionGoodEvents.Data(), E_thr_Plastic);
        if(i == 0)
        {
            Edep[i] -> Draw("PID:(TMath::Log10(TotThick * (TotThick + Ed_LV_Plastic_calo)))>>hDisentangle", ConditionGoodEvents_PID2.Data(), "colz");
        }
        else
        {
            Edep[i] -> Draw("PID:(TMath::Log10(TotThick * (TotThick + Ed_LV_Plastic_calo)))>>+hDisentangle", ConditionGoodEvents_PID2.Data(), "colz");
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


    TH1DLog *hYield[NFiles];
    TH1D *hYield_Log[NFiles];

    TH1DLog *hGen[NFiles];
    TH1D *hGen_Log[NFiles];


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
