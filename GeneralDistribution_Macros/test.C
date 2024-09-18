#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Function to create a histogram based on input file
TH1F createpTHist(TString fileName, TString legendLabel, Int_t colorVal) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Float_t>> pxBranch(reader, "px");
    TTreeReaderValue<vector<Float_t>> pyBranch(reader, "py");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 5000, 150, 200);
    hist.SetLineColor(colorVal);
    hist.SetLineWidth(2);

    // Event loop
    while (reader.Next()) {

        vector<Float_t> pxVal = *pxBranch;
        vector<Float_t> pyVal = *pyBranch;

        // Loop through particles produced in a collision
        for (Int_t i = 0; i < pxVal.size(); i++) {
            hist.Fill(calculatepT(pxVal[i], pyVal[i]));
        }
    }    

    hist.SetStats(0);
    f->Close();
    delete f;
    return hist;
}


void test() { 

    // Creating a histogram array
    //TH1F histarr[6];

    /*// Calling createHist function for all data files
    histarr[0] = createpTHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", kBlack);
    histarr[1] = createpTHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", kRed);
    histarr[2] = createpTHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", kAzure);
    histarr[3] = createpTHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createpTHist("mupmum_3000GeV.root", "mupmum (3000 GeV)", kMagenta);
    histarr[5] = createpTHist("mupmum_10000GeV.root", "mupmum (10000 GeV)", kOrange+1);*/

    // Creating a histogram stack object
    //THStack *hstack = new THStack("hstack", "p_{T} Distribution (all particles); p_{T} [GeV]; Entries");
    TH1F testHist = createpTHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", kBlack);
    TLegend *l = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object
    l->AddEntry(&testHist, testHist.GetName(), "L");

    /*// Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "L");
    }*/
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 600);
    c1->cd();
    c1->SetGrid();

    //hstack->Draw("nostack");
    //hstack->GetXaxis()->SetRangeUser(0, 10);
    testHist.Draw();
    l->Draw();
    c1->SetLogy();
    //hstack->GetXaxis()->SetTitleOffset(1.3);
    
    // Saving canvas c1 to pdf file
    c1->Print("test4.pdf"); 

    // Deallocating memory
    delete c1;
    //delete hstack;
    delete l;
}
