#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Function to create a histogram based on input file
TH1F createJetPtHist(TString fileName, TString legendLabel, Int_t lineColor) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Float_t>> jetPtBranch(reader, "genJetPt");
    //TTreeReaderValue<vector<Int_t>> chgBranch(reader, "chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 600, 0, 6000);
    hist.SetLineColor(lineColor);
    hist.SetLineWidth(2);

    while (reader.Next()) {
        vector<Float_t> jetPtVal = *jetPtBranch;
        for (Int_t i = 0; i < jetPtVal.size(); i++) {
                hist.Fill(jetPtVal[i]);
        }
    }    

    f->Close();
    delete f;
    return hist;
}


void jetPtDist() { 

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for all data files
    histarr[0] = createJetPtHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", kBlack);
    histarr[1] = createJetPtHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", kRed);
    histarr[2] = createJetPtHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", kAzure);
    histarr[3] = createJetPtHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createJetPtHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", kMagenta);
    histarr[5] = createJetPtHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", kOrange+1);

    // Creating a histogram stack pointer
    THStack *hstack = new THStack ("hstack", "Jet p_{T} Distribution; Jet p_{T} [GeV]; Entries");
    //hstack->SetMaximum(140000.0);

    TLegend *l = new TLegend(0.7, 0.6, 0.9, 0.9); // Creating legend pointer

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "L");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 600);

    hstack->Draw("nostack");
    l->Draw();
    gPad->SetLogy();
    c1->SetGrid();
    //gPad->SetLogx();
    //hstack->GetXaxis()->SetTitleOffset(1.3);

    // Saving canvas c1 to pdf file
    c1->Print("jetPtDist3.pdf"); 
    
    // Deallocating memory
    delete hstack;
    delete l;
    delete c1;
}
