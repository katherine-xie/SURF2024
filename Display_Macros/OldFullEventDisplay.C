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

#include <iostream>
#include <string>
#include <vector>

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Dataformat
//#include "include/smartJetName.h"
//#include "include/particleData.h"
//#include "include/jetData.h"
//#include "include/boostedEvtData.h"
//#include "include/eventData.h"

// Global variables for nextEvent() function
std::string title = "e^{+}e^{#minus} #rightarrow W^{+}W^{#minus} (163 GeV)";
std::string fullPath = "/Users/katherinexie/SURF2024/futureCollidersData/epem_WW_163GeV.root";
int currentEvtIndx;
float currentPtCut; // Represents pT cutoff

void OldFullEventDisplay(int eventIndx = 236, 
		                  float ptCut = 0, 
		                  bool doWTA = false,
		                  bool verbose = 0) {

  currentEvtIndx = eventIndx;
  currentPtCut = ptCut;

  TFile *f = TFile::Open(fullPath.c_str(), "read");
  TTreeReader reader("trackTree");

  // Setup branches for the jets
  TTreeReaderValue<vector<vector<float>>> pPt(reader, "genDau_pt");
  TTreeReaderValue<vector<vector<float>>> pEta(reader, "genDau_eta");
  TTreeReaderValue<vector<vector<float>>> pPhi(reader, "genDau_phi");
  TTreeReaderValue<vector<vector<int>>> pChg(reader, "genDau_chg");
  TTreeReaderValue<vector<vector<int>>> pPID(reader, "genDau_pid");

  // Setup branches for other particles
  TTreeReaderValue<vector<float>> pxBranch(reader, "px");
  TTreeReaderValue<vector<float>> pyBranch(reader, "py");
  TTreeReaderValue<vector<float>> pzBranch(reader, "pz");
  TTreeReaderValue<vector<int>> chgBranch(reader, "chg");
  TTreeReaderValue<vector<int>> pidBranch(reader, "pid");

  // Take the event of interest
  reader.SetEntry(eventIndx);

  // Creating canvas for the four views
  TCanvas *cFull = new TCanvas("cFull", "Event Display", 1000, 1000);
  cFull->Divide(2,2);
  cFull->SetFillColor(kBlack);

  // Top View (Azimuthal View)
  TCanvas *c1 = (TCanvas*)cFull->GetPad(1); 
  c1->SetFillColor(kBlack);  
  TView *view = TView::CreateView(1);
  view->SetRange(-1,-1,-1,1,1,1);
  view->TopView(c1);

  // Side View
  TCanvas *c2 = (TCanvas*)cFull->GetPad(2); 
  c2->SetFillColor(kBlack);  
  TView *view2 = TView::CreateView(1);
  view2->SetRange(-1,-1,-1,1,1,1);
  view2->SideView(c2);
  
  // Front View
  TCanvas *c3 = (TCanvas*)cFull->GetPad(3); 
  c3->SetFillColor(kBlack);  
  TView *view3 = TView::CreateView(1);
  view3->SetRange(-1,-1,-1,1,1,1);
  view3->FrontView(c3);
  
  // Perspective View
  TCanvas *c4 = (TCanvas*)cFull->GetPad(4); 
  c4->SetFillColor(kBlack);  
  TView *view4 = TView::CreateView(1);
  view4->SetRange(-1,-1,-1,1,1,1);
  view4->ZoomView(c4, 10);  

  /*TLegend *leg = new TLegend(0.1,0.1,0.9,0.9);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextColor(kWhite);*/

  // Canvas for the interactive viewer
  TCanvas *cFullInteractive= new TCanvas("cA","Interactive Viewer", 1000, 1000); 
  cFullInteractive->SetFillColor(kBlack);  
  TView *viewFullInteractive = TView::CreateView(1);
  viewFullInteractive->SetRange(-1,-1,-1,1,1,1);

  // Create XYZ axes as lines
  cFullInteractive->cd();
  TPolyLine3D *axes[3];
  
  for (int i = 0; i < 3; i++) {
    axes[i] = new TPolyLine3D(2);
    axes[i]->SetLineColor(kGray+1);
    axes[i]->SetLineWidth(1);
    axes[i]->SetLineStyle(9);
  } 

  // X Axis Line
  axes[0]->SetPoint(0, -10, 0, 0);
  axes[0]->SetPoint(1, 10, 0, 0);
  axes[0]->Draw();
  
  // Y Axis Line
  axes[1]->SetPoint(0, 0, -10, 0);
  axes[1]->SetPoint(1, 0, 10, 0);
  axes[1]->Draw(); 

  // Z Axis Line
  axes[2]->SetPoint(0, 0, 0, -10);
  axes[2]->SetPoint(1, 0, 0, 10);
  axes[2]->Draw();  

  // Create labels for the axes
  TLatex *axesLabels[3];
  
  for (int i = 0; i < 3; i++) {
    axesLabels[i] = new TLatex();
    axesLabels[i]->SetTextColor(kGray+1);
    axesLabels[i]->SetTextSize(0.04);
    axesLabels[i]->SetTextFont(12);
  }
 
  // X Label
  axesLabels[0]->SetText(-0.6, -0.3, "x");
  axesLabels[0]->Draw("same");

  // Y Label
  axesLabels[1]->SetText(0.6, -0.3, "y");
  axesLabels[1]->Draw("same");

  // Z Label
  axesLabels[2]->SetText(0, 0.6, "z");
  axesLabels[2]->Draw("same"); 

  // Helix arrays to store the tracks
  THelix *jetHelix[1000];
  THelix *particleHelix[1000];

  /*
  if(!doWTA) {
    helix[999] = new THelix(0,0,0,thrust.Px(),thrust.Py(),thrust.Pz(),0.00000001);
    helix[999]->SetRange(-2,2);
    helix[999]->SetLineColor(8);
    helix[999]->SetLineWidth(3);
    c1->cd();
    helix[999]->Draw();
    c2->cd();
    helix[999]->Draw();
    c3->cd();
    helix[999]->Draw();
    
    helix[998] = new THelix(0,0,0,wta1.Px(),wta1.Py(),wta1.Pz(),0.00000001);
    if(wta1.Pz()<0) helix[998]->SetRange(-1,0);
    if(wta1.Pz()>=0) helix[998]->SetRange(0,1);
    helix[998]->SetLineColor(38);
    helix[998]->SetLineWidth(3);
    c1->cd();
    helix[998]->Draw();
    c2->cd();
    helix[998]->Draw();
    c3->cd();
    helix[998]->Draw();
    
    helix[997] = new THelix(0,0,0,wta2.Px(),wta2.Py(),wta2.Pz(),0.000001);
    if(wta2.Pz()<0) helix[997]->SetRange(-1,0);
    if(wta2.Pz()>=0) helix[997]->SetRange(0,1);
    helix[997]->SetLineColor(38);
    helix[997]->SetLineWidth(3);
    c1->cd();
    helix[997]->Draw();
    c2->cd();
    helix[997]->Draw();
    c3->cd();
    helix[997]->Draw();
    leg->AddEntry(helix[999],"ALEPH Archived Data Event Display","t");
    leg->AddEntry(helix[999],"Thrust Axis","l");
    leg->AddEntry(helix[998],"WTA Jet Axis 1","l");
    leg->AddEntry(helix[997],"WTA Jet Axis 2","l");
    
    helix[992] = new THelix(0,0,0,1,1,1,0);
    helix[993] = new THelix(0,0,0,1,1,1,0);
    helix[994] = new THelix(0,0,0,1,1,1,0);
    helix[995] = new THelix(0,0,0,1,1,1,0);
    helix[996] = new THelix(0,0,0,1,1,1,0);
    formatHelix(helix[993],1,1,1,0);
    leg->AddEntry(helix[993],"Tracks in Leading Jet (#Delta R<0.8)","l");
    formatHelix(helix[994],1,1,2,0);
    leg->AddEntry(helix[994],"Tracks in Subleading Jet (#Delta R<0.8)","l");
    formatHelix(helix[995],1,1,3,0);
    leg->AddEntry(helix[995],"Tracks in Third Jet (#Delta R<0.8)","l");
    formatHelix(helix[996],1,1,4,0);
    leg->AddEntry(helix[996],"Tracks in Fourth Jet (#Delta R<0.8)","l");
    formatHelix(helix[992],1,1,0,0);
    leg->AddEntry(helix[992],"Other Tracks","l");
  }
  */

  int nJetHelix = 0; // Index for the jet helix array
  int totalNcount = 0; // counter to keep track of all charged particlkes

  // Looping through daughter branches (jets) (pPt->size())
  //std::cout << "*********************** Particles In Jets ***********************" << std::endl;
  //std::cout << "Jet pT, eta, phi, px, py, pz" << std::endl;

  for (int j = 0; j < 1; j++) {
    for(int i = 0; i < (pPt->at(j).size()); i++) {

      // std::cout << pPt->at(j).size() << std::endl;
      if((pChg->at(j)).at(i) == 0) continue;
      if((pPt->at(j)).at(i) < ptCut) continue;

      // Setting jet track colors based on pid
      totalNcount++;
      int jetTrackColor = 1;
      if (j>0) {jetTrackColor = 1;} // Assume pion
      if ((pPID->at(j)).at(i) == TMath::Abs(2212)) {jetTrackColor = 2;} // proton
      if ((pPID->at(j)).at(i) == TMath::Abs(321)) {jetTrackColor = 0;} // kaon
      if ((pPID->at(j)).at(i) == TMath::Abs(11)) {jetTrackColor = 5;} // electron
      if ((pPID->at(j)).at(i) == TMath::Abs(13)) {jetTrackColor = 4;} // muon

      // Calculating the daughter momenta
      float jetPx, jetPy, jetPz, jetPt;
      float phi = (pPhi->at(j)).at(i);
      jetPt = (pPt->at(j)).at(i);
      jetPx = jetPt*TMath::Cos(phi);
      jetPy = jetPt*TMath::Sin(phi);
      jetPz = jetPt*TMath::SinH((pEta->at(j)).at(i));

      // if (i>15) continue;
      std::cout << "Jet " << j << ", " << "Particle " << i << ": " << jetPt << ", " << (pEta->at(j)).at(i) << ", " << (pPhi->at(j)).at(i) << ", " << jetPx << ", "  << jetPy << ", " << jetPz << std::endl; 

      /*
      if(i%4==0) helix[nHelix] = new THelix(0, 0, 0, 10, 10, 5, 1 * 1.5);
      if(i%4==1) helix[nHelix] = new THelix(0, 0, 0, 10, 10, -5, 1 * 1.5);
      if(i%4==2) helix[nHelix] = new THelix(0, 0, 0, 10, 10, 5, -1 * 1.5);
      if(i%4==3) helix[nHelix] = new THelix(0, 0, 0, 10, 10, -5, -1 * 1.5);

      if(i%4==0 || i%4==2) formatHelix(helix[nHelix], 5, 14.14, trackColor,doWTA);
      if(i%4==1 || i%4==3) formatHelix(helix[nHelix], -5, 14.14, trackColor,doWTA);
      */

      // Formatting helixes
      jetHelix[nJetHelix] = new THelix(0, 0, 0, jetPx, jetPy, jetPz, (pChg->at(j)).at(i) * 0.2);
      formatHelix(jetHelix[nJetHelix], jetPz, jetPt, jetTrackColor, doWTA);

      c1->cd();
      jetHelix[nJetHelix]->Draw("same");
      c2->cd();
      jetHelix[nJetHelix]->Draw("same");
      c3->cd();
      jetHelix[nJetHelix]->Draw("same");
      c4->cd();
      jetHelix[nJetHelix]->Draw("same");
      cFullInteractive->cd();
      jetHelix[nJetHelix]->Draw("same");
      nJetHelix++;
    }
  }

  
  // Looping through the particles not in jets
  std::cout << "******************** Particles Not In Jets ********************" << std::endl;
  std::cout << "pT, px, py, pz" << endl;
  
  for (int k = 0; k < 5; k++) {

    float particlePt = calculatePt(pxBranch->at(k), pyBranch->at(k)); // calculate pT
    if(chgBranch->at(k) == 0) continue;
    if(particlePt < ptCut) continue;
    
    totalNcount++;

    // Formatting the tracks based on particle type
    int trackColor = 1; // Assume pion
    if (pidBranch->at(k) == TMath::Abs(2212)) {trackColor = 2;} // proton
    if (pidBranch->at(k) == TMath::Abs(321)) {trackColor = 0;} // kaon
    if (pidBranch->at(k)== TMath::Abs(11)) {trackColor = 5;} // electron
    if (pidBranch->at(k) == TMath::Abs(13)) {trackColor = 4;} // muon

    std::cout << "Particle " << k << ": " << particlePt << ", " << pxBranch->at(k) << ", " << pyBranch->at(k) << ", " << pzBranch->at(k) << std::endl; 

    particleHelix[k] = new THelix(0, 0, 0, pxBranch->at(k), pyBranch->at(k), pzBranch->at(k), chgBranch->at(k) * 0.2);
    formatHelix(particleHelix[k], pzBranch->at(k), particlePt, trackColor, doWTA);

    c1->cd();
    particleHelix[k]->Draw("same");
    c2->cd();
    particleHelix[k]->Draw("same");
    c3->cd();
    particleHelix[k]->Draw("same");
    c4->cd();
    particleHelix[k]->Draw("same");
    cFullInteractive->cd();
    particleHelix[k]->Draw("same");
  } 

  // Labelling the different views on the c canvas
  c1->cd();
  TLatex *l1 = new TLatex(-0.97,0.90,"Azimuthal View");
  l1->SetTextColor(kWhite);
  l1->SetTextFont(12);
  l1->Draw();

  c2->cd();
  TLatex *l2 = new TLatex(-0.97,0.90,"Side View");
  l2->SetTextColor(kWhite);
  l2->SetTextFont(12);
  l2->Draw();

  c3->cd();
  TLatex *l3 = new TLatex(-0.97,0.90,"Front View");
  l3->SetTextColor(kWhite);
  l3->SetTextFont(12);
  l3->Draw();

  c4->cd();
  TLatex *l4 = new TLatex(-0.97,0.90,"Perspective View");
  l4->SetTextColor(kWhite);
  l4->SetTextFont(12);
  l4->Draw(); 
  
  // Labeling the cFullInteractive canvas
  cFullInteractive->cd();

  TLatex *fileLabel = new TLatex(0.2, 0.7, title.c_str());
  fileLabel->SetTextFont(12);
  fileLabel->SetTextColor(kWhite);
  fileLabel->Draw();

  std::string s1 = "Event Index: ";
  s1.append(std::to_string(currentEvtIndx));

  TLatex *eventLabel = new TLatex(0.2, 0.6, s1.c_str());
  eventLabel->SetTextColor(kWhite);
  eventLabel->SetTextSize(0.035);
  eventLabel->SetTextFont(12);
  eventLabel->Draw("same");

  std::string s2 = "Total N_{ch} : ";
  s2.append(std::to_string(totalNcount));
  TLatex *nchLabel = new TLatex(0.2, 0.5, s2.c_str());
  nchLabel->SetTextColor(kWhite);
  nchLabel->SetTextSize(0.035);
  nchLabel->SetTextFont(12);
  nchLabel->Draw("same");

  // Legend for the particle types
  TLegend *leg1 = new TLegend(0.05,0.4,0.4,0.6);
  leg1->SetBorderSize(0);
  leg1->SetFillStyle(0);

  TH1D * dummy1 = new TH1D("dummy1","dummy1",1,0,1);
  dummy1->SetLineColor(kWhite);
  TH1D * dummy2 = new TH1D("dummy2","dummy2",1,0,1);
  dummy2->SetLineColor(kGreen);
  TH1D * dummy3 = new TH1D("dummy3","dummy3",1,0,1);
  dummy3->SetLineColor(kMagenta);
  TH1D * dummy4 = new TH1D("dummy4","dummy4",1,0,1);
  dummy4->SetLineColor(kCyan-4);
  TH1D * dummy5 = new TH1D("dummy5","dummy5",1,0,1);
  dummy5->SetLineColor(kYellow); 

  // Labeling hadrons
  leg1->AddEntry(dummy1,"#pi^{#pm}","l");
  leg1->AddEntry(dummy2,"K^{#pm}","l");
  leg1->AddEntry(dummy3,"p + #bar{p}","l");
  leg1->AddEntry(dummy4,"e^{#minus}","l");
  leg1->AddEntry(dummy5,"#mu^{#minus}","l");

  leg1->SetTextColor(kWhite);
  leg1->SetTextFont(12);
  leg1->Draw("same");

  //cA->SaveAs(Form("../plots/EventDisplay_%d.pdf",eventIndx));
  //cA->SaveAs(Form("../plots/EventDisplay_%d.png",eventIndx));
}
