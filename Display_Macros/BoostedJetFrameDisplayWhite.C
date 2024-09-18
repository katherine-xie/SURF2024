#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TAttLine.h"
#include "THelix.h"
#include "TView.h"
#include <string>
#include "TAttPad.h"
#include "TMath.h"
#include "TVector3.h"
#include "TView3D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"

#include <Pythia8/Pythia.h> 
#include <Math/Vector4D.h> // Header for the ROOT::Math::LorentzVector class
#include <Math/Boost.h> // Header for the ROOT::Math::Boost class
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double>> LorentzVector;

#include <iostream>
#include <string>
#include <vector>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
#include "SURF_FUNCTIONS/coordinateTools.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Dataformat
//#include "include/smartJetName.h"
//#include "include/particleData.h"
//#include "include/jetData.h"
//#include "include/boostedEvtData.h"
//#include "include/eventData.h"

// Global variables 
std::string title = "#mu^{+}#mu^{#minus} (10,000 GeV)";
std::string fullPath = "/Users/katherinexie/SURF2024/futureCollidersData/mupmum_10000GeV.root";
int currentEvtIndx;
int currentJetIndx;
float currentPtCut; // Represents pT cutoff

Pythia8::Pythia pythia; // Initialize Pythia
Pythia8::ParticleData &particleData = pythia.particleData; // Access the particle data table

void BoostedJetFrameDisplayWhite(int eventIndx = 76784, 
                            int jetIndx = 0,
		                    float ptCut = 0, 
		                    bool doWTA = false,
		                    bool verbose = 0) {

    currentEvtIndx = eventIndx;
    currentJetIndx = jetIndx;
    currentPtCut = ptCut;

    TFile *f = TFile::Open(fullPath.c_str(), "read");
    TTreeReader reader("trackTree");

    // Setup branches (use TTreeReader)
    TTreeReaderValue<vector<vector<float>>> pPt(reader, "genDau_pt");
    TTreeReaderValue<vector<vector<float>>> pEta(reader, "genDau_eta");
    TTreeReaderValue<vector<vector<float>>> pPhi(reader, "genDau_phi");
    TTreeReaderValue<vector<vector<int>>> pChg(reader, "genDau_chg");
    TTreeReaderValue<vector<vector<int>>> pPID(reader, "genDau_pid");

    // Take the event of interest
    reader.SetEntry(eventIndx);

    // Creating canvas for a four-way view
    TCanvas *c = new TCanvas("c", "Boosted Jet Display", 1000, 1000);
    c->Divide(2,2);
    c->SetFillColor(0);

    // Top View (AKA Azimuthal View)
    TCanvas *c1 = (TCanvas*)c->GetPad(1);
    c1->SetFillColor(0);  
    TView *view = TView::CreateView(1);
    view->SetRange(-1,-1,-1,1,1,1);
    view->TopView(c1);

    // Side View
    TCanvas *c2 = (TCanvas*)c->GetPad(2); 
    c2->SetFillColor(0);  
    TView *view2 = TView::CreateView(1);
    view2->SetRange(-1,-1,-1,1,1,1);
    view2->SideView(c2);
    
    // Front View
    TCanvas *c3 = (TCanvas*)c->GetPad(3); 
    c3->SetFillColor(0);  
    TView *view3 = TView::CreateView(1);
    view3->SetRange(-1,-1,-1,1,1,1);
    view3->FrontView(c3);
    
    // Perspective View
    TCanvas *c4 = (TCanvas*)c->GetPad(4); 
    c4->SetFillColor(0);  
    TView *view4 = TView::CreateView(1);
    view4->SetRange(-1,-1,-1,1,1,1);

    // Creating a canvas for the interactive viewer
    TCanvas *cA = new TCanvas("cA","Interactive Viewer", 1000, 1000);
    cA->SetFillColor(0);  
    TView *viewA = TView::CreateView(1);
    viewA->SetRange(-1,-1,-1,1,1,1);
    

/*
    // Create XYZ axes as lines
    TPolyLine3D *axes[3];
    for (int i = 0; i < 3; i ++) {
        axes[i] = new TPolyLine3D(2);
        axes[i]->SetLineColor(kGray+1);
        axes[i]->SetLineWidth(1);
        axes[i]->SetLineStyle(9);
    } 
    axes[0]->SetPoint(0, -10, 0, 0);
    axes[0]->SetPoint(1, 10, 0, 0);
    axes[0]->Draw();
    axes[1]->SetPoint(0, 0, -10, 0);
    axes[1]->SetPoint(1, 0, 10, 0);
    axes[1]->Draw();
    axes[2]->SetPoint(0, 0, 0, -10);
    axes[2]->SetPoint(1, 0, 0, 10);
    axes[2]->Draw(); 
*/

    // Drawing the jet axis (for visualization purposes) using lines
    TPolyLine3D *line = new TPolyLine3D(2);
    line->SetPoint(1, 0, 0, -5); // First point
    line->SetPoint(2, 0, 0, 5); // Second point
    line->SetLineColor(kRed); 

    TPolyLine3D *unitLine = new TPolyLine3D(2);
    unitLine->SetPoint(1, 0, 0, -0.5); 
    unitLine->SetPoint(2, 0, 0, 1); 
    unitLine->SetLineColor(kRed); 

    // Labeling the different views on the four-panel canvas and adding the jet axis
    c1->cd();
    TLatex *l1 = new TLatex(-0.97,0.90,"Azimuthal View");
    l1->SetTextColor(kBlack);
    l1->SetTextFont(12);
    l1->Draw();
    unitLine->Draw();

    c2->cd();
    TLatex *l2 = new TLatex(-0.97,0.90,"Side View");
    l2->SetTextColor(kBlack);
    l2->SetTextFont(12);
    l2->Draw();
    unitLine->Draw();

    c3->cd();
    TLatex *l3 = new TLatex(-0.97,0.90,"Front View");
    l3->SetTextColor(kBlack);
    l3->SetTextFont(12);
    l3->Draw();
    unitLine->Draw();

    c4->cd();
    TLatex *l4 = new TLatex(-0.97,0.90,"Perspective View");
    l4->SetTextColor(kBlack);
    l4->SetTextFont(12);
    l4->Draw(); 
    unitLine->Draw();
    
    cA->cd();
    line->Draw();

    // Calculating the jet axis vector (using pT, eta, phi for the rotation)
    float jetPt = calculateJetPt(pPt->at(currentJetIndx), pPhi->at(currentJetIndx));
    float jetEta = calculateJetEta(pPt->at(currentJetIndx), pEta->at(currentJetIndx), pPhi->at(currentJetIndx));
    float jetPhi = calculateJetPhi(pPt->at(currentJetIndx), pPhi->at(currentJetIndx));

    // Lab frame momenta 
    double jetPx = calculateJetPx(pPt->at(currentJetIndx), pPhi->at(currentJetIndx));
    double jetPy = calculateJetPy(pPt->at(currentJetIndx), pPhi->at(currentJetIndx));
    double jetPz = calculateJetPz(pPt->at(currentJetIndx), pEta->at(currentJetIndx));

    // Calculating the jet momentum in the JET FRAME
    double jetP = sqrt((jetPx*jetPx) + (jetPy*jetPy) + (jetPz*jetPz));

    // Initialize a jet vector (using pt, eta, phi) for the rotation
    TVector3 jetVectorRotate;
    jetVectorRotate.SetPtEtaPhi(jetPt, jetEta, jetPhi);   

    // Initializing the TLorentzVector for the jet
    //TLorentzVector jetTLorentzVector;
    
    // Looping through the jet to find the total jet mass
    double ESum = 0;
    for (Int_t i = 0; i < (*pPt)[currentJetIndx].size(); i++) {
        ESum += calculateEnergy(calculatePx((*pPt)[currentJetIndx][i], (*pPhi)[currentJetIndx][i]),
                                calculatePy((*pPt)[currentJetIndx][i], (*pPhi)[currentJetIndx][i]),
                                calculatePz((*pPt)[currentJetIndx][i], (*pEta)[currentJetIndx][i]),
                                particleData.m0((*pPID)[currentJetIndx][i]));
    }

    // Calculating the boost vector (in jet frame)
    ROOT::Math::Boost boost(0.0, 0.0, -jetP/ESum);

    int count = 0; // Counter to count N_ch
    THelix *helix[1000]; // Creating a helix array to store the jet tracks

    // Main for-loop that loops through particles at the specified jet index
    for (int i = 0; i < (*pPt)[currentJetIndx].size(); i++) {
        
        if ((*pChg)[currentJetIndx][i] == 0) {continue;} // Check if particle is charged
        if ((*pPt)[currentJetIndx][i] < ptCut) {continue;} // Check if pT is within limits
        count++; // Update counter

        // Determine track color based on particle type (PID)
        int trackColor = -1; // Assume pion
        if (currentJetIndx > 0) {trackColor = -1;} // (I'm not sure what this line is for)
        if ((*pPID)[currentJetIndx][i] == TMath::Abs(2212)) {trackColor = 2;} // proton
        if ((*pPID)[currentJetIndx][i] == TMath::Abs(321)) {trackColor = 0;} // kaon
        if ((*pPID)[currentJetIndx][i] == TMath::Abs(11)) {trackColor = 5;} // electron
        if ((*pPID)[currentJetIndx][i] == TMath::Abs(13)) {trackColor = 4;} // muon

        // ***** APPLYING ROTATION HERE *****
        // Initialize a vector for the individual particle
        TVector3 particleVector;
        particleVector.SetPtEtaPhi((*pPt)[currentJetIndx][i],
                                   (*pEta)[currentJetIndx][i], 
                                   (*pPhi)[currentJetIndx][i]);
        
        // Calculate pt, eta, phi WRT the jet axis
        float pt = ptWRTJet(jetVectorRotate, particleVector);
        float phi = phiWRTJet(jetVectorRotate, particleVector);
        float eta = etaWRTJet(jetVectorRotate, particleVector);

        // Find the rotated px, py, pz components and finding E
        float px = pt*TMath::Cos(phi);
        float py = pt*TMath::Sin(phi);
        float pz = pt*TMath::SinH(eta);
        float m0 = particleData.m0((*pPID)[currentJetIndx][i]);
        float E = calculateEnergy(px, py, pz, m0);

        // ***** APPLYING BOOST HERE *****
       // LorentzVector pLorentzVec(px, py, pz, E);
        LorentzVector jetFrameParticle(px, py, pz, E);
        LorentzVector boostedParticle = boost(jetFrameParticle); 

        // Creating and formatting the helix track 
        helix[i] = new THelix(0, 0, 0, 
                              boostedParticle.Px(), 
                              boostedParticle.Py(), 
                              boostedParticle.Pz(), 
                              (*pChg)[currentJetIndx][i] * 0.01);

        // Recalculating pT for the formatHelix function
        float newPt = calculatePt(boostedParticle.Px(), boostedParticle.Py());
        formatHelixWhiteCanvas(helix[i], boostedParticle.Pz(), newPt, trackColor, doWTA);

        // Drawing the track on all canvases
        c1->cd();
        helix[i]->Draw();
        c2->cd();
        helix[i]->Draw();
        c3->cd();
        helix[i]->Draw();
        c4->cd();
        helix[i]->Draw();
        cA->cd();
        helix[i]->Draw();
    }


    // Legend for the axis lines in the four-view canvas
    c2->cd();
    TLegend *legAxes = new TLegend(0.05, 0.4, 0.4, 0.6);
    legAxes->SetBorderSize(0);
    legAxes->SetFillStyle(0);

    // Place holders for each legend entry
    TH1D *holder1 = new TH1D("holder1", "holder1", 1, 0, 1);
    holder1->SetLineColor(kRed);
    TH1D *holder2 = new TH1D("holder2", "holder2", 1, 0, 1);
    holder2->SetLineColor(kGreen);
    TH1D *holder3 = new TH1D("holder3","holder3", 1, 0, 1);
    holder3->SetLineColor(kBlue);

    // Labeling the axis legend
    legAxes->AddEntry(holder1, "x-axis", "l");
    legAxes->AddEntry(holder2, "y-axis", "l");
    legAxes->AddEntry(holder3, "z-axis", "l");

    legAxes->SetTextColor(kBlack);
    legAxes->SetTextFont(12);
    legAxes->Draw("same");
  
    // Labeling canvas cA
    cA->cd();
    
    // Add the title to canvas cA
    TLatex *fileLabel = new TLatex(0.2, 0.7, title.c_str());
    fileLabel->SetTextFont(12);
    fileLabel->SetTextColor(kBlack);
    fileLabel->Draw();

    // Add the event index to cA
    std::string s1 = "Event Index: ";
    s1.append(std::to_string(currentEvtIndx));
    TLatex *eventLabel = new TLatex(0.2, 0.6, s1.c_str());
    eventLabel->SetTextColor(kBlack);
    eventLabel->SetTextSize(0.035);
    eventLabel->SetTextFont(12);
    eventLabel->Draw("same");

    // Add the jet index to cA
    std::string s2 = "Jet Index: ";
    s2.append(std::to_string(currentJetIndx));
    TLatex *jetLabel = new TLatex(0.2, 0.5, s2.c_str());
    jetLabel->SetTextColor(kBlack);
    jetLabel->SetTextSize(0.035);
    jetLabel->SetTextFont(12);
    jetLabel->Draw("same");

    // Add the N_ch label to cA
    std::string s3 = "Jet N_{ch}: ";
    s3.append(std::to_string(count));
    TLatex *nchLabel = new TLatex(0.2, 0.4, s3.c_str());
    nchLabel->SetTextColor(kBlack);
    nchLabel->SetTextSize(0.035);
    nchLabel->SetTextFont(12);
    nchLabel->Draw("same");

/*
    // Adding pT cut label
    std::string s4 = "p_{T} #geq ";
    s4.append(std::to_string(currentPtCut));
    TLatex *pTCutLabel = new TLatex(0.2, 0.3, s4.c_str());
    pTCutLabel->SetTextColor(kWhite);
    pTCutLabel->SetTextSize(0.035);
    pTCutLabel->SetTextFont(12);
    pTCutLabel->Draw("same"); */

  // Legend for the particle types
  TLegend *leg1 = new TLegend(0.05, 0.3, 0.4, 0.6);
  leg1->SetBorderSize(0);
  leg1->SetFillStyle(0);

  TH1D * dummy1 = new TH1D("dummy1","dummy1",1,0,1);
  dummy1->SetLineColor(kBlack);
  dummy1->SetLineWidth(2);

  TH1D * dummy2 = new TH1D("dummy2","dummy2",1,0,1);
  dummy2->SetLineColor(kSpring);
  dummy2->SetLineWidth(2);

  TH1D * dummy3 = new TH1D("dummy3","dummy3",1,0,1);
  dummy3->SetLineColor(kMagenta);
  dummy3->SetLineWidth(2);

  TH1D * dummy4 = new TH1D("dummy4","dummy4",1,0,1);
  dummy4->SetLineColor(kAzure-3);
  dummy4->SetLineWidth(3);

  TH1D * dummy5 = new TH1D("dummy5","dummy5",1,0,1);
  dummy5->SetLineColor(kOrange+7); 
  dummy5->SetLineWidth(2);

  // Labeling hadrons
  leg1->AddEntry(dummy1,"#pi^{#pm}","l");
  leg1->AddEntry(dummy2,"K^{#pm}","l");
  leg1->AddEntry(dummy3,"p + #bar{p}","l");
  leg1->AddEntry(dummy4,"e^{#minus}","l");
  leg1->AddEntry(dummy5,"#mu^{#minus}","l");

    leg1->SetTextColor(kBlack);
    leg1->SetTextFont(12);
    leg1->Draw("same");

    //cA->SaveAs(Form("../plots/EventDsplay_%d.pdf",eventIndx));
    //cA->SaveAs(Form("../plots/EventDisplay_%d.png",eventIndx));
}