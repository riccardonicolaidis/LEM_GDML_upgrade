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
    ifstream FileNames("pFileNames.txt");
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
    /*                           GEOMETRICAL DEFINITIONS                          */
    /* -------------------------------------------------------------------------- */


    double ThetaDir[Ntot];
    double PhiDir[Ntot];
    double xHole[Ntot];
    double yHole[Ntot];
    TMarker *markerPositionHole[Ntot];
    TMarker *markerProjDir[Ntot];

   

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
        BranchName[9 + i] = Form("Ed_Veto%d",i);
        ++j;
    }
    
    BranchName[j++] = "Ed_DrilledVeto";
    BranchName[j++] = "Ed_BottomVeto";

    int IndexStartSensors = j;


    int CopyNumber = 0;
    for(int i = 0; i < Ntot; ++i)
    {
        BranchName[j]        = Form("Thin_%d_ID%d",i,CopyNumber);
        BranchName[j + Ntot] = Form("Thick_%d_ID%d",i,CopyNumber);
            if (CopyNumber == 0)
            {
                ThinTot  = Form("(%s)", BranchName[j].Data());
                ThickTot = Form("(%s)", BranchName[j + Ntot].Data());

                xHole[CopyNumber]              = 0.;
                yHole[CopyNumber]              = 0.;
                PhiDir[CopyNumber]             = 0.;
                ThetaDir[CopyNumber]           = 0.;
            }
            else
            {
                ThinTot  = Form("(%s + %s)",ThinTot.Data(), BranchName[j].Data());
                ThickTot = Form("(%s + %s)",ThickTot.Data(), BranchName[j + Ntot].Data());
                
                double phi = 2*TMath::Pi()*(CopyNumber-1)/(Ntot-1);
                double theta = 37 * TMath::Pi() / 180.;
                double RadiusEptagon = 2.2 + sqrt(150/TMath::Pi());
                
                xHole[CopyNumber]  = RadiusEptagon * std::cos(phi);
                yHole[CopyNumber]  = RadiusEptagon * std::sin(phi);

                PhiDir[CopyNumber]             = phi;
                ThetaDir[CopyNumber]           = theta;
            }

            markerPositionHole[CopyNumber] = new TMarker(xHole[CopyNumber],yHole[CopyNumber],20);
            markerPositionHole[CopyNumber] -> SetMarkerColor(kRed);
            markerPositionHole[CopyNumber] -> SetMarkerSize(1.2);

            markerProjDir[CopyNumber]      = new TMarker(TMath::Cos(PhiDir[CopyNumber])*ThetaDir[CopyNumber],TMath::Sin(PhiDir[CopyNumber])*ThetaDir[CopyNumber],20);
            markerProjDir[CopyNumber]      -> SetMarkerColor(kRed);
            markerProjDir[CopyNumber]      -> SetMarkerSize(1.2);

            ++j;
            ++CopyNumber;
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
        }


        for(int k = IndexStartSensors; k < (12+Ntot*2); ++k)
        {
            Edep[i] -> SetAlias(Form("wnorm%d",k),"(sin(2 *pi*rndm)*sqrt(-2*log(rndm)))");
            Edep[i] -> SetAlias(Form("g%s",BranchName[k].Data()), Form("((%s)*(1 + (wnorm%d * %f)))",BranchName[k].Data(),k,ResSilicon));
        }

        /* ------------------- Incident direction of the particle ------------------- */

        Edep[i] -> SetAlias("NormP", Form("(TMath::Sqrt(TMath::Power(%s,2) + TMath::Power(%s,2) + TMath::Power(%s,2)))", BranchName[6].Data(), BranchName[7].Data(), BranchName[8].Data()));
        for(int k = 6; k <= 7; ++k)
        {
            Edep[i] -> SetAlias(DirName[k-6].Data(), Form("((%s)/(NormP))", BranchName[k].Data()));
        }
        Edep[i] -> SetAlias(DirName[2].Data(), Form("(-(%s)/(NormP))", BranchName[8].Data()));

        // Legend
        // [0] : Theta
        // [1] : Phi
        Edep[i] -> SetAlias(PolarAngle[0].Data()   , Form("((TMath::ACos(%s)))", DirName[2].Data())); // *(180/3.415927))
        Edep[i] -> SetAlias(PolarAngle[1].Data()   , Form("(TMath::ATan2(%s,%s))", DirName[1].Data(), DirName[0].Data())); // *(180/3.415927))
        Edep[i] -> SetAlias(NewPolarAngle[1].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Cos(%s)),(TMath::Cos(%s))))",              PolarAngle[0].Data(), PolarAngle[0].Data(), PolarAngle[1].Data() )); // *(180/3.415927))
        Edep[i] -> SetAlias(NewPolarAngle[0].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Sin(%s)*TMath::Sin(%s)),TMath::Cos(%s)))", PolarAngle[0].Data(), PolarAngle[1].Data(), NewPolarAngle[1].Data(), PolarAngle[0].Data())); // *(180/3.415927))
        
        /* -------------------- Particle identification parameter ------------------- */

        j = IndexStartSensors;
        CopyNumber = 0;
        cout << "Defining total energy in File " << i << " : " << FileName[i].Data() << endl;
        
        for(int k = 0; k < Ntot; ++k)
        {
            TotEnergyName[CopyNumber] = Form("Tot_%d_ID%d",k,CopyNumber);
            PIDName[CopyNumber] = Form("PID_%d_ID%d",k,CopyNumber);
            TotEnergyCondition[CopyNumber]  = Form("(%s + %s",BranchName[j].Data(), BranchName[j + Ntot].Data());

            for(int l = 0 ; l < N_plastic ; ++l)
            {
                TotEnergyCondition[CopyNumber] += Form("+ (g%s)*(g%s > %g)", BranchName[9+l].Data(), BranchName[9+l].Data(),E_th_plastic);
            }
            TotEnergyCondition[CopyNumber] += ")";
            Edep[i] -> SetAlias(TotEnergyName[CopyNumber], TotEnergyCondition[CopyNumber]);
            Edep[i] -> SetAlias(PIDName[CopyNumber], Form("(TMath::Log10(g%s * %s))",BranchName[j].Data(), TotEnergyName[CopyNumber].Data()));
            ++j;
            ++CopyNumber;
            cout << Form("i= %d CopyNumber= %d",k, CopyNumber) << endl;
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

    E_th_Vetoed = 0.01;
    ConditionDrilledVeto       = Form("(%s <= %g)", BranchName[9+N_plastic].Data(), E_th_Vetoed);
    //ConditionEnergyDispersion  = Form("((%s) - (%s) - (%s) - (%s) - (%s) - (%s) - (%s)) < %g", BranchName[1].Data(), ThinTot.Data(), ThickTot.Data(), BranchName[9].Data(),BranchName[43].Data(), BranchName[44].Data(), BranchName[45].Data(), E_th_Vetoed);
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

    TString ChipSelectionMask;
    TString MaskTotalChip;

    int ChipList[Ntot];
    for(int i = 0; i < Ntot; ++i)
    {
        ChipList[i] = i;
    }

    for(int i = 0; i < Ntot; ++i)
    {
        if(i == 0)
        {
            ChipSelectionMask = Form("(%s == %i)", BranchName[0].Data(), ChipList[i]);
            cout << ChipSelectionMask.Data() << endl;
        } 
        else
        {
            ChipSelectionMask = Form("%s || (%s == %i)", ChipSelectionMask.Data(), BranchName[0].Data(), ChipList[i]);
            cout << ChipSelectionMask.Data() << endl;
        }
    }
    MaskTotalChip = Form("(%s) && (%s)", ChipSelectionMask.Data(), ConditionGoodEvents.Data());

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
    int ColorsPlot[8] = {kRed,kRed,kBlue,kYellow,kYellow,kBlack,kBlue};



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
            nDirX = pDirX/Norm;
            nDirY = pDirY/Norm;
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
        for(CopyNumber = 0 ; CopyNumber < Ntot; ++CopyNumber)
        {
            
            Edep[i] -> Draw("ProjY:ProjX", ConditionGoodEventsSinglePair[CopyNumber].Data(), "");
            gAngles[i][CopyNumber] = new TGraph(Edep[i]->GetSelectedRows(), Edep[i]->GetV2(), Edep[i]->GetV1());
            gAngles[i][CopyNumber] -> SetMarkerColor(ColorsPlot[CopyNumber]);
            gAngles[i][CopyNumber] -> SetMarkerStyle(8);
            gAngles[i][CopyNumber] -> SetMarkerSize(1.);

            gAngles[i][CopyNumber] -> Print();

            //gAngles[i][CopyNumber] -> Print();
            cout << Form("%d \t%g \t%g", CopyNumber, gAngles[i][CopyNumber]-> GetRMS(1), gAngles[i][CopyNumber]-> GetRMS(2)) << " " << Edep[i]->GetSelectedRows()<< endl;
            mgAngles[i] -> Add(gAngles[i][CopyNumber]);
            if(i > 0)
            {
                mgAnglesProtonsAlpha -> Add(gAngles[i][CopyNumber]);
            }
        }

        gPad -> SetGrid();
        mgAngles[i] -> GetXaxis() -> SetTitle("Angle projection X [deg]");
        mgAngles[i] -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

        mgAngles[i] -> GetXaxis() -> SetRangeUser(-200, 200);
        mgAngles[i] -> GetYaxis() -> SetRangeUser(-200, 200);

        mgAngles[i] -> Draw("AP");


        cAngleGraphs[i] -> SaveAs(Form("../docs/Angles_%d.pdf", i));
        cAngleGraphs[i] -> SaveAs(Form("../docs/Angles_%d.svg", i));
        cAngleGraphs[i] -> SaveAs(Form("../docs/Angles_%d.png", i));


    }
    

    CAnglesProtonsAlpha = new TCanvas("AnglesProtonsAlpha", "AnglesProtonsAlpha", 900, 700);
    CAnglesProtonsAlpha -> cd();
    mgAnglesProtonsAlpha -> GetXaxis() -> SetTitle("Angle projection X [deg]");
    mgAnglesProtonsAlpha -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

    mgAnglesProtonsAlpha -> GetXaxis() -> SetRangeUser(-200, 200);
    mgAnglesProtonsAlpha -> GetYaxis() -> SetRangeUser(-200, 200);

    mgAnglesProtonsAlpha -> Draw("AP");
    gPad -> SetGrid();


    

    CAnglesProtonsAlpha -> SaveAs("../docs/assets/images/AnglesProtonsAlpha.pdf");
    CAnglesProtonsAlpha -> SaveAs("../docs/assets/images/AnglesProtonsAlpha.svg");
    CAnglesProtonsAlpha -> SaveAs("../docs/assets/images/AnglesProtonsAlpha.png");



    return;
}