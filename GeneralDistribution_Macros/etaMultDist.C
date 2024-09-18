#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Function to create a histogram based on input file
TH1F createEtaMultHist(TString fileName, TString legendLabel, Int_t NCutOff, Int_t lineColor) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Float_t>> pxBranch(reader, "px");
    TTreeReaderValue<vector<Float_t>> pyBranch(reader, "py");
    TTreeReaderValue<vector<Float_t>> pzBranch(reader, "pz");
    TTreeReaderValue<vector<Int_t>> chgBranch(reader, "chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 100, -10, 10);
    hist.SetLineColor(lineColor);
    hist.SetLineWidth(2);

    while (reader.Next()) {

        vector<Float_t> pxVal = *pxBranch;
        vector<Float_t> pyVal = *pyBranch;
        vector<Float_t> pzVal = *pzBranch;
        vector<Int_t> chgVal = *chgBranch;

        Int_t count = 0; // Charged multiplicity counter

        // Looping through particles in the event to check N_ch
        for (Int_t i = 0; i < chgVal.size(); i++) {
            if (chgVal[i] < 0 || chgVal[i] > 0) {count++;} 
        }

        // Only fill histogram if N > NCutOff and if particle is charged
        if (count > NCutOff) {
            for (Int_t i = 0; i < chgVal.size(); i++) {
                if (chgVal[i] < 0 || chgVal[i] > 0) {
                    Float_t theta = calculateTheta(pxVal[i], pyVal[i], pzVal[i]);
                    hist.Fill(calculateEta(theta));
                }
            }
       }
    }

    f->Close();
    delete f;
    return hist;
}


void etaMultDist() { 

    // Creating a histogram array
    TH1F histarr[6][4];

    //Calling createHist function for all data files
    for (Int_t i = 0; i < 4; i++) {
        histarr[0][i] = createEtaMultHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", 45+(5*i), kBlack);
        histarr[1][i] = createEtaMultHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", 45+(5*i), kRed);
        histarr[2][i] = createEtaMultHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", 45+(5*i), kAzure);
        histarr[3][i] = createEtaMultHist("LEP1_1.root", "LEP1_1", 45+(5*i), kGreen+1);
        histarr[4][i] = createEtaMultHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", 45+(5*i), kMagenta);
        histarr[5][i] = createEtaMultHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", 45+(5*i), kOrange+1);
    }


    // Creating a histogram stack pointer and legend
    THStack *hstack[4];
    TLegend *l[4];

    hstack[0] = new THStack ("hstack1", "#eta Distribution (For N_{ch} > 45); #eta; Entries");
    hstack[1] = new THStack ("hstack2", "#eta Distribution (For N_{ch} > 50); #eta; Entries");
    hstack[2] = new THStack ("hstack3", "#eta Distribution (For N_{ch} > 55); #eta; Entries");
    hstack[3] = new THStack ("hstack4", "#eta Distribution (For N_{ch} > 60); #eta; Entries");
    for (Int_t i = 0; i < 4; i++) {l[i] = new TLegend(0.6, 0.6, 0.9, 0.9);}

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 4; i++) {
        for (Int_t j = 0; j < 6; j++) {
            hstack[i]->Add(&histarr[j][i]);
            l[i]->AddEntry(&histarr[j][i], histarr[j][i].GetName(), "L");
        }
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 800);
    c1->Divide(2,2);

    for (Int_t i = 1; i < 5; i++) {
        c1->cd(i);
        hstack[i-1]->Draw("nostack");
        l[i-1]->Draw();
        gPad->SetGrid();
        gPad->SetLogy();
    } 
    
    // Saving canvas c1 to pdf file
    c1->Print("etaMultDist2.pdf"); 

    // Deallocating memory
    delete c1; 
}
