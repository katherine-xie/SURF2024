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
#include "SURF_FUNCTIONS/coordinateTools.h"

R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);

// Global variables for nextEvent() function
std::string title = "e^{+}e^{#minus} #rightarrow Z^{0}H (240 GeV)";
std::string fullPath = "/Users/katherinexie/SURF2024/futureCollidersData/epem_ZH_240GeV.root";
int currentEvtIndx;
float currentPtCut; // Represents pT cutoff

void EventDisplayHorizontal_Black(int eventIndx = 54321, 
		              float ptCut = 0, 
		              bool doWTA = false,
		              bool verbose = 0) {

  currentEvtIndx = eventIndx;
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

  //std::cout << (pPt->at(0)).at(0) << " " << (pEta->at(0)).at(0) << " " << (pPhi->at(0)).at(0) << " " << (pChg->at(0)).at(0) << std::endl;

  TCanvas *c = new TCanvas("c", "Event Display", 10000, 2000);  
  c->SetFillColor(kBlack);
  c->Divide(4, 1, 0, 0, 0);

  // Labeling the different views on canvas c
  // NOTE: Due to the rotation of the helix axis horizontally, the different views will also shift

  // Top View (Azimuthal View) -> Front View 
  TCanvas *c1 = (TCanvas*)c->GetPad(1); //new TCanvas("AzimuthalView", "AzimuthalView", 600, 600);
  c1->SetFillColor(kBlack);  
  TView *view = TView::CreateView(1);
  view->SetRange(-1,-1,-1,1,1,1);
  view->TopView(c1); // Becomes the front view

  // Side View -> Azimuthal View
  TCanvas *c2 = (TCanvas*)c->GetPad(2); 
  c2->SetFillColor(kBlack);  
  TView *view2 = TView::CreateView(1);
  view2->SetRange(-1,-1,-1,1,1,1);
  view2->SideView(c2); // Becomes the azimuthal view
  
  // Front View -> Side View
  TCanvas *c3 = (TCanvas*)c->GetPad(3); 
  c3->SetFillColor(kBlack);  
  TView *view3 = TView::CreateView(1);
  view3->SetRange(-1,-1,-1,1,1,1);
  view3->FrontView(c3); // Becomes the side view

  // Perspective View (stays the same)
  TCanvas *c4 = (TCanvas*)c->GetPad(4); 
  c4->SetFillColor(kBlack);  
  TView *view4 = TView::CreateView(1);
  view4->SetRange(-1,-1,-1,1,1,1);

  gPad->SetLeftMargin(0);

/*
  gPad->SetLeftMargin(0.05);
  gPad->SetRightMargin(0.05);
  gPad->SetBottomMargin(0.05);
  gPad->SetTopMargin(0.05); */

  // Canvas for the interactive viewer
  TCanvas *cA = new TCanvas("cA","Interactive Viewer", 5000, 5000); 
  cA->SetFillColor(kBlack);  
  TView *viewA = TView::CreateView(1);
  viewA->SetRange(-2, -2, -2, 2, 2, 2);

/*
 // Canvas for the side by side
  TCanvas *cSide = new TCanvas("cA","Interactive Viewer", 5000, 1000); 
  cSide->SetFillColor(kBlack);  
  cSide->Divide(1, 2);

  // Top View (Azimuthal View) -> Front View 
  TCanvas *cSide1 = (TCanvas*)cSide->GetPad(1); 
  c1->SetFillColor(kBlack);  
  TView *view = TView::CreateView(1);
  view->SetRange(-1,-1,-1,1,1,1);
  view->TopView(cSide1); // Becomes the front view

  // Side View -> Azimuthal View
  TCanvas *cSide2 = (TCanvas*)cSide->GetPad(2); 
  c2->SetFillColor(kBlack);  
  TView *view2 = TView::CreateView(1);
  view2->SetRange(-1,-1,-1,1,1,1);
  view2->SideView(c2); // Becomes the azimuthal view*/

  // Create XYZ axes as lines on cA canvas
  cA->cd();
  TPolyLine3D *axes[3];
  
  for (int i = 0; i < 3; i++) {
    axes[i] = new TPolyLine3D(2);
    //axes[i]->SetLineColor(kGray+1);
    axes[i]->SetLineWidth(1);
    axes[i]->SetLineStyle(9);
  } 

  // Z-Axis Line
  axes[0]->SetPoint(0, -10, 0, 0);
  axes[0]->SetPoint(1, 10, 0, 0);
  axes[0]->SetLineColor(kGray+1);
  axes[0]->Draw();
  
  // X-Axis Line
  axes[1]->SetPoint(0, 0, -10, 0);
  axes[1]->SetPoint(1, 0, 10, 0);
  axes[1]->SetLineColor(kGray+1);
  axes[1]->Draw(); 

  // Y-Axis Line
  axes[2]->SetPoint(0, 0, 0, -10);
  axes[2]->SetPoint(1, 0, 0, 10);
  axes[2]->SetLineColor(kGray+1);
  axes[2]->Draw();  

  // Create labels for the axes
  TLatex *axesLabels[3];
  
  for (int i = 0; i < 3; i++) {
    axesLabels[i] = new TLatex();
    //axesLabels[i]->SetTextColor(kGray+1);
    axesLabels[i]->SetTextSize(0.04);
    axesLabels[i]->SetTextFont(12);
  }

  // Helix array to store the jet tracks
  THelix *helix[1000]; 

  // Draw charged particles
  int nHelix = 0;
  int count = 0;
  for (int j = 0; j < pPt->size(); j++) {

  // std::cout << pPt->size() << std::endl;
    for(int i = 0; i < (*pPt)[j].size(); i++) {

      // std::cout << pPt->at(j).size() << std::endl;
      if ((*pChg)[j][i] == 0) {continue;}
      if ((*pPt)[j][i] < ptCut) {continue;} 

      // Jet tracks based on the particle type
      count++;
      int trackColor = 1;

      if (j>0) {trackColor = 1;} // Assume pion
      if ((*pPID)[j][i] == TMath::Abs(2212)) {trackColor = 2;} // proton
      if ((*pPID)[j][i] == TMath::Abs(321)) {trackColor = 0;} // kaon
      if ((*pPID)[j][i] == TMath::Abs(11)) {trackColor = 5;} // electron
      if ((*pPID)[j][i] == TMath::Abs(13)) {trackColor = 4;} // muon

      float phi = (*pPhi)[j][i];
      float pt = (*pPt)[j][i];

      float px = pt*TMath::Cos(phi);
      float py = pt*TMath::Sin(phi);
      float pz = pt*TMath::SinH((pEta->at(j)).at(i));

      helix[nHelix] = new THelix(0, 0, 0, px, py, pz, (*pChg)[j][i] * 0.01);
      helix[nHelix]->SetAxis(1, 0, 0);
      formatHelixBlackCanvas(helix[nHelix], pz, pt, trackColor, doWTA);

      c1->cd();
      helix[nHelix]->Draw();

      c2->cd();
      helix[nHelix]->Draw();

      c3->cd();
      helix[nHelix]->Draw();

      c4->cd();
      helix[nHelix]->Draw();
      
      cA->cd();
      helix[nHelix]->Draw();
      nHelix++;
    }
  }

  TLatex *l1 = new TLatex(-0.80,0.80, "Front View"); 
  TLatex *l2 = new TLatex(-0.80,0.80, "Azimuthal View"); 
  TLatex *l3 = new TLatex(-0.80,0.80, "Side View");
  TLatex *l4 = new TLatex(-0.80,0.80, "Perspective View");

  // Labelling the different views for the four-way view

  // Azimuthal View -> Front View
  c1->cd();
  l1->SetTextSize(0.1);
  //l1->SetTextAlign(2);
  l1->SetTextColor(kWhite);
  l1->SetTextFont(12);
  l1->Draw();

  // Side View -> Azimuthal View
  c2->cd();
  l2->SetTextSize(0.1);
  //l2->SetTextAlign(2);
  l2->SetTextColor(kWhite);
  l2->SetTextFont(12);
  l2->Draw();

  // Front View -> Side View
  c3->cd();
  l3->SetTextSize(0.1);
  //l3->SetTextAlign(2);
  l3->SetTextColor(kWhite);
  l3->SetTextFont(12);
  l3->Draw();

  // Perspective View stays the same
  c4->cd();
  l4->SetTextSize(0.1);
  //l4->SetTextAlign(2);
  l4->SetTextColor(kWhite);
  l4->SetTextFont(12);
  l4->Draw(); 

  // Legend for the axis lines for canvas c (4-way view)
  c2->cd();
  TLegend *legAxes = new TLegend(0.05, 0.4, 0.4, 0.6);
  legAxes->SetBorderSize(0);
  legAxes->SetFillStyle(0);

  // Place holders for each legend entry
  // Note: Due to the rotation of the helix axis horizontally, the coordinate axes will rotate as well
  TH1D *holder1 = new TH1D("holder1", "holder1", 1, 0, 1);
  holder1->SetLineColor(kRed); // x axis -> z axis
  TH1D *holder2 = new TH1D("holder2", "holder2", 1, 0, 1);
  holder2->SetLineColor(kGreen); // y axis -> x axis
  TH1D *holder3 = new TH1D("holder3","holder3", 1, 0, 1);
  holder3->SetLineColor(kBlue); // z axis -> y axis

  /*
  // Labeling the axis legend 
  legAxes->AddEntry(holder2, "x-axis", "l");
  legAxes->AddEntry(holder3, "y-axis", "l");
  legAxes->AddEntry(holder1, "z-axis", "l");

  legAxes->SetTextColor(kWhite);
  legAxes->SetTextFont(12);
  legAxes->Draw("same"); */
  
  cA->cd();
  TLatex *fileLabel = new TLatex(0.2, 0.7, title.c_str());
  fileLabel->SetTextFont(12);
  fileLabel->SetTextSize(0.08);
  fileLabel->SetTextColor(kWhite);
  fileLabel->Draw();

  std::string s1 = "Event Index: ";
  s1.append(std::to_string(currentEvtIndx));
  TLatex *eventLabel = new TLatex(0.2, 0.6, s1.c_str());
  eventLabel->SetTextColor(kWhite);
  eventLabel->SetTextSize(0.04);
  eventLabel->SetTextFont(12);
  eventLabel->Draw("same"); 

  std::string s2 = "Total N_{ch}: ";
  s2.append(std::to_string(count));
  TLatex *nchLabel = new TLatex(0.2, 0.5, s2.c_str());
  nchLabel->SetTextColor(kWhite);
  nchLabel->SetTextSize(0.04);
  nchLabel->SetTextFont(12);
  nchLabel->Draw("same");

/*
  std::string s3 = "p_{T} #geq ";
  s3.append(std::to_string(currentPtCut));
  TLatex *pTCutLabel = new TLatex(0.2, 0.4, s3.c_str());
  pTCutLabel->SetTextColor(kWhite);
  pTCutLabel->SetTextSize(0.035);
  pTCutLabel->SetTextFont(12);
  pTCutLabel->Draw("same"); */

  // Legend for the particle types
  TLegend *leg1 = new TLegend(0.05, 0.3, 0.4, 0.6);
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
  leg1->SetTextSize(0.06);
  leg1->SetMargin(0.3);
  leg1->Draw("same");

  //cA->SaveAs("displayTest.pdf"); 
  //cA->SaveAs(Form("../plots/EventDisplay_%d.pdf",eventIndx));
  //cA->SaveAs(Form("../plots/EventDisplay_%d.png",eventIndx));
}
