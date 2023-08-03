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
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THStack.h"


using namespace std;

class ScatterPlotMatrix
{
    public:

        ScatterPlotMatrix(int NumberOfVariables, int NumberOfTTrees);

        ~ScatterPlotMatrix();

        void SetConditionSelection(TString ConditionSelectionV)
        {
            ConditionSelection = ConditionSelectionV;
        };

        void AddTTree(TTree **TTreeV)
        {
            if(index_TTree == 0)
            {
                TTrees.resize(N_TTrees);
            }        
            TTrees[index_TTree] = *TTreeV;
            TTrees[index_TTree] -> Print();
            index_TTree++;
        };

        void AddVariableName(TString VariableNameV)
        {
            if(index_Variable == 0)
            {
                VariableNames.resize(N_Variables);
            }
            VariableNames[index_Variable] = VariableNameV;
            cout << "VariableNames[" << index_Variable << "]: " << VariableNames[index_Variable] << endl;
            index_Variable++;
        };

        void SetNbins(int NbinsV)
        {
            Nbins = NbinsV;
        };


        void BuildPlot();


    private:

    int index_TTree = 0;
    int index_Variable = 0;

    int N_TTrees;
    int N_Variables;
    
    vector<TTree*> TTrees;
    vector<TString> VariableNames;
    TString ConditionSelection;
    int Nbins;

    vector<TH1D*> Histograms;
    vector<THStack*> Stacks;
    vector<TGraph*> Graphs;
    vector<TMultiGraph*> MultiGraphs;

    TCanvas *canvas;

};


ScatterPlotMatrix::ScatterPlotMatrix(int NumberOfVariables, int NumberOfTTrees)
{
    TTrees.resize(NumberOfTTrees);
    VariableNames.resize(NumberOfVariables);
    N_TTrees = NumberOfTTrees;
    N_Variables = NumberOfVariables;
}


ScatterPlotMatrix::~ScatterPlotMatrix()
{
    TTrees.clear();
    VariableNames.clear();
}

void ScatterPlotMatrix::BuildPlot()
{
    vector<vector <double>> Xmins(N_Variables);
    vector<vector <double>> Xmaxs(N_Variables);
    for(int i = 0; i < N_Variables; ++i)
    {
        Xmins[i].resize(N_TTrees);
        Xmaxs[i].resize(N_TTrees);
    }


    // I have to search for maximum and minimum inside all the TTrees
    for(int k = 0; k < N_TTrees; ++k)
    {
        TTree *localTTree = TTrees[k];
        localTTree -> Print();
        for(int i = 0; i < N_Variables; ++i)
        {
            cout << "Variable: " << VariableNames[i] << endl;
            localTTree -> Draw(VariableNames[i].Data(), ConditionSelection.Data(), "goff");
            Xmins[i][k] = TMath::MinElement(localTTree->GetSelectedRows(), localTTree->GetV1());
            Xmaxs[i][k] = TMath::MaxElement(localTTree->GetSelectedRows(), localTTree->GetV1());
            cout << "Xmin: " << Xmins[i][k] << endl;
            cout << "Xmax: " << Xmaxs[i][k] << endl;
        }        
    }

    vector<double> Xmin2(N_Variables);
    vector<double> Xmax2(N_Variables);

    for(int i = 0; i < N_Variables; ++i)
    {
        Xmin2[i] = *min_element(Xmins[i].begin(), Xmins[i].end());
        Xmax2[i] = *max_element(Xmaxs[i].begin(), Xmaxs[i].end());
        cout << "Xmin2[" << i << "]: " << Xmin2[i] << endl;
        cout << "Xmax2[" << i << "]: " << Xmax2[i] << endl;
    }


    // Plot Matrix format
    /*H  0  0  0*/
    /*C  H  0  0*/
    /*C  C  H  0*/
    /*C  C  C  H*/

    int N_Diagonal = VariableNames.size();
    int N_ScatterPlots = N_Diagonal*(N_Diagonal-1)/2;


    Histograms.resize(N_Diagonal*N_Variables);
    Stacks.resize(N_Diagonal);
    Graphs.resize(N_ScatterPlots*N_Variables);
    MultiGraphs.resize(N_ScatterPlots);

    // Create Canvas
    canvas = new TCanvas("canvas", "canvas", 1000, 1000);
    canvas->Divide(N_Diagonal, N_Diagonal);

    int index_histogram = 0;
    int index_graph = 0;



    //      0  1  2  3
    //  0   1  2  3  4
    //  1   5  6  7  8
    //  2   9 10 11 12
    //  3  13 14 15 16

    for(int i = 0; i < N_Variables; ++i)
    {
        for(int j = 0; j < N_Variables; ++j)
        {
            canvas -> cd(1 + i*N_Diagonal + j);
            if(i == j)
            {
                // Create Histogram diagonal
                Stacks[i] = new THStack("stack", "");
                for(int k = 0; k < N_TTrees; ++k)
                {
                    Histograms[index_histogram] = new TH1D(Form("h_%d", index_histogram), "histogram", Nbins, Xmin2[i], Xmax2[i]);
                    TTrees[k]->Draw(VariableNames[i] + Form(">>h_%d", index_histogram), ConditionSelection.Data(), "");
                    Histograms[index_histogram]->SetLineColor(k+1);
                    Histograms[index_histogram]->SetLineWidth(2);
                    Histograms[index_histogram]->SetStats(0);
                    Stacks[i]->Add(Histograms[index_histogram]);
                    index_histogram++;
                }
                Stacks[i]->Draw("nostack");
                gPad -> SetGrid();
            }
            else if (j < i)
            {
                // Create Scatter Plot
                MultiGraphs[i+j] = new TMultiGraph();
                for(int k = 0; k < N_TTrees; ++k)
                {
                    Graphs[index_graph] = new TGraph();
                    TTrees[k]->Draw(VariableNames[i] + ":" + VariableNames[j], ConditionSelection.Data(), "goff");
                    Graphs[index_graph] = new TGraph(TTrees[k]->GetSelectedRows(), TTrees[k]->GetV2(), TTrees[k]->GetV1());
                    Graphs[index_graph]->SetMarkerStyle(20);
                    Graphs[index_graph]->SetMarkerSize(0.05);
                    Graphs[index_graph]->SetMarkerColor(k+1);
                    Graphs[index_graph]->SetLineColor(k+1);
                    Graphs[index_graph]->SetLineWidth(2);
                    Graphs[index_graph] -> SetMarkerColorAlpha(k+1, 0.2);
                    Graphs[index_graph] -> SetLineColorAlpha(k+1, 0.2);
                    Graphs[index_graph] -> SetFillColorAlpha(k+1, 0.2);
                    MultiGraphs[i+j]->Add(Graphs[index_graph]);
                    index_graph++;
                }
                MultiGraphs[i+j]->Draw("AP");
                gPad -> SetGrid();
            }
            else
            {
                // Do Nothing
            }
        }
    }


    // Matrix N*N
    // Total Number Elements = N^2
    // Diagonal Elements = N
    // Off Diagonal Elements = N^2 - N = N(N-1)

    canvas -> SaveAs("ScatterPlotMatrix.pdf");





}

