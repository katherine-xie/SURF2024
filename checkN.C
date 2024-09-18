#include <iostream>
#include <vector>
#include <string>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

void checkN() {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";
    TString fileName = "mupmum_10000GeV.root";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");

    // Linking Branches
    TTreeReaderValue<vector<vector<Float_t>>> dauPtBranch(reader, "genDau_pt");
    TTreeReaderValue<vector<vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    Float_t ptCut = 0; // pT cutoff  variable
    Float_t NCutOff = 60;
    Int_t numEventsPrinted = 0;
    Int_t highestEventIndex = 0;
    Int_t highestN = 0;

    while (reader.Next()) {
        Int_t eventIndex = reader.GetCurrentEntry();
        Int_t countN = 0; 

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPtBranch->size(); i++) {

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPtBranch)[i].size(); j++) {
                if ((*dauChgBranch)[i][j] == 0) {continue;} // Checking if particle is charged
                if ((*dauPtBranch)[i][j] < ptCut) {continue;}
                countN++;
            }
        }

        if (countN > NCutOff) {
        std::cout << std::to_string(eventIndex) << ": " << std::to_string(countN) << std::endl;
        numEventsPrinted++;
        }

        if (countN >= highestN) {
            highestN = countN;
            highestEventIndex = eventIndex;
        }
        
    }

    std::cout << "There are " << numEventsPrinted << " events with N_{ch} > " << NCutOff << std::endl;
    std::cout << "The highest N_{ch} is " << highestN << " particles at event index " << highestEventIndex << endl;
}