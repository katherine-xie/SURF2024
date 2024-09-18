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


void calculateRidgePeakRatio() {

    std::string fullPathDir = "/Users/katherinexie/SURF2024/SURFMacros/twoParticleCorr_Epem163GeV.root";

    TFile *f = new TFile(fullPathDir.c_str(), "read"); // Opening file
    TFile *fout = new TFile("testProjection.root", "recreate"); // Creating output file

    TH1D* hProjectedBefore;
    f->GetObject("projectedHist", hProjectedBefore);
    TH1D* hProjectedAfter = (TH1D*)hProjectedBefore->Clone();
    hProjectedBefore->Write(); // Before any changes

    // Finding Global Minimum Value
    Double_t globalMinBin = hProjectedBefore->GetMinimumBin();
    Double_t globalMinVal = hProjectedBefore->GetBinContent(globalMinBin);

    std::cout << "The global minimum value is: " << globalMinBin << std::endl;
    std::cout << "The  global minimum value is: " << globalMinVal << std::endl;
    std::cout << std::endl;

    Int_t count = 1;

    std::cout << "SHIFTING HISTOGRAM BASELINE: " << std::endl;
    // Shifting the histogram by that amount
    for (Int_t i = 1; i <= hProjectedAfter->GetNbinsX(); i++) {
        Double_t currBinContent = hProjectedAfter->GetBinContent(i);
        std::cout << count << ". Before: " << currBinContent << ", After: " << currBinContent - globalMinVal << std::endl;
        hProjectedAfter->SetBinContent(i, currBinContent - globalMinVal);
        count++;
    }
    std::cout << std::endl;

    hProjectedAfter->SetMinimum(0);
    hProjectedAfter->Write(); 

    /*
    // Finding local min (in range 0 to pi)
    hProjectedBefore->GetXaxis()->SetRangeUser(0, TMath::Pi());
    Int_t localMinBin = hProjectedBefore->GetMinimumBin();
    Double_t localMinVal = hProjectedBefore->GetBinContent(localMinBin); */

    // Trying local min as the midpoint 
    Int_t localMinBin = hProjectedBefore->GetXaxis()->FindBin((TMath::Pi())/2.0);
    Double_t localMinVal = hProjectedBefore->GetBinContent(localMinBin); 

    std::cout << "The local minimum value is: " << localMinBin << std::endl;
    std::cout << "The local minimum value is: " << localMinVal << std::endl;
    std::cout << std::endl;

    // ***** PEAK CALCULATIONS *****
    Int_t zeroBin = hProjectedAfter->GetXaxis()->FindBin(0.0);
    Int_t highBin = localMinBin;

    Int_t range = highBin - zeroBin;
    Int_t lowBin = zeroBin - range;

    std::cout << std::endl;
    std::cout << "Zero Bin: " << zeroBin << std::endl;
    std::cout << "Range: " << range << std::endl;
    
    std::cout << "Peak Low Bin: " << lowBin << std::endl;
    std::cout << "Peak High Bin: " << highBin << std::endl;

    Double_t peakSum = hProjectedAfter->Integral(lowBin, highBin);
    std::cout << "Peak Integral: " << peakSum << std::endl;

    // ***** RIDGE CALCULATIONS *****
    Int_t piBin = hProjectedAfter->GetXaxis()->FindBin(TMath::Pi());
    Int_t ridgeRange = piBin - localMinBin;

    Int_t ridgeLowBin = (piBin - ridgeRange)+1;
    Int_t ridgeHighBin = piBin + ridgeRange;

    std::cout << std::endl;
    std::cout << "Ridge Low Bin: " << ridgeLowBin << std::endl;
    std::cout << "Ridge High Bin: " << ridgeHighBin << std::endl;

    Double_t ridgeSum = hProjectedAfter->Integral(ridgeLowBin, ridgeHighBin);
    std::cout << "Ridge Integral: " << ridgeSum << std::endl;

    std::cout << "FINAL RATO: " << peakSum/ridgeSum << std::endl;

    f->Close();
    fout->Close();
}