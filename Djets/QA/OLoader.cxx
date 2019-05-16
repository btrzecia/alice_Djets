#include "OLoader.h"

OLoader::OLoader(const TString &fileName, bool fusion, bool debug):
    TFile_(new TFile(fileName)),debug_(debug){
    fusion_ = fusion;
    if(!TFile_)errorTerminate_ = true;
    if(errorTerminate_) return;
    std::cout<<"Initiating OLoader for "<<fileName<<"..."<<std::endl;
    TIter nextkey(TFile_->GetListOfKeys());
    TKey *key;
    while((key=(TKey*)nextkey())){ //iter over all objects in file
        TObject *obj = key->ReadObj();
        OLoad(obj,""); //pass these elements into loader switch function
    }
    std::cout<<"OLoader sequence complete..."<<std::endl;
}

///
/// \brief OLoader::OLoad loader switch function - will determine objets types and call specific loading function
/// \param obj TObject from file
/// \param p
///
void OLoader::OLoad(TObject *obj, TString p){
    if(errorTerminate_) return;
    if(obj->InheritsFrom(TH1::Class()) ||
       obj->InheritsFrom(TH2::Class()) ||
       obj->InheritsFrom(TH3::Class()))
        OLoadTHX(obj,p+obj->GetName());
    if(obj->InheritsFrom(TList::Class()))
        OLoadTList(obj,p+obj->GetName());
    if(obj->InheritsFrom(TDirectory::Class()))
        OLoadTDirectory(obj,p+obj->GetName());
    if(obj->InheritsFrom(THnSparse::Class()))
        OLoadSparse(obj,p+obj->GetName());
    if(obj->InheritsFrom(AliNormalizationCounter::Class()))
        OLoadNorm(obj);


}
void OLoader::OLoadNorm(TObject *obj){
    if(debug_)std::cout<<"Linking norm: "<<obj->GetName()<<std::endl;
    Nevents_ = ((AliNormalizationCounter*)obj)->GetNEventsForNorm();
}

void OLoader::OLoadTHX(TObject *obj, TString p){
    if(errorTerminate_) return;
    if(p.Contains("MBN0") || fusion_ == false){
        if(debug_)std::cout<<"Linking histogram: "<<p<<std::endl;
        if(p.Contains("MBN0")) p.ReplaceAll("MBN0","MBNX");
        if(obj->InheritsFrom(TH1::Class()))((TH1*)obj)->SetName(this->OGetDataName()+obj->GetName());
        if(obj->InheritsFrom(TH2::Class()))((TH2*)obj)->SetName(this->OGetDataName()+obj->GetName());
        if(obj->InheritsFrom(TH3::Class()))((TH3*)obj)->SetName(this->OGetDataName()+obj->GetName());
        if(obj->InheritsFrom(TH1F::Class())) TH1F_.insert(std::make_pair(p,(TH1F*)obj));
        if(obj->InheritsFrom(TH1D::Class())) TH1D_.insert(std::make_pair(p,(TH1D*)obj));
        if(obj->InheritsFrom(TH1I::Class())) TH1I_.insert(std::make_pair(p,(TH1I*)obj));
        if(obj->InheritsFrom(TH2F::Class())) TH2F_.insert(std::make_pair(p,(TH2F*)obj));
        if(obj->InheritsFrom(TH2D::Class())) TH2D_.insert(std::make_pair(p,(TH2D*)obj));
        if(obj->InheritsFrom(TH2I::Class())) TH2I_.insert(std::make_pair(p,(TH2I*)obj));
        if(obj->InheritsFrom(TH3F::Class())) TH3F_.insert(std::make_pair(p,(TH3F*)obj));
        if(obj->InheritsFrom(TH3D::Class())) TH3D_.insert(std::make_pair(p,(TH3D*)obj));
        if(obj->InheritsFrom(TH3I::Class())) TH3I_.insert(std::make_pair(p,(TH3I*)obj));
    }
    else if(p.Contains("MBN") && fusion_ == true){
        if(debug_)std::cout<<"Adding histogram: "<<p<<" to ";
        p.ReplaceAll("MBN1","MBNX");
        p.ReplaceAll("MBN2","MBNX");
        p.ReplaceAll("MBN3","MBNX");
        p.ReplaceAll("MBN4","MBNX");
        p.ReplaceAll("MBN5","MBNX");
        p.ReplaceAll("MBN6","MBNX");
        p.ReplaceAll("MBN7","MBNX");
        p.ReplaceAll("MBN8","MBNX");
        p.ReplaceAll("MBN9","MBNX");
        if(debug_)std::cout<<p<<std::endl;
        if(obj->InheritsFrom(TH1F::Class())) TH1F_.find(p)->second->Add((TH1F*)obj);
        if(obj->InheritsFrom(TH1D::Class())) TH1D_.find(p)->second->Add((TH1D*)obj);
        if(obj->InheritsFrom(TH1I::Class())) TH1I_.find(p)->second->Add((TH1I*)obj);
        if(obj->InheritsFrom(TH2F::Class())) TH2F_.find(p)->second->Add((TH2F*)obj);
        if(obj->InheritsFrom(TH2D::Class())) TH2D_.find(p)->second->Add((TH2D*)obj);
        if(obj->InheritsFrom(TH2I::Class())) TH2I_.find(p)->second->Add((TH2I*)obj);
        if(obj->InheritsFrom(TH3F::Class())) TH3F_.find(p)->second->Add((TH3F*)obj);
        if(obj->InheritsFrom(TH3D::Class())) TH3D_.find(p)->second->Add((TH3D*)obj);
        if(obj->InheritsFrom(TH3I::Class())) TH3I_.find(p)->second->Add((TH3I*)obj);
    }
}

void OLoader::OLoadTList(TObject *obj, TString p){
    if(errorTerminate_) return;
    if(debug_)std::cout<<"Linking list: "<<p<<std::endl;
    for(int i = 0;i<=((TList*)obj)->GetLast();i++){
        OLoad(((TList*)obj)->At(i),p+".");
    }
}

void OLoader::OLoadPrint(){
    if(errorTerminate_) return;
    for(auto elem : THnSparseF_) std::cout << "THnSparseF: " << elem.first <<std::endl;
    for(auto elem : TH1F_) std::cout << "TH1F: " << elem.first <<std::endl;
    for(auto elem : TH1D_) std::cout << "TH1D: " << elem.first <<std::endl;
    for(auto elem : TH1I_) std::cout << "TH1I: " << elem.first <<std::endl;
    for(auto elem : TH2F_) std::cout << "TH2F: " << elem.first <<std::endl;
    for(auto elem : TH2D_) std::cout << "TH2D: " << elem.first <<std::endl;
    for(auto elem : TH2I_) std::cout << "TH2I: " << elem.first <<std::endl;
    for(auto elem : TH3F_) std::cout << "TH3F: " << elem.first <<std::endl;
    for(auto elem : TH3D_) std::cout << "TH3D: " << elem.first <<std::endl;
    for(auto elem : TH3I_) std::cout << "TH3I: " << elem.first <<std::endl;
}

void OLoader::OLoadTDirectory(TObject *obj, TString p){
    if(errorTerminate_) return;
    if(debug_)std::cout<<"Linking Directory: "<<p<<std::endl;
    //std::cout<<"NKey"<<((TDirectoryFile*)obj)->GetNkeys()<<std::endl;
    TIter next(((TDirectoryFile*)obj)->GetListOfKeys());
    TKey *key;
     while ((key = (TKey*)next())) {
    //for(int i = 0;i<=((TDirectoryFile*)obj)->GetNkeys();i++){
       OLoad(key->ReadObj(),p+".");

    }

};
void OLoader::OLoadSparse(TObject *obj, TString p){
   if(errorTerminate_) return;
   if(debug_) std::cout<<"Linking THnSparse: "<<p<<std::endl;
    if(p.Contains("MBN0") || fusion_ == false){
        if(p.Contains("MBN0")) p.ReplaceAll("MBN0","MBNX");
        if(obj->InheritsFrom(THnSparseF::Class()))((THnSparseF*)obj)->SetName(this->OGetDataName()+obj->GetName());
        if(obj->InheritsFrom(THnSparseF::Class())) THnSparseF_.insert(std::make_pair(p,(THnSparseF*)obj));
    }
    else if(p.Contains("MBN") && fusion_ == true){
        if(debug_)std::cout<<"Adding THnSparse: "<<p<<" to ";
        p.ReplaceAll("MBN1","MBNX");
        p.ReplaceAll("MBN2","MBNX");
        p.ReplaceAll("MBN3","MBNX");
        p.ReplaceAll("MBN4","MBNX");
        p.ReplaceAll("MBN5","MBNX");
        p.ReplaceAll("MBN6","MBNX");
        p.ReplaceAll("MBN7","MBNX");
        p.ReplaceAll("MBN8","MBNX");
        p.ReplaceAll("MBN9","MBNX");
        if(debug_)std::cout<<p<<std::endl;
        if(obj->InheritsFrom(THnSparseF::Class())) THnSparseF_.find(p)->second->Add((THnSparseF*)obj);
    }
};

TH1* OLoader::OGetHisto1(TString p){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(TH1F_.find(p)!= TH1F_.end()) return TH1F_.find(p)->second;
    else if(TH1D_.find(p)!= TH1D_.end()) return TH1D_.find(p)->second;
    else if(TH1I_.find(p)!= TH1I_.end()) return TH1I_.find(p)->second;
    else {
        std::cout<<"Cannot find 1D histogram: "<<p<<std::endl;
        return nullptr;
    }
}

TH2* OLoader::OGetHisto2(TString p){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(TH2F_.find(p)!= TH2F_.end()) return TH2F_.find(p)->second;
    else if(TH2D_.find(p)!= TH2D_.end()) return TH2D_.find(p)->second;
    else if(TH2I_.find(p)!= TH2I_.end()) return TH2I_.find(p)->second;
    else {
        std::cout<<"Cannot find 2D histogram: "<<p<<std::endl;
        return nullptr;
    }
}

TH3* OLoader::OGetHisto3(TString p){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(TH3F_.find(p)!= TH3F_.end()) return TH3F_.find(p)->second;
    else if(TH3D_.find(p)!= TH3D_.end()) return TH3D_.find(p)->second;
    else if(TH3I_.find(p)!= TH3I_.end()) return TH3I_.find(p)->second;
    else {
        std::cout<<"Cannot find 3D histogram: "<<p<<std::endl;
        return nullptr;
    }
}

THnSparse* OLoader::OGetSparse(TString p){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(THnSparseF_.find(p)!= THnSparseF_.end()) return THnSparseF_.find(p)->second;
    else {
        std::cout<<"Cannot find THnSparse: "<<p<<std::endl;
        return nullptr;
    }
}

void OLoader::OSetX(TString p, Double_t x_min, Double_t x_max){
    if(nameSpace_ != "") p = nameSpace_ + p;

    if(TH1F_.find(p)!= TH1F_.end())TH1F_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH1D_.find(p)!= TH1D_.end()) TH1D_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH1I_.find(p)!= TH1I_.end()) TH1I_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH2F_.find(p)!= TH2F_.end()) TH2F_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH2D_.find(p)!= TH2D_.end()) TH2D_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH2I_.find(p)!= TH2I_.end()) TH2I_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH3F_.find(p)!= TH3F_.end()) TH3F_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH3D_.find(p)!= TH3D_.end()) TH3D_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
    else if(TH3I_.find(p)!= TH3I_.end()) TH3I_.find(p)->second->GetXaxis()->SetRangeUser(x_min,x_max);
}

void OLoader::OSetXf(TString p, Double_t x_min, Double_t x_max){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(TH1F_.find(p)!= TH1F_.end())TH1F_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH1D_.find(p)!= TH1D_.end()) TH1D_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH1I_.find(p)!= TH1I_.end()) TH1I_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH2F_.find(p)!= TH2F_.end()) TH2F_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH2D_.find(p)!= TH2D_.end()) TH2D_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH2I_.find(p)!= TH2I_.end()) TH2I_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH3F_.find(p)!= TH3F_.end()) TH3F_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH3D_.find(p)!= TH3D_.end()) TH3D_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
    else if(TH3I_.find(p)!= TH3I_.end()) TH3I_.find(p)->second->GetXaxis()->SetLimits(x_min,x_max);
}


void OLoader::OSetY(TString p, Double_t y_min, Double_t y_max){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(TH1F_.find(p)!= TH1F_.end()) TH1F_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH1D_.find(p)!= TH1D_.end()) TH1D_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH1I_.find(p)!= TH1I_.end()) TH1I_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH2F_.find(p)!= TH2F_.end()) TH2F_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH2D_.find(p)!= TH2D_.end()) TH2D_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH2I_.find(p)!= TH2I_.end()) TH2I_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH3F_.find(p)!= TH3F_.end()) TH3F_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH3D_.find(p)!= TH3D_.end()) TH3D_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
    else if(TH3I_.find(p)!= TH3I_.end()) TH3I_.find(p)->second->GetYaxis()->SetRangeUser(y_min,y_max);
}

void OLoader::ODo1DprojectionX(TString p, Double_t *ydiv, UInt_t nbins){
   // for(UInt_t i = 0 ; i < nbins; i++ ){
    //    std::cout<<xdiv[i]<<std::endl;
   // }
    if(nameSpace_ != "") p = nameSpace_ + p;
    TH2* tmp = nullptr;
    if(TH2F_.find(p)!= TH2F_.end()){
        tmp = TH2F_.find(p)->second;
        for(UInt_t i = 0 ; i < nbins-1; i++ ){
            TString stmp = "";
            TH1F_.insert(std::make_pair(p+"_py_"+ydiv[i]+"_"+ydiv[i+1],(TH1F*)tmp->ProjectionX(this->OGetDataName()+p+"_py_"+ydiv[i]+"_"+ydiv[i+1],tmp->GetYaxis()->FindBin(ydiv[i]),tmp->GetYaxis()->FindBin(ydiv[i+1]))));
            TH1F_.find(p+"_py_"+ydiv[i]+"_"+ydiv[i+1])->second->SetTitle(stmp+TH1F_.find(p+"_py_"+ydiv[i]+"_"+ydiv[i+1])->second->GetTitle()+"_py_"+ydiv[i]+"_"+ydiv[i+1]);
            TH1F_.find(p+"_py_"+ydiv[i]+"_"+ydiv[i+1])->second->GetXaxis()->SetTitle(tmp->GetXaxis()->GetTitle());
            TH1F_.find(p+"_py_"+ydiv[i]+"_"+ydiv[i+1])->second->GetYaxis()->SetTitle("Counts");
        }
    }
    else if(TH2D_.find(p)!= TH2D_.end()){
        tmp =  TH2D_.find(p)->second;
        for(UInt_t i = 0 ; i < nbins-1; i++ ){
            TString stmp = "";
            TH1D_.insert(std::make_pair(p+"_py_"+ydiv[i]+"_"+ydiv[i+1],(TH1D*)tmp->ProjectionX(this->OGetDataName()+p+"_py_"+ydiv[i]+"_"+ydiv[i+1],tmp->GetBin(ydiv[i],0),tmp->GetBin(ydiv[i+1],0))));
            TH1D_.find(p+"_px_"+ydiv[i]+"_"+ydiv[i+1])->second->SetTitle(stmp+TH1D_.find(p+"_px_"+ydiv[i]+"_"+ydiv[i+1])->second->GetTitle()+"_px_"+ydiv[i]+"_"+ydiv[i+1]);
        }
    }
    else if(TH2I_.find(p)!= TH2I_.end()){
        tmp =  TH2I_.find(p)->second;
        for(UInt_t i = 0 ; i < nbins-1; i++ ){
            TString stmp = "";
            TH1I_.insert(std::make_pair(p+"_py_"+ydiv[i]+"_"+ydiv[i+1],(TH1I*)tmp->ProjectionX(this->OGetDataName()+p+"_py_"+ydiv[i]+"_"+ydiv[i+1],tmp->GetBin(ydiv[i],0),tmp->GetBin(ydiv[i+1],0))));
            TH1I_.find(p+"_py_"+ydiv[i]+"_"+ydiv[i+1])->second->SetTitle(stmp+TH1I_.find(p+"_py_"+ydiv[i]+"_"+ydiv[i+1])->second->GetTitle()+"_py_"+ydiv[i]+"_"+ydiv[i+1]);
        }
    }
}

void OLoader::ODo1DprojectionY(TString p, Double_t *xdiv, UInt_t nbins){
   // for(UInt_t i = 0 ; i < nbins; i++ ){
    //    std::cout<<xdiv[i]<<std::endl;
   // }
    if(nameSpace_ != "") p = nameSpace_ + p;
    TH2* tmp = nullptr;
    if(TH2F_.find(p)!= TH2F_.end()){
        tmp = TH2F_.find(p)->second;
        for(UInt_t i = 0 ; i < nbins-1; i++ ){
            TString stmp = "";
            TH1F_.insert(std::make_pair(p+"_px_"+xdiv[i]+"_"+xdiv[i+1],(TH1F*)tmp->ProjectionY(this->OGetDataName()+p+"_px_"+xdiv[i]+"_"+xdiv[i+1],tmp->GetXaxis()->FindBin(xdiv[i]),tmp->GetXaxis()->FindBin(xdiv[i+1]))));
            TH1F_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->SetTitle(stmp+TH1F_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->GetTitle()+"_px_"+xdiv[i]+"_"+xdiv[i+1]);
            TH1F_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->GetXaxis()->SetTitle(tmp->GetYaxis()->GetTitle());
            TH1F_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->GetYaxis()->SetTitle("Counts");
        }
    }
    else if(TH2D_.find(p)!= TH2D_.end()){
        tmp =  TH2D_.find(p)->second;
        for(UInt_t i = 0 ; i < nbins-1; i++ ){
            TString stmp = "";
            TH1D_.insert(std::make_pair(p+"_px_"+xdiv[i]+"_"+xdiv[i+1],(TH1D*)tmp->ProjectionY(this->OGetDataName()+p+"_px_"+xdiv[i]+"_"+xdiv[i+1],tmp->GetBin(xdiv[i],0),tmp->GetBin(xdiv[i+1],0))));
            TH1D_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->SetTitle(stmp+TH1D_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->GetTitle()+"_px_"+xdiv[i]+"_"+xdiv[i+1]);
        }
    }
    else if(TH2I_.find(p)!= TH2I_.end()){
        tmp =  TH2I_.find(p)->second;
        for(UInt_t i = 0 ; i < nbins-1; i++ ){
            TString stmp = "";
            TH1I_.insert(std::make_pair(p+"_px_"+xdiv[i]+"_"+xdiv[i+1],(TH1I*)tmp->ProjectionY(this->OGetDataName()+p+"_px_"+xdiv[i]+"_"+xdiv[i+1],tmp->GetBin(xdiv[i],0),tmp->GetBin(xdiv[i+1],0))));
            TH1I_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->SetTitle(stmp+TH1I_.find(p+"_px_"+xdiv[i]+"_"+xdiv[i+1])->second->GetTitle()+"_px_"+xdiv[i]+"_"+xdiv[i+1]);
        }
    }
}

void OLoader::OSetColor(TString p, Int_t c, Float_t m){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(TH1F_.find(p)!= TH1F_.end()){
        TH1F_.find(p)->second->SetMarkerColor(c);
        TH1F_.find(p)->second->SetLineColor(c);
        TH1F_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH1D_.find(p)!= TH1D_.end()){
        TH1D_.find(p)->second->SetMarkerColor(c);
        TH1D_.find(p)->second->SetLineColor(c);
        TH1D_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH1I_.find(p)!= TH1I_.end()){
        TH1I_.find(p)->second->SetMarkerColor(c);
        TH1I_.find(p)->second->SetLineColor(c);
        TH1I_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH2F_.find(p)!= TH2F_.end()){
        TH2F_.find(p)->second->SetMarkerColor(c);
        TH2F_.find(p)->second->SetLineColor(c);
        TH2F_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH2D_.find(p)!= TH2D_.end()){
        TH2D_.find(p)->second->SetMarkerColor(c);
        TH2D_.find(p)->second->SetLineColor(c);
        TH2D_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH2I_.find(p)!= TH2I_.end()){
        TH2I_.find(p)->second->SetMarkerColor(c);
        TH2I_.find(p)->second->SetLineColor(c);
        TH2I_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH3F_.find(p)!= TH3F_.end()){
        TH3F_.find(p)->second->SetMarkerColor(c);
        TH3F_.find(p)->second->SetLineColor(c);
        TH3F_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH3D_.find(p)!= TH3D_.end()){
        TH3D_.find(p)->second->SetMarkerColor(c);
        TH3D_.find(p)->second->SetLineColor(c);
        TH3D_.find(p)->second->SetMarkerSize(m);
    }
    else if(TH3I_.find(p)!= TH3I_.end()){
        TH3I_.find(p)->second->SetMarkerColor(c);
        TH3I_.find(p)->second->SetLineColor(c);
        TH3I_.find(p)->second->SetMarkerSize(m);
    }
}

void OLoader::OSetProp(TH1* h, TString scale, Int_t rebin, Double_t weight){
    if(scale == "nevents")h->Scale(1./Nevents_);
    if(scale == "nentries")h->Scale(1./h->GetEntries());
    if (weight !=0)h->Scale(weight);
    if(rebin !=0)h->Rebin(rebin);
};

void OLoader::OCutSparse(TString p, Int_t axis, Double_t min, Double_t max){
    if(nameSpace_ != "") p = nameSpace_ + p;
    if(THnSparseF_.find(p)!= THnSparseF_.end()){
        THnSparseF_.find(p)->second->GetAxis(axis)->SetRangeUser(min,max);
    }
}

void OLoader::OProjectSparse(TString p, Int_t axis){
    if(nameSpace_ != "") p = nameSpace_ + p;
    Int_t c = 0;
    if(THnSparseF_.find(p)!= THnSparseF_.end()){
        while(TH1F_.find(p+axis+TString::Itoa(c,10))!= TH1F_.end()){
            //std::cout<<"looping "<<c<<" while "<<((TH1F_.find(p+axis+TString::Itoa(c,10))!= TH1F_.end()) ? "1" : "0")<<std::endl;
            c++;
        }
       // std::cout<<"Intersting sparse projection "<<p+axis+c<<std::endl;
        TH1F_.insert(std::make_pair(p+axis+TString::Itoa(c,10),(TH1F*)(THnSparseF_.find(p)->second->Projection(axis))));
        TH1F_.find(p+axis+TString::Itoa(c,10))->second->SetName(this->OGetDataName()+p+axis+c);

    }
}



