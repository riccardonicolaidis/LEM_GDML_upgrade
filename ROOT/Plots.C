#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TColor.h"
#include "TStyle.h"
#include "TString.h"
#include "TText.h"


using namespace std;

int Plots()
{
    vector<TString> files;

    files.push_back("/data1/home/rnicolai/LEM_GDML_upgrade/OUT/Output_Geant4Simulation_Isotopes_20231007/Analysis_output/GDML_file_0/PID_plots/Yield_e-.root");
    files.push_back("/data1/home/rnicolai/LEM_GDML_upgrade/OUT/Output_Geant4Simulation_Isotopes_20231007/Analysis_output/GDML_file_0/PID_plots/Yield_proton.root");
    files.push_back("/data1/home/rnicolai/LEM_GDML_upgrade/OUT/Output_Geant4Simulation_Isotopes_20231007/Analysis_output/GDML_file_0/PID_plots/Yield_alpha.root");


    vector<TString> CanvasNames;
    CanvasNames.push_back("cYield_0");
    CanvasNames.push_back("cYield_1");
    CanvasNames.push_back("cYield_2");


    vector<TCanvas*> canvases;
    canvases.reserve(files.size());

    vector<TH1D*> hists;
    hists.reserve(files.size());


    vector<TColor*> Color_conf;

    Color_conf.resize(3);
    Color_conf[0] = new TColor(1756, 242./255., 53./255, 141./255);
    Color_conf[1] = new TColor(1757, 4./255., 104./255, 191./255);
    Color_conf[2] = new TColor(1758, 242./255., 152./255, 73./255);


    for (int i = 0; i < files.size(); i++)
    {
        TFile* file = new TFile(files[i], "READ");
        canvases[i] = (TCanvas*)file->Get(CanvasNames[i]);

        TList *primitives = canvases[i]->GetListOfPrimitives();

        TIter next(primitives);
        TObject *obj;

        while ((obj = next())) {
            // Verifica se l'oggetto è un istogramma
            if (obj->InheritsFrom(TH1D::Class())) {
                hists[i] = (TH1D *)obj;
                hists[i]->SetLineColor(Color_conf[i]->GetNumber());
                hists[i]->SetFillColor(Color_conf[i]->GetNumber());
                hists[i]->SetMarkerColor(Color_conf[i]->GetNumber());
                hists[i]->SetLineWidth(10);
                hists[i]->SetMarkerStyle(20);
                hists[i]->SetMarkerSize(1);
                hists[i]->SetStats(0);
                hists[i]->SetTitle("");
                hists[i]->GetXaxis()->SetTitle("Energy [MeV]");
                hists[i]->GetYaxis()->SetTitle("Geometric Factor [cm^{2} sr]");
            }
        }
    }


    int scale = 3;
    TCanvas *c1 = new TCanvas("c1", "c1", 1000*scale, 600*scale);

    gPad -> SetBottomMargin(0.15);
    gPad -> SetLeftMargin(0.15);



    double xmin = hists[0]->GetBinLowEdge(1);
    double xmax = hists[2]->GetBinLowEdge(hists[2]->GetNbinsX()) + hists[2]->GetBinWidth(hists[2]->GetNbinsX());

    
    TH1 *DrawFrame = c1->DrawFrame(xmin, 0.02, xmax, 0.7);

    DrawFrame->GetXaxis()->SetTitle("Energy [MeV]");
    DrawFrame->GetYaxis()->SetTitle("Geometric Factor [cm^{2} sr]");
    DrawFrame->GetXaxis()->SetLabelFont(42);
    DrawFrame->GetXaxis()->SetLabelSize(0.05);
    DrawFrame->GetXaxis()->SetTitleSize(0.05);
    DrawFrame->GetYaxis()->SetTitleFont(42);
    DrawFrame->GetYaxis()->SetLabelFont(42);
    DrawFrame->GetYaxis()->SetLabelSize(0.05);
    DrawFrame->GetYaxis()->SetTitleSize(0.05);
    DrawFrame->SetTitle("");



    hists[0]->Draw("same");
    hists[0]->GetXaxis()->SetTitle("Energy [MeV]");
    hists[0]->GetYaxis()->SetTitle("Geometric Factor [cm^{2} sr]");
    hists[1]->Draw("same");
    hists[2]->Draw("same");

    hists[0]->GetXaxis()->SetTitle("Energy [MeV]");
    hists[0]->GetYaxis()->SetTitle("Geometric Factor [cm^{2} sr]");

    hists[2]->GetXaxis()->SetTitle("Energy [MeV]");
    hists[2]->GetYaxis()->SetTitle("Geometric Factor [cm^{2} sr]");


    gPad -> SetLogy();
    gPad -> SetGrid();
    gPad -> SetLogx();

    TLegend *leg = new TLegend(0.15, 0.7, 0.4, 0.9);
    leg -> AddEntry(hists[0], "electrons", "l");
    leg -> AddEntry(hists[1], "protons", "l");
    leg -> AddEntry(hists[2], "#alpha", "l");
    leg -> Draw();


    TText *text = new TText(30., 0.5, "Preliminary");
    text -> SetTextSize(0.06);
    text -> SetTextFont(42);
    text -> SetTextColor(kGray+3);
    text -> SetTextAlign(12);
    text -> Draw("same");

    c1 -> SaveAs("Yield.png");
    c1 -> SaveAs("Yield.pdf");
    c1 -> SaveAs("Yield.C");
    c1 -> SaveAs("Yield.root");






    return 0;
}