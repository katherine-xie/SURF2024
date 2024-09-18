#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TMath.h"
#include "vector"

void JetAnalyzer(){
    TFile* fTree=new TFile("LEP1_1.root","READ");
    TTreeReader reader("trackTree",fTree);
    // if (reader.IsInvalid()) {
    if (reader.GetTree() == nullptr) {
        cerr << "Invalid tree! :-(\n";
        return;
    }

    TTreeReaderValue<std::vector<float>> mpx(reader,"px");
    //mpx is a pointer to std::vector<float> 
    TTreeReaderValue<std::vector<std::vector<int>>> mgenDau_pid(reader,"genDau_pid");
    //mgenDau_pid is a pointer to std::vector<std::vector<int>> 
    
    while(reader.Next()){
        //double px=&mpx;
        //cout<<mpx->size()<<endl;
        //for(int i=0;i<mpx->size();i++){
        //    cout<<(*mpx)[i]<<endl;//(*mpx) is a std::vector<float> 
        //}
        //cout<<mgenDau_pid->size()<<endl;
        for(int i=0;i<mgenDau_pid->size();i++){
            //cout<<(*mgenDau_pid)[i].size()<<endl;
            for (int j=0;j<(*mgenDau_pid)[i].size();j++){
                cout<<(*mgenDau_pid)[i][j]<<endl;
            }
        }
        cout<<"====="<<endl;
        
    }

    delete fTree;
}