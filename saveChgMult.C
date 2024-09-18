#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Function to create a histogram based on input file
void saveData(TString fileName) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the input file
    TFile *fin = TFile::Open(pathdir + fileName + ".root", "read");

    // Getting trackTree (as a pointer)
    TTree *trackTree = (TTree*)fin->Get("trackTree");

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<vector<Int_t>> chgBranch(reader, "chg");

    // Creating variable to hold mult. values
    Int_t chgMultVal;

    // Creating branch to store charge mult. values in trackTree
    TBranch *multBranch = trackTree->Branch("chargedMultiplicity", &chgMultVal, "chargedMultiplicity/I");

    while (reader.Next()) {

        vector<Int_t> chgVal = *chgBranch;
        Int_t count = 0;

        for (Int_t i = 0; i < chgVal.size(); i++) {
            if (chgVal[i] < 0 || chgVal[i] > 0) {
                count++;
            }
        }

        chgMultVal = count;
        multBranch->Fill();
    }    

    // Write the changes back to the file
    trackTree->Write(fileName + "_updated.root");
    fin->Close();
    delete fin;
    delete trackTree;
}


void saveChgMult() { 

    saveData("epem_ttbar_365GeV");
    /*
    histarr[0] = createChgHist("epem_ttbar_365GeV", "epem_ttbar (365 GeV)", kBlack);
    histarr[1] = createChgHist("epem_WW_163GeV", "epem_WW (163 GeV)", kRed);
    histarr[2] = createChgHist("epem_ZH_240GeV", "epem_ZH (240 GeV)", kAzure);
    histarr[3] = createChgHist("LEP1_1", "LEP1_1", kGreen+1);
    histarr[4] = createChgHist("mupmum_3000GeV", "mupmum (3000 GeV)", kMagenta);
    histarr[5] = createChgHist("mupmum_10000GeV", "mupmum (10000 GeV)", kOrange+1); */


    
}
