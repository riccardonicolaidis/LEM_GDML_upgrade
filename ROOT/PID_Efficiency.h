#ifndef PID_H
#define PID_H

// C++ include
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

// ROOT include
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
#include "TLine.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TList.h"
#include "TObjArray.h"
#include "TTreeFormula.h"
#include "TEllipse.h"
#include "TObjString.h"
#include "THStack.h"
#include "TColor.h"
#include "TRandom2.h"
#include "TH1.h"

// Custom include
#include "./TH1DLog.h"
#include "./TH2DLog.h"
#include "./TH2DLogX.h"
#include "./Histo2D.h"



int PID_Efficiency( vector<TTree*>  Edep,
                    TString         PID_Variable,
                    TString         Energy_Variable,
                    TString         Condition,
                    vector<TString> ParticleNames,
                    TString         outputdir,
                    TString         outputname)
{

    // Find the minimum and maximum for the plots


    vector<double> minX;
    vector<double> maxX;
    vector<double> minY;
    vector<double> maxY;    

    for(int i = 0; i < Edep.size(); ++i)
    {
        Edep[i] -> Draw(Form("%s:%s", PID_Variable.Data(), Energy_Variable.Data()), Condition.Data(), "goff");
        minX.push_back(TMath::MinElement(Edep[i]->GetSelectedRows(), Edep[i]->GetV2()));
        maxX.push_back(TMath::MaxElement(Edep[i]->GetSelectedRows(), Edep[i]->GetV2()));
        minY.push_back(TMath::MinElement(Edep[i]->GetSelectedRows(), Edep[i]->GetV1()));
        maxY.push_back(TMath::MaxElement(Edep[i]->GetSelectedRows(), Edep[i]->GetV1()));
        cout << "minX: " << minX[i] << endl;
        cout << "maxX: " << maxX[i] << endl;
        cout << "minY: " << minY[i] << endl;
        cout << "maxY: " << maxY[i] << endl;
    }; 

    double minX_All = TMath::MinElement(minX.size(), &minX[0]);
    double maxX_All = TMath::MaxElement(maxX.size(), &maxX[0]);
    double minY_All = TMath::MinElement(minY.size(), &minY[0]);
    double maxY_All = TMath::MaxElement(maxY.size(), &maxY[0]);


    // HISTOGRAM PID

    TH2DLogX *h2logx = new TH2DLogX();
    h2logx -> SetName("h2");
    h2logx -> SetTitle("");
    h2logx -> SetXAxis(minX_All, maxX_All, 150);
    h2logx -> SetYAxis(minY_All, maxY_All, 150);
    h2logx -> SetXTitle("E_{tot} [MeV]");
    h2logx -> SetYTitle("PID");
    h2logx -> GenerateHistogram();



    TH2D *h2 = (TH2D*) h2logx -> GetHistogram();
    vector<TH2D*> hclones;
    for(int i = 0; i < Edep.size(); ++i)
    {
        hclones.push_back((TH2D*)h2->Clone());
        hclones[i] -> SetName(Form("h2_%d", i));
    }



    for(int i = 0; i < Edep.size(); ++i)
    {
        Edep[i] -> Draw(Form("%s:%s>>+h2_%d", PID_Variable.Data(), Energy_Variable.Data(),i), Condition.Data(), "goff");
        h2 -> Add(hclones[i]);
    }

    // Set the palette
    gStyle -> SetPalette(kLake);
    TColor::InvertPalette();


    // Draw the histogram
    TCanvas *c2 = new TCanvas("c2", "c2", 2000, 1000);
    c2 -> Divide(2,1);
    c2 -> cd(1);
    gPad -> SetLogx();
    gPad -> SetLogz();
    

    h2 -> Draw("col");
    h2->SetStats(0);
    c2 -> Update();


    vector<double> PID_bin_edges ;
    PID_bin_edges.push_back(minY_All); // Is about -3
    PID_bin_edges.push_back(0.);
    PID_bin_edges.push_back(0.7);
    PID_bin_edges.push_back(1.4);
    PID_bin_edges.push_back(1.6);
    PID_bin_edges.push_back(maxY_All);

    vector<double> Edep_fake_bin_edges;
    int nDiv = 3;
    double step = (maxX_All - minX_All)/nDiv;
    for(int i = 0; i < nDiv; ++i)
    {
        Edep_fake_bin_edges.push_back(minX_All + i*step);
    }
    Edep_fake_bin_edges.push_back(maxX_All);



    TH2D* hTable = new TH2D("hTable", "hTable", Edep_fake_bin_edges.size()-1, &Edep_fake_bin_edges[0], PID_bin_edges.size()-1, &PID_bin_edges[0]);

    for(int i = 0; i < nDiv; ++i)
    {
        hTable -> GetXaxis() -> SetBinLabel(i+1, ParticleNames[i]);
        // Take h2_i (hclones[i])
        // sum all the entries (integral) over all the x axis 
        // Sum over only the interval of the PID bin on Y

        int TotalEntries = Edep[i] -> GetEntries();

        Edep[i] -> Draw("gPID", "", "goff");
        Double_t *values = Edep[i] -> GetV1();

        for(int j = 0; j < Edep[i] -> GetSelectedRows(); ++j)
        {
            double xbin_center = hTable -> GetXaxis() -> GetBinCenter(i+1);
            hTable -> Fill(xbin_center, values[j], 100./TotalEntries);
        }
    }
    
    // Round all the contents of the table at second decimal

    for(int i = 0; i < hTable -> GetNbinsX(); ++i)
    {
        for(int j = 0; j < hTable -> GetNbinsY(); ++j)
        {
            double content = hTable -> GetBinContent(i+1, j+1);
            hTable -> SetBinContent(i+1, j+1, round(content*100.)/100.);
        }
    }

    c2 -> cd(2);
    hTable -> GetZaxis() -> SetRangeUser(0, 400);
    hTable -> SetStats(0);
    hTable -> Draw("col text");


    c2 -> SaveAs(outputdir + "/" + outputname + ".pdf");
    c2 -> SaveAs(outputdir + "/" + outputname + ".png");
    c2 -> SaveAs(outputdir + "/" + outputname + ".root");





    return 0;
}

#endif