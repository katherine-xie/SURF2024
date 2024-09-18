#include <iostream>
#include <vector>
#include <string>
// Using namespace std

// Function to calculate pT
Float_t calculatepT(Float_t x, Float_t y){
    return sqrt(x*x + y*y);
}

// Function to create a histogram based on input file
TH1F createHist(TString fileName, TString legendLabel, Int_t colorVal, Int_t markerVal) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<float>> pxBranch(reader, "px");
    TTreeReaderValue<vector<float>> pyBranch(reader, "py");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 100, 0, 10);
    hist.SetMarkerStyle(markerVal);
    hist.SetMarkerColor(colorVal);

    while (reader.Next()) {

        vector<float> pxVal = *pxBranch;
        vector<float> pyVal = *pyBranch;

        for (Int_t i = 0; i < pxVal.size(); i++) {
            Float_t pT = calculatepT(pxVal[i], pyVal[i]);
            hist.Fill(pT);
        }
    }    

    f->Close();
    delete f;
    return hist;
}


void pTMarkers() { 

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for all data files
    histarr[0] = createHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", 1, 21);
    histarr[1] = createHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", 2, 22);
    histarr[2] = createHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", 3, 23);
    histarr[3] = createHist("LEP1_1.root", "LEP1_1", 4, 24);
    histarr[4] = createHist("mupmum_3000GeV.root", "mupmum (3000 GeV)", 5, 25);
    histarr[5] = createHist("mupmum_10000GeV.root", "mupmum (10000 GeV)", 6, 26);

    // Creating a histogram stack object
    THStack hstack ("hstack", "Transverse Momentum Distribution; pT; Entries");
    TLegend l(0.7, 0.6, 0.9, 0.9); // TLegend object

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack.Add(&histarr[i]);
        l.AddEntry(&histarr[i], histarr[i].GetName(), "P");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 600);
    c1->cd();
    c1->SetGrid();
    hstack.Draw("Pnostack");
    l.Draw();
    gPad->SetLogx();
    
    // Saving canvas c1 to pdf file
    c1->Print("pTDist.pdf"); 

    // Deallocating memory
    delete c1;
}
