#include <iostream>
#include <vector>
#include <string>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Function to create a histogram based on input file
TH1F createEtaHist(TString fileName, TString legendLabel, Int_t multCutOff, Int_t lineColor) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<std::vector<std::vector<Float_t>>> dauPtBranch(reader, "genDau_pt");
    TTreeReaderValue<std::vector<std::vector<Float_t>>> dauPhiBranch(reader, "genDau_phi");
    TTreeReaderValue<std::vector<std::vector<Float_t>>> dauEtaBranch(reader, "genDau_eta");
    TTreeReaderValue<std::vector<std::vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Finding the multiplicity of each jet and storing it in 2D vector
    std::vector<std::vector<Int_t>> multVect;
    while (reader.Next()) {
        std::vector<Int_t> jetVect; // Vector to store the multiplicities of each jet in an event
        for (Int_t jet = 0; jet < dauPtBranch->size(); jet++) {
            Int_t jetMultCount = 0;
            for (Int_t p = 0; p < (*dauPtBranch)[jet].size(); p++) {
                if ((*dauChgBranch)[jet][p] == 0) {continue;}
                jetMultCount++;
            }
            jetVect.push_back(jetMultCount);
        }
        multVect.push_back(jetVect);
    } 

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 100, -4, 4);

    Int_t countSelectedJets = 0;

    reader.Restart();
    // Event Loop
    while (reader.Next()) {
        
        Int_t eventIndex = reader.GetCurrentEntry();

        // Jet loop        
        for (Int_t i = 0; i < dauChgBranch->size(); i++) {

            if (multVect[eventIndex][i] < multCutOff) {continue;}
            countSelectedJets++;

            // Particle loop
            for (Int_t j = 0; j < (*dauChgBranch)[i].size(); j++) {
                if ((*dauChgBranch)[i][j] == 0) {continue;}
                Float_t px = calculatePx((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                Float_t py = calculatePy((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                Float_t pz = calculatePz((*dauPtBranch)[i][j], (*dauEtaBranch)[i][j]);

                Float_t theta = calculateTheta(px, py, pz);
                hist.Fill(calculateEta(theta));
            }
        }
    }

    // ***** NORMALIZATION *****
    // We want to make the signal hist independent of the number of jets and the bin width
    Float_t binWidth = (8)/100.0;
    hist.Scale(1.0/(binWidth*countSelectedJets));

    // ***** HISTOGRAM CUSTOMIZATION *****
    hist.SetLineColor(lineColor);
    hist.SetLineWidth(20);

    //hist.SetMarkerStyle(21);
    //hist.SetMarkerSize(0.7);
    //hist.SetMarkerColor(lineColor); 

    hist.SetTitleFont(132, "T");
    hist.SetTitleFont(132, "X");
    hist.SetTitleFont(132, "Y");

    f->Close();
    delete f;
    return hist;
}


void etaDist() { 
  
    
    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for data files
    histarr[0] = createEtaHist("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", 12, kRed);
    histarr[1] = createEtaHist("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", 13, kAzure);
    histarr[2] = createEtaHist("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", 15, kBlack);
    histarr[3] = createEtaHist("LEP1_1.root", "LEP", 12, kGreen+1);
    histarr[4] = createEtaHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3,000 GeV)", 29, kMagenta);
    histarr[5] = createEtaHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10,000 GeV)", 52, kOrange+1); 

    // Creating a histogram stack pointer
    THStack *hstack = new THStack ("hstack", "");
    TLegend *l = new TLegend(0.7, 0.6, 0.9, 0.9); // Creating legend pointer

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "L");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 1000, 1000);
    c1->cd();

    // ***** HISTOGRAM CUSTOMIZATION *****
    hstack->Draw("nostack hist");
    
    hstack->GetXaxis()->SetTitle("#eta");
    hstack->GetXaxis()->SetTitleFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetTitleOffset(0.7);
    hstack->GetXaxis()->SetTitleSize(0.07);
    hstack->GetXaxis()->SetLabelSize(0.04);

    hstack->GetYaxis()->SetTitle("#frac{dN_{jet}^{entries}}{d#Delta#eta}");
    hstack->GetYaxis()->SetTitleFont(132);
    hstack->GetYaxis()->SetLabelFont(132);
    hstack->GetYaxis()->SetTitleOffset(1);
    hstack->GetYaxis()->SetTitleSize(0.06);
    hstack->GetYaxis()->SetLabelSize(0.04); 

    gPad->SetLeftMargin(0.16);
    gPad->SetRightMargin(0.01);
    gPad->SetBottomMargin(0.13);
    gPad->SetTopMargin(0.01);

    gStyle->SetCanvasBorderSize(3);
    gStyle->SetPadBorderSize(0);
    
    l->SetBorderSize(0); // Remove the border
    l->SetFillStyle(0);
    l->SetTextFont(12);
    
    l->Draw(); 
    
    //gPad->SetLogy();
    
    // Saving canvas c1 to pdf file
    c1->Print("etaDist.root"); 

    // Deallocating memory
    delete hstack;
    delete l;
    delete c1;
}
