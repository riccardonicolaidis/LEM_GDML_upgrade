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

using namespace std;

void GeometricFactor()
{

    // Definitions for the Geometrical Factor part
    TH1D    *hAcceptance[3];
    TH1D    *hFinal[3];
    TH1D    *hIncident[3];
    TCanvas *cFinal[3];
    double ERange[3][2];
    double ERangeGen[3][2];
    double dNdEGen;
    double NgenCalibrated;
    double Area;
    double Radius;
    int NGen;

    // ********************************************
    //           GEOMETRICAL FACTOR SETTINGS
    // ********************************************
    // Range of energy generated (see in run_..._.mac)
    ERangeGen[0][0] = 0.001;
    ERangeGen[0][1] = 10.;
    ERangeGen[1][0] = 0.001;
    ERangeGen[1][1] = 90.;
    ERangeGen[2][0] = 0.001;
    ERangeGen[2][1] = 300.;
    // Range of energy of the plots
    ERange[0][0] = 0.001;
    ERange[0][1] = 10.;
    ERange[1][0] = 0.001;
    ERange[1][1] = 90.;
    ERange[2][0] = 0.001;
    ERange[2][1] = 300.;
    // Number of bins
    int Nbins[3] = {70, 100, 100};
    Area = TMath::Pi() * 1.8 * 1.8;
    NGen = 50000000;
    // ********************************************
    // ********************************************
/* -------------------------------------------------------------------------- */
    /*                                   OPTIONS                                  */
    /* -------------------------------------------------------------------------- */

    int Nfiles = 3;


    cout << "Nfiles = " << Nfiles << endl;


    /* -------------------------------------------------------------------------- */
    /*                                 /FILE NAMES                                */
    /* -------------------------------------------------------------------------- */

    TString FileName[Nfiles];
    
    // Read the FileNames.txt and for each line of the file fill the TString FileName[]
    ifstream FileNames("FileNames.txt");
    if(!FileNames.is_open())
    {
        cout << "FileNames.txt not found" << endl;
        return;
    }
    else
    {
        for(int i = 0; i < Nfiles; i++)
        {
            FileNames >> FileName[i];
        }
    }


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
    /*                                BRANCH NAMES                                */
    /* -------------------------------------------------------------------------- */




    int Ntot = 7;
    int N_plastic = 2;
    int NumberOfBranches = 11 + 2*Ntot + N_plastic;


    TString BranchName[11+2*Ntot+N_plastic];
    TString TotEnergyName[Ntot];
    TString TotEnergyCondition[Ntot];
    TString PIDName[Ntot];
    TString DirName[3];
    TString PolarAngle[2];
    TString NewPolarAngle[2];
    TString AliasETot = "ETot";

    TString ThinTot;
    TString ThickTot;

    DirName[0] = "DirX";
    DirName[1] = "DirY";
    DirName[2] = "DirZ";

    PolarAngle[0] = "dirTheta";
    PolarAngle[1] = "dirPhi";

    NewPolarAngle[0] = "dirThetaNew";
    NewPolarAngle[1] = "dirPhiNew";

    int j;




   

    /* -------------------------------------------------------------------------- */
    /*                             PLOTTING PARAMETERS                            */
    /* -------------------------------------------------------------------------- */

    double ResSilicon  = 0.01;
    double ResCZT      = 0.05;
    double ResPlastic  = 0.3;
    double E_min_thin  = 0.01;  // Thick layer
    double E_min_thick = 0.03; // Thin layer
    double E_th_Vetoed = 0.2;  // Energy dispersion (Veto threshold)
    double E_th_plastic = 0.2;


    double Emaxx = log10(500);
    double Emaxy = log10(400);
    double Eminx = log10(0.08);
    double Eminy = log10(0.0004);
    double Nbinsx = 600;
    double Nbinsy = 600;


/* -------------------------------------------------------------------------- */
/*               Constructiong the Branch names to read the tree              */
/* -------------------------------------------------------------------------- */


    BranchName[0] = "NumberID";
    BranchName[1] = "RandEnergy";
    BranchName[2] = "RandNumber";
    BranchName[3] = "Xgen";
    BranchName[4] = "Ygen";
    BranchName[5] = "Zgen";
    BranchName[6] = "pDirX";
    BranchName[7] = "pDirY";
    BranchName[8] = "pDirZ";

    j = 9;
    for(int i = 0; i < N_plastic; ++i)
    {
        BranchName[9 + i] = Form("Ed_LV_P%d",i+1);
        ++j;
    }
    
    BranchName[j++] = "Ed_LV_V1";
    BranchName[j++] = "Ed_LV_V2";

    int IndexStartSensors = j;


    for(int i = 0; i < Ntot; ++i)
    {
        if(i == 0)
        {
            BranchName[IndexStartSensors + i]        = Form("Ed_LV_THIN");
            BranchName[IndexStartSensors + i + Ntot] = Form("Ed_LV_THICK");
        }
        else
        {
            BranchName[IndexStartSensors + i]        = Form("Ed_LV_THIN00%d", i);
            BranchName[IndexStartSensors + i + Ntot] = Form("Ed_LV_THICK00%d", i);
        }


        if (i == 0)
        {
            ThinTot  = Form("(%s)", BranchName[IndexStartSensors + i].Data());
            ThickTot = Form("(%s)", BranchName[IndexStartSensors + i + Ntot].Data());
        }
        else
        {
            ThinTot  = Form("(%s + %s)",ThinTot.Data(), BranchName[IndexStartSensors + i].Data());
            ThickTot = Form("(%s + %s)",ThickTot.Data(), BranchName[IndexStartSensors + i + Ntot].Data());                
        }
    }


    cout << "Branches Names : \n############################" << endl;
    for(int i = 0; i < NumberOfBranches; ++i)
    {
        cout << i << " " << BranchName[i].Data() << endl;
    }


    /* -------------------------------------------------------------------------- */
    /*                              Alias definitions                             */
    /* -------------------------------------------------------------------------- */

    /* -------------------------------------------------------------------------- */
    /*                              ALIAS DEFINITIONS                             */
    /* -------------------------------------------------------------------------- */

    for(int i = 0; i < Nfiles ; ++i)
    {
        cout << "Setting alias in File " << i << " : " << FileName[i].Data() << endl;
        /* -------------------------------- SMEARING -------------------------------- */
        /* ------------------------ YOU ONLY NEED TO ADD A g ------------------------ */

        for(int k = 0; k < N_plastic+2; ++k)
        {
            Edep[i] -> SetAlias(Form("wnorm%d",9+k),"(sin(2 *pi*rndm)*sqrt(-2*log(rndm)))");
            Edep[i] -> SetAlias(Form("g%s",BranchName[9+k].Data()), Form("((%s)*(1 + (wnorm%d * %f)))",BranchName[k+9].Data(),k+9,ResPlastic));
            cout << Form("Alias: g%s",BranchName[9+k].Data()) << endl;
        }


        for(int k = 0; k < 2*Ntot; ++k)
        {
            Edep[i] -> SetAlias(Form("wnorm%d",k),"(sin(2 *pi*rndm)*sqrt(-2*log(rndm)))");
            Edep[i] -> SetAlias(Form("g%s",BranchName[IndexStartSensors + k].Data()), Form("((%s)*(1 + (wnorm%d * %f)))",BranchName[IndexStartSensors + k].Data(),k,ResSilicon));
            cout << Form("Alias: g%s",BranchName[IndexStartSensors + k].Data()) << endl;
        }

        /* ------------------- Incident direction of the particle ------------------- */

        Edep[i] -> SetAlias("NormP", Form("(TMath::Sqrt(TMath::Power(%s,2) + TMath::Power(%s,2) + TMath::Power(%s,2)))", BranchName[6].Data(), BranchName[7].Data(), BranchName[8].Data()));
        for(int k = 6; k <= 8; ++k)
        {
            Edep[i] -> SetAlias(DirName[k-6].Data(), Form("((-%s)/(NormP))", BranchName[k].Data()));
            cout << Form("Alias: %s",DirName[k-6].Data()) << endl;
        }

        // Legend
        // [0] : Theta
        // [1] : Phi
        Edep[i] -> SetAlias(PolarAngle[0].Data()   , Form("((TMath::ACos(-%s)))", DirName[2].Data())); // *(180/3.415927))
        Edep[i] -> SetAlias(PolarAngle[1].Data()   , Form("(TMath::ATan2(-%s,-%s))", DirName[1].Data(), DirName[0].Data())); // *(180/3.415927))
        Edep[i] -> SetAlias(NewPolarAngle[1].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Cos(%s)),(TMath::Cos(%s))))",              PolarAngle[0].Data(), PolarAngle[0].Data(), PolarAngle[1].Data() )); // *(180/3.415927))
        Edep[i] -> SetAlias(NewPolarAngle[0].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Sin(%s)*TMath::Sin(%s)),TMath::Cos(%s)))", PolarAngle[0].Data(), PolarAngle[1].Data(), NewPolarAngle[1].Data(), PolarAngle[0].Data())); // *(180/3.415927))
        
        cout << Form("Alias: %s",PolarAngle[0].Data()) << endl;
        cout << Form("Alias: %s",PolarAngle[1].Data()) << endl;
        cout << Form("Alias: %s",NewPolarAngle[0].Data()) << endl;
        cout << Form("Alias: %s",NewPolarAngle[1].Data()) << endl;

        /* -------------------- Particle identification parameter ------------------- */

        cout << "Defining total energy in File " << i << " : " << FileName[i].Data() << endl;
        
        for(int k = 0; k < Ntot; ++k)
        {
            TotEnergyName[k] = Form("Tot%d",k);
            PIDName[k] = Form("PID%d",k);
            TotEnergyCondition[k]  = Form("(%s + %s",BranchName[IndexStartSensors+k].Data(), BranchName[IndexStartSensors + Ntot + k].Data());

            for(int l = 0 ; l < N_plastic ; ++l)
            {
                TotEnergyCondition[k] += Form("+ (g%s)*(g%s > %g)", BranchName[9+l].Data(), BranchName[9+l].Data(),E_th_plastic);
            }
            TotEnergyCondition[k] += ")";
            Edep[i] -> SetAlias(TotEnergyName[k], TotEnergyCondition[k]);
            Edep[i] -> SetAlias(PIDName[k], Form("(TMath::Log10(g%s * %s))",BranchName[IndexStartSensors + k].Data(), TotEnergyName[k].Data()));
            ++j;
        }
                
        
        /* ------------------------------- Total Thin ------------------------------- */
        /* ------------------------------- Total Thick ------------------------------ */
    }


    /* -------------------------------------------------------------------------- */
    /*                   Definition of the SELECTION conditions                   */
    /* -------------------------------------------------------------------------- */

    TString ConditionPairSilicon[Ntot];
    TString ConditionPairSiliconAll;
    TString ConditionEnergyDispersion;
    TString ConditionDrilledVeto;
    TString ConditionGoodEvents;
    TString ConditionGoodEventsSinglePair[Ntot];


    for(int i = 0; i< Ntot; ++i)
    {
        cout << "Defining good events for pair" << BranchName[IndexStartSensors+i].Data() << " & " <<BranchName[IndexStartSensors+ Ntot+i].Data() << endl;
        ConditionPairSilicon[i] = Form("((%s > %g) && (%s > %g))", BranchName[IndexStartSensors+i].Data(), E_min_thin, BranchName[IndexStartSensors + Ntot + i].Data(), E_min_thick);
        cout << "ConditionPairSilicon[" << i << "] = " << ConditionPairSilicon[i].Data() << endl;
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s > %g) && (%s > %g))", BranchName[IndexStartSensors+i].Data(), E_min_thin, BranchName[IndexStartSensors + Ntot + i].Data(), E_min_thick);
        }
        else 
        {
            ConditionPairSiliconAll += Form("|| ((%s > %g) && (%s > %g))", BranchName[IndexStartSensors+i].Data(), E_min_thin, BranchName[IndexStartSensors + Ntot + i].Data(), E_min_thick);
        }
    }

    E_th_Vetoed = 0.00000001;
    ConditionDrilledVeto       = Form("(%s <= %g)", BranchName[9+N_plastic].Data(), E_th_Vetoed);
    ConditionEnergyDispersion  = Form("((%s)" , BranchName[1].Data());
    ConditionEnergyDispersion += Form("- (%s)", ThinTot.Data());
    ConditionEnergyDispersion += Form("- (%s)", ThickTot.Data());
    for(int i = 0; i < N_plastic; ++i)
    {
        ConditionEnergyDispersion += Form("- (%s)", BranchName[9+i].Data());
    }
    ConditionEnergyDispersion += Form(") < %g", E_th_Vetoed);
    ConditionGoodEvents        = Form("(%s) && (%s) && (%s)", ConditionPairSiliconAll.Data(), ConditionDrilledVeto.Data(), ConditionEnergyDispersion.Data());
    for(int i = 0; i < Ntot; ++i)
    {
        cout << "Defining good events for pair" << BranchName[IndexStartSensors+i].Data() << " & " <<BranchName[IndexStartSensors+ Ntot+i].Data() << endl;
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionDrilledVeto.Data(), ConditionEnergyDispersion.Data());
    }




    for(int i = 0 ; i < 3 ; ++i)
    {
        cFinal[i] = new TCanvas(Form("cFinal_%d", i), Form("Final energy %d", i), 900, 700);
        
        hIncident[i]   = new TH1D(Form("hIncident_%d", i), Form("Incident energy %d", i), Nbins[i], ERange[i][0], ERange[i][1]);
        hIncident[i] -> SetTitle(Form("Incident energy %d: Log10(E[MeV]): Counts", i));
        hAcceptance[i] = new TH1D(Form("hAcceptance_%d", i), Form("Acceptance %d", i), Nbins[i], ERange[i][0], ERange[i][1]);        
        hAcceptance[i] -> SetTitle(Form("Accepted events %d: Log10(E[MeV]): Counts", i));
        Edep[i] -> Draw(Form("%s>>hIncident_%d", BranchName[1].Data(), i), "", "");
        Edep[i] -> Draw(Form("%s>>hAcceptance_%d", BranchName[1].Data(), i), ConditionGoodEvents.Data(), "");
        
        NgenCalibrated = NGen * (ERange[i][1] - ERange[i][0])/ (ERangeGen[i][1] - ERangeGen[i][0]);;
        dNdEGen = NGen * (hAcceptance[i] -> GetBinWidth(1))/ (ERangeGen[i][1] - ERangeGen[i][0]);
        
        hFinal[i] = (TH1D*) hAcceptance[i] -> Clone(Form("hFinal_%d", i));
        hFinal[i] -> Scale(TMath::Pi() * Area / dNdEGen);
        hFinal[i] -> SetTitle(Form(" ; Energy [MeV]; Geometrical Factor [cm^2 rad]"));
        hFinal[i] -> SetStats(0);
        hFinal[i] -> SetMarkerStyle(8);
        hFinal[i] -> SetMarkerSize(0.8);
        hFinal[i] -> SetMarkerColor(kBlue);
        hFinal[i] -> SetLineWidth(2);
        hFinal[i] -> Draw();
        gPad -> SetGrid();
        gPad -> SetLogy();
        cFinal[i] -> Update();
        cFinal[i] -> Draw();
        cout << "#############################" << endl;
        cout << "Geometrical factor " << i << ": " << endl;
        cout << TMath::Pi() * Area * (hAcceptance[i] -> GetEntries()) / NgenCalibrated << endl;

        // Save image in all possible formats
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.png",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.pdf",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.root",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.C",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.eps",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.ps",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.jpg",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.gif",i));
        cFinal[i] -> SaveAs(Form("../docs/assets/GeometricalFactor/Acceptance%d.jpeg",i));
    }


    /* -------------------------------------------------------------------------- */
    /*                                New Settings                                */
    /* -------------------------------------------------------------------------- */

    TH1D    *hAcceptance2[3];
    TH1D    *hFinal2[3];
    TH1D    *hIncident2[3];
    double Xmin = 0.09;
    double Xmax = 400.0;
    // Number of bins
    int Nbins2[3] = {4000, 800, 600};

    
    TCanvas *c1 = new TCanvas("c1", "Geometrical factor", 1000, 600);

    


    for(int i = 0 ; i < 3 ; ++i)
    {
        c1 -> cd();
        hIncident2[i]   = new TH1D(Form("hIncident2_%d", i), Form("Incident energy %d", i), Nbins2[i], Xmin, Xmax);
        hIncident2[i] -> SetTitle(Form("Incident energy %d: Log10(E[MeV]): Counts", i));
        hAcceptance2[i] = new TH1D(Form("hAcceptance2_%d", i), Form("Acceptance %d", i), Nbins2[i], Xmin, Xmax);        
        hAcceptance2[i] -> SetTitle(Form("Accepted events %d: Log10(E[MeV]): Counts", i));
        Edep[i] -> Draw(Form("%s>>hIncident2_%d", BranchName[1].Data(), i), "", "");
        Edep[i] -> Draw(Form("%s>>hAcceptance2_%d", BranchName[1].Data(), i), ConditionGoodEvents.Data(), "");
        
        dNdEGen = NGen * (hAcceptance2[i] -> GetBinWidth(1))/ (ERangeGen[i][1] - ERangeGen[i][0]);
        
        hFinal2[i] = (TH1D*) hAcceptance2[i] -> Clone(Form("hFinal2_%d", i));
        hFinal2[i] -> Scale(TMath::Pi() * Area / dNdEGen);
        hFinal2[i] -> SetTitle(Form(" ; Energy [MeV]; Geometrical Factor [cm^2 sr]"));
        hFinal2[i] -> SetStats(0);
        hFinal2[i] -> SetMarkerStyle(8);
        hFinal2[i] -> SetMarkerSize(0.8);
        hFinal2[i] -> SetLineWidth(2);
    }
    delete c1;

    TCanvas *cGeomAll = new TCanvas("cGeomAll", "Geometrical factor", 1200, 600);



    TColor *color[3];
    color[0] = new TColor(1756, 242./255., 53./255, 141./255);
    color[1] = new TColor(1757, 4./255., 104./255, 191./255);
    color[2] = new TColor(1758, 242./255., 152./255, 73./255);



    for(int i = 0 ; i < 3 ; ++i)
    {
        hFinal2[i] -> GetYaxis() -> SetRangeUser(1e-4,1.3);
        hFinal2[i] -> GetXaxis() -> SetRangeUser(1e-2,300.);
        hFinal2[i] -> SetLineColor(color[i] -> GetNumber());
        hFinal2[i] -> SetMarkerColor(color[i] -> GetNumber());

        hFinal2[i] -> Draw("Esame");        
    }


    TLegend  *legend = new TLegend(0.1, 0.1, 0.3, 0.3);
    legend -> AddEntry(hFinal2[0], "electrons", "lep");
    legend -> AddEntry(hFinal2[1], "protons", "lep");
    legend -> AddEntry(hFinal2[2], "alpha", "lep");
    //legend -> AddEntry(gDEMETER, "DEMETER", "lp");
    //legend -> AddEntry(gHEPPL, "HEPP-L : narrow", "f");
    //legend -> AddEntry(gHEPPL2, "HEPP-L : wide", "f");
    legend -> Draw();

    gPad -> SetGrid();
    gPad -> SetLogy();
    gPad -> SetLogx();





    return;
}