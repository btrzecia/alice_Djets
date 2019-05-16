#include "OCanvas.h"


void OCanvas::OCreate(TString p, Int_t x, Int_t y){
    TCanvas_.insert(std::make_pair(p,new TCanvas(p,p,800*x,800*y)));
    if(x != 1 || y != 1) TCanvas_.find(p)->second->Divide(x,y);
}

void OCanvas::OPlace(TString p, TH2* h, Int_t pad, TString opt){
    //std::cout<<TCanvas_.find(p)->second->cd(pad)->GetListOfPrimitives()->GetLast()<<std::endl;
    if(TCanvas_.find(p)== TCanvas_.end()) return;
    if(TCanvas_.find(p)->second->cd(pad)->GetListOfPrimitives()->GetLast() == -1){
        TCanvas_.find(p)->second->cd(pad);
        if(opt.Contains("logy")){
            TCanvas_.find(p)->second->cd(pad)->SetLogy();
            opt.ReplaceAll("logy","");
        }
        if(opt.Contains("logx")){
            TCanvas_.find(p)->second->cd(pad)->SetLogx();
            opt.ReplaceAll("logx","");
        }
        h->Draw(opt);
    }
    else{
        if(opt.Contains("logy")) opt.ReplaceAll("logy","");
        if(opt.Contains("logx")) opt.ReplaceAll("logx","");
        h->Draw(opt+"same");
    }
}

void OCanvas::OPlace(TString p, TH1* h, Int_t pad, TString opt,TString stat){
    //std::cout<<TCanvas_.find(p)->second->cd(pad)->GetListOfPrimitives()->GetLast()<<std::endl;
    if(TCanvas_.find(p)== TCanvas_.end()) return;

    if(TCanvas_.find(p)->second->cd(pad)->GetListOfPrimitives()->GetLast() == -1){
        //std::cout<<"inserting stack "<<p+pad<<std::endl;
        THStack_.insert(std::make_pair(p+pad,new THStack(p+pad,h->GetTitle())));
        THStack_.find(p+pad)->second->Add(h);
        TCanvas_.find(p)->second->cd(pad);
        if(opt == "logy")TCanvas_.find(p)->second->cd(pad)->SetLogy();
        THStack_.find(p+pad)->second->Draw("nostack");
       // h->SetStats(1);
       // gStyle->SetOptStat(1000000010);
       // h->GetPainter()->PaintStat(1,0);
        //std::cout<<"title "<<h->GetXaxis()->GetTitle()<<" "<<h->GetYaxis()->GetTitle()<<std::endl;
        THStack_.find(p+pad)->second->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
        THStack_.find(p+pad)->second->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
        //if(THStack_.find(p+pad)== THStack_.end())std::cout<<"stack found"<<p+pad<<std::endl;
        //TCanvas_.find(p)->second->cd(pad);
        //TH1dummy_.insert(std::make_pair(p+pad+"dummy",(TH1*)h->Clone(p+pad+"dummy")));
        //TH1dummy_.find(p+pad+"dummy")->second->Draw();
        //h->Draw(opt);
    }
    else{
        //std::cout<<"adding histogram"<<std::endl;
        THStack_.find(p+pad)->second->Add(h);
        /*std::cout<<h->GetTitle()<<"saved min: "<<TH1dummy_.find(p+pad+"dummy")->second->GetMinimum()<<" histo min: "<<h->GetBinContent(h->GetMinimumBin())<<" saved max:"<<TH1dummy_.find(p+pad+"dummy")->second->GetMaximum()<<" histo max"<<h->GetBinContent(h->GetMaximumBin())<<std::endl;
        if(TH1dummy_.find(p+pad+"dummy")->second->GetMaximum() < h->GetBinContent(h->GetMaximumBin()))
            TH1dummy_.find(p+pad+"dummy")->second->SetMaximum(h->GetBinContent(h->GetMaximumBin()));
        if(TMath::Abs(TH1dummy_.find(p+pad+"dummy")->second->GetMinimum()) > h->GetBinContent(h->GetMinimumBin()))
            TH1dummy_.find(p+pad+"dummy")->second->SetMinimum(h->GetBinContent(h->GetMinimumBin()));*/
        //h->Draw(opt+"same");
    }
}

void OCanvas::OPlaceRatio(TString p, TH1* h1, TH1* h2, Int_t pad, TString opt){
    if(TCanvas_.find(p)== TCanvas_.end()) return;
    if(TCanvas_.find(p)->second->cd(pad)->GetListOfPrimitives()->GetLast() == -1){
        TString tmp = h2->GetName();
        h2->SetName(tmp+"sec");
        //TRatioPlot_.insert(std::make_pair(p+pad,new TRatioPlot(h1,h2,"divsym")));
        auto rt = new TRatioPlot(h1,h2,"divsym");
        TCanvas_.find(p)->second->cd(pad);
        TCanvas_.find(p)->second->SetTicks(0,1);
        //TRatioPlot_.find(p+pad)->second->Draw();
        //TRatioPlot_.find(p+pad)->second->SetH1DrawOpt("E");
        //TRatioPlot_.find(p+pad)->second->SetH2DrawOpt("E");

        rt->SetH1DrawOpt("E");
        rt->SetH2DrawOpt("E");
        rt->Draw();
        if(opt == "logy")rt->GetUpperPad()->SetLogy();
        TCanvas_.find(p)->second->Update();


    }
}

void OCanvas::OForceStack(TString p, Int_t pad, Double_t x_min, Double_t x_max){
    THStack_.find(p+pad)->second->GetXaxis()->SetRangeUser(x_min,x_max);
}

void OCanvas:: OPlace(TString p, TLegend *l, Int_t pad, TString opt){
    TCanvas_.find(p)->second->cd(pad);
    if(opt=="logy") opt = "";
    l->Draw(opt);
}

void OCanvas::ODraw(TString p){
    for(auto elem : THStack_){
        if(elem.first.Contains(p)){
            TString pad = elem.first;
            pad = pad.ReplaceAll(p,"");
            TCanvas_.find(p)->second->cd(pad.Atoi());
            //elem.second->SetMaximum(1e7);
            elem.second->Draw("nostack");
        }
    }
}

void OCanvas::OFlush(TString folder, TString sufix){
    gSystem->Exec("mkdir -p "+folder+"/"+sufix);
    for(auto elem : TCanvas_) elem.second->SaveAs(folder+"/"+sufix+"/"+elem.first+"."+sufix);
}

void OCanvas::OLegend(TString p, TH1* h,TString name){
    if(TLegend_.find(p)== TLegend_.end()){
        //std::cout<<"Inserting legend: "<<p<<std::endl;
        TLegend_.insert(std::make_pair(p,new TLegend(0.2,0.20,0.8,0.85)));
    }
    TLegend_.find(p)->second->AddEntry(h,name,"f");
    //std::cout<<"Adding legend: "<<p<<" "<<h->GetName()<<" "<<name<<std::endl;

}

TLegend* OCanvas::OGetLegend(TString p){
    if(TLegend_.find(p)!= TLegend_.end())
        return TLegend_.find(p)->second;
    else{
        std::cout<<"No legend of name: "<<p<<" found"<<std::endl;
        return nullptr;
    }

}

