import os
import shutil
import sys
import time
import subprocess

import numpy as np
import matplotlib.pyplot as plt
import ROOT as rt
import uproot as up


def ProvaTree():
    fileTree = rt.TFile("Output_prova.root", "RECREATE")
    tree = rt.TTree("tree", "tree")

    x = np.zeros(1, dtype=float)
    y = np.zeros(1, dtype=float)
    z = np.zeros(1, dtype=float)

    tree.Branch("x", x, "x/D")
    tree.Branch("y", y, "y/D")
    tree.Branch("z", z, "z/D")

    # Genera x uniforme e y gaussiano
    for i in range(100000):
        x[0] = np.random.uniform(0, 10)
        z[0] = np.random.uniform(0, 10)
        y[0] = np.random.normal(0, 1)
        tree.Fill()

    tree.Write()

    c1 = rt.TCanvas("c1", "c1", 800, 600)
    h1 = rt.TH2D("h1", "h1", 100, 0, 10, 100, -3, 3)
    tree.Draw("y:x>>h1", "", "colz")
    c1.SaveAs("Output_prova.png")
    fileTree.Close()


def AggiungiAlias(fname):
    filetree = rt.TFile(fname, "UPDATE")
    tree = filetree.Get("tree")
    
    fnameupdated = fname.replace(".root", "_updated.root")
    filetreeupdated = rt.TFile(fnameupdated, "RECREATE")
    treeupdated = tree.CloneTree()
    treeupdated.SetAlias("r1", "(x+y)")
    treeupdated.SetAlias("r2", "(x-y)")
    treeupdated.SetAlias("r3", "(TMath::Sqrt(x*x+y*y))")

    treeupdated.Write()
    filetreeupdated.Close()

    #tree.Write()
    filetree.Close()

def Plot(fname):
    ftree = rt.TFile(fname, "READ")
    tree = ftree.Get("tree")

    c1 = rt.TCanvas("c1", "c1", 800, 600)
    tree.Draw("r1:r2", "", "colz")
    c1.SaveAs("Output_prova_r1_r2.png")

    fnameupdated = fname.replace(".root", "_updated.root")
    ftreeupdated = rt.TFile(fnameupdated, "READ")
    treeupdated = ftreeupdated.Get("tree")

    c2 = rt.TCanvas("c2", "c2", 800, 600)
    treeupdated.Draw("r1:r2", "", "colz")
    c2.SaveAs("Output_prova_r1_r2_updated.png")


if __name__ == "__main__":
    ProvaTree()
    AggiungiAlias("Output_prova.root")
    Plot("Output_prova.root")