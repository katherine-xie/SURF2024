#include <iostream>
#include <vector>
#include <string>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

void findEventMultPercentile() {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";
    TString fileName = "mupmum_10000GeV.root";
    Float_t percentile = 0.99;

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");

    // Linking Branches
    TTreeReaderValue<std::vector<std::vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Finding the multiplicity of particles in each event
    std::vector<Int_t> multVec;

    reader.Restart(); // Ensururing that event loop starts at beginning

    // Event loop
    while (reader.Next()) {

        Int_t count = 0;

        // Jet loop
        for (Int_t jet = 0; jet < dauChgBranch->size(); jet++) {
            // Particle loop
            for (Int_t p = 0; p < (*dauChgBranch)[jet].size(); p++) {
                if ((*dauChgBranch)[jet][p] == 0) {continue;}
                count++;
            }

        }
        multVec.push_back(count); // Adding the event N_ch to multVect
    }

    sort(multVec.begin(), multVec.end()); // Sorting vector

    // Finding percentile index (I am rounding up)
    Int_t index = std::ceil(percentile * multVec.size()) - 1; // The - 1 is to adjust for a 0-based indexing system in C++
    
    // Finding number of entries greater or equal to the percentile
    Int_t numEntries = 0;
 
    for (Int_t i = 0; i < multVec.size(); i++) {
        if (multVec[i] < multVec[index]) {continue;}
        numEntries++;
    }
    
    std::cout << "There are a total of " << multVec.size() << " events in " << fileName << std::endl;
    std::cout << "The 99th percentile for the event N_ch is " << multVec[index] << std::endl;
    std::cout << "There are " << numEntries << " entries greater than the 99th percentile" << std::endl;
}