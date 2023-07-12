#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h"
#include "TROOT.h"
#include "TRandom3.h"

#include "./ScatterPlotMatrix.h"


using namespace std;

int Test2()
{
    ScatterPlotMatrix *SPM = new ScatterPlotMatrix(3, 2);

    TTree *tree = new TTree("tree", "tree");
    TTree *tree2 = new TTree("tree2", "tree2");
    
    double x, y, z;
    double x2, y2, z2;
    int NumberEntries = 100000;
    TRandom3 *r = new TRandom3(0);
    tree-> Branch("x", &x, "x/D");
    tree-> Branch("y", &y, "y/D");
    tree-> Branch("z", &z, "z/D");

    tree2-> Branch("x", &x2, "x/D");
    tree2-> Branch("y", &y2, "y/D");
    tree2-> Branch("z", &z2, "z/D");


    for (int i = 0; i < NumberEntries; i++)
    {
        x = r->Gaus(0, 1);
        y = r->Gaus(2, 1.5);
        z = r->Gaus(1.5, 4);

        x2 = x+0.5*y;
        y2 = y+0.5*z;
        z2 = z+0.5*x;

        tree->Fill();
        tree2->Fill();
    }


    SPM->AddTTree(&tree);
    SPM->AddTTree(&tree2);
    SPM->AddVariableName("x");
    SPM->AddVariableName("y");
    SPM->AddVariableName("z");
    SPM->SetNbins(100);
    SPM->SetConditionSelection("");
    SPM->BuildPlot();





    return 0;
}