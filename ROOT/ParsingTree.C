#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>


#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"



using namespace std;


int ParsingTree()
{

    string FileText = "FileNames.txt";
    ifstream File(FileText);

    TString FileNames[3];
    TFile *f[3];
    TFile *fp[3];

    TTree *t[3];
    TTree *tp[3];

    double EdThin[7];
    double EdThick[7];

    int NumberID;

    TString FileNamesParsed[3];


            TString TupleNames[18];
        TupleNames[0] = "Ed_LV_V1";
        TupleNames[1] = "Ed_LV_V2";
        TupleNames[2] = "Ed_LV_P1";
        TupleNames[3] = "Ed_LV_P2";
        TupleNames[4] = "Ed_LV_THIN";
        TupleNames[5] = "Ed_LV_THICK";
        for(int i = 0; i < 6;++i)
        {
            TupleNames[6+2*i] = Form("%s00%d",TupleNames[4].Data(), i+1);
            TupleNames[7+2*i] = Form("%s00%d",TupleNames[5].Data(), i+1);
        }

        for(int i = 0; i < 18; ++i)
        {
            cout << TupleNames[i] << endl;
        }


        TString TupleNamesParsed[18];
        TupleNamesParsed[0] = "Ed_DrilledVeto";
        TupleNamesParsed[1] = "Ed_BottomVeto";
        TupleNamesParsed[2] = "Ed_Veto0";
        TupleNamesParsed[3] = "Ed_Veto1";

        for(int i = 0 ; i < 7; ++i)
        {
            TupleNamesParsed[4+2*i] = Form("Thin_%d_ID%d",i,i);
            TupleNamesParsed[5+2*i] = Form("Thick_%d_ID%d",i,i);
        }

        for(int i = 0; i < 18; ++i)
        {
            cout << TupleNamesParsed[i] << endl;
        }


    // Get filenames from FileNames.txt
    for(int i = 0; i < 3; i++)
    {
        File >> FileNames[i];
        cout << FileNames[i] << endl;

        FileNamesParsed[i] = Form("p%s", FileNames[i].Data());
        cout << FileNamesParsed[i] << endl;

        f[i] = new TFile(FileNames[i]);
        t[i] = (TTree*)f[i]->Get("Edep");

        fp[i] = new TFile(FileNamesParsed[i], "RECREATE");
        tp[i] = t[i] -> CopyTree("");

        for(int j = 0; j < 4; ++j)
        {
            tp[i] -> SetAlias(TupleNamesParsed[j].Data(), TupleNames[j].Data());
        }

        for(int j = 0; j < 7; ++j)
        {
            tp[i] -> SetAlias(TupleNamesParsed[4+2*j].Data(), TupleNames[4+2*j].Data());
            tp[i] -> SetAlias(TupleNamesParsed[5+2*j].Data(), TupleNames[5+2*j].Data());
        }
        



        TBranch *branchNumberID = tp[i] -> Branch("NumberID", &NumberID, "NumberID/I");

        for(int j = 0; j < 7; ++j)
        {
            tp[i] -> SetBranchAddress(TupleNames[4+2*j].Data(), &EdThin[j]);
            tp[i] -> SetBranchAddress(TupleNames[5+2*j].Data(), &EdThick[j]);
        }


        bool AtLeastOne = false;

        for(int j = 0; j < tp[i]->GetEntries(); ++j)
        {
            tp[i] -> GetEntry(j);
            AtLeastOne = false;
            NumberID = -2;
            for(int k = 0; k < 7; ++k)
            {
                if((EdThin[k] > 0.000) && (EdThick[k] > 0.000) && (~AtLeastOne))
                {
                    AtLeastOne = true;
                    NumberID = k;
                }
                else if((EdThin[k] > EdThin[NumberID]) && (EdThick[k] > EdThick[NumberID]) && (AtLeastOne))
                {
                    cout << "Number ID Changed from " << NumberID << " to " << k << endl;
                    NumberID = k;
                }
            }
            branchNumberID -> Fill();
        }

        //branchNumberID -> Write();
        fp[i] -> Write();
        //fp[i] -> Close();


    }



    return 0;
}