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

// Custom include
#include "./TH1DLog.h"
#include "./TH2DLog.h"
#include "./TH2DLogX.h"



void  CreateHistogram2D(
    vector<TTree*> tree,
    TString Title,
    TString XTitle,
    TString YTitle,
    TString DrawRule,
    TString CutRule,
    TString SaveAsPath,
    bool YLog
)
{
    cout << "CreateTheHistogram2D" << endl;
    cout << "Title: " << Title << endl;
    cout << "XTitle: " << XTitle << endl;
    cout << "YTitle: " << YTitle << endl;
    cout << "DrawRule: " << DrawRule << endl;
    cout << "CutRule: " << CutRule << endl;
    cout << "SaveAsPath: " << SaveAsPath << endl;
    cout << "YLog: " << YLog << endl;

    // Create the histogram

    TH2D *histogram;
    TH2DLogX *histogramLogX;
    TH2DLog *histogramLog;
    

    // Get Max Min of X and Y


    vector<double> minX;
    vector<double> maxX;
    vector<double> minY;
    vector<double> maxY;
    for(int i = 0; i < tree.size(); ++i)
    {
        tree[i] -> Draw(DrawRule, CutRule, "goff");
        minX.push_back(TMath::MinElement(tree[i]->GetSelectedRows(), tree[i]->GetV2()));
        maxX.push_back(TMath::MaxElement(tree[i]->GetSelectedRows(), tree[i]->GetV2()));
        minY.push_back(TMath::MinElement(tree[i]->GetSelectedRows(), tree[i]->GetV1()));
        maxY.push_back(TMath::MaxElement(tree[i]->GetSelectedRows(), tree[i]->GetV1()));
    }

    double minXAll = TMath::MinElement(minX.size(), &minX[0]);
    double maxXAll = TMath::MaxElement(maxX.size(), &maxX[0]);
    double minYAll = TMath::MinElement(minY.size(), &minY[0]);
    double maxYAll = TMath::MaxElement(maxY.size(), &maxY[0]);


    int NbinsX = 200;
    int NbinsY = 200;

    if(YLog)
    {
        histogramLog = new TH2DLog();
        histogramLog -> SetXAxis(minXAll, maxXAll, NbinsX);
        histogramLog -> SetYAxis(minYAll, maxYAll, NbinsY);
        histogramLog -> SetName("hLog");
        histogramLog -> SetTitle(Title);
        histogramLog -> SetXTitle(XTitle);
        histogramLog -> SetYTitle(YTitle);
        histogramLog -> GenerateHistogram();
        histogram = (TH2D*) histogramLog -> GetHistogram();
    }
    else
    {
        histogramLogX = new TH2DLogX();
        histogramLogX -> SetXAxis(minXAll, maxXAll, NbinsX);
        histogramLogX -> SetYAxis(minYAll, maxYAll, NbinsY);
        histogramLogX -> SetName("hLog");
        histogramLogX -> SetTitle(Title);
        histogramLogX -> SetXTitle(XTitle);
        histogramLogX -> SetYTitle(YTitle);
        histogramLogX -> GenerateHistogram();
        histogram = (TH2D*) histogramLogX -> GetHistogram();
    }


    for(int i = 0; i < tree.size(); ++i)
    {
        if(i == 0)
        {
            tree[i] -> Draw(Form("%s>>hLog", DrawRule.Data()), CutRule, "goff");
        }
        else
        {
            tree[i] -> Draw(Form("%s>>+hLog", DrawRule.Data()), CutRule, "goff");
        }        
    }

    TCanvas *canvas = new TCanvas("c", "c", 1000, 1000);

    histogram -> Draw("colz");
    gPad -> SetLogx();
    if(YLog)
    {
        gPad -> SetLogy();
    }
    gPad -> SetLogz();
    gPad -> SetGrid();

    vector<TString> SaveAsPathExtensions;
    SaveAsPathExtensions.push_back(SaveAsPath + ".png");
    SaveAsPathExtensions.push_back(SaveAsPath + ".pdf");
    SaveAsPathExtensions.push_back(SaveAsPath + ".root");

    for(int i = 0; i < SaveAsPathExtensions.size(); ++i)
    {
        canvas -> SaveAs(SaveAsPathExtensions[i]);
    }



    return;
}
