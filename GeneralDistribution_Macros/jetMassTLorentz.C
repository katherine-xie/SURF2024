#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include <Pythia8/Pythia.h> 
using namespace Pythia8;
#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
#include "SURF_FUNCTIONS/coordinateTools.h"

R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);
// Function to create a histogram based on input file
TH1F createJetMassHist2(TString fileName, TString legendLabel, Int_t colorVal) {

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

    //hist.SetMarkerStyle(21);
    //hist.SetMarkerSize(0.7);
    //hist.SetMarkerColor(colorVal); 

    reader.Restart();

    // Event loop
    while (reader.Next()) {

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPtBranch->size(); i++) {
            TLorentzVector jet; // TLorentz vector for each jet

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPidBranch)[i].size(); j++) {

                Float_t px = calculatePx((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                Float_t py = calculatePy((*dauPtBranch)[i][j], (*dauPhiBranch)[i][j]);
                Float_t pz = calculatePz((*dauPtBranch)[i][j], (*dauEtaBranch)[i][j]);
                Float_t E = calculateEnergy(px, py, pz, particleData.m0((*dauPidBranch)[i][j]));

                TLorentzVector particle; // Initialize TLorentz vector (for individual particles)
                particle.SetPxPyPzE(px, py, pz, E);

                jet += particle;
                
            }
            hist.Fill(jet.M()); // Filling histogram of the invariant mass (for each jet)
        }
    }    
    
    // ***** NORMALIZATION *****
    // We want to make the hist independent of the number of jets and the bin width
    Float_t binWidth = 1;
    hist.Scale(1.0/(binWidth));


    f->Close();
    delete f;
    return hist;
}

void jetMassTLorentz() {

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for data files
    histarr[0] = createJetMassHist2("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", kRed);
    histarr[1] = createJetMassHist2("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", kAzure);
    histarr[2] = createJetMassHist2("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", kBlack);
    histarr[3] = createJetMassHist2("LEP1_1.root", "LEP1_1", kGreen+1);
    histarr[4] = createJetMassHist2("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", kMagenta);
    histarr[5] = createJetMassHist2("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", kOrange+1); 

    // Creating a histogram stack pointer
    THStack *hstack = new THStack ("hstack", "Jet Invariant Mass Distribution");
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

    hstack->Draw("nostack hist");

    hstack->GetXaxis()->SetTitle("m_{invariant} [GeV]");
    hstack->GetXaxis()->SetTitleFont(132);

    hstack->GetYaxis()->SetTitle("#frac{dN_{entries}}{d#Deltam_{invariant}}");
    hstack->GetYaxis()->SetTitleFont(132);
    hstack->GetYaxis()->SetTitleOffset(1.2);

    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetYaxis()->SetLabelFont(132);
    
    l->Draw();
    l->SetBorderSize(0); // Remove the border
    l->SetFillStyle(0);
    l->SetTextFont(132);
    
    gPad->SetLogy();
    //gPad->SetLogx();

    // Saving canvas c1 to pdf file
    c1->Print("jetMassTLorentz2.root"); 

    // Deallocating memory
    delete hstack;
    delete l;
    delete c1;
}