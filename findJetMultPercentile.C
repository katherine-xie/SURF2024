#include <iostream>
#include <vector>
#include <string>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

void findJetMultPercentile() {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";
    TString fileName = "epem_WW_163GeV.root";
    Float_t percentile = 0.99;

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");

    // Linking Branches
   // TTreeReaderValue<std::vector<std::vector<Float_t>>> dauPtBranch(reader, "genDau_pt");
    TTreeReaderValue<std::vector<std::vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Finding the multiplicity of each jet and storing it in a vector
    std::vector<Int_t> multVec;

    //Int_t totalJetNum = 0;

    reader.SetEntry(0); // Restarting event loop

    // Event loop
    while (reader.Next()) {

        //totalJetNum += dauChgBranch->size();

        // Jet loop
        for (Int_t jet = 0; jet < dauChgBranch->size(); jet++) {

            Int_t jetMultCount = 0;

            // Particle loop
            for (Int_t p = 0; p < (*dauChgBranch)[jet].size(); p++) {
                if ((*dauChgBranch)[jet][p] == 0) {continue;}
                jetMultCount++;
            }

            multVec.push_back(jetMultCount); // Adding each jet N_ch to multVect
        }

    }

    sort(multVec.begin(), multVec.end()); // Sorting vector

    // Finding percentile index
    Int_t index = std::ceil(percentile * multVec.size()) - 1; // The - 1 is to adjust for a 0-based indexing system in C++
    
    
    // Finding number of entries greater than the percentile
    Int_t numEntries = 0;
 
    for (Int_t i = 0; i < multVec.size(); i++) {
        if (multVec[i] < 12) {continue;}
        numEntries++;
    }
    
    std::cout << "There are a total of " << multVec.size() << " jets in " << fileName << std::endl;
    std::cout << "The 99th percentile for jet N_ch is " << multVec[index] << std::endl;
    std::cout << "There are " << numEntries << " entries greater than the 99th percentile" << std::endl;
}