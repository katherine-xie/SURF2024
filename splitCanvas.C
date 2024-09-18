#include <iostream>
#include <vector>
#include <string>
// Using namespace std

// Function to create a histogram based on input file
TH1F createChgHist(TString fileName, TString legendLabel, Int_t xMax, Int_t markerColor, Int_t markerStyle) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Int_t>> chgBranch(reader, "chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, xMax, 0, xMax);
    hist.SetMarkerColor(markerColor);
    hist.SetMarkerStyle(markerStyle);
    hist.SetMarkerSize(0.8);

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

void test() {
    TStyle *style = new TStyle("style", "Style for a Split Canvas"); 

    // Canvas settings 
	style->SetCanvasColor(0); 
	style->SetCanvasBorderMode(0); 
	style->SetCanvasBorderSize(0); 

    // Axis settings
    style->SetLabelFont(44, "XYZ"); // Font applied to X, Y, and Z axes
    style->SetLabelSize(0.05, "XYZ");
    style->SetTitleFont(44, "XYZ");
    style->SetTitleSize(0.06, "XYZ");

    // Legend settings
    style->SetLegendBorderSize(0);
    style->SetLegendFillColor(0);
    style->SetLegendFont(44);

    style->SetPadGridX(true);
    style->SetPadGridY(true);
    
    style->cd();
	//gROOT->ForceStyle();
}

void splitCanvas() { 

    test();
    //gStyle->SetPadLeftMargin(0.16);

    // Creating a histogram array
    TH1F harr1[3];
    TH1F harr2[3];

    // Calling createHist function for all data files
    harr1[0] = createChgHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", 70, kRed, 20);
    harr1[1] = createChgHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", 70, kAzure, 21);
    harr1[2] = createChgHist("LEP1_1.root", "LEP1_1", 75, kGreen+1, 22);

    harr2[0] = createChgHist("epem_ttbar_365GeV.root", "epem_ttbar (365 GeV)", 200, kBlack, 20);
    harr2[1] = createChgHist("mupmum_3000GeV.root", "mupmum (3000 GeV)", 200, kMagenta, 21);
    harr2[2] = createChgHist("mupmum_10000GeV.root", "mupmum (10000 GeV)", 200, kOrange+1, 22);

    // Creating a histogram stack object
    THStack *hstack1 = new THStack("hstack1", "Charged Particle Multiplicity Distribution; Number of Particles; Entries");
    THStack *hstack2 = new THStack("hstack2", "; Number of Particles; Entries");

    TLegend *l1 = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object
    TLegend *l2 = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 3; i++) {
        hstack1->Add(&harr1[i]);
        l1->AddEntry(&harr1[i], harr1[i].GetName(), "P");

        hstack2->Add(&harr2[i]);
        l2->AddEntry(&harr2[i], harr2[i].GetName(), "P");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 800);
    
    c1->Divide(1, 2);

    c1->cd(1);
    hstack1->Draw("Pnostack");
    l1->Draw();
    //gPad->SetGrid();

    c1->cd(2);
    hstack2->Draw("Pnostack");
    l2->Draw();
    //gPad->SetGrid();

    // Update the canvas to render all the changes
    c1->Update();
    
    //hstack->GetYaxis()->SetTitleOffset(0.8);
    //hstack->GetYaxis()->SetLabelOffset(0.01);

    // Saving canvas c1 to pdf file
    c1->Print("chgMultDist.pdf"); 

    // Deallocating memory
    delete c1;
    delete hstack1;
    delete l1;
    delete hstack2;
    delete l2;
    
}
