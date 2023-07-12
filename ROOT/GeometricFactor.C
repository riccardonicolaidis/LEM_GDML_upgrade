#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMarker.h"
#include "TMath.h"
#include "TColor.h"
#include "TLegend.h"
#include "THStack.h"

#include "TH1DLog.h"

using namespace std;

void GeometricFactor(
        TString filename_e,
        TString filename_e_noExt,
        TString filename_p,
        TString filename_p_noExt,
        TString filename_alpha,
        TString filename_alpha_noExt,
        double Emin_e = 0.0,
        double Emax_e = 0.0,
        double Emin_p = 0.0,
        double Emax_p = 0.0,
        double Emin_alpha = 0.0,
        double Emax_alpha = 0.0,
        TString destination = "",
        int NumberPairsSensors = 5,
        double ResSilicon = 0.03,
        double ResPlastic = 0.03,
        double E_thr_Thin = 0.03,
        double E_thr_Thick = 0.03,
        double E_thr_Plastic = 0.1
)
{
    
    TString FileName[3] = {filename_e, filename_p, filename_alpha};
    TString FileName_noExt[3] = {filename_e_noExt, filename_p_noExt, filename_alpha_noExt};
    double Emins[3] = {Emin_e, Emin_p, Emin_alpha};
    double Emaxs[3] = {Emax_e, Emax_p, Emax_alpha};

    int Nfiles = 3;


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


    // 
    


    /* -------------------------------------------------------------------------- */
    TCanvas *cFinal[3];
    double ERange[3][2];
    double ERangeGen[3][2];
    double NgenCalibrated;
    double Area;
    double Radius;
    int NGen;

    // ********************************************
    //           GEOMETRICAL FACTOR SETTINGS
    // ********************************************
    // Range of energy generated (see in run_..._.mac)
    ERangeGen[0][0] = Emin_e;
    ERangeGen[0][1] = Emax_e;
    ERangeGen[1][0] = Emin_p;
    ERangeGen[1][1] = Emax_p;
    ERangeGen[2][0] = Emin_alpha;
    ERangeGen[2][1] = Emax_alpha;
    // Range of energy of the plots
    ERange[0][0] = Emin_e;
    ERange[0][1] = Emax_e;
    ERange[1][0] = Emin_p;
    ERange[1][1] = Emax_p;
    ERange[2][0] = Emin_alpha;
    ERange[2][1] = Emax_alpha;
    // Number of bins
    int Nbins[3] = {100, 100, 100};
    Radius = 1.3;
    Area = TMath::Pi() * Radius * Radius;
    NGen = 5000000;

    double dNdE_Gen[3];
    for(int i = 0; i < 3; ++i)
    {
        dNdE_Gen[i] = NGen / (ERangeGen[i][1] - ERangeGen[i][0]);
    }

    // ********************************************
    // ********************************************
    /* -------------------------------------------------------------------------- */
    /*                                   OPTIONS                                  */
    /* -------------------------------------------------------------------------- */



    /* -------------------------------------------------------------------------- */
    /*                                 /FILE NAMES                                */
    /* -------------------------------------------------------------------------- */


    for(int i = 0; i < Nfiles; i++)
    {
        cout << "FileName[" << i << "] = " << FileName[i] << endl;
    }

    TFile *file[Nfiles];
    TTree *Edep[Nfiles];


    for(int i = 0; i < Nfiles ; ++i)
    {
        file[i] = TFile::Open(FileName[i]);
        Edep[i] = (TTree*) file[i] -> Get("Edep");
        Edep[i] -> Print();
    }


    /* -------------------------------------------------------------------------- */
    /*                   Definition of the SELECTION conditions                   */
    /* -------------------------------------------------------------------------- */

    TString ConditionPairSilicon[NumberPairsSensors];
    TString ConditionPairSiliconAll;
    TString ConditionVetoedEvents;
    TString ConditionGoodEvents;
    TString ConditionGoodEventsSinglePair[NumberPairsSensors];

    for(int i = 0; i< NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors+i].Data() << " & " <<BranchName[iStartSensors+ NumberPairsSensors+i].Data() << endl;
        ConditionPairSilicon[i] = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        cout << "ConditionPairSilicon[" << i << "] = " << ConditionPairSilicon[i].Data() << endl;
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
        else 
        {
            ConditionPairSiliconAll += Form("|| ((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
    }

    ConditionVetoedEvents = Form("(%s < %g) && (%s < %g)", BranchName[iBranchEnergies].Data(), E_thr_Plastic, BranchName[iBranchEnergies + 1].Data(), E_thr_Plastic);

    ConditionGoodEvents = Form("(%s) && (%s)", ConditionPairSiliconAll.Data(), ConditionVetoedEvents.Data());
    
    TString ConditionGoodEvents_Ch0 = Form("(%s) && (%s)", ConditionPairSilicon[0].Data(), ConditionVetoedEvents.Data());
    TString ConditionGoodEvents_ChOther = "(";
    for(int i = 1; i < NumberPairsSensors; ++i)
    {
        if(i == 1)
        {
            ConditionGoodEvents_ChOther += Form("(%s)", ConditionPairSilicon[i].Data());
        }
        else
        {
            ConditionGoodEvents_ChOther += Form("|| (%s)", ConditionPairSilicon[i].Data());
        }
    }
    ConditionGoodEvents_ChOther += ")";
    ConditionGoodEvents_ChOther += "&&";
    ConditionGoodEvents_ChOther += Form("(%s)", ConditionVetoedEvents.Data());
    
    cout << "ConditionGoodEvents_Ch0 = " << ConditionGoodEvents_Ch0.Data() << endl;
    cout << "ConditionGoodEvents_ChOther = " << ConditionGoodEvents_ChOther.Data() << endl;
    
    for(int i = 0; i < NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors_Thin+i].Data() << " & " <<BranchName[iStartSensors_Thick+i].Data() << endl;
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionVetoedEvents.Data());
    }



    TH1DLog *hAccepted_Log[Nfiles];
    TH1D *hAccepted[Nfiles];
    TCanvas *cAccepted[Nfiles];

    for(int i = 0; i < Nfiles; ++i)
    {
        hAccepted_Log[i] = new TH1DLog();
        hAccepted_Log[i] -> SetName(Form("hAccepted_Log_%d", i));
        hAccepted_Log[i] -> SetTitle("");
        hAccepted_Log[i] -> SetXAxis(ERangeGen[i][0], ERangeGen[i][1], Nbins[i]);
        hAccepted_Log[i] -> SetXTitle("E_{gen} [MeV]");
        hAccepted_Log[i] -> SetYTitle("Geometric Factor [cm^{2} sr]");
        hAccepted_Log[i] -> GenerateHistogram();
        hAccepted[i] = hAccepted_Log[i] -> GetHistogram();

        cAccepted[i] = new TCanvas(Form("cAccepted_%d", i), Form("cAccepted_%d", i));
        cAccepted[i] -> cd();
        Edep[i] -> Draw(Form("%s >> hAccepted_Log_%d", BranchName[0].Data(), i), ConditionGoodEvents.Data(), "E");
        

        int NumberOfBins = hAccepted[i] -> GetNbinsX();
        // loop on all bins

        for(int j = 0; j < NumberOfBins; ++j)
        {
            double BinWidth = hAccepted[i] -> GetBinWidth(j+1);
            double NGenCalibrated = dNdE_Gen[i] * BinWidth;
            double NAccepted = hAccepted[i] -> GetBinContent(j+1);
            hAccepted[i] -> SetBinContent(j+1, NAccepted * TMath::Pi()*Area / NGenCalibrated);
            // Set Bin Error
            hAccepted[i] -> SetBinError(j+1, TMath::Sqrt(NAccepted) * TMath::Pi()*Area / NGenCalibrated);
        }
        hAccepted[i] -> Sumw2();
        hAccepted[i] -> SetLineColor(kBlue);
        hAccepted[i] -> SetLineWidth(2);
        hAccepted[i] -> SetMarkerColor(kBlue);
        hAccepted[i] -> SetMarkerStyle(20);
        hAccepted[i] -> SetMarkerSize(0.5);


        // Multiply the histogram by the acceptance of the aperture defined at the beginning of the script
        hAccepted[i] -> Draw();

        gPad -> SetLogx();
        gPad -> SetLogy();
        gPad -> SetGrid();


        // Save the histogram in a file
        cAccepted[i] -> SaveAs(Form("%s/Acceptances_%s.pdf", destination.Data(), FileName_noExt[i].Data()));

    }


    TH1DLog *hAccepted_Ch0_Log[Nfiles];
    TH1D *hAccepted_Ch0[Nfiles];
    TH1DLog *hAccepted_ChOther_Log[Nfiles];
    TH1D *hAccepted_ChOther[Nfiles];
    THStack *hAccepted_Stack[Nfiles];
    TLegend *lAccepted[Nfiles];



    for(int i = 0 ; i < Nfiles ; ++i)
    {
        hAccepted_Ch0_Log[i] = new TH1DLog();
        hAccepted_Ch0_Log[i] -> SetName(Form("hAccepted_Ch0_Log_%d", i));
        hAccepted_Ch0_Log[i] -> SetTitle("");
        hAccepted_Ch0_Log[i] -> SetXAxis(ERangeGen[i][0], ERangeGen[i][1], Nbins[i]);
        hAccepted_Ch0_Log[i] -> SetXTitle("E_{gen} [MeV]");
        hAccepted_Ch0_Log[i] -> SetYTitle("Geometric Factor [cm^{2} sr]");
        hAccepted_Ch0_Log[i] -> GenerateHistogram();
        hAccepted_Ch0[i] = hAccepted_Ch0_Log[i] -> GetHistogram();

        hAccepted_ChOther_Log[i] = new TH1DLog();
        hAccepted_ChOther_Log[i] -> SetName(Form("hAccepted_ChOther_Log_%d", i));
        hAccepted_ChOther_Log[i] -> SetTitle("");
        hAccepted_ChOther_Log[i] -> SetXAxis(ERangeGen[i][0], ERangeGen[i][1], Nbins[i]);
        hAccepted_ChOther_Log[i] -> SetXTitle("E_{gen} [MeV]");
        hAccepted_ChOther_Log[i] -> SetYTitle("Geometric Factor [cm^{2} sr]");
        hAccepted_ChOther_Log[i] -> GenerateHistogram();
        hAccepted_ChOther[i] = hAccepted_ChOther_Log[i] -> GetHistogram();


        hAccepted_Stack[i] = new THStack(Form("hAccepted_Stack_%d", i), Form("hAccepted_Stack_%d", i));
        Edep[i] -> Draw(Form("%s >> hAccepted_Ch0_Log_%d", BranchName[0].Data(), i), ConditionGoodEvents_Ch0.Data(), "E");
        Edep[i] -> Draw(Form("%s >> hAccepted_ChOther_Log_%d", BranchName[0].Data(), i), ConditionGoodEvents_ChOther.Data(), "E");

        int NumberOfBins = hAccepted_Ch0[i] -> GetNbinsX();
        // loop on all bins
        for(int j = 0; j < NumberOfBins; ++j)
        {
            double BinWidth = hAccepted_Ch0[i] -> GetBinWidth(j+1);
            double NGenCalibrated = dNdE_Gen[i] * BinWidth;
            double NAccepted_Ch0 = hAccepted_Ch0[i] -> GetBinContent(j+1);
            double NAccepted_ChOther = hAccepted_ChOther[i] -> GetBinContent(j+1);
            hAccepted_Ch0[i] -> SetBinContent(j+1, NAccepted_Ch0 * TMath::Pi()*Area / NGenCalibrated);
            hAccepted_ChOther[i] -> SetBinContent(j+1, NAccepted_ChOther * TMath::Pi()*Area / NGenCalibrated);
            // Set Bin Error
            hAccepted_Ch0[i] -> SetBinError(j+1, TMath::Sqrt(NAccepted_Ch0) * TMath::Pi()*Area / NGenCalibrated);
            hAccepted_ChOther[i] -> SetBinError(j+1, TMath::Sqrt(NAccepted_ChOther) * TMath::Pi()*Area / NGenCalibrated);
        }

        hAccepted_Ch0[i] -> Sumw2();
        hAccepted_ChOther[i] -> Sumw2();

        hAccepted_Ch0[i] -> SetLineColor(kRed);
        hAccepted_Ch0[i] -> SetLineWidth(2);
        hAccepted_Ch0[i] -> SetMarkerColor(kRed);
        hAccepted_Ch0[i] -> SetMarkerStyle(20);
        hAccepted_Ch0[i] -> SetMarkerSize(0.5);

        hAccepted_ChOther[i] -> SetLineColor(kBlue);
        hAccepted_ChOther[i] -> SetLineWidth(2);
        hAccepted_ChOther[i] -> SetMarkerColor(kBlue);
        hAccepted_ChOther[i] -> SetMarkerStyle(20);
        hAccepted_ChOther[i] -> SetMarkerSize(0.5);


        hAccepted_Ch0[i] -> Scale(TMath::Pi()*Area);
        hAccepted_ChOther[i] -> Scale(TMath::Pi()*Area);

        hAccepted_Ch0[i] -> SetMarkerColor(kRed);
        hAccepted_Ch0[i] -> SetLineColor(kRed);

        hAccepted_ChOther[i] -> SetMarkerColor(kBlue);
        hAccepted_ChOther[i] -> SetLineColor(kBlue);

        hAccepted_Stack[i] -> Add(hAccepted_Ch0[i]);
        hAccepted_Stack[i] -> Add(hAccepted_ChOther[i]);

        lAccepted[i] = new TLegend(0.1, 0.1, 0.4, 0.3);
        lAccepted[i] -> AddEntry(hAccepted_Ch0[i], "Geometric Factor Ch0", "l");
        lAccepted[i] -> AddEntry(hAccepted_ChOther[i], "Geometric Factor Ch: 1 2 3 4", "l");

        TCanvas *cChannelGeomFactor = new TCanvas(Form("cChannelGeomFactor_%d", i), Form("cChannelGeomFactor_%d", i), 1200, 600);
        hAccepted_Stack[i] -> Draw("nostack");
        lAccepted[i] -> Draw("same");
        gPad -> SetLogx();
        gPad -> SetLogy();
        gPad -> SetGrid();
        cChannelGeomFactor -> SaveAs(Form("%s/ChannelGeomFactor_%s.pdf", destination.Data(), FileName_noExt[i].Data()));
    }










    THStack *hGeomAll_Stack = new THStack("hGeomAll_Stack", "hGeomAll_Stack");
    TH1DLog *hGeomAll[3];
    TH1D *hGeomAll_Log[3];
    TLegend *lGeomAll = new TLegend(0.1, 0.1, 0.4, 0.3);
    TString LegendName[3] = {"electrons", "protons", "alpha"};

    TColor *color[3];
    color[0] = new TColor(1756, 242./255., 53./255, 141./255);
    color[1] = new TColor(1757, 4./255., 104./255, 191./255);
    color[2] = new TColor(1758, 242./255., 152./255, 73./255);


    double Emin = Emins[0]; // Electron minimum
    double Emax = Emaxs[2]; // Alpha maximum
    int NbinAll = 120;
    for(int i = 0; i < Nfiles; ++i)
    {
        hGeomAll[i] = new TH1DLog();
        hGeomAll[i] -> SetName(Form("hGeomAll_%d", i));
        hGeomAll[i] -> SetTitle("");
        hGeomAll[i] -> SetXAxis(Emin, Emax, NbinAll);
        hGeomAll[i] -> SetXTitle("E_{kin} [MeV]");
        hGeomAll[i] -> SetYTitle("Geometric Factor [cm^{2} sr]");   
        hGeomAll[i] -> GenerateHistogram();
        hGeomAll_Log[i] = hGeomAll[i] -> GetHistogram();

        Edep[i] -> Draw(Form("%s >> hGeomAll_%d", BranchName[0].Data(), i), ConditionGoodEvents.Data(), "E");

        int NumberOfBins = hGeomAll_Log[i] -> GetNbinsX();
        for(int j = 0; j < NumberOfBins; ++j)
        {
            double BinWidth = hGeomAll_Log[i] -> GetBinWidth(j+1);
            double NGenCalibrated = dNdE_Gen[i] * BinWidth;
            double NAccepted = hGeomAll_Log[i] -> GetBinContent(j+1);
            hGeomAll_Log[i] -> SetBinContent(j+1, NAccepted / NGenCalibrated);
            hGeomAll_Log[i] -> SetBinError(j+1, TMath::Sqrt(NAccepted) / NGenCalibrated);
        }

        hGeomAll_Log[i] -> Sumw2();
        hGeomAll_Log[i] -> Scale(TMath::Pi()*Area);
        hGeomAll_Log[i] -> SetMarkerColor(color[i] -> GetNumber());
        hGeomAll_Log[i] -> SetLineColor(color[i] -> GetNumber());
        hGeomAll_Log[i] -> SetLineWidth(2);
        hGeomAll_Log[i] -> SetMarkerStyle(20);
        hGeomAll_Log[i] -> SetMarkerSize(0.5);
        hGeomAll_Stack -> Add(hGeomAll_Log[i]);

        lGeomAll -> AddEntry(hGeomAll_Log[i], LegendName[i].Data(), "l");

    }

    TCanvas *cGeomAll = new TCanvas("cGeomAll", "Geometrical factor", 1200, 600);
    hGeomAll_Stack -> Draw("nostack");
    hGeomAll_Stack -> SetTitle("");
    hGeomAll_Stack -> GetXaxis() -> SetTitle("E_{kin} [MeV]");
    hGeomAll_Stack -> GetYaxis() -> SetTitle("Geometric Factor [cm^{2} sr]");

    lGeomAll -> Draw("same");

    gPad -> SetLogx();
    gPad -> SetLogy();
    gPad -> SetGrid();
    cGeomAll -> SaveAs(Form("%s/GeomAll.pdf", destination.Data()));
    //cGeomAll -> SaveAs(Form("%s/GeomAll.root", destination.Data()));









    return;
}