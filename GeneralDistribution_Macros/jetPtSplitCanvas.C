#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);


// Function to create a histogram based on input file
TH1F createJetPtSplitHist(TString fileName, TString legendLabel, Int_t xMax, Int_t numBins, Int_t lineColor) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Float_t>> jetPtBranch(reader, "genJetPt");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, numBins, 0, xMax);
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


void jetPtSplitCanvas() { 

    //gStyle->SetPadLeftMargin(0.16);

    // Creating a histogram array
    TH1F harr1[4];
    TH1F harr2[2];

    // Calling createHist function for all data files
    harr1[0] = createJetPtSplitHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", 250, 500, kRed);
    harr1[1] = createJetPtSplitHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", 250, 500, kAzure);
    harr1[2] = createJetPtSplitHist("LEP1_1.root", "LEP1_1", 250, 500, kGreen+1);
    harr1[3] = createJetPtSplitHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", 250, 500, kBlack);

    harr2[0] = createJetPtSplitHist("mupmum_3000GeV.root", "mupmum (3000 GeV)", 6000, 600, kMagenta);
    harr2[1] = createJetPtSplitHist("mupmum_10000GeV.root", "mupmum (10000 GeV)", 6000, 600, kOrange+1);

    // Creating a histogram stack object
    THStack *hstack1 = new THStack("hstack1", "Jet p_{T} Distribution;p_{T} [GeV]; Entries");
    THStack *hstack2 = new THStack("hstack2", ";p_{T} [GeV]; Entries");

    TLegend *l1 = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object
    TLegend *l2 = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 4; i++) {
        hstack1->Add(&harr1[i]);
        l1->AddEntry(&harr1[i], harr1[i].GetName(), "L");
    }
   
    for (Int_t i = 0; i < 2; i++) {
        hstack2->Add(&harr2[i]);
        l2->AddEntry(&harr2[i], harr2[i].GetName(), "L");
    }
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 800);

    c1->Divide(1, 2);
    
    /*
    // Access the individual pads
    TPad *pad1 = (TPad*)c1->GetPad(1);
    TPad *pad2 = (TPad*)c1->GetPad(2);
    pad1->SetBottomMargin(0.05);
    pad2->SetTopMargin(0.05); */

    c1->cd(1);
    hstack1->Draw("nostack");
    l1->Draw();
    gPad->SetGrid();
    gPad->SetLogy();
    //gPad->SetLogx();
    hstack1->GetXaxis()->SetTitleOffset(1.3);

    c1->cd(2);
    hstack2->Draw("nostack");
    l2->Draw();
    gPad->SetGrid();
    gPad->SetLogy();
    hstack2->GetXaxis()->SetTitleOffset(1.3);

    //hstack->GetYaxis()->SetTitleOffset(0.8);
    //hstack->GetYaxis()->SetLabelOffset(0.01);


    // Saving canvas c1 to pdf file
    c1->Print("jetPtSplitCanvas2.pdf"); 

    // Deallocating memory
    delete c1;
    delete hstack1;
    delete l1;
    delete hstack2;
    delete l2;
    
}
