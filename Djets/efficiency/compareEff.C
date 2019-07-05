//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include <string>
#include <sstream>
#include <iostream>
#include <TF2.h>

#include "config.h"

int D0Color = kRed+1;
int D0JetColor = kBlue+1;
TString outDir = "out/plots";

void getEfficiency(TString inFileD0, TString inFileD0Jet, double plotmin, double plotmax, bool isEta=0, int ptdmin = 0);
void getEfficiency(TH1F *hD0, TH1F *hD0Jet, double plotmin, double plotmax, bool isEta=0, int ptdmin = 0);

void getDist(TString inFileD0, TString inFileD0Jet, int ptdmin = 0);

void setHistoDetails(TH1 *h, Color_t color, Style_t Mstyle, Size_t size = 0.9, Width_t width=2, bool isIntegral = 0);

TFile *fEffD;

void compareEff(
TString D0File = "out/D0Eff_y05.root",
TString D0JetFile = "out/DjetEff_y05_prompt_jetpt-1_200.root",
//TString D0JetFile = "out/DjetEffNoJetCut_y05_prompt_jetpt-1_200.root",
double plotmin = 2, double plotmax = 36)
{
	gROOT->ForceStyle();
    gStyle->SetOptStat(000);
    gSystem->Exec(Form("mkdir -p %s",outDir.Data()));


    fEffD = new TFile("EfficiencyD0.root","RECREATE");


    getEfficiency(D0File,D0JetFile,plotmin,plotmax);
    //getEfficiency("out/D0Eff_eta.root","out/DjetEffEta_prompt_jetpt-1_200.root",-1,1,1,2);

    getDist("out/D0Eff.root","out/DjetEff_prompt_jetpt-1_200.root");
    //getDist("out/D0Eff.root","out/DjetNoEtaCut_prompt_jetpt-1_200.root");
    //getDist("out/D0Eff.root","out/DjetEff_prompt_jetpt-1_200.root");

    //getDist("out/D0Eff.root","out/DjetEffNoMCCut_prompt_jetpt-1_200.root");



fEffD->Close();

}

void getEfficiency(TString D0File, TString D0JetFile, double plotmin, double plotmax, bool isEta,int ptdmin){


    TFile *inFileD0 = new TFile(D0File.Data(),"read");
    TFile *inFileD0Jet = new TFile(D0JetFile.Data(),"read");

    TH1F *hEffD0;
    TH1F *hEffD0Jet;
    if(isEta) {
        hEffD0 = (TH1F*)inFileD0->Get("h");
        hEffD0Jet= (TH1F*)inFileD0Jet->Get("hEff");
    }
    else {
        hEffD0 = (TH1F*)inFileD0->Get("h_reb");
        hEffD0Jet= (TH1F*)inFileD0Jet->Get("hEff_reb");
    }


    getEfficiency(hEffD0,hEffD0Jet,plotmin,plotmax,isEta,ptdmin);

}


void getEfficiency(TH1F *hEffD0, TH1F *hEffD0Jet, double plotmin, double plotmax, bool isEta,int ptdmin){


    hEffD0->SetTitle("");
    if(isEta)hEffD0->SetName("D0EfficiencyEta");
    else hEffD0->SetName("D0Efficiency");
    hEffD0->SetMarkerColor(D0Color);
    hEffD0->SetLineColor(D0Color);
    hEffD0->SetMarkerStyle(20);
    hEffD0->SetMarkerSize(1.2);
    if(isEta) hEffD0->GetXaxis()->SetTitle(Form("#it{y}_{%s} (GeV/#it{c})",fDmesonS.Data()));
    else  hEffD0->GetXaxis()->SetTitle(Form("#it{p}_{T,%s} (GeV/#it{c})",fDmesonS.Data()));
    hEffD0->GetYaxis()->SetTitle("Acceptance #times Efficiency");
    hEffD0->GetXaxis()->SetLabelSize(0.04);
    hEffD0->GetXaxis()->SetTitleSize(0.04);
    hEffD0->GetXaxis()->SetTitleOffset(1.);
    hEffD0->GetYaxis()->SetTitleOffset(1.);
    hEffD0->GetYaxis()->SetLabelSize(0.045);
    hEffD0->GetYaxis()->SetTitleSize(0.05);
    hEffD0->GetXaxis()->SetRangeUser(plotmin,plotmax);
    if(!isEta) hEffD0->SetMaximum(1);
    else hEffD0->SetMaximum(0.2);

    hEffD0Jet->SetTitle("");
    if(isEta)    hEffD0Jet->SetName("D0jetEfficiencyEta");
    else hEffD0Jet->SetName("D0jetEfficiency");
    hEffD0Jet->SetMarkerColor(D0JetColor);
    hEffD0Jet->SetLineColor(D0JetColor);
    hEffD0Jet->SetMarkerStyle(21);
    hEffD0Jet->SetMarkerSize(1.2);
    hEffD0Jet->GetXaxis()->SetTitle(Form("#it{p}_{T,%s} (GeV/#it{c})",fDmesonS.Data()));
    hEffD0Jet->GetYaxis()->SetTitle("Acceptance #times Efficiency");
    hEffD0Jet->GetXaxis()->SetLabelSize(0.04);
    hEffD0Jet->GetXaxis()->SetTitleSize(0.05);
    hEffD0Jet->GetXaxis()->SetTitleOffset(1.);
    hEffD0Jet->GetYaxis()->SetTitleOffset(1.);
    hEffD0Jet->GetYaxis()->SetLabelSize(0.045);
    hEffD0Jet->GetYaxis()->SetTitleSize(0.05);
    hEffD0Jet->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hEffD0Jet->SetMaximum(1);

    TLegend *leg = new TLegend(0.12,0.7,0.45,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.045);
    if(isEta){
        leg->AddEntry(hEffD0,"D0","p");
        leg->AddEntry(hEffD0Jet,"D0-jet, |#eta^{jet}|<0.5","p");
    }
    else {
        leg->AddEntry(hEffD0,"D0, |y^{D0}|<0.5","p");
        leg->AddEntry(hEffD0Jet,"D0-jet, |y^{D0}|<0.5 |#eta^{jet}|<0.5","p");
    }

    TPaveText *pvD = new TPaveText(0.15,0.6,0.45,0.65,"brNDC");
    pvD->SetFillStyle(0);
    pvD->SetBorderSize(0);
    pvD->SetTextFont(42);
    pvD->SetTextSize(0.045);
    pvD->SetTextAlign(11);
    pvD->AddText(Form("p_{T,D0} > %d",ptdmin));


    TCanvas *cEff2 = new TCanvas("cEff2","cEff2",1400,1000);
    //hEffD0->SetMaximum(0.4);
    hEffD0->Draw();
    hEffD0Jet->Draw("same");
    leg->Draw("same");
    if(isEta) pvD->Draw("same");

    fEffD->cd();
    if(!isEta){
        hEffD0->Write();
        hEffD0Jet->Write();
    }
    if(isEta){
        cEff2->SaveAs(Form("%s/DjetEff_eta_ptD%d.pdf",outDir.Data(),ptdmin));
        cEff2->SaveAs(Form("%s/DjetEff_eta_ptD%d.png",outDir.Data(),ptdmin));
    }
    else{
        cEff2->SaveAs(Form("%s/DjetEff_y05.pdf",outDir.Data()));
        cEff2->SaveAs(Form("%s/DjetEff_y05.png",outDir.Data()));
    }
}


void getDist(TString D0File, TString D0JetFile, int ptdmin){


    //TF2 *etaY1 = new TF2("etaY1",Log((Sqrt(1.87*1.87+y*y*CosH(x)*CosH(x))+y*SinH(x))/(Sqrt(1.87*1.87+y*y))),0,1,0,10);

    TFile *inFileD0 = new TFile(D0File.Data(),"read");
    TFile *inFileD0Jet = new TFile(D0JetFile.Data(),"read");

    TH2D *hPtYD0MC = (TH2D*)inFileD0->Get("CFHFccontainer0_central_SelStep1_proj_0_1_proj_1_0");
    TH2D *hPtYD0reco = (TH2D*)inFileD0->Get("CFHFccontainer0_central_SelStep8_proj_0_1_proj_1_0");
    TH2D *hPtYD0JetMC = (TH2D*)inFileD0Jet->Get("sparseMC_0_proj_8_7");
    TH2D *hPtYD0Jetreco = (TH2D*)inFileD0Jet->Get("sparsereco_0_proj_8_7");

    if(!hPtYD0MC || !hPtYD0reco ) { std::cerr << "D0 histos not extracted!!!" << std::endl;  return;}
    if(!hPtYD0JetMC || !hPtYD0Jetreco) { std::cerr << "D0 jet histos not extracted!!!" << std::endl;  return;}


    hPtYD0MC->SetTitle("");
    //hPtYD0MC->GetXaxis()->SetRangeUser(-1,1);
    //hPtYD0MC->GetYaxis()->SetRangeUser(0,5);
    hPtYD0MC->GetXaxis()->SetTitle("y D^{0}");
    hPtYD0MC->GetYaxis()->SetTitle("p_{T, D^{0}}");

    hPtYD0reco->SetTitle("");
    //hPtYD0reco->GetXaxis()->SetRangeUser(-1,1);
    //hPtYD0reco->GetYaxis()->SetRangeUser(0,5);
    hPtYD0reco->GetXaxis()->SetTitle("y D^{0}");
    hPtYD0reco->GetYaxis()->SetTitle("p_{T, D^{0}}");

    hPtYD0JetMC->SetTitle("");
    //hPtYD0JetMC->GetXaxis()->SetRangeUser(-1,1);
    //hPtYD0JetMC->GetYaxis()->SetRangeUser(0,5);
    hPtYD0JetMC->GetXaxis()->SetTitle("y D^{0}");
    hPtYD0JetMC->GetYaxis()->SetTitle("p_{T, D^{0}}");

    hPtYD0Jetreco->SetTitle("");
    //hPtYD0Jetreco->GetXaxis()->SetRangeUser(-1,1);
    //hPtYD0Jetreco->GetYaxis()->SetRangeUser(0,5);
    hPtYD0Jetreco->GetXaxis()->SetTitle("y D^{0}");
    hPtYD0Jetreco->GetYaxis()->SetTitle("p_{T, D^{0}}");


    TLegend *leg = new TLegend(0.5,0.7,0.85,0.85);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.045);

    TLegend *leg2 = new TLegend(0.5,0.53,0.85,0.68);
    leg2->SetBorderSize(0);
    leg2->SetTextSize(0.045);

    TPaveText *pvD = new TPaveText(0.15,0.7,0.35,0.8,"brNDC");
    pvD->SetFillStyle(0);
    pvD->SetBorderSize(0);
    pvD->SetTextFont(42);
    pvD->SetTextSize(0.045);
    pvD->SetTextAlign(11);



    TCanvas *cEffMCD0 = new TCanvas("cEffMCD0","cEffMCD0",1400,1000);
    hPtYD0MC->Draw("colz");
    pvD->AddText("D0 MC");
    pvD->Draw("same");
    cEffMCD0->SaveAs(Form("%s/DjetPtY_MCD0.pdf",outDir.Data()));
    cEffMCD0->SaveAs(Form("%s/DjetPtY_MCD0.png",outDir.Data()));


    TCanvas *cEffMCD0Jet = new TCanvas("cEffMCD0Jet","cEffMCD0Jet",1400,1000);
    hPtYD0JetMC->Draw("colz");
    pvD->Clear("");
    pvD->AddText("D0-jet MC");
    pvD->Draw("same");
    cEffMCD0Jet->SaveAs(Form("%s/DjetPtY_MCD0Jet.pdf",outDir.Data()));
    cEffMCD0Jet->SaveAs(Form("%s/DjetPtY_MCD0Jet.png",outDir.Data()));

    TCanvas *cEffrecoD0 = new TCanvas("cEffrecoD0","cEffrecoD0",1400,1000);
    hPtYD0reco->Draw("colz");
    pvD->Clear("");
    pvD->AddText("D0 reco");
    pvD->Draw("same");
    cEffrecoD0->SaveAs(Form("%s/DjetPtY_recoD0.pdf",outDir.Data()));
    cEffrecoD0->SaveAs(Form("%s/DjetPtY_recoD0.png",outDir.Data()));

    TCanvas *cEffrecoD0Jet = new TCanvas("cEffrecoD0Jet","cEffrecoD0Jet",1400,1000);
    hPtYD0Jetreco->Draw("colz");
    pvD->Clear("");
    pvD->AddText("D0-jet reco");
    pvD->Draw("same");
    cEffrecoD0Jet->SaveAs(Form("%s/DjetPtY_recoD0Jet.pdf",outDir.Data()));
    cEffrecoD0Jet->SaveAs(Form("%s/DjetPtY_recoD0Jet.png",outDir.Data()));


    pvD = new TPaveText(0.5,0.4,0.85,0.45,"brNDC");
    pvD->SetFillStyle(0);
    pvD->SetBorderSize(0);
    pvD->SetTextFont(42);
    pvD->SetTextSize(0.045);
    pvD->SetTextAlign(11);


double etacut = 0.3;
double ptdminInt = 2;

    TH1D *hPtD0MC = dynamic_cast<TH1D*>( hPtYD0MC->ProjectionY("hPtD0MC",hPtYD0MC->GetXaxis()->FindBin(-etacut),hPtYD0MC->GetXaxis()->FindBin(etacut)) );
    TH1D *hPtD0JetMC = dynamic_cast<TH1D*>( hPtYD0JetMC->ProjectionY("hPtD0JetMC",hPtYD0JetMC->GetXaxis()->FindBin(-etacut),hPtYD0JetMC->GetXaxis()->FindBin(etacut)) );
    TH1D *hPtD0reco = dynamic_cast<TH1D*>( hPtYD0reco->ProjectionY("hPtD0reco",hPtYD0reco->GetXaxis()->FindBin(-etacut),hPtYD0reco->GetXaxis()->FindBin(etacut)) );
    TH1D *hPtD0Jetreco = dynamic_cast<TH1D*>( hPtYD0Jetreco->ProjectionY("hPtD0Jetreco",hPtYD0Jetreco->GetXaxis()->FindBin(-etacut),hPtYD0Jetreco->GetXaxis()->FindBin(etacut)) );

    hPtD0MC->SetName("ptD0MC");
    hPtD0JetMC->SetName("ptD0JetMC");
    hPtD0reco->SetName("ptD0reco");
    hPtD0Jetreco->SetName("ptD0Jetreco");

    hPtD0MC->Rebin(5);
    hPtD0reco->Rebin(5);

    setHistoDetails(hPtD0MC,D0Color,20,1.2,1,1);
    setHistoDetails(hPtD0reco,D0Color,24,1.2,1,1);
    setHistoDetails(hPtD0JetMC,D0JetColor,21,1.2,1,1);
    setHistoDetails(hPtD0Jetreco,D0JetColor,25,1.2,1,1);

    hPtD0MC->GetXaxis()->SetRangeUser(ptdminInt,40);
    hPtD0reco->GetXaxis()->SetRangeUser(ptdminInt,40);
    hPtD0JetMC->GetXaxis()->SetRangeUser(ptdminInt,40);
    hPtD0Jetreco->GetXaxis()->SetRangeUser(ptdminInt,40);

    Double_t norm = 1;
    Double_t scale = norm/(hPtD0MC->Integral(hPtD0MC->FindBin(ptdminInt),hPtD0MC->FindBin(40)));
    hPtD0MC->Scale(scale);
    //hPtD0MC->Scale(1./41.81E6);
    scale = norm/(hPtD0reco->Integral(hPtD0reco->FindBin(ptdminInt),hPtD0reco->FindBin(40)));
    hPtD0reco->Scale(scale);
    scale = norm/(hPtD0JetMC->Integral(hPtD0JetMC->FindBin(ptdminInt),hPtD0JetMC->FindBin(40)));
    hPtD0JetMC->Scale(scale);
    //hPtD0JetMC->Scale(1./93.62E6);
    scale = norm/(hPtD0Jetreco->Integral(hPtD0Jetreco->FindBin(ptdminInt),hPtD0Jetreco->FindBin(40)));
    hPtD0Jetreco->Scale(scale);

    int bins = (40-ptdminInt)*2;
    TH1D *hPtRatioMC = new TH1D("hPtRatioMC","true pT ratio",bins,ptdminInt,40);
    TH1D *hPtRatioreco = new TH1D("hPtRatioreco","reco pT ratio",bins,ptdminInt,40);

    TH1D *hPtEffD0 = new TH1D("hPtEffD0","true pT ratio",bins,ptdminInt,40);
    TH1D *hPtEffD0Jet = new TH1D("hPtEffD0Jet","reco pT ratio",bins,ptdminInt,40);

    for(int i=1; i<hPtD0MC->GetNbinsX();i++){
        double ratio = 0;
        double binD0 = hPtRatioMC->GetXaxis()->GetBinCenter(i);
        double binD0Jet = hPtRatioMC->GetXaxis()->GetBinCenter(i);
        int binD0i = hPtD0MC->GetXaxis()->FindBin(binD0);
        int binD0Jeti = hPtD0JetMC->GetXaxis()->FindBin(binD0Jet);

        if (hPtD0MC->GetBinContent(binD0i)) ratio = hPtD0JetMC->GetBinContent(binD0Jeti) / hPtD0MC->GetBinContent(binD0i);
        else ratio = 0;
        hPtRatioMC->SetBinContent(i,ratio);
        if(hPtD0reco->GetBinContent(binD0i)) ratio = hPtD0Jetreco->GetBinContent(binD0Jeti) / hPtD0reco->GetBinContent(binD0i);
        else ratio = 0;
        hPtRatioreco->SetBinContent(i,ratio);

        if(hPtD0MC->GetBinContent(binD0i)) ratio = hPtD0reco->GetBinContent(binD0Jeti) / hPtD0MC->GetBinContent(binD0i);
        else ratio = 0;
        hPtEffD0->SetBinContent(i,ratio);

        if(hPtD0JetMC->GetBinContent(binD0i)) ratio = hPtD0Jetreco->GetBinContent(binD0Jeti) / hPtD0JetMC->GetBinContent(binD0i);
        else ratio = 0;
        hPtEffD0Jet->SetBinContent(i,ratio);


    }
    hPtRatioMC->SetLineColor(kRed);
    hPtRatioreco->SetLineColor(kBlue);

    hPtEffD0->SetLineColor(kRed);
    hPtEffD0Jet->SetLineColor(kBlue);


//            (TH1D*)hPtD0MC->Clone("hPtRatioMC");
 //   hPtRatioMC->Divide(hPtD0JetMC);

 //   TH1D *hPtRatioreco = (TH1D*)hPtD0reco->Clone("hPtRatioreco");
//    hPtRatioreco->Divide(hPtD0Jetreco);

    hPtRatioMC->SetTitle("");
    hPtRatioMC->GetYaxis()->SetRangeUser(0.5,1.6);
    hPtRatioMC->GetXaxis()->SetRangeUser(0,36);

    hPtRatioreco->SetTitle("");
    hPtRatioreco->GetYaxis()->SetRangeUser(0.5,1.6);
    hPtRatioreco->GetXaxis()->SetRangeUser(0,36);

    TCanvas *cRatio = new TCanvas("cRatio","cRatio",1400,1000);

     hPtRatioMC->Draw();
     hPtRatioreco->Draw("same");

     cRatio->SaveAs(Form("%s/PtRatios.pdf",outDir.Data()));
     cRatio->SaveAs(Form("%s/PtRatios.png",outDir.Data()));


     TCanvas *cE = new TCanvas("cE","cE",1400,1000);

      hPtEffD0->Draw();
      hPtEffD0Jet->Draw("same");

    std::cout << "Bin width: " << hPtD0MC->GetBinWidth(1) << "\t " << hPtD0JetMC->GetBinWidth(1) << std::endl;

    hPtD0JetMC->SetMaximum(hPtD0JetMC->GetMaximum()*10);
    hPtD0JetMC->SetMinimum(hPtD0JetMC->GetMinimum()*0.1);

    TCanvas *cPt = new TCanvas("cPt","cPt",1400,1000);
    cPt->SetLogy();
    hPtD0JetMC->Draw();
    hPtD0reco->Draw("same");
    hPtD0Jetreco->Draw("same");
    hPtD0MC->Draw("same");

    leg->AddEntry(hPtD0MC,"D0 MC","p");
    leg->AddEntry(hPtD0JetMC,"D0-jet MC,|#eta^{jet}|<0.5","p");
    leg->Draw("same");

    leg2->AddEntry(hPtD0reco,"D0 reco","p");
    leg2->AddEntry(hPtD0Jetreco,"D0-jet reco,|#eta^{jet}|<0.5","p");
    leg2->Draw("same");

    TPaveText *pvY = new TPaveText(0.12,0.15,0.35,0.2,"brNDC");
    pvY->SetFillStyle(0);
    pvY->SetBorderSize(0);
    pvY->SetTextFont(42);
    pvY->SetTextSize(0.045);
    pvY->SetTextAlign(11);
    pvY->AddText("|y^{D0}|<0.5");
    pvY->Draw("same");

    cPt->SaveAs(Form("%s/DPtComparison.pdf",outDir.Data()));
    cPt->SaveAs(Form("%s/DPtComparison.png",outDir.Data()));

    fEffD->cd();
    hPtD0JetMC->Write();
    hPtD0reco->Write();
    hPtD0Jetreco->Write();
    hPtD0MC->Write();


return;

    const int ptbin = 4;
    double ptdminArray[ptbin] = {0,2,5,12};
    TH1F *hEffD0, *hEffD0Jet;
    TH1F *hEtaD0MC, *hEtaD0reco, *hEtaD0JetMC, *hEtaD0Jetreco;

    for(int i=0;i<ptbin;i++){

        int ptdminBin = (int)ptdminArray[i];

        TH1D *hEtaD0MC = dynamic_cast<TH1D*>( hPtYD0MC->ProjectionX("hEtaD0MC",hPtYD0MC->GetXaxis()->FindBin(ptdminBin),hPtYD0MC->GetXaxis()->FindBin(36)) );
        TH1D *hEtaD0reco = dynamic_cast<TH1D*>( hPtYD0JetMC->ProjectionX("hEtaD0reco",hPtYD0JetMC->GetXaxis()->FindBin(ptdminBin),hPtYD0JetMC->GetXaxis()->FindBin(036)) );
        TH1D *hEtaD0JetMC = dynamic_cast<TH1D*>( hPtYD0reco->ProjectionX("hEtaD0JetMC",hPtYD0reco->GetXaxis()->FindBin(ptdminBin),hPtYD0reco->GetXaxis()->FindBin(36)) );
        TH1D *hEtaD0Jetreco = dynamic_cast<TH1D*>( hPtYD0Jetreco->ProjectionX("hEtaD0Jetreco",hPtYD0Jetreco->GetXaxis()->FindBin(ptdminBin),hPtYD0Jetreco->GetXaxis()->FindBin(36)) );

        setHistoDetails(hEtaD0MC,D0Color,20,1.2,1,1);
        setHistoDetails(hEtaD0reco,D0Color,24,1.2,1,1);
        setHistoDetails(hEtaD0JetMC,D0JetColor,21,1.2,1,1);
        setHistoDetails(hEtaD0Jetreco,D0JetColor,25,1.2,1,1);

        Double_t norm = 1;
        Double_t scale = norm/(hEtaD0MC->Integral(hEtaD0MC->FindBin(0),hEtaD0MC->FindBin(40)));
        hEtaD0MC->Scale(scale);
        scale = norm/(hEtaD0reco->Integral(hEtaD0reco->FindBin(0),hEtaD0reco->FindBin(40)));
        hEtaD0reco->Scale(scale);
        scale = norm/(hEtaD0JetMC->Integral(hEtaD0JetMC->FindBin(0),hEtaD0JetMC->FindBin(40)));
        hEtaD0JetMC->Scale(scale);
        scale = norm/(hEtaD0Jetreco->Integral(hEtaD0Jetreco->FindBin(0),hEtaD0Jetreco->FindBin(40)));
        hEtaD0Jetreco->Scale(scale);

        TCanvas *cEtaMC = new TCanvas("cEtaMC","cEtaMC",1400,1000);
        hEtaD0MC->Draw();;
        hEtaD0JetMC->Draw("same");

        leg = new TLegend(0.1,0.75,0.35,0.9);
        leg->SetBorderSize(0);
        leg->SetTextSize(0.04);
        leg->AddEntry(hEtaD0MC,"D0 MC","p");
        leg->AddEntry(hEtaD0JetMC,"D0-jet MC,|#eta^{jet}|<0.5","p");
        leg->Draw("same");

        pvD->Clear("");
        pvD->AddText(Form("p_{T,D0} > %d",ptdminBin));
        pvD->Draw("same");

        cEtaMC->SaveAs(Form("%s/DjetEtaMC_pTD%d.pdf",outDir.Data(),ptdminBin));
        cEtaMC->SaveAs(Form("%s/DjetEtaMC_pTD%d.png",outDir.Data(),ptdminBin));


        TCanvas *cEtareco = new TCanvas("cEtareco","cEtareco",1400,1000);
        hEtaD0reco->Draw();
        hPtD0Jetreco->Draw("same");

        leg = new TLegend(0.12,0.75,0.35,0.9);
        leg->SetBorderSize(0);
        leg->SetTextSize(0.04);
        leg->AddEntry(hEtaD0reco,"D0 reco","p");
        leg->AddEntry(hEtaD0Jetreco,"D0-jet reco,|#eta^{jet}|<0.5","p");
        leg->Draw("same");


        pvD->Clear("");
        pvD->AddText(Form("p_{T,D0} > %d",ptdminBin));
        pvD->Draw("same");

        cEtareco->SaveAs(Form("%s/DjetEtaReco_pTD%d.pdf",outDir.Data(),ptdminBin));
        cEtareco->SaveAs(Form("%s/DjetEtaReco_pTD%d.png",outDir.Data(),ptdminBin));

        //getEfficiency(hEffD0,hEffD0Jet,-1,1,true,ptdminBin);
    }

}

void setHistoDetails(TH1 *h, Color_t color, Style_t Mstyle, Size_t Msize, Width_t width, bool isIntegral){

    h->SetTitle("");
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    h->SetMarkerStyle(Mstyle);
    h->SetMarkerSize(Msize);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetXaxis()->SetTitleOffset(1.);
    h->GetYaxis()->SetTitleOffset(1.);
    h->GetYaxis()->SetLabelSize(0.045);
    h->GetYaxis()->SetTitleSize(0.05);



    return;

}
