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
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TMath.h"


using namespace std;

// root SetAliases.C\(\"ciao\"\)

void SetAliases(TString filename,
                TString filename_noExt,
                TString destination,
                int NumberPairsSensors = 5,
                double ResSilicon = 0.03,
                double ResPlastic = 0.03,
                double E_thr_Thin = 0.04,
                double E_thr_Thick = 0.04,
                double E_thr_Plastic = 0.04)
{
    std::cout << "Setting aliases for " << filename << endl;

    TString AliasETot = "ETot";

    TString DirName[3] = {"DirX", "DirY", "DirZ"};
    TString PolarAngle[2] = {"dirTheta", "dirPhi"};
    TString NewPolarAngle[2] = {"dirThetaNew", "dirPhiNew"};


    int NumberBranches = 7 + 3 + 2*NumberPairsSensors;
    std::cout << "NumberBranches = " << NumberBranches << endl;


    TString BranchName[NumberBranches];

    int iBranch = 0;
    BranchName[iBranch++] = "RandEnergy";
    BranchName[iBranch++] = "Xgen";
    BranchName[iBranch++] = "Ygen";
    BranchName[iBranch++] = "Zgen";
    int iBranchDir = iBranch;
    BranchName[iBranch++] = "pDirX";
    BranchName[iBranch++] = "pDirY";
    BranchName[iBranch++] = "pDirZ";

    int iBranchEnergies = iBranch;
    BranchName[iBranch++] = "Ed_LV_Veto_bottom";
    BranchName[iBranch++] = "Ed_LV_Veto_top";
    int iBranchEnergies_Calo = iBranch;
    BranchName[iBranch++] = "Ed_LV_Plastic_calo";

    int iStartSensors = iBranch;
    int iStartSensors_Thin = iStartSensors;
    int iStartSensors_Thick = iStartSensors + NumberPairsSensors;

    for(int iPair = 0; iPair < NumberPairsSensors; iPair++)
    {
        BranchName[iBranch] = "Ed_LV_Silicon_Thin_" + to_string(iPair);
        BranchName[iBranch+NumberPairsSensors] = "Ed_LV_Silicon_Thick_" + to_string(iPair);
        iBranch++;
    }

    for(int i = 0; i < NumberBranches; i++)
    {
        std::cout << "BranchName[" << i << "] = " << BranchName[i] << endl;
    }

    TFile *file = new TFile(filename, "READ");
    TFile *file_alias = new TFile(destination +"/" + filename_noExt + "_alias.root", "RECREATE");
    TTree *Edep_old = (TTree*) (file -> Get("Edep"));

    file_alias -> cd();
    TTree *Edep = Edep_old -> CloneTree();

    // Number of branches with energy deposition
    int NumberEnergySensors = 2*NumberPairsSensors + 2 + 1;


    // Definition of gaussian random variables
    // For smearing the energy deposition
    for(int i = 0; i < NumberEnergySensors; i++)
    {
        Edep -> SetAlias(Form("wnorm%d", i), "(sin(2 *pi*rndm)*sqrt(-2*log(rndm)))");
    }

    // Definition of the gaussian smeared energies deposited
    for(int i = 0; i < NumberEnergySensors; ++i)
    {
        Edep -> SetAlias(Form("g%s", BranchName[i + iBranchEnergies].Data()), Form("(%s)*(1 + wnorm%d * %g)", BranchName[i + iBranchEnergies].Data(), i, (i>= iStartSensors) ? ResSilicon : ResPlastic));
        std::cout << "g" << BranchName[i + iBranchEnergies].Data() << " = " << Form("(%s)*(1 + wnorm%d * %g)", BranchName[i + iBranchEnergies].Data(), i, (i>= iStartSensors) ? ResSilicon : ResPlastic) << endl;
    }

    TString TotThin = "(";
    TString TotThick = "(";
    TString gTotThin = "(";
    TString gTotThick = "(";

    for(int i = 0; i < NumberPairsSensors; i++)
    {
        if(i == 0)
        {
            TotThin += Form("(%s)", BranchName[i + iStartSensors_Thin].Data());
            TotThick += Form("(%s)", BranchName[i + iStartSensors_Thick].Data());
            gTotThin += Form("(g%s)", BranchName[i + iStartSensors_Thin].Data());
            gTotThick += Form("(g%s)", BranchName[i + iStartSensors_Thick].Data());
        }
        else
        {
            TotThin += Form("+(%s)", BranchName[i + iStartSensors_Thin].Data());
            TotThick += Form("+(%s)", BranchName[i + iStartSensors_Thick].Data());
            gTotThin += Form("+(g%s)", BranchName[i + iStartSensors_Thin].Data());
            gTotThick += Form("+(g%s)", BranchName[i + iStartSensors_Thick].Data());
        }
    }

    TotThin += ")";
    TotThick += ")";
    gTotThin += ")";
    gTotThick += ")";

    Edep -> SetAlias("TotThin", TotThin.Data());
    Edep -> SetAlias("TotThick", TotThick.Data());
    Edep -> SetAlias("gTotThin", gTotThin.Data());
    Edep -> SetAlias("gTotThick", gTotThick.Data());


    std::cout << "TotThin = " << TotThin << endl;
    std::cout << "TotThick = " << TotThick << endl;
    std::cout << "gTotThin = " << gTotThin << endl;
    std::cout << "gTotThick = " << gTotThick << endl;

    TString TotalEnergy = Form("((%s) + (%s)) + (%s)", TotThin.Data(), TotThick.Data(), BranchName[iBranchEnergies_Calo].Data());
    TString gTotalEnergy = Form("((%s) + (%s)) + (g%s)", gTotThin.Data(), gTotThick.Data(), BranchName[iBranchEnergies_Calo].Data());

    Edep -> SetAlias("TotalEnergy", TotalEnergy.Data());
    Edep -> SetAlias("gTotalEnergy", gTotalEnergy.Data());


    Edep -> SetAlias("NormP", "TMath::Sqrt(pDirX*pDirX + pDirY*pDirY + pDirZ*pDirZ)");
    for(int i = 0; i < 3; ++i)
    {
        if(i == 2)
        {
            Edep -> SetAlias(DirName[i].Data(), Form("(-%s)/NormP", BranchName[i+iBranchDir].Data()));
        }
        else
        {
            Edep -> SetAlias(DirName[i].Data(), Form("%s/NormP", BranchName[i+iBranchDir].Data()));
        }
    }

    Edep -> SetAlias(PolarAngle[0].Data(), Form("TMath::ACos(%s)", DirName[2].Data()));
    Edep -> SetAlias(PolarAngle[1].Data(), Form("TMath::ATan2(%s, %s)", DirName[1].Data(), DirName[0].Data()));
    Edep -> SetAlias(NewPolarAngle[1].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Cos(%s)),(TMath::Cos(%s))))",              PolarAngle[0].Data(), PolarAngle[0].Data(), PolarAngle[1].Data() )); // *(180/3.415927))
    Edep -> SetAlias(NewPolarAngle[0].Data(), Form("(TMath::ATan2((TMath::Sin(%s)*TMath::Sin(%s)*TMath::Sin(%s)),TMath::Cos(%s)))", PolarAngle[0].Data(), PolarAngle[1].Data(), NewPolarAngle[1].Data(), PolarAngle[0].Data())); // *(180/3.415927))


    TString ConditionPairSilicon[NumberPairsSensors];
    TString ConditionPairSiliconAll;
    TString ConditionDrilledVeto;
    TString ConditionGoodEvents;
    TString ConditionGoodEventsSinglePair[NumberPairsSensors];

    for(int i = 0; i< NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors+i].Data() << " & " <<BranchName[iStartSensors+ NumberPairsSensors+i].Data() << endl;
        ConditionPairSilicon[i] = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        cout << "ConditionPairSilicon[" << i << "] = " << ConditionPairSilicon[i].Data() << endl;
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
        else 
        {
            ConditionPairSiliconAll += Form("|| ((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
    }

    ConditionDrilledVeto = Form("(%s < %g) && (%s < %g)", BranchName[iBranchEnergies].Data(), E_thr_Plastic, BranchName[iBranchEnergies + 1].Data(), E_thr_Plastic);

    ConditionGoodEvents = Form("(%s) && (%s) && (%s)", ConditionPairSiliconAll.Data(), ConditionDrilledVeto.Data(), ConditionDrilledVeto.Data());
    for(int i = 0; i < NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors_Thin+i].Data() << " & " <<BranchName[iStartSensors_Thick+i].Data() << endl;
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionDrilledVeto.Data());
    }


    TCanvas *cAngles;
    TH2D *hAngles;

    int ChipList[NumberPairsSensors];
    for(int i = 0; i < NumberPairsSensors; ++i)
    {
        ChipList[i] = i;
    }

    TGraph *gAngles[NumberPairsSensors];
    TMultiGraph *mgAngles;
    TMultiGraph *mgAnglesProtonsAlpha;
    TCanvas *CAnglesProtonsAlpha;
    TFile *tProjFile;
    TTree *tProj;
    TCanvas *cAngleGraphs;
    int ColorsPlot[8] = {kBlack,kBlue,kRed,kBlue,kRed,kBlue,kRed};

    double projXAngle, projYAngle;
    double pDirX, pDirY, pDirZ;
    double Norm;
    double nDirX, nDirY, nDirZ;
    double Theta, Phi;


    cAngleGraphs = new TCanvas(Form("Directions: %s", filename_noExt.Data()),Form("Directions: %s", filename_noExt.Data()),900,700);
    cAngleGraphs -> cd();

    tProjFile = new TFile(destination+"/"+"angles_"+filename_noExt+".root","RECREATE");
    tProj = new TTree("tProj","tProj");

    tProj -> Branch("ProjX", &projXAngle, "ProjX/D");
    tProj -> Branch("ProjY", &projYAngle, "ProjY/D");
    tProj -> Branch("pDirX", &pDirX, "pDirX/D");
    tProj -> Branch("pDirY", &pDirY, "pDirY/D");
    tProj -> Branch("pDirZ", &pDirZ, "pDirZ/D");
    tProj -> Branch("Theta", &Theta, "Theta/D");
    tProj -> Branch("Phi", &Phi, "Phi/D");


    Edep -> SetBranchAddress(BranchName[6].Data(), &pDirX);
    Edep -> SetBranchAddress(BranchName[7].Data(), &pDirY);
    Edep -> SetBranchAddress(BranchName[8].Data(), &pDirZ);

    mgAngles = new TMultiGraph();

    for(int k = 0; k < (Edep -> GetEntries()); ++k)
    {
        Edep -> GetEntry(k);
        Norm= TMath::Sqrt(pDirX*pDirX + pDirY*pDirY + pDirZ*pDirZ);
        nDirX = -pDirX/Norm;
        nDirY = -pDirY/Norm;
        nDirZ = -pDirZ/Norm;

        Theta = TMath::ACos(nDirZ);
        Phi   = TMath::ATan2(nDirY, nDirX);

        projXAngle = (Theta) * TMath::Cos(Phi) * (180/TMath::Pi());
        projYAngle = (Theta) * TMath::Sin(Phi) * (180/TMath::Pi());

        tProj -> Fill();
    }

    Edep -> AddFriend(tProj);
    tProjFile -> Write();
    

    cout << Form("Particle %s", filename_noExt.Data()) << endl;
    cout << "Copy No \t Std X \t Std Y \t # Events " << endl;
    for(int k = 0 ; k < NumberPairsSensors; ++k)
    {
        
        Edep -> Draw("ProjY:ProjX", ConditionGoodEventsSinglePair[k].Data(), "");
        gAngles[k] = new TGraph(Edep->GetSelectedRows(), Edep->GetV2(), Edep->GetV1());
        gAngles[k] -> SetMarkerColor(ColorsPlot[k]);
        gAngles[k] -> SetMarkerStyle(8);
        gAngles[k] -> SetMarkerSize(0.6);

        gAngles[k] -> Print();

        //gAngles[CopyNumber] -> Print();
        cout << Form("%d \t%g \t%g", k, gAngles[k]-> GetRMS(1), gAngles[k]-> GetRMS(2)) << " " << Edep->GetSelectedRows()<< endl;
        mgAngles -> Add(gAngles[k]);
    }

    gPad -> SetGrid();
    mgAngles -> GetXaxis() -> SetTitle("Angle projection X [deg]");
    mgAngles -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

    mgAngles -> GetXaxis() -> SetRangeUser(-180, 180);
    mgAngles -> GetYaxis() -> SetRangeUser(-180, 180);

    mgAngles -> Draw("AP");

    cAngleGraphs -> SaveAs( destination+"/"+"Angles_"+filename_noExt+".pdf");





    // //Fare un plot di tutti i branch
    // TCanvas *c1 = new TCanvas("c1", "c1", 5000, 4000);
    // // Divide the canva in 5 horizontal by 4 vertical
    // c1 -> Divide(5, 4);

    // for(int i = 0; i < NumberBranches; i++)
    // {
    //     c1 -> cd(i+1);
    //     // Check if in the BranchName there is 'Ed'
    //     if(BranchName[i].Contains("Ed"))
    //     {
    //         Edep -> Draw(Form("%s", BranchName[i].Data()), Form("%s > 0.", BranchName[i].Data()), "");
    //     }
    //     else
    //     {
    //         Edep -> Draw(Form("%s", BranchName[i].Data()), "", "");
    //     }
    //     gPad -> SetLogy();
    //     gPad -> SetGrid();
    // }
    //c1 -> SaveAs(destination+"/" + "Branches_" + filename_noExt+ ".pdf");


    // Montecarlo quantities plots

    TCanvas *c2 = new TCanvas("c2", "c2", 4000, 2000);

    // Divide the canva in 4 horizontal and 2 vertical
    c2 -> Divide(4, 2);

    for(int i = 0; i < 7; ++i)
    {
        c2 -> cd(i+1);
        Edep -> Draw(Form("%s", BranchName[i].Data()), "", "");
        gPad -> SetLogy();
        gPad -> SetGrid();
    }

    c2 -> SaveAs(destination+"/" + "Montecarlo_" + filename_noExt+ ".pdf");


    // Energy deposition plots

    TCanvas *c3 = new TCanvas("c3", "c3", 5000, 4000);
    c3 -> Divide(4,3);

    for(int i = 0; i < 12; ++i)
    {
        c3 -> cd(i+1);
        if(i == 0)
        {
            TString ToDraw = Form("%s + %s", BranchName[i + iBranchEnergies].Data(), BranchName[i + iBranchEnergies+1].Data());
            Edep -> Draw(Form("%s",ToDraw.Data()), Form("(%s)>0",ToDraw.Data()) , "");
        }
        else
        {
            Edep -> Draw(Form("%s", BranchName[i + 1 + iBranchEnergies].Data()), "", "");
        }
        gPad -> SetLogy();
        gPad -> SetGrid();
    }

    c3 -> SaveAs(destination+"/" + "Energies_" + filename_noExt+ ".pdf");
    

    file_alias -> cd();
    Edep -> Write();
    file_alias -> Write();
    file_alias -> Close();
    file -> Close();
    delete file;

    return;
}