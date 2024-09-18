#include <iostream> 
using namespace std;
#include <vector>
#include <string>
#include <Pythia8/Pythia.h> 
using namespace Pythia8;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Function to create a histogram based on input file
TH1F createJetMassHist(TString fileName, TString legendLabel, Int_t colorVal) {

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

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 300, 0, 300);
    hist.SetLineColor(colorVal);
    hist.SetLineWidth(2);

    //Int_t numEntries = 0; // Initializing counter for tot. num of entries

    // Event loop
    while (reader.Next()) {

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPtBranch->size(); i++) {

            // Initializing sum variables
            Float_t sumE = 0;
            Float_t sumPx = 0;
            Float_t sumPy = 0;
            Float_t sumPz = 0;

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPtBranch)[i].size(); j++) {

                // Checking if particle is charged
                if ((*dauChgBranch)[i][j] < 0 || (*dauChgBranch)[i][j] > 0) {

                    Float_t px = calculatePx((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                    Float_t py = calculatePy((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                    Float_t pz = calculatePz((*dauPtBranch)[i][j], (*dauEtaBranch)[i][j]);
                    Float_t E = calculateEnergy(px, py, pz, particleData.m0((*dauPidBranch)[i][j]));
                    //Float_t E = calculateEnergy(px, py, pz, getRestMass((*dauPidBranch)[i][j]));
                    sumPx += px;
                    sumPy += py;
                    sumPz += pz;
                    sumE += E;
                }
            }

            Float_t jetMass = sqrt((sumE * sumE) - (sumPx * sumPx) - (sumPy * sumPy) - (sumPz * sumPz));
            hist.Fill(jetMass); 
            //numEntries++;          
        }
    }    

    //cout << "Number of Entries for " << legendLabel << ": " << numEntries << endl;
    
    f->Close();
    delete f;
    return hist;
}

void jetMass() {

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for data files
    histarr[0] = createJetMassHist("epem_ttbar_365GeV.root", "e_{+}e_{-}#rightarrowt#bar{t} (365 GeV)", kBlack);
    histarr[1] = createJetMassHist("epem_WW_163GeV.root", "epem_WW (163 GeV)", kRed);
    histarr[2] = createJetMassHist("epem_ZH_240GeV.root", "epem_ZH (240 GeV)", kAzure);
    histarr[3] = createJetMassHist("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createJetMassHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", kMagenta);
    histarr[5] = createJetMassHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", kOrange+1); 

    // Creating a histogram stack pointer
    THStack *hstack = new THStack ("hstack", "Jet Invariant Mass Distribution; Mass [GeV]; Entries");
    TLegend *l = new TLegend(0.7, 0.6, 0.9, 0.9); // Creating legend pointer

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
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    gPad->SetLogy();
    //gPad->SetLogx();

    // Saving canvas c1 to pdf file
    c1->Print("jetMass2.pdf"); 

    // Deallocating memory
    delete hstack;
    delete l;
    delete c1;
}