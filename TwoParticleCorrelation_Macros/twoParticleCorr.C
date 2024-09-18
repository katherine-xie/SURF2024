#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TAttLine.h"
#include "THelix.h"
#include "TView.h"
#include "TRandom.h"
#include "TAttPad.h"
#include "TMath.h"
#include "TVector3.h"
#include "TView3D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"

#include <iostream>
#include <string>
#include <vector>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
#include "SURF_FUNCTIONS/coordinateTools.h"

R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Global variables
//std::string title = "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)";
std::string fullPathDir = "/Users/katherinexie/SURF2024/futureCollidersData/epem_WW_163GeV.root";
TFile *f = new TFile(fullPathDir.c_str(), "read"); // Opening file

// Creating TTreeReader object and linking branches
TTreeReader* reader = new TTreeReader("trackTree");

// Linking Branches
TTreeReaderValue<vector<vector<Float_t>>> pPt(*reader, "genDau_pt");
TTreeReaderValue<vector<vector<Float_t>>> pPhi(*reader, "genDau_phi");
TTreeReaderValue<vector<vector<Float_t>>> pEta(*reader, "genDau_eta");
TTreeReaderValue<vector<vector<Int_t>>> pChg(*reader, "genDau_chg"); 

// Function that returns the distribution of the signal function 
TH2F createSignalDist(std::vector<std::vector<Int_t>> triggIndices,
                      std::vector<std::vector<Int_t>> assocIndices) {
    
    // Histogram for the signal distribution
    TH2F hSignal("hSignal", "Signal Distribution;#Delta#eta;#Delta#phi", 100, -5, 5, 100, -TMath::Pi(), TMath::Pi());

    // Note: assocIndices must be larger or equal to triggIndices
    if (assocIndices.size() >= triggIndices.size()) {

        // Loop through both vectors
        for (Int_t i = 0; i < triggIndices.size(); i++) {
            for (Int_t j = 0; j < assocIndices.size(); j++) {

                // Check to see if the trigger and associate particles are from the same event
                if (triggIndices[i][0] == assocIndices[j][0]) {
                    reader->SetEntry(triggIndices[i][0]); // Go to the event index

                    // Initializing various indices (for readability)
                    Int_t triggJetIndex = triggIndices[i][1];
                    Int_t triggParticleIndex = triggIndices[i][2];

                    Int_t assocJetIndex = assocIndices[j][1];
                    Int_t assocParticleIndex = assocIndices[j][2];

                    // Calculating delta eta and delta phi
                    Float_t deltaEta = (*pEta)[assocJetIndex][assocParticleIndex] - (*pEta)[triggJetIndex][triggParticleIndex];
                    Float_t deltaPhi = (*pPhi)[assocJetIndex][assocParticleIndex] - (*pPhi)[triggJetIndex][triggParticleIndex];

                    hSignal.Fill(deltaEta, deltaPhi);
                }
            }
        }
    }
    else {std::cout << "Vector out of bounds error: The size of assocIndices must be larger or equal to triggIndices" << std::endl;}

    //hSignal->SetMaximum(50);
    f->Close();
    return hSignal;
}

/*
// Function that returns the distribution of the mixed-event background function
TH2F createBackgroundDist(std::vector<std::vector<Int_t>> triggIndices, std::vector<Int_t> multVec) {

    // Histogram for the background distribution
    TH2F hBackground("hBackground", "Background Distribution;#Delta#eta;#Delta#phi", 100, -5, 5, 100, -TMath::Pi(), TMath::Pi());

    for (Int_t i = 0; i < triggIndices.size(); i++) {
        
        // Setting indices for the trigger particle
        Int_t triggEventIndex = triggIndices[i][0];
        Int_t triggJetIndex = triggIndices[i][1];
        Int_t triggParticleIndex = triggIndices[i][2];

        // Counters for mixed events
        Int_t numMixedEvents = 10;
        Int_t count = 0;

        while (count < numMixedEvents) {
            // Randomly generate an event
            TRandom rand; 
            Float_t randEventIndex = rand.Uniform(0, reader->GetEntries());

            if (multVec[randEventIndex] < 50) {continue;} // Making sure it is within the multiplicity bin
            count++;

            // Going to the specific event
            reader->SetEntry(randEventIndex);

            // Looping through particles in that event
            for (Int_t jet = 0; jet < pPt->size(); jet++) {
                for (Int_t p = 0; p < (*pPt)[jet].size(); p++) {

                    // Applying conditions
                    if ((*pChg)[jet][p] == 0) {continue;}
                    if (TMath::Abs((*pEta)[i][j]) > 2.5) {continue;}

                    // Applying the condition for an associate particle
                    if ((*pPt)[jet][p] > 2 && (*pPt)[jet][p] < 5) {

                        Int_t assocJetIndx = jet;
                        Int_t assocParticleIndx = p;

                        // Calculating delta eta and delta phi
                        Float_t deltaEta = (*pEta)[assocJetIndx][assocParticleIndx] - (*pEta)[triggJetIndex][triggParticleIndex];
                        Float_t deltaPhi = (*pPhi)[assocJetIndx][assocParticleIndx] - (*pPhi)[triggJetIndex][triggParticleIndex];

                        hBackground.Fill(deltaEta, deltaPhi);
                    }
                }
            }
        }
    }
    f->Close();
    return hBackground;
} */

void twoParticleCorr() {

    // Vector of trigger particle and associate particle indices
    // Stores: event index, jet index, daughter particle index as a 2D vector
    std::vector<std::vector<Int_t>> triggIndices;
    std::vector<std::vector<Int_t>> assocIndices; 

    // Counting the multiplcitiy
    std::vector<Int_t> multVec; // Store the multiplcity values in a vector
    while (reader->Next()) {
        Int_t multiplicity = 0; // Counter for N_ch
        for (Int_t i = 0; i < pPt->size(); i++) {
            for (Int_t j = 0; j < (*pPt)[i].size(); j++) {
                if ((*pChg)[i][j] != 0) {multiplicity++;}
            }
        }
        multVec.push_back(multiplicity);
    }

    reader->SetEntry(0); // Restarting event loop

    // Finding the trigger and associate particles
    // Event loop
    while (reader->Next()) {

        Int_t entryIndex = reader->GetCurrentEntry();
        
        // Checking if the current event is in the multiplicty bin
        if (multVec[entryIndex] < 40) {continue;}

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < pPt->size(); i++) {

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*pPt)[i].size(); j++) {

                // Checking if particle is charged and is in the eta range
                if ((*pChg)[i][j] == 0) {continue;}
                if (TMath::Abs((*pEta)[i][j]) >= 2.5) {continue;}
      
                // Checking if particle is an associate/trigger particle
                if ((*pPt)[i][j] > 1 && (*pPt)[i][j] < 2) {
                    assocIndices.push_back({entryIndex, i, j});
                }
                else if ((*pPt)[i][j] > 2 && (*pPt)[i][j] < 3) {
                    triggIndices.push_back({entryIndex, i, j});
                }
            }
        }
    }

    /*
    // Printing out triggIndices (just to check)
    for (Int_t i = 0; i < triggIndices.size(); i++) {
        for (Int_t j = 0; j < triggIndices[i].size(); j++) {
            std::cout << triggIndices[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "The size of the trigger indices vector is " << triggIndices.size() << std::endl;
    */
    
    /*
    // Printing out assocIndicies (just to check)
    for (Int_t i = 0; i < assocIndices.size(); i++) {
        for (Int_t j = 0; j < assocIndices[i].size(); j++) {
            std::cout << assocIndices[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "The size of the associate indices vector is " << assocIndices.size() << std::endl; 
    */
    

    // Creating canvas for the signal histogram
    TCanvas *cSignal = new TCanvas("cSignal", "Canvas for the Signal Distribution", 800, 600);
    TH2F signalHist = createSignalDist(triggIndices, assocIndices);

    cSignal->cd();
    signalHist.Draw("SURF1");
    cSignal->Print("testSignal.root"); 
    
    // Creating canvas for the background histogram
    //TCanvas *cBackground = new TCanvas("cBackround", "Canvas for the Background Distribution", 800, 600);

    //TH2F backgroundHist = createBackgroundDist(triggIndices);

    //cBackground->cd();
    //backgroundHist.Draw("SURF1");
    //cBackground->Print("testBackgroundFunc.root"); 

    f->Close();
    delete cSignal;
    //delete cBackground;

}