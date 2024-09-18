#include <iostream> 
#include <vector>
#include <string>
#include <Pythia8/Pythia.h> 

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
#include "SURF_FUNCTIONS/coordinateTools.h"

R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

TH1F createJetFrameEtaHist(TString fileName, TString legendLabel, Int_t N_cutoff, Int_t colorVal) {

    TString pathdir = "/Users/katherinexie/SURF2024/futureCollidersData/";

    // Opening the file
    TFile *f = TFile::Open(pathdir + fileName);

    // Creating TTreeReader object
    TTreeReader reader("trackTree");
    TTreeReaderValue<std::vector<std::vector<Float_t>>> dauPtBranch(reader, "genDau_pt");
    TTreeReaderValue<std::vector<std::vector<Float_t>>> dauPhiBranch(reader, "genDau_phi");
    TTreeReaderValue<std::vector<std::vector<Float_t>>> dauEtaBranch(reader, "genDau_eta");
    TTreeReaderValue<std::vector<std::vector<Int_t>>> dauChgBranch(reader, "genDau_chg");

    // Creating histogram object
    TH1F hist(legendLabel, "Histogram for " + legendLabel, 100, -2, 10);

    /* FOR EVENT MULTIPLCITY
    // Finding the multiplicity of each event and storing it in a vector
    std::vector<Int_t> eventMultVec;
    while (reader.Next()) {
        Int_t eventMultCount = 0;
        for (Int_t jet = 0; jet < dauPtBranch->size(); jet++) {
            for (Int_t p = 0; p < (*dauPtBranch)[jet].size(); p++) {
                if ((*dauChgBranch)[jet][p] == 0) {continue;}
                eventMultCount++;
            }
        }
        eventMultVec.push_back(eventMultCount);
    }
    */

    // Finding the multiplicity of each jet and storing it in 2D vector
    std::vector<std::vector<Int_t>> multVect;
    while (reader.Next()) {
        std::vector<Int_t> jetVect; // Vector to store the multiplicities of each jet in an event
        for (Int_t jet = 0; jet < dauPtBranch->size(); jet++) {
            Int_t jetMultCount = 0;
            for (Int_t p = 0; p < (*dauPtBranch)[jet].size(); p++) {
                if ((*dauChgBranch)[jet][p] == 0) {continue;}
                jetMultCount++;
            }
            jetVect.push_back(jetMultCount);
        }
        multVect.push_back(jetVect);
    } 

    /*
    // Testing the multVect by printing it out
    for (Int_t i = 0; i < multVect.size(); i++) {
        for (Int_t j = 0; j < multVect[i].size(); j++) {
            std::cout << multVect[i][j] << " ";
        }
        std::cout << std::endl;
    } */
     
    // Event loop
    reader.Restart(); // Restarting event loop

    Int_t countSelectedJets = 0;
    //Int_t countSelectedEvents = 0;
    //Int_t jetTotalNum = 0; 

    while (reader.Next()) {
        
        Int_t currEventIndex = reader.GetCurrentEntry();
        //if (eventMultVec[currEventIndex] < N_cutoff) {continue;} // Applying the N_ch cut for each event (for event multiplicity)
        //countSelectedEvents++;

        // Loop through daughter branches (jets)
        for (Int_t i = 0; i < dauPtBranch->size(); i++) {  

            if (multVect[currEventIndex][i] < N_cutoff) {continue;}
            countSelectedJets++;

            // Vector for each jet with components pT, eta, phi
            TVector3 jet;
            
            jet.SetPtEtaPhi(calculateJetPt((*dauPtBranch)[i], (*dauPhiBranch)[i]),  
                            calculateJetEta((*dauPtBranch)[i], (*dauEtaBranch)[i], (*dauPhiBranch)[i]),
                            calculateJetPhi((*dauPtBranch)[i], (*dauPhiBranch)[i])); 

            std::vector<TVector3> particlesVec; // Vector to hold the particles (and their respective TVector)

            // Loop through particles within a jet
            for (Int_t j = 0; j < (*dauPtBranch)[i].size(); j++) {

                // Checking if particle is charged
                if ((*dauChgBranch)[i][j] == 0) {continue;}

                TVector3 particle;

                particle.SetPtEtaPhi((*dauPtBranch)[i][j],
                                     (*dauEtaBranch)[i][j],
                                     (*dauPhiBranch)[i][j]);

                particlesVec.push_back(particle);
            }

            for (Int_t j = 0; j < particlesVec.size(); j++) {hist.Fill(etaWRTJet(jet, particlesVec[j]));} // Filling histogram
        }
    } 

    // ***** NORMALIZATION *****
    Float_t binWidth = 12.0/100.0;
    hist.Scale(1.0/(countSelectedJets * binWidth));

    // ***** HISTOGRAM CUSTOMIZATION *****

    hist.SetLineColor(colorVal);
    hist.SetLineWidth(8);

/*
    hist.SetMarkerStyle(21);
    hist.SetMarkerSize(0.7);
    hist.SetMarkerColor(colorVal); */

    hist.SetTitleFont(132, "T");
    hist.SetTitleFont(132, "X");
    hist.SetTitleFont(132, "Y");

    /*std::cout << "The total number of entries for " << legendLabel << " is " << hist.GetEntries() << std::endl;
    //std::cout << "There are " << countSelectedEvents << " events greater than the 99th percentile." << std::endl;
    std::cout << "The total number of jets in the 99th percentile is " << countSelectedJets << std::endl;
    //std::cout << "The total number of jets (in all events) is " << jetTotalNum << std::endl;*/
    std::cout << std::endl;
    
    f->Close();
    delete f;
    return hist;
}

void jetFrameEtaDist() {

    // Creating a histogram array
    TH1F histarr[6];

    // Calling createHist function for data files
    histarr[0] = createJetFrameEtaHist("epem_WW_163GeV.root", "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)", 12, kRed);
    histarr[1] = createJetFrameEtaHist("epem_ZH_240GeV.root", "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)", 13, kAzure);
    histarr[2] = createJetFrameEtaHist("epem_ttbar_365GeV.root", "e^{+}e^{#minus} #rightarrow t#bar{t} (365 GeV)", 15, kBlack);
    histarr[3] = createJetFrameEtaHist("LEP1_1.root", "LEP1_1", 12, kGreen+1);
    histarr[4] = createJetFrameEtaHist("mupmum_3000GeV.root", "#mu^{+}#mu^{#minus} (3,000 GeV)", 29, kMagenta);
    histarr[5] = createJetFrameEtaHist("mupmum_10000GeV.root", "#mu^{+}#mu^{#minus} (10,000 GeV)", 52, kOrange+1); 

    // Creating a histogram stack and a legend
    THStack *hstack = new THStack ("hstack", "");
    //THStack *hstack = new THStack ("hstack", "#eta* Distribution for High N_{ch}^{jet};#eta*; Entries");

    TLegend *l = new TLegend(0.6, 0.6, 0.9, 0.9); // Creating legend pointer
    l->SetBorderSize(0); // Remove the border
    l->SetFillStyle(0);
    l->SetTextFont(132);
    
    // Looping through all histograms and adding it to hstack 
    for (Int_t i = 0; i < 6; i++) {
        hstack->Add(&histarr[i]);
        l->AddEntry(&histarr[i], histarr[i].GetName(), "l");
    }
   
    // Creating canvas
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 1000, 1000);
    //gStyle->SetTitleFontSize(0.05);
    c1->cd();
    //gPad->SetGrid();
    //gStyle->SetGridColor(kGray);
    //gStyle->SetGridStyle(2);

    // ***** HISTOGRAM CUSTOMIZATION *****
    hstack->Draw("nostack hist");
    
    hstack->GetXaxis()->SetTitle("#eta*");
    hstack->GetXaxis()->SetTitleFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetLabelFont(132);
    hstack->GetXaxis()->SetTitleOffset(0.7);
    hstack->GetXaxis()->SetTitleSize(0.07);
    hstack->GetXaxis()->SetLabelSize(0.04);

    hstack->GetYaxis()->SetTitle("#frac{dN_{jet}^{entries}}{d#Delta#eta*}");
    hstack->GetYaxis()->SetTitleFont(132);
    hstack->GetYaxis()->SetLabelFont(132);
    hstack->GetYaxis()->SetTitleOffset(1);
    hstack->GetYaxis()->SetTitleSize(0.06);
    hstack->GetYaxis()->SetLabelSize(0.04);

    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.01);
    gPad->SetBottomMargin(0.13);
    gPad->SetTopMargin(0.01);

    gStyle->SetCanvasBorderSize(3);
    gStyle->SetPadBorderSize(0);
    //gPad->SetLogy();

    /*
    l->SetBorderSize(0); // Remove the border
    l->SetFillStyle(0);
    l->SetTextFont(132);
    l->Draw();

    gPad->SetLogy(); */

    // Saving canvas c1 to pdf file
    c1->Print("jetFrameEtaDist7.root"); 

    // Deallocating memory
    delete hstack;
    delete l;
    delete c1; 
}