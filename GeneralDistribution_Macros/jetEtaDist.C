#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Function to create a histogram based on input file
TH1F createJetEtaHist(TString fileName, TString legendLabel, Int_t lineColor) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Float_t>> jetEtaBranch(reader, "genJetEta");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 100, -10, 10);
    hist.SetLineColor(lineColor);
    hist.SetLineWidth(2);

    while (reader.Next()) {

        vector<Float_t> jetEtaVal = *jetEtaBranch;

        for (Int_t i = 0; i < jetEtaVal.size(); i++) {
            hist.Fill(jetEtaVal[i]);
        }
    }    

    f->Close();
    delete f;
    return hist;
}


void jetEtaDist() { 

    gStyle->SetPadLeftMargin(0.16);

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for all data files
    histarr[0] = createJetEtaHist("epem_ttbar_365GeV.root", "e_{+}e_{-}#rightarrowt#bar{t} (365 GeV)", kBlack);
    histarr[1] = createJetEtaHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", kRed);
    histarr[2] = createJetEtaHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", kAzure);
    histarr[3] = createJetEtaHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createJetEtaHist("mupmum_3000GeV.root", "mupmum (3000 GeV)", kMagenta);
    histarr[5] = createJetEtaHist("mupmum_10000GeV.root", "mupmum (10000 GeV)", kOrange+1);

    // Creating a histogram stack pointer
    THStack *hstack = new THStack ("hstack", "Jet #eta Distribution; Jet #eta; Entries");
    
    // Creating legend pointer
    TLegend *l = new TLegend(0.7, 0.6, 0.9, 0.9); 

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "L");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 600);
    c1->cd();
    c1->SetGrid();

    hstack->Draw("nostack");
    l->Draw();
    //gPad->SetLogx();
    
    // Saving canvas c1 to pdf file
    c1->Print("jetEtaDist.pdf"); 

    // Deallocating memory
    delete hstack;
    delete l;
    delete c1;
}
