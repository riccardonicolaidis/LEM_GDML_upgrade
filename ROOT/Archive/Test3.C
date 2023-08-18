#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THStack.h"
#include "TF1.h"

int Test3()
{

    TTree *tree = new TTree("tree", "tree");

    double x, y, z;

    tree -> Branch("x", &x, "x/D");
    tree -> Branch("y", &y, "y/D");
    tree -> Branch("z", &z, "z/D");

    TRandom3 *r = new TRandom3(0);

    int N = 1e6;

    for (int i = 0; i < N; i++)
    {
        x = r->Gaus(0, 1);
        y = r->Gaus(1., 1.5);
        z = r->Gaus(1.5, 0.3);

        tree->Fill();
    }


    TH1D *h[3];
    TF1 *f[3];


    for(int i = 0; i < 3; i++)
    {
        h[i] = new TH1D(Form("h%d", i), Form("h%d", i), 100, -2, 5);
        h[i] -> SetLineColor(i + 1);
        h[i] -> SetLineWidth(2);
        h[i] -> SetStats(0);
        h[i] -> SetMarkerStyle(20);
        h[i] -> SetMarkerSize(0.5);
        h[i] -> SetMarkerColor(i + 1);

        f[i] = new TF1(Form("f%d", i), "gaus", -2, 5);
        f[i] -> SetLineColor(i + 1);
        f[i] -> SetLineWidth(2);

    }

    THStack *hs = new THStack("hs", "hs");




    tree->Draw("x>>h0","", "goff");
    tree->Draw("y>>h1","", "goff");
    tree->Draw("z>>h2","", "goff");

    for(int i = 0; i < 3; i++)
    {
        h[i] -> Fit(Form("f%d", i), "R");
        hs -> Add(h[i], "PE");
    }

    TCanvas *c = new TCanvas("c", "c", 800, 800);
    hs -> Draw("nostack");
    gPad -> SetGrid();
    gPad -> SetLogy();






    return 0;
}