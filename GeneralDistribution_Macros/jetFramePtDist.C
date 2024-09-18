#include <iostream> 
using namespace std;
#include <vector>
#include <string>
#include <Pythia8/Pythia.h> 
using namespace Pythia8;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
#include "SURF_FUNCTIONS/coordinateTools.h"

R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

TH1F createJetFramePtHist(TString fileName, TString legendLabel, Int_t numBins, Int_t xMax, Int_t colorVal) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<vector<Float_t>>> dauPtBranch(reader, "genDau_pt");
    TTreeReaderValue<vector<vector<Float_t>>> dauEtaBranch(reader, "genDau_eta");
    TTreeReaderValue<vector<vector<Float_t>>> dauPhiBranch(reader, "genDau_phi");
    TTreeReaderValue<vector<vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Creating histogram and setting attributes
    TH1F hist(legendLabel, "Histogram for " + legendLabel, numBins, 0, xMax);
    hist.SetLineColor(colorVal);
    hist.SetLineWidth(2);

    // Event loop
    while (reader.Next()) {

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPtBranch->size(); i++) {

            // Vector for each jet with components pT, eta, phi
            TVector3 jet;
            jet.SetPtEtaPhi(calculateJetPt((*dauPtBranch)[i], (*dauPhiBranch)[i]),  
                            calculateJetEta((*dauPtBranch)[i], (*dauEtaBranch)[i], (*dauPhiBranch)[i]),
                            calculateJetPhi((*dauPtBranch)[i], (*dauPhiBranch)[i])); 

            vector<TVector3> particlesVec; // Vector to hold the particles 

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPtBranch)[i].size(); j++) {
                // Checking if particle is charged
                if ((*dauChgBranch)[i][j] < 0 || (*dauChgBranch)[i][j] > 0) {

                    // Initialize vector (for individual particles)
                    TVector3 particle;
                    
                    particle.SetPtEtaPhi((*dauPtBranch)[i][j],
                                         (*dauEtaBranch)[i][j],
                                         (*dauPhiBranch)[i][j]);

                    particlesVec.push_back(particle);
                }
            }
            
            for (Int_t j = 0; j < particlesVec.size(); j++) {hist.Fill(ptWRTJet(jet, particlesVec[j]));} // Filling histogram
        }
    }    
    
    f->Close();
    delete f;
    return hist;
}

void jetFramePtDist() {

    // Creating a histogram array
    TH1F harr1[4];
    TH1F harr2[2];

    // Calling createHist function for all data files
    harr1[0] = createJetFramePtHist("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", 150, 200, kBlack);
    harr1[1] = createJetFramePtHist("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", 150, 200, kRed);
    harr1[2] = createJetFramePtHist("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", 150, 200, kAzure);
    harr1[3] = createJetFramePtHist("LEP1_1.root", "LEP1_1", 150, 200, kGreen+1);

    harr2[0] = createJetFramePtHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3000 GeV)", 200, 800, kMagenta);
    harr2[1] = createJetFramePtHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10000 GeV)", 200, 800, kOrange+1);

    // Creating a histogram stack object
    THStack *hstack1 = new THStack("hstack1", "p*_{T} Distribution (w.r.t. Jet Frame);p*_{T} [GeV]; Entries");
    THStack *hstack2 = new THStack("hstack2", ";p*_{T} [GeV]; Entries");

    TLegend *l1 = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object
    TLegend *l2 = new TLegend(0.7, 0.6, 0.9, 0.9); // TLegend object

    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 4; i++) {
        hstack1->Add(&harr1[i]);
        l1->AddEntry(&harr1[i], harr1[i].GetName(), "L");
    }
   
    for (Int_t i = 0; i < 2; i++) {
        hstack2->Add(&harr2[i]);
        l2->AddEntry(&harr2[i], harr2[i].GetName(), "L");
    }
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 800, 800);

    c1->Divide(1, 2);

    c1->cd(1);
    hstack1->Draw("nostack");
    l1->Draw();
    gPad->SetGrid();
    gPad->SetLogy();
    //gPad->SetLogx();
    hstack1->GetXaxis()->SetTitleOffset(1.3);

    c1->cd(2);
    hstack2->Draw("nostack");
    l2->Draw();
    gPad->SetGrid();
    gPad->SetLogy();
    hstack2->GetXaxis()->SetTitleOffset(1.3);

    //hstack->GetYaxis()->SetTitleOffset(0.8);
    //hstack->GetYaxis()->SetLabelOffset(0.01);

    // Saving canvas c1 to pdf file
    c1->Print("jetFramePtDist4.root"); 

    // Deallocating memory
    delete c1;
    delete hstack1;
    delete l1;
    delete hstack2;
    delete l2;
}