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

std::vector<std::vector<Int_t>> sortEvents() {

    std::vector<std::vector<Int_t>> allSortedEventIndices;
    
    reader->Restart();
    while (reader->Next()) {

        Int_t currEventIndex = reader->GetCurrentEntry();
        Int_t counter = 0;

        // JET LOOP
        for (Int_t jetIndex = 0; jetIndex < pPt->size(); jetIndex++) {        
            // PARTICLE LOOP
            for (Int_t particleIndex = 0; particleIndex < (*pPt)[jetIndex].size(); particleIndex++) {
                if ((*pChg)[jetIndex][particleIndex] == 0) {continue;}
                counter++;
            }
        }

        allSortedEventIndices.push_back({counter, currEventIndex});
    }

    std::sort(allSortedEventIndices.begin(), allSortedEventIndices.end(), sortcol); // Sorting vector

    for (Int_t i = 0; i < allSortedEventIndices.size(); i++) {
        for (Int_t j = 0; j < allSortedEventIndices[i].size(); j++) {
            std::cout << allSortedEventIndices[i][j] << " ";
        }
        std::cout << std::endl;
    }
    return allSortedEventIndices;
}


void findLargestEvents() {

    Int_t topN = 5;

    std::vector<std::vector<Int_t>> fullIndices = sortEvents();

    std::cout << "Top " << topN << " Events for " << title << ": " << std::endl;
    for (Int_t i = 0; i < topN; i++) {
        std::cout << i+1 << ". " << fullIndices[i][0] << " particles (Event " << fullIndices[i][1] << ")" << std::endl;
    }
    
}