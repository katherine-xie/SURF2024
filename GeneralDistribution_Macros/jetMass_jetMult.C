#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <Pythia8/Pythia.h> 
using namespace Pythia8;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

TH2F create2DHist1(TString fileName, TString legendLabel, Int_t xMax, Int_t yMax) {

    Pythia8::Pythia pythia; // Initialize Pythia
    ParticleData &particleData = pythia.particleData; // Access the particle data table

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
             "Jet Mass vs. N_{ch} for " + legendLabel + ";Mass [GeV];N_{ch}", 
             xMax, 0, xMax,
             yMax, 0, yMax);
    
    // Event loop
    while (reader.Next()) {

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPidBranch->size(); i++) {

            TLorentzVector jet; // TLorentz vector for each jet
            Int_t count = 0; // Counter for N_ch (per jet)

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPtBranch)[i].size(); j++) {

                TLorentzVector particle; // Initialize TLorentz vector (for individual particles)

                // Checking if particle is charged
                if ((*dauChgBranch)[i][j] < 0 || (*dauChgBranch)[i][j] > 0) {
                    count++;
                    Float_t px = calculatePx((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                    Float_t py = calculatePy((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                    Float_t pz = calculatePz((*dauPtBranch)[i][j], (*dauEtaBranch)[i][j]);
                    Float_t E = calculateEnergy(px, py, pz, particleData.m0((*dauPidBranch)[i][j]));
                    particle.SetPxPyPzE(px, py, pz, E);
                }

                jet += particle;
            }
            h2D.Fill(jet.M(), count); // Filling histogram of the invariant mass (for each jet) and N_ch
        }  
    }
    h2D.SetStats(0);
    //h2D.GetZaxis()->SetRangeUser(0, 200e3);
    f->Close();
    delete f;
    return h2D;
}

void jetMass_jetMult() {

    // Creating a histogram array
    TH2F histarr[6];

    //Calling createHist function for all data files
    histarr[0] = create2DHist1("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", 100, 100);
    histarr[1] = create2DHist1("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", 100, 100);
    histarr[2] = create2DHist1("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", 100, 100);
    histarr[3] = create2DHist1("LEP1_1.root", "LEP1_1", 100, 100);
    histarr[4] = create2DHist1("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", 600, 200);
    histarr[5] = create2DHist1("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", 1500, 200);

    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 612, 792);
    c1->Divide(2, 3, 0.005, 0.005);

    for (Int_t i = 1; i < 7; i++) {
        c1->cd(i);
        histarr[i-1].Draw("COLZ");
        gPad->SetGrid();
        gPad->SetLogz();
    } 
    
    // Saving canvas c1 to pdf file
    c1->Print("jetMass_jetMult.pdf"); 

    // Deallocating memory
    delete c1; 

}