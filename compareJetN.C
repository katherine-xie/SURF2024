#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TAttLine.h"
#include "THelix.h"
#include "TView.h"
#include "TRandom.h"
#include "TAttPad.h"
#include "TMath.h"
#include "TVector3.h"
#include "TView3D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"

#include <iostream>
#include <string>
#include <vector>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
#include "SURF_FUNCTIONS/coordinateTools.h"

R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Global variables
std::string title = "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)";
std::string fullPathDir = "/Users/katherinexie/SURF2024/futureCollidersData/epem_ttbar_365GeV.root";
TFile *f = new TFile(fullPathDir.c_str(), "read"); // Opening file

// Creating TTreeReader object and linking branches
TTreeReader* reader = new TTreeReader("trackTree");

// Setup branches for other particles
TTreeReaderValue<std::vector<std::vector<Float_t>>> pPt(*reader, "genDau_pt");
TTreeReaderValue<std::vector<std::vector<Float_t>>> pEta(*reader, "genDau_eta");
TTreeReaderValue<std::vector<std::vector<Float_t>>> pPhi(*reader, "genDau_phi");
TTreeReaderValue<std::vector<std::vector<Int_t>>> pChg(*reader, "genDau_chg");
TTreeReaderValue<std::vector<std::vector<Int_t>>> pPID(*reader, "genDau_pid");

std::vector<Int_t> findTopTwoJetN(Int_t eventIndex) {

    reader->SetEntry(eventIndex);

    std::vector<Int_t> jetNVec;
    std::vector<Int_t> topTwoN;

    for (Int_t i = 0; i < pChg->size(); i++) {
        Int_t countN = 0;
        for (Int_t j = 0; j < (*pChg)[i].size(); j++) {
            if ((*pChg)[i][j] == 0) {continue;}
            countN++;
        }
        jetNVec.push_back(countN);
    }
    
    std::sort(jetNVec.begin(), jetNVec.end(), greater<Int_t>()); // Sorting vector

    if (jetNVec.size() < 2) {
        topTwoN.push_back(jetNVec[0]);
        topTwoN.push_back(0);
    }
    else {
        topTwoN.push_back(jetNVec[0]);
        topTwoN.push_back(jetNVec[1]);
    }
    return topTwoN;
}

void compareJetN() {
    std::string histTitle = "N_{ch} of Leading and Sub-Leading Jets for " + title;
    TCanvas *c = new TCanvas("c", "Canvas for Leading and Sub-Leading Jets N", 800, 600);
    TH2I *hist = new TH2I("hist", histTitle.c_str(), 50, 0, 50, 50, 0, 50);

    // Event Loop
    reader->Restart();
    while (reader->Next()) {
        Int_t currEventIndex = reader->GetCurrentEntry();
        std::vector<Int_t> topTwoVals = findTopTwoJetN(currEventIndex);
        //std::cout << topTwoVals[0] << " " << topTwoVals[1] << std::endl;
        hist->Fill(topTwoVals[0], topTwoVals[1]);
    } 

    // ***** HISOTGRAM CUSTOMIZATION *****
    hist->SetXTitle("Leading Jet N_{ch}");
    hist->SetYTitle("Sub-Leading Jet N_{ch}");
    hist->SetZTitle("Entries");

    hist->SetTitleFont(132, "T");
    hist->SetTitleFont(132, "XYZ");

    hist->SetTitleOffset(1.3, "X");
    hist->SetTitleOffset(1.3, "Y");
    hist->SetTitleOffset(-0.5, "Z");

    hist->SetLabelFont(132, "T");
    hist->SetLabelFont(132, "XYZ");

    hist->SetStats(0);
    hist->Draw("COLZ");   

    gPad->SetLogz();

    c->Print("compareJetN.pdf");
    delete hist;
    delete c;
}