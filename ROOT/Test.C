#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TMath.h"
#include "TH1D.h"
#include "TString.h"
#include "TRandom3.h"

using namespace std;

int Test(TString Title = "Prova", int NumberEntries = 50000)
{
    TTree *tree = new TTree("tree", "tree");
    
    double x;
    TRandom3 *r = new TRandom3(0);
    tree-> Branch("x", &x, "x/D");

    for (int i = 0; i < NumberEntries; i++)
    {
        x = r->Gaus(0, 1);
        tree->Fill();
    }

    TString Condition = "abs(x)<2.5";

    TCanvas *c = new TCanvas("c", "c", 800, 600);

    tree->Draw("x", Condition.Data(), "goff");
    double Xmin = TMath::MinElement(tree->GetSelectedRows(), tree->GetV1());
    double Xmax = TMath::MaxElement(tree->GetSelectedRows(), tree->GetV1());
    TH1D *h = new TH1D("h", "h", 100, Xmin, Xmax);
    tree->Draw("x>>h", Condition.Data(), "goff");


    h->Draw();
    h -> SetTitle(Title);
    h -> Fit("gaus");
    c->SaveAs(Title + ".png");



    return 0;
}