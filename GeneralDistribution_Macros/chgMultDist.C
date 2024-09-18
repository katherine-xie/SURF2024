#include <iostream>
#include <vector>
#include <string>
// Using namespace std

// Function to create a histogram based on input file
TH1F createChgHist(TString fileName, TString legendLabel, Int_t colorVal) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Int_t>> chgBranch(reader, "chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 100, 0, 200);
    hist.SetLineColor(colorVal);
    hist.SetLineWidth(2);

    while (reader.Next()) {

        vector<Int_t> chgVal = *chgBranch;

        Int_t count = 0;

        for (Int_t i = 0; i < chgVal.size(); i++) {
            if (chgVal[i] < 0 || chgVal[i] > 0) {
                count++;
            }
        }

        hist.Fill(count);
        count = 0;
    }    

    f->Close();
    delete f;
    return hist;
}


void chgMultDist() { 

    gStyle->SetPadLeftMargin(0.16);

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for all data files
    histarr[0] = createChgHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", kBlack);
    histarr[1] = createChgHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", kRed);
    histarr[2] = createChgHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", kAzure);
    histarr[3] = createChgHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createChgHist("mupmum_3000GeV.root", "mupmum (3000 GeV)", kMagenta);
    histarr[5] = createChgHist("mupmum_10000GeV.root", "mupmum (10000 GeV)", kOrange+1);

    // Creating a histogram stack pointer
    THStack *hstack = new THStack ("hstack", "Charged Particle Multiplicity; (Even) Number of Particles; Entries");
    
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

    //hstack->GetYaxis()->SetTitleOffset(0.8);
    //hstack->GetYaxis()->SetLabelOffset(0.01);


    // Saving canvas c1 to pdf file
    c1->Print("test.pdf"); 

    // Deallocating memory
    delete c1;
    delete hstack;
    delete l;

    
}
