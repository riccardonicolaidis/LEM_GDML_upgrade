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
#include "TF2.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TMath.h"
#include "./TH1DLog.h"
#include "./TH2DLog.h"
#include "./TH2DLogX.h"
#include "TList.h"
#include "TKey.h"
#include "TObject.h"
#include "TObjArray.h"


using namespace std;

// root SetAliases.C\(\"ciao\"\)

void SetAliases(TString filename,
                TString filename_noExt,
                TString destination,
                double E_thr_Thin = 0.04,
                double E_thr_Thick = 0.04,
                double E_thr_Plastic = 0.1,
                int NumberPairsSensors = 5,
                double ResSilicon = 0.01,
                double ResPlastic = 0.2)
{

    ofstream Log;
    Log.open(destination+"/"+"Log_"+filename_noExt+".txt");

    std::cout << "Setting aliases for " << filename << endl;
    Log << "Setting aliases for " << filename << endl;

    TString NameFile = filename_noExt;
    TString DirName[3] = {"DirX", "DirY", "DirZ"};
    TString PolarAngle[2] = {"dirTheta", "dirPhi"};
    TString NewPolarAngle[2] = {"dirThetaNew", "dirPhiNew"};

    /* Number of branches */
    int NumberBranches = 7 + 3 + 2*NumberPairsSensors;
    std::cout << "NumberBranches = " << NumberBranches << endl;
    Log << "NumberBranches = " << NumberBranches << endl;
    /* Definition of Branches Names*/
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
    BranchName[iBranch++] = "Ed_LV_PlasticVetoBottom";
    BranchName[iBranch++] = "Ed_LV_PlasticVetoTop";
    int iBranchEnergies_Calo = iBranch;
    BranchName[iBranch++] = "Ed_LV_Calo";
    int iStartSensors = iBranch;
    int iStartSensors_Thin = iStartSensors;
    int iStartSensors_Thick = iStartSensors + NumberPairsSensors;

    for(int iPair = 0; iPair < NumberPairsSensors; iPair++)
    {
        BranchName[iBranch] = "Ed_LV_SiliconDetector_Thin_" + to_string(iPair);
        BranchName[iBranch+NumberPairsSensors] = "Ed_LV_SiliconDetector_Thick_" + to_string(iPair);
        iBranch++;
    }

    /* Print Branch names */
    for(int i = 0; i < NumberBranches; i++)
    {
        std::cout << "BranchName[" << i << "] = " << BranchName[i] << endl;
        Log << "BranchName[" << i << "] = " << BranchName[i] << endl;
    }

    /* Open the file to be read */
    TFile *file = new TFile(filename, "READ");
    Log << "File " << filename << " opened" << endl;

    // Print the file structure in the log file
    Log << "File structure:" << endl;
    TList *list = file -> GetListOfKeys();
    for(int i = 0; i < list -> GetSize(); ++i)
    {
        Log << "Key " << i << " = " << list -> At(i) -> GetName() << endl;
    }

    

    /* Open the file with new trees where we are going to define aliases */
    TFile *file_alias = new TFile(destination +"/" + filename_noExt + "_alias.root", "RECREATE");
    /* Old tree without aliases */
    TTree *Edep_old = (TTree*) (file -> Get("Edep"));

    // Print the file structure in the log file
    Log << "TTree structure:" << endl;
    TObjArray *list2 = Edep_old -> GetListOfBranches();
    for(int i = 0; i < list2 -> GetSize(); ++i)
    {
        Log << "Branch " << i << " = " << list2 -> At(i) -> GetName() << endl;
    }

    Log.close();

    file_alias -> cd();
    /* New tree with aliases */
    TTree *Edep = Edep_old -> CloneTree();

    /* Number of sensors for energy measurement */
    int NumberEnergySensors = 2*NumberPairsSensors + 2 + 1;


    /* Total energy deposited in the sensors */
    TString TotThin = "(";
    TString TotThick = "(";
    TString gTotThin = "(";
    TString gTotThick = "(";

    for(int i = 0; i < NumberPairsSensors; i++)
    {
        if(i == 0)
        {
            TotThin += Form("(%s)*(%s > %g)", BranchName[i + iStartSensors_Thin].Data(), BranchName[i + iStartSensors_Thin].Data(), E_thr_Thin);
            TotThick += Form("(%s)*(%s > %g)", BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
            gTotThin += Form("(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thin].Data(), BranchName[i + iStartSensors_Thin].Data(), E_thr_Thin);
            gTotThick += Form("(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
        }
        else
        {
            TotThin += Form("+(%s)*(%s > %g)", BranchName[i + iStartSensors_Thin].Data(), BranchName[i + iStartSensors_Thin].Data(), E_thr_Thin);
            TotThick += Form("+(%s)*(%s > %g)", BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
            gTotThin += Form("+(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thin].Data(), BranchName[i + iStartSensors_Thin].Data(), E_thr_Thin);
            gTotThick += Form("+(g%s)*(g%s > %g)", BranchName[i + iStartSensors_Thick].Data(), BranchName[i + iStartSensors_Thick].Data(), E_thr_Thick);
        }
    }

    TotThin += ")";
    TotThick += ")";
    gTotThin += ")";
    gTotThick += ")";


    TString TotalEnergy = Form("((%s) + (%s) + (%s)*(%s > %g))", TotThin.Data(), TotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    TString gTotalEnergy = Form("((%s) + (%s) + (g%s)*(%s > %g))", gTotThin.Data(), gTotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);

    TString TotThickCalo = Form("((%s) + (%s)*(%s > %g))", TotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    TString gTotThickCalo = Form("((%s) + (g%s)*(g%s > %g))", gTotThick.Data(), BranchName[iBranchEnergies_Calo].Data(), BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);

    TString PID = "TMath::Log10((TotalEnergy)*(TotThin))";
    TString gPID = "TMath::Log10((gTotalEnergy)*(gTotThin))";
    TString PID_noCalo = Form("TMath::Log10((%s)*(TotThin))", BranchName[iBranchEnergies_Calo].Data());
    TString gPID_noCalo = Form("TMath::Log10((g%s)*(gTotThin))", BranchName[iBranchEnergies_Calo].Data());


    /* Gaussian random variables */
    for(int i = 0; i < NumberEnergySensors; i++)
    {
        Edep -> SetAlias(Form("wnorm%d", i), "(sin(2 *pi*rndm)*sqrt(-2*log(rndm)))");
    }

    /* Gaussian smeared energy depositions */
    /* For smeared quantities add a "g" in front of the logic volume name */
    for(int i = 0; i < NumberEnergySensors; ++i)
    {
        Edep -> SetAlias(Form("g%s", BranchName[i + iBranchEnergies].Data()), Form("(%s)*(1 + wnorm%d * %g)", BranchName[i + iBranchEnergies].Data(), i, ((i + iBranchEnergies)>= iStartSensors) ? ResSilicon : ResPlastic));
        std::cout << "g" << BranchName[i + iBranchEnergies].Data() << " = " << Form("(%s)*(1 + wnorm%d * %g)", BranchName[i + iBranchEnergies].Data(), i, ((i + iBranchEnergies)>= iStartSensors) ? ResSilicon : ResPlastic) << endl;
    }

    Edep -> SetAlias("TotThin", TotThin.Data());
    Edep -> SetAlias("TotThick", TotThick.Data());
    Edep -> SetAlias("gTotThin", gTotThin.Data());
    Edep -> SetAlias("gTotThick", gTotThick.Data());


    std::cout << "TotThin = " << TotThin << endl;
    std::cout << "TotThick = " << TotThick << endl;
    std::cout << "gTotThin = " << gTotThin << endl;
    std::cout << "gTotThick = " << gTotThick << endl;


    Edep -> SetAlias("TotThickCalo", TotThickCalo.Data());
    Edep -> SetAlias("gTotThickCalo", gTotThickCalo.Data());
    Edep -> SetAlias("TotalEnergy", TotalEnergy.Data());
    Edep -> SetAlias("gTotalEnergy", gTotalEnergy.Data());


    // Defining the Non Confinement Violation Factor
    Edep -> SetAlias("NCVF", "(RandEnergy - TotalEnergy)/RandEnergy");    


    Edep -> SetAlias("PID", PID.Data());
    Edep -> SetAlias("gPID", gPID.Data());
    Edep -> SetAlias("PID_noCalo", PID_noCalo.Data());
    Edep -> SetAlias("gPID_noCalo", gPID_noCalo.Data());


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
    TString ConditionNoCalo;
    TString NumberPairsHit = "(";

    for(int i = 0; i< NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors+i].Data() << " & " <<BranchName[iStartSensors+ NumberPairsSensors+i].Data() << endl;
        ConditionPairSilicon[i] = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        cout << "ConditionPairSilicon[" << i << "] = " << ConditionPairSilicon[i].Data() << endl;
        if(i == 0)
        {
            ConditionPairSiliconAll = Form("((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
            NumberPairsHit += Form("1*((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
        else 
        {
            ConditionPairSiliconAll += Form("|| ((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
            NumberPairsHit += Form(" + 1*((%s > %g) && (%s > %g))", BranchName[iStartSensors+i].Data(), E_thr_Thin, BranchName[iStartSensors + NumberPairsSensors + i].Data(), E_thr_Thick);
        }
    }
    NumberPairsHit += ")";

    ConditionDrilledVeto = Form("(%s < %g) && (%s < %g)", BranchName[iBranchEnergies].Data(), E_thr_Plastic, BranchName[iBranchEnergies + 1].Data(), E_thr_Plastic);
    cout << "ConditionDrilledVeto = " << ConditionDrilledVeto.Data() << endl;
    ConditionGoodEvents = Form("(%s < 2.0) && (%s) && (%s)", NumberPairsHit.Data(), ConditionPairSiliconAll.Data(), ConditionDrilledVeto.Data());
    cout << "ConditionGoodEvents = " << ConditionGoodEvents.Data() << endl;
    for(int i = 0; i < NumberPairsSensors; ++i)
    {
        cout << "Defining good events for pair" << BranchName[iStartSensors_Thin+i].Data() << " & " <<BranchName[iStartSensors_Thick+i].Data() << endl;
        ConditionGoodEventsSinglePair[i] = Form("(%s) && (%s)", ConditionPairSilicon[i].Data(), ConditionDrilledVeto.Data());
        cout << "ConditionGoodEventsSinglePair[" << i << "] = " << ConditionGoodEventsSinglePair[i].Data() << endl;
    }

    ConditionNoCalo = Form("(%s < %g)", BranchName[iBranchEnergies_Calo].Data(), E_thr_Plastic);
    cout << "ConditionNoCalo = " << ConditionNoCalo.Data() << endl;

    /* Files with the computed angles from MonteCarlo Momentum direction */
    double projXAngle, projYAngle;
    double pDirX, pDirY, pDirZ;
    double Norm;
    double nDirX, nDirY, nDirZ;
    double Theta, Phi;

    
    TFile *tProjFile;
    TTree *tProj;
    tProjFile = new TFile(destination+"/"+"angles_"+filename_noExt+".root","RECREATE");
    tProj = new TTree("tProj","tProj");

    tProj -> Branch("ProjX", &projXAngle, "ProjX/D");
    tProj -> Branch("ProjY", &projYAngle, "ProjY/D");
    tProj -> Branch("pDirX", &pDirX, "pDirX/D");
    tProj -> Branch("pDirY", &pDirY, "pDirY/D");
    tProj -> Branch("pDirZ", &pDirZ, "pDirZ/D");
    tProj -> Branch("Theta", &Theta, "Theta/D");
    tProj -> Branch("Phi", &Phi, "Phi/D");


    Edep -> SetBranchAddress(BranchName[iBranchDir].Data(), &pDirX);
    Edep -> SetBranchAddress(BranchName[iBranchDir+1].Data(), &pDirY);
    Edep -> SetBranchAddress(BranchName[iBranchDir+2].Data(), &pDirZ);

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






    /* Figures for MonteCarlo diagnostics */

    TCanvas *cAngles;
    TH2D *hAngles;
    TGraph *gAngles[NumberPairsSensors];
    TMultiGraph *mgAngles;
    TMultiGraph *mgAnglesProtonsAlpha;
    TCanvas *CAnglesProtonsAlpha;
    TCanvas *cAngleGraphs;
    int ColorsPlot[8] = {kBlack,kBlue,kRed,kBlue,kRed,kBlue,kRed};



    cAngleGraphs = new TCanvas(Form("Directions: %s", filename_noExt.Data()),Form("Directions: %s", filename_noExt.Data()),900,700);
    cAngleGraphs -> cd();

    mgAngles = new TMultiGraph();

    cout << Form("Particle %s", filename_noExt.Data()) << endl;
    cout << "Copy No \t Std X \t Std Y \t # Events " << endl;
    for(int k = 0 ; k < NumberPairsSensors; ++k)
    {
        TH2D *hAngles;
        TH2D *hGenPosition = new TH2D(Form("hGenPosition_%d", k), Form("hGenPosition_%d", k), 100, -35, 35, 100, -35, 35);
        if(NameFile.Contains("e-"))
        {
            hAngles = new TH2D(Form("h2DAngles_%d", k), Form("hAngles_%d", k), 100, -45, 45, 100, -45, 45);
        }
        else
        {
            hAngles = new TH2D(Form("h2DAngles_%d", k), Form("hAngles_%d", k), 100, -45, 45, 100, -45, 45);
        }

        Edep -> Draw(Form("ProjY:ProjX>>h2DAngles_%d", k), ConditionGoodEventsSinglePair[k].Data(), "colz");
        TCanvas *cAngles = new TCanvas(Form("cAngles_%d", k), Form("cAngles_%d", k), 900, 700);
        hAngles -> Draw("colz");
        hAngles -> SaveAs(destination + "/" + "2DAngHisto_" + Form("%d_", k)  + filename_noExt + ".root");
        hAngles -> GetXaxis() -> SetTitle("Angle projection X [deg]");
        hAngles -> GetYaxis() -> SetTitle("Angle projection Y [deg]");
        cAngles -> SaveAs(destination+"/"+"2DAngHistoFigure_" + Form("%d_", k) +filename_noExt + "_" + filename_noExt + ".pdf");
        cAngles -> Close();

        Edep -> Draw(Form("Ygen:Xgen>>hGenPosition_%d", k), ConditionGoodEventsSinglePair[k].Data(), "colz");
        TCanvas *cGenPosition = new TCanvas(Form("cGenPosition_%d", k), Form("cGenPosition_%d", k), 900, 700);
        hGenPosition -> Draw("colz");
        hGenPosition -> GetXaxis() -> SetTitle("X [mm]");
        hGenPosition -> GetYaxis() -> SetTitle("Y [mm]");
        cGenPosition -> SaveAs(destination+"/"+"GenPosition_" + Form("%d_", k) +filename_noExt + "_" + filename_noExt + ".pdf");
        cGenPosition -> Close();
        
        Edep -> Draw("ProjY:ProjX", ConditionGoodEventsSinglePair[k].Data(), "");

        gAngles[k] = new TGraph(Edep->GetSelectedRows(), Edep->GetV2(), Edep->GetV1());
        gAngles[k] -> SetMarkerColor(ColorsPlot[k]);
        gAngles[k] -> SetMarkerStyle(8);
        gAngles[k] -> SetMarkerSize(0.6);

        //gAngles[k] -> Print();

        //gAngles[CopyNumber] -> Print();
        cout << Form("%d \t%g \t%g", k, gAngles[k]-> GetRMS(1), gAngles[k]-> GetRMS(2)) << " " << Edep->GetSelectedRows()<< endl;
        mgAngles -> Add(gAngles[k]);
    }

    gPad -> SetGrid();
    mgAngles -> GetXaxis() -> SetTitle("Angle projection X [deg]");
    mgAngles -> GetYaxis() -> SetTitle("Angle projection Y [deg]");

    
    if(NameFile.Contains("e-"))
    {
        mgAngles -> GetXaxis() -> SetRangeUser(-70, 70);
        mgAngles -> GetYaxis() -> SetRangeUser(-70, 70);    
    }
    else
    {
    mgAngles -> GetXaxis() -> SetRangeUser(-40, 40);
    mgAngles -> GetYaxis() -> SetRangeUser(-40, 40);
    }


    mgAngles -> Draw("AP");

    cAngleGraphs -> SaveAs( destination+"/"+"Angles_"+filename_noExt+".pdf");




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