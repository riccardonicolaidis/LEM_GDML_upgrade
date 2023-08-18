#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "TCanvas.h"
#include "TMath.h"
#include "TH1D.h"
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TMarker.h"
#include "TGraph.h"
#include "TBranch.h"
#include "TMultiGraph.h"
#include "TLine.h"

using namespace std;



void AngleResolutionDescoped()
{

    int Nfiles = 3;
    /* -------------------------------------------------------------------------- */
    /*                                   OPTIONS                                  */
    /* -------------------------------------------------------------------------- */




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



    /* -------------------------------------------------------------------------- */
    /*                     Specific part for ANGLE RESOLUTION                     */
    /* -------------------------------------------------------------------------- */

    TCanvas *cAngles[Nfiles];
    TH2D *hAngles[Nfiles];

    int ChipList[Ntot];
    for(int i = 0; i < Ntot; ++i)
    {
        ChipList[i] = i;
    }


    //return;

    /* -------------------------------------------------------------------------- */
/*     Definisco i TGraph per gli angoli in modo da avere diversi grafici     */
/* -------------------------------------------------------------------------- */

    TGraph *gAngles[Nfiles][Ntot];
    TMultiGraph *mgAngles[Nfiles];
    TMultiGraph *mgAnglesProtonsAlpha;
    TCanvas *CAnglesProtonsAlpha;
    TFile *tProjFile[Nfiles];
    TTree *tProj[Nfiles];
    TCanvas *cAngleGraphs[3];
    int ColorsPlot[8] = {kBlack,kBlue,kRed,kBlue,kRed,kBlue,kRed};



    double projXAngle, projYAngle;
    double pDirX, pDirY, pDirZ;
    double Norm;
    double nDirX, nDirY, nDirZ;
    double Theta, Phi;

    mgAnglesProtonsAlpha = new TMultiGraph();
  

    for(int i = 0; i < Nfiles; ++i)
    {
        cAngleGraphs[i] = new TCanvas(Form("Directions: %d", i),Form("Directions: %d", i),900,700);
        cAngleGraphs[i] -> cd();

        tProjFile[i] = new TFile(Form("../ROOT/Tree%d.root",i),"RECREATE");
        tProj[i] = new TTree(Form("Proj_%d", i),Form("Proj_%d", i));

        tProj[i] -> Branch("ProjX", &projXAngle, "ProjX/D");
        tProj[i] -> Branch("ProjY", &projYAngle, "ProjY/D");
        tProj[i] -> Branch("pDirX", &pDirX, "pDirX/D");
        tProj[i] -> Branch("pDirY", &pDirY, "pDirY/D");
        tProj[i] -> Branch("pDirZ", &pDirZ, "pDirZ/D");
        tProj[i] -> Branch("Theta", &Theta, "Theta/D");
        tProj[i] -> Branch("Phi", &Phi, "Phi/D");


        Edep[i] -> SetBranchAddress(BranchName[6].Data(), &pDirX);
        Edep[i] -> SetBranchAddress(BranchName[7].Data(), &pDirY);
        Edep[i] -> SetBranchAddress(BranchName[8].Data(), &pDirZ);

        mgAngles[i] = new TMultiGraph();

        for(int k = 0; k < (Edep[i] -> GetEntries()); ++k)
        {
            Edep[i] -> GetEntry(k);
            Norm= TMath::Sqrt(pDirX*pDirX + pDirY*pDirY + pDirZ*pDirZ);
            nDirX = -pDirX/Norm;
            nDirY = -pDirY/Norm;
            nDirZ = -pDirZ/Norm;

            Theta = TMath::ACos(nDirZ);
            Phi   = TMath::ATan2(nDirY, nDirX);

            projXAngle = (Theta) * TMath::Cos(Phi) * (180/TMath::Pi());
            projYAngle = (Theta) * TMath::Sin(Phi) * (180/TMath::Pi());

            tProj[i] -> Fill();
        }

        Edep[i] -> AddFriend(tProj[i]);
        tProjFile[i] -> Write();
        

        cout << Form("Particle %d", i) << endl;
        cout << "Copy No \t Std X \t Std Y \t # Events " << endl;
        for(int k = 0 ; k < Ntot; ++k)
        {
            
            Edep[i] -> Draw("ProjY:ProjX", ConditionGoodEventsSinglePair[k].Data(), "");
            gAngles[i][k] = new TGraph(Edep[i]->GetSelectedRows(), Edep[i]->GetV2(), Edep[i]->GetV1());
            gAngles[i][k] -> SetMarkerColor(ColorsPlot[k]);
            gAngles[i][k] -> SetMarkerStyle(8);
            gAngles[i][k] -> SetMarkerSize(0.6);

            gAngles[i][k] -> Print();

            //gAngles[i][CopyNumber] -> Print();
            cout << Form("%d \t%g \t%g", k, gAngles[i][k]-> GetRMS(1), gAngles[i][k]-> GetRMS(2)) << " " << Edep[i]->GetSelectedRows()<< endl;
            mgAngles[i] -> Add(gAngles[i][k]);
            if(i > 0)
            {
                mgAnglesProtonsAlpha -> Add(gAngles[i][k]);
            }
        }

        gPad -> SetGrid();
        mgAngles[i] -> GetXaxis() -> SetTitle("Angle projection X [deg]");
        mgAngles[i] -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

        mgAngles[i] -> GetXaxis() -> SetRangeUser(-180, 180);
        mgAngles[i] -> GetYaxis() -> SetRangeUser(-180, 180);

        mgAngles[i] -> Draw("AP");


        cAngleGraphs[i] -> SaveAs(Form("../docs/Angles_%d.pdf", i));
        cAngleGraphs[i] -> SaveAs(Form("../docs/Angles_%d.svg", i));
        cAngleGraphs[i] -> SaveAs(Form("../docs/Angles_%d.png", i));


    }
    

    CAnglesProtonsAlpha = new TCanvas("AnglesProtonsAlpha", "AnglesProtonsAlpha", 900, 700);
    CAnglesProtonsAlpha -> cd();
    mgAnglesProtonsAlpha -> GetXaxis() -> SetTitle("Angle projection X [deg]");
    mgAnglesProtonsAlpha -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

    mgAnglesProtonsAlpha -> GetXaxis() -> SetRangeUser(-180, 180);
    mgAnglesProtonsAlpha -> GetYaxis() -> SetRangeUser(-180, 180);

    mgAnglesProtonsAlpha -> Draw("AP");
    gPad -> SetGrid();


    

    CAnglesProtonsAlpha -> SaveAs("../docs/assets/images/AnglesProtonsAlpha.pdf");
    CAnglesProtonsAlpha -> SaveAs("../docs/assets/images/AnglesProtonsAlpha.svg");
    CAnglesProtonsAlpha -> SaveAs("../docs/assets/images/AnglesProtonsAlpha.png");



    return;
}