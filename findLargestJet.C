#include <iostream>
#include <vector>
#include <string>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Global variables
std::string title = "#mu^{+}#mu^{#minus} (3,000 GeV)";
std::string fullPathDir = "/Users/katherinexie/SURF2024/futureCollidersData/mupmum_3000GeV.root";
TFile *f = new TFile(fullPathDir.c_str(), "read"); // Opening file

// Creating TTreeReader object and linking branches
TTreeReader* reader = new TTreeReader("trackTree");

// Setup branches for other particles
TTreeReaderValue<std::vector<std::vector<Float_t>>> pPt(*reader, "genDau_pt");
TTreeReaderValue<std::vector<std::vector<Float_t>>> pEta(*reader, "genDau_eta");
TTreeReaderValue<std::vector<std::vector<Float_t>>> pPhi(*reader, "genDau_phi");
TTreeReaderValue<std::vector<std::vector<Int_t>>> pChg(*reader, "genDau_chg");
TTreeReaderValue<std::vector<std::vector<Int_t>>> pPID(*reader, "genDau_pid");


// Function to sort jet multiplcities (based on column)
bool sortcol(const std::vector<Int_t>& v1, const std::vector<Int_t>& v2)
{
    return v1[0] > v2[0];
}

std::vector<std::vector<Int_t>> sortAllJets() {

    std::vector<std::vector<Int_t>> allSortedIndices;
    
    reader->Restart();
    while (reader->Next()) {

        Int_t currEventIndex = reader->GetCurrentEntry();
        
        // JET LOOP
        for (Int_t jetIndex = 0; jetIndex < pPt->size(); jetIndex++) {        
            
            Int_t jetCounter = 0;

            // PARTICLE LOOP
            for (Int_t particleIndex = 0; particleIndex < (*pPt)[jetIndex].size(); particleIndex++) {

                if ((*pChg)[jetIndex][particleIndex] == 0) {continue;}
                jetCounter++;
                
            }

            allSortedIndices.push_back({jetCounter, currEventIndex, jetIndex});
        }
    }

    std::sort(allSortedIndices.begin(), allSortedIndices.end(), sortcol); // Sorting vector

    for (Int_t i = 0; i < allSortedIndices.size(); i++) {
        for (Int_t j = 0; j < allSortedIndices[i].size(); j++) {
            std::cout << allSortedIndices[i][j] << " ";
        }
        std::cout << std::endl;
    }
    return allSortedIndices;
}


void findLargestJet() {

    Int_t topN = 5;

    std::vector<std::vector<Int_t>> fullIndices = sortAllJets();

    std::cout << "Top " << topN << " Jets for " << title << ": " << std::endl;
    for (Int_t i = 0; i < topN; i++) {
        std::cout << i+1 << ". " << fullIndices[i][0] << " particles (Event " << fullIndices[i][1] << ", Jet " << fullIndices[i][2] << ")" << std::endl;
    }
    
    /*
    Float_t ptCut = 0; // pT cutoff  variable

    Int_t highestEventIndex = 0;
    Int_t highestJetIndex = 0;
    Int_t highestJetN = 0;

    reader->Restart();
    while (reader->Next()) {

        Int_t eventIndex = reader->GetCurrentEntry();
        
        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < pPt->size(); i++) {

            Int_t jetN = 0; // Counter for particles in each jet

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*pPt)[i].size(); j++) {
                if ((*pChg)[i][j] == 0) {continue;} // Checking if particle is charged
                if ((*pPt)[i][j] < ptCut) {continue;}
                jetN++;
            }

            if (jetN >= highestJetN) {
                highestJetN = jetN;
                highestEventIndex = eventIndex;
                highestJetIndex = i;
            }
        }
    }
    std::cout << "The largest jet occurs at event " << highestEventIndex << ", jet number " << highestJetIndex << " with " << highestJetN << " charged particles." << std::endl; */
}

