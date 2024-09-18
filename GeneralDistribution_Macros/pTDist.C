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
    TTreeReaderValue<vector<Int_t>> chgBranch(reader, "chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 700, 0, 700);
    hist.SetLineColor(colorVal);
    hist.SetLineWidth(15);

    // Event loop
    while (reader.Next()) {

        vector<Float_t> pxVal = *pxBranch;
        vector<Float_t> pyVal = *pyBranch;
        vector<Int_t> chgVal = *chgBranch;

        // Loop through particles produced in a collision
        for (Int_t i = 0; i < chgVal.size(); i++) {
            if (chgVal[i] < 0 || chgVal[i] > 0) {
                hist.Fill(calculatePt(pxVal[i], pyVal[i]));
            }
        }
    }    

    // Normalization
    //hist.Scale(1.0/(hist.GetEntries()));
    f->Close();
    delete f;
    return hist;
}


void pTDist() { 

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for all data files
    histarr[0] = createpTHist("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", kBlack);
    histarr[1] = createpTHist("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", kRed);
    histarr[2] = createpTHist("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", kAzure);
    histarr[3] = createpTHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createpTHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3,000 GeV)", kMagenta);
    histarr[5] = createpTHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10,000 GeV)", kOrange+1);

    // Creating a histogram stack object
    THStack *hstack = new THStack("hstack", "");
    //hstack->SetMaximum(500000.0);

    TLegend *l = new TLegend(0.5, 0.6, 0.9, 0.9); // TLegend object

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "L");
  
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 5000, 1000);
    c1->cd();

    //hstack->Draw("nostack hist");
/*
    // ***** HISTOGRAM CUSTOMIZATION *****    
    hstack->GetXaxis()->SetTitle("p_{T} [GeV]");
    hstack->GetXaxis()->SetTitleFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetTitleOffset(0.8);
    hstack->GetXaxis()->SetTitleSize(0.06);
    hstack->GetXaxis()->SetLabelSize(0.04);

    hstack->GetYaxis()->SetTitle("#frac{dN_{entries}}{d#Deltap_{T}}");
    hstack->GetYaxis()->SetTitleFont(132);
    hstack->GetYaxis()->SetLabelFont(132);
    hstack->GetYaxis()->SetTitleOffset(1.2);
    hstack->GetYaxis()->SetTitleSize(0.03);
    hstack->GetYaxis()->SetLabelSize(0.04); 

    gPad->SetLeftMargin(0.13);
    gPad->SetRightMargin(0.01);
    gPad->SetBottomMargin(0.14);
    gPad->SetTopMargin(0.01);
    gPad->SetLogy(); */

    gStyle->SetCanvasBorderSize(3);
    gStyle->SetPadBorderSize(0);
    gStyle->SetFillColor(0);

    l->Draw();
    l->SetHeader("Legend");
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    l->SetTextFont(12);
    //l->SetTextSize(50);

    TLegendEntry *header = (TLegendEntry*)l->GetListOfPrimitives()->First();
    header->SetTextAlign(kHAlignCenter + kVAlignCenter);
    header->SetTextFont(12);
    //header->SetTextSize(50);

    // Saving canvas c1 to pdf file
    c1->Print("pTDist.root"); 

    // Deallocating memory
    delete c1;
    delete hstack;
    delete l;
}
