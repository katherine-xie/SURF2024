#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Function to create a histogram based on input file
TH1F createJetMultHist(TString fileName, TString legendLabel, Int_t colorVal) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 200, 0, 200);
    hist.SetLineColor(colorVal);
    hist.SetLineWidth(8);

    // Finding the multiplicity of each jet and storing it in 2D vector
    std::vector<std::vector<Int_t>> multVect;
    while (reader.Next()) {
        std::vector<Int_t> jetVect; // Vector to store the multiplicities of each jet in an event
        for (Int_t jet = 0; jet < dauChgBranch->size(); jet++) {
            Int_t jetMultCount = 0;
            for (Int_t p = 0; p < (*dauChgBranch)[jet].size(); p++) {
                if ((*dauChgBranch)[jet][p] == 0) {continue;}
                jetMultCount++;
            }
            jetVect.push_back(jetMultCount);
        }
        multVect.push_back(jetVect);
    } 

    // Event loop
    reader.Restart(); // Restarting event loop
    Int_t countSelectedJets = 0;

    while (reader.Next()) {

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauChgBranch->size(); i++) {

            Int_t count = 0;

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauChgBranch)[i].size(); j++) {

                // Checking if particle is charged
                if ((*dauChgBranch)[i][j] < 0 || (*dauChgBranch)[i][j] > 0) {
                    count++;
                }
            }
            
            hist.Fill(count); // Filling histogram
        }
    }    

    f->Close();
    delete f;
    return hist;
}


void jetChgMultDist() { 

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for data files
    histarr[0] = createJetMultHist("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", kRed);
    histarr[1] = createJetMultHist("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", kAzure);
    histarr[2] = createJetMultHist("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", kBlack);
    histarr[3] = createJetMultHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createJetMultHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", kMagenta);
    histarr[5] = createJetMultHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", kOrange+1); 

    // Creating a histogram stack and a legend
    THStack *hstack = new THStack ("hstack", "");

    TLegend *l = new TLegend(0.6, 0.6, 0.9, 0.9); // Creating legend pointer
    l->SetBorderSize(0); // Remove the border
    l->SetFillStyle(0);
    l->SetTextFont(132);

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "l");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 1000, 1000);

  // ***** HISTOGRAM CUSTOMIZATION *****
    hstack->Draw("nostack hist");
    
    hstack->GetXaxis()->SetTitle("N_{ch}^{jet}");
    hstack->GetXaxis()->SetTitleFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetTitleOffset(0.8);
    hstack->GetXaxis()->SetTitleSize(0.07);
    hstack->GetXaxis()->SetLabelSize(0.04);

    hstack->GetYaxis()->SetTitle("Entries");
    hstack->GetYaxis()->SetTitleFont(132);
    hstack->GetYaxis()->SetLabelFont(132);
    hstack->GetYaxis()->SetTitleOffset(1);
    hstack->GetYaxis()->SetTitleSize(0.06);
    hstack->GetYaxis()->SetLabelSize(0.04);

    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.01);
    gPad->SetBottomMargin(0.13);
    gPad->SetTopMargin(0.01);
    gPad->SetLogy();

    gStyle->SetCanvasBorderSize(3);
    gStyle->SetPadBorderSize(0);

    /*
    l->SetBorderSize(0); // Remove the border
    l->SetFillStyle(0);
    l->SetTextFont(132);
    l->Draw();

 */

    // Saving canvas c1 to pdf file
    c1->Print("jetMultTest.root"); 

    // Deallocating memory
    delete c1;
    delete hstack;
    delete l;
    
}
