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

// Dataformat
//#include "include/smartJetName.h"
//#include "include/particleData.h"
//#include "include/jetData.h"
//#include "include/boostedEvtData.h"
//#include "include/eventData.h"

void formatHelix(THelix *h, float pz, float pt, int color, bool doWTA){
  
  // Setting line attributes
  if(color==0) h->SetLineColor(kGreen);
  if(color==1) h->SetLineColor(kBlack);
  if(color==2) h->SetLineColor(kMagenta);
  if(color==3) h->SetLineColor(kRed);
  if(color==4) h->SetLineColor(kYellow);
  if(color==5) h->SetLineColor(kCyan-4);
  if(color==-1) h->SetLineColor(kBlack);
  h->SetLineWidth(1);
  
  float rangeBound = 1.0;
  if(!doWTA && pt<1.0 && TMath::Abs(pz)<0.5) {rangeBound = 2 * TMath::Abs(pz);}
  if(!doWTA && pt<1.5 && TMath::Abs(pz)<0.5) {rangeBound = TMath::Abs(pz);}
  if(!doWTA && pt>1.5) {rangeBound = rangeBound*pz/pt*10;}
  if(!doWTA && pt>5) {rangeBound = 0.2;}

  h->SetRange(0, rangeBound);

  if(pz<0) {h->SetRange(-rangeBound, 0);}
  if(pz>0)  {h->SetRange(0, 1, kHelixZ);}
  else {h->SetRange(-1, 0, kHelixZ);}

  //h->SetRange(-2,2,kLabX);
  //h->SetRange(-2,2,kLabY);
}

// Global variables for nextEvent() function
std::string title = "#mu^{+}#mu^{#minus} (10000 GeV)";
std::string fullPath = "/Users/katherinexie/SURF2024/futureCollidersData/mupmum_10000GeV.root";
int currentEvtIndx;
float currentPtCut; // Represents pT cutoff

void EventDisplay(int eventIndx = 1234, 
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

  /* Draw Thrust and WTA axis
  TVector3 thrust = TVector3(0,0,0);
  thrust.SetMagThetaPhi(10,event.TTheta,event.TPhi);
  TVector3 wta1 = TVector3(0,0,0);
  if(WTAjet.nref>0) wta1.SetMagThetaPhi(10,2*TMath::ATan(TMath::Exp(-WTAjet.jteta[0])),WTAjet.jtphi[0]);
  TVector3 wta2 = TVector3(0,0,0);
  if(WTAjet.nref>1) wta2.SetMagThetaPhi(10,2*TMath::ATan(TMath::Exp(-WTAjet.jteta[1])),WTAjet.jtphi[1]);
  */

  //std::cout << (pPt->at(0)).at(0) << " " << (pEta->at(0)).at(0) << " " << (pPhi->at(0)).at(0) << " " << (pChg->at(0)).at(0) << std::endl;

  TCanvas *c = new TCanvas("c", "Event Display", 1000, 1000);  
  c->SetFillColor(0);
  c->Divide(2, 2);

  // Top View (Azimuthal View)
  TCanvas *c1 = (TCanvas*)c->GetPad(1); //new TCanvas("AzimuthalView", "AzimuthalView", 600, 600);
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
  TCanvas *c3 = (TCanvas*)c->GetPad(3); //new TCanvas("AzimuthalThrustView", "AzimuthalThrustView", 600, 600);
  c3->SetFillColor(0);  
  TView *view3 = TView::CreateView(1);
  view3->SetRange(-1,-1,-1,1,1,1);
  view3->FrontView(c3);

  // Perspective View
  TCanvas *c4 = (TCanvas*)c->GetPad(4); //new TCanvas("AzimuthalThrustView","AzimuthalThrustView",600,600);
  c4->SetFillColor(0);  
  TView *view4 = TView::CreateView(1);
  view4->SetRange(-1,-1,-1,1,1,1);

  TCanvas *cA = new TCanvas("cA","Interactive Viewer", 1000, 1000); //new TCanvas("AzimuthalView","AzimuthalView",600,600);
  cA->SetFillColor(0);  
  TView *viewA = TView::CreateView(1);
  viewA->SetRange(-1,-1,-1,1,1,1);

  // Create XYZ axes as lines on cA canvas
  cA->cd();
  TPolyLine3D *axes[3];
  
  for (int i = 0; i < 3; i++) {
    axes[i] = new TPolyLine3D(2);
    //axes[i]->SetLineColor(kGray+1);
    axes[i]->SetLineWidth(1);
    axes[i]->SetLineStyle(9);
  } 

  // X Axis Line
  axes[0]->SetPoint(0, -10, 0, 0);
  axes[0]->SetPoint(1, 10, 0, 0);
  axes[0]->SetLineColor(kGray+1);
  axes[0]->Draw();
  
  // Y Axis Line
  axes[1]->SetPoint(0, 0, -10, 0);
  axes[1]->SetPoint(1, 0, 10, 0);
  axes[1]->SetLineColor(kGray+1);
  axes[1]->Draw(); 

  // Z Axis Line
  axes[2]->SetPoint(0, 0, 0, -10);
  axes[2]->SetPoint(1, 0, 0, 10);
  axes[2]->SetLineColor(kGray+1);
  axes[2]->Draw();  

  // Create labels for the axes
  TLatex *axesLabels[3];
  
  for (int i = 0; i < 3; i++) {
    axesLabels[i] = new TLatex();
    axesLabels[i]->SetTextColor(kGray+1);
    axesLabels[i]->SetTextSize(0.04);
    axesLabels[i]->SetTextFont(12);
  }
 
 /*
  // X Label
  axesLabels[0]->SetText(-0.6, -0.3, "x");
  axesLabels[0]->Draw("same");

  // Y Label
  axesLabels[1]->SetText(0.6, -0.3, "y");
  axesLabels[1]->Draw("same");

  // Z Label
  axesLabels[2]->SetText(0, 0.6, "z");
  axesLabels[2]->Draw("same"); */

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

  // Helix array to store the jet tracks
  THelix *helix[1000]; 

  // Draw charged particles
  int nHelix = 0;
  int count = 0;
  for (int j = 0; j < (pPt->size()); j++) {

  // std::cout << pPt->size() << std::endl;
    for(int i = 0; i < (pPt->at(j).size()); i++) {

      // std::cout << pPt->at(j).size() << std::endl;
      if((pChg->at(j)).at(i) == 0) continue;
      if((pPt->at(j)).at(i) < ptCut) continue;

      // Jet tracks based on the particle type
      count++;
      int trackColor = 1;

      if (j>0) {trackColor = 1;} // Assume pion
      if ((pPID->at(j)).at(i) == TMath::Abs(2212)) {trackColor = 2;} // proton
      if ((pPID->at(j)).at(i) == TMath::Abs(321)) {trackColor = 0;} // kaon
      if ((pPID->at(j)).at(i) == TMath::Abs(11)) {trackColor = 5;} // electron
      if ((pPID->at(j)).at(i) == TMath::Abs(13)) {trackColor = 4;} // muon

      //if(jet.jtpt[0]*TMath::CosH(jet.jteta[0])>0 && dR(jet.jteta[0],jet.jtphi[0],particle.eta[i],particle.phi[i])<0.8) trackColor = 1;
      //else if(jet.jtpt[1]*TMath::CosH(jet.jteta[1])>0 && dR(jet.jteta[1],jet.jtphi[1],particle.eta[i],particle.phi[i])<0.8) trackColor = 2;
      //else if(jet.jtpt[2]*TMath::CosH(jet.jteta[2])>0 && dR(jet.jteta[2],jet.jtphi[2],particle.eta[i],particle.phi[i])<0.8) trackColor = 3;
      //else if(jet.jtpt[3]*TMath::CosH(jet.jteta[3])>0 && dR(jet.jteta[3],jet.jtphi[3],particle.eta[i],particle.phi[i])<0.8) trackColor = 4;
      //if(doWTA && boosted.pt[i]<0.01) trackColor = 5;
      //if (verbose) std::cout <<  jet.jteta[0] << " " << jet.jtphi[0] <<" " <<  particle.eta[i] << " " << particle.phi[i] << std::endl;
      //if (verbose) std::cout <<  dR(jet.jteta[0],jet.jtphi[0],particle.eta[i],particle.phi[i]) << std::endl;

      Float_t px,py,pz,pt;
      float phi = (pPhi->at(j)).at(i);
      pt = (pPt->at(j)).at(i);
      px = pt*TMath::Cos(phi);
      py = pt*TMath::Sin(phi);
      pz = pt*TMath::SinH((pEta->at(j)).at(i));

      // if (i>15) continue;
      std::cout << j << " " << i << " " << pt << " " << (pEta->at(j)).at(i) << " " << (pPhi->at(j)).at(i) << " " << px << " "  << py<<" " << pz <<  std::endl; 

      /*
      if(i%4==0) helix[nHelix] = new THelix(0,0,0,10,10,5,1 * 1.5);
      if(i%4==1) helix[nHelix] = new THelix(0,0,0,10,10,-5,1 * 1.5);
      if(i%4==2) helix[nHelix] = new THelix(0,0,0,10,10,5,-1 * 1.5);
      if(i%4==3) helix[nHelix] = new THelix(0,0,0,10,10,-5,-1 * 1.5);

      if(i%4==0 || i%4==2) formatHelix(helix[nHelix],5,14.14,trackColor,doWTA);
      if(i%4==1 || i%4==3) formatHelix(helix[nHelix],-5,14.14,trackColor,doWTA);
      */ 

      helix[nHelix] = new THelix(0, 0, 0, px, py, pz, (pChg->at(j)).at(i) * 0.2);
      formatHelix(helix[nHelix], pz, pt, trackColor, doWTA);

      c1->cd();
      helix[nHelix]->Draw("same");

      c2->cd();
      helix[nHelix]->Draw("same");

      c3->cd();
      helix[nHelix]->Draw("same");

      c4->cd();
      helix[nHelix]->Draw("same");
      
      cA->cd();
      helix[nHelix]->Draw("same");
      nHelix++;
    }
  }

  // Labelling the different views
  c1->cd();
  TLatex *l1 = new TLatex(-0.97,0.90, "Azimuthal View");
  l1->SetTextColor(kBlack);
  l1->SetTextFont(12);
  l1->Draw();

  c2->cd();
  TLatex *l2 = new TLatex(-0.97,0.90, "Side View");
  l2->SetTextColor(kBlack);
  l2->SetTextFont(12);
  l2->Draw();

  c3->cd();
  // TLatex *l3 = new TLatex(-0.97,0.90,"Azimuthal Thrust View");
  TLatex *l3 = new TLatex(-0.97,0.90, "Front View");
  l3->SetTextColor(kBlack);
  l3->SetTextFont(12);
  l3->Draw();

  c4->cd();
  TLatex *l4 = new TLatex(-0.97,0.90, "Perspective View");
  l4->SetTextColor(kBlack);
  l4->SetTextFont(12);
  l4->Draw(); 

  c2->cd();
  // Legend for the axis lines
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
  
  cA->cd();
  TLatex *fileLabel = new TLatex(0.2, 0.7, title.c_str());
  fileLabel->SetTextFont(12);
  fileLabel->SetTextColor(kBlack);
  fileLabel->Draw();

  std::string s1 = "Event Index: ";
  s1.append(std::to_string(currentEvtIndx));

  TLatex *eventLabel = new TLatex(0.2, 0.6, s1.c_str());
  eventLabel->SetTextColor(kBlack);
  eventLabel->SetTextSize(0.035);
  eventLabel->SetTextFont(12);
  eventLabel->Draw("same");

  std::string s2 = "N_{ch} in Jets: ";
  s2.append(std::to_string(count));
  TLatex *nchLabel = new TLatex(0.2, 0.5, s2.c_str());
  nchLabel->SetTextColor(kBlack);
  nchLabel->SetTextSize(0.035);
  nchLabel->SetTextFont(12);
  nchLabel->Draw("same");

  // Legend for the particle types
  TLegend *leg1 = new TLegend(0.05,0.4,0.4,0.6);
  leg1->SetBorderSize(0);
  leg1->SetFillStyle(0);

  TH1D * dummy1 = new TH1D("dummy1","dummy1",1,0,1);
  dummy1->SetLineColor(kBlack);
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

  leg1->SetTextColor(kBlack);
  leg1->SetTextFont(12);
  leg1->Draw("same");

  //cA->SaveAs(Form("../plots/EventDisplay_%d.pdf",eventIndx));
  //cA->SaveAs(Form("../plots/EventDisplay_%d.png",eventIndx));
}
   
/*
void nextEvent(){
  JetEventDisplay(currentEvtIndx+1, currentPtCut);
} 
*/