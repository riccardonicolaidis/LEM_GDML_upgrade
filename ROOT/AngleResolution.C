#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

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
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TH2D.h"


using namespace std;


int AngleResolution
(
    TString filename,
    TString filename_noExt,
    TString destination
)
{
    TFile *file = new TFile(filename, "READ");
    TTree *Edep = (TTree*)file->Get("tree");

    




    return 0;
}