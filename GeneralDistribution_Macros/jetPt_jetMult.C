#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <Pythia8/Pythia.h> 
using namespace Pythia8;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

TH2F create2DHist2(TString fileName, TString legendLabel, Int_t xMax, Int_t yMax) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<vector<Float_t>>> dauPtBranch(reader, "genDau_pt");
    TTreeReaderValue<vector<vector<Float_t>>> dauPhiBranch(reader, "genDau_phi");
    TTreeReaderValue<vector<vector<Float_t>>> dauEtaBranch(reader, "genDau_eta");
    TTreeReaderValue<vector<vector<Int_t>>> dauPidBranch(reader, "genDau_pid");
    TTreeReaderValue<vector<vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Creating 2D Histogram
    TH2F h2D(legendLabel, 
             "Jet p_{T} vs. N_{ch} for " + legendLabel + ";Jet p_{T} [GeV];N_{ch}", 
             xMax, 0, xMax,
             yMax, 0, yMax);
    
    // Event loop
    while (reader.Next()) {

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPtBranch->size(); i++) {
            Int_t count = 0; // Counter for N_ch (per jet)

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPtBranch)[i].size(); j++) {
                // Checking if particle is charged
                if ((*dauChgBranch)[i][j] < 0 || (*dauChgBranch)[i][j] > 0) {
                    count++;
                }
            }
            Float_t jetPt = calculateJetPt((*dauPtBranch)[i], (*dauPhiBranch)[i]); 
            h2D.Fill(jetPt, count); // Filling histogram 
        }  
    }
    h2D.SetStats(0);
    //h2D.GetZaxis()->SetRangeUser(0, 200e3);
    f->Close();
    delete f;
    return h2D;
}

void jetPt_jetMult() {

    // Creating a histogram array
    TH2F histarr[6];

    //Calling createHist function for all data files
    histarr[0] = create2DHist2("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", 200, 60);
    histarr[1] = create2DHist2("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", 200, 60);
    histarr[2] = create2DHist2("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", 200, 60);
    histarr[3] = create2DHist2("LEP1_1.root", "LEP1_1", 200, 60);
    histarr[4] = create2DHist2("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", 6000, 200);
    histarr[5] = create2DHist2("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", 6000, 200);

    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 612, 792);
    c1->Divide(2, 3);

    for (Int_t i = 1; i < 7; i++) {
        c1->cd(i);
        histarr[i-1].Draw("COLZ");
        histarr[i-1].GetXaxis()->SetTitleOffset(1.4);
        gPad->SetGrid();
        gPad->SetLogz();
    } 
    
    // Saving canvas c1 to pdf file
    c1->Print("jetPt_jetMult3.pdf"); 

    // Deallocating memory
    delete c1; 

}