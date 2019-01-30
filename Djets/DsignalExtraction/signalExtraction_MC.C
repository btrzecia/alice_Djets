/**************************************************************************
 * Copyright(c) 1998-2017, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

 // Extraction of raw (or efficiency-corrected) D-jet pT spectrum
 // inv. mass method: side-band
//
//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

const int ptDbinsN = 10;
double ptDbins[ptDbinsN+1] = { 3,4,5,6,7,8,10,12,16,24,36 };

#include "signalExtraction.h"

void signalExtraction_MC(
  TString dataFile = "/home/basia/Work/alice/analysis/pPb_run2/D0jet/outMC/AnalysisResults_",
  TString lhcprod = "fast_R03_D0MCPythia_default", // if one file: e.g. LHC16k, LHC16kl ... ; for more than one file: LHC16: -- pattern of the out file is: PATH_TO_OUTFILE/AnalysiResultsLHCPROD
  TString out = "signalExtraction",
  bool isMoreFiles = 0, TString prod = "kl",    // for more than 1 file, for one file leave it empty
  bool save = 1,
  bool postfix = 0, TString listName = "Cut"
)

{
    TString plotsDir="/plots";
    TString outdir = out;
    gSystem->Exec(Form("mkdir %s",outdir.Data()));
    gSystem->Exec(Form("mkdir %s%s",outdir.Data(),plotsDir.Data()));

    TString histName;
    if(fDmesonSpecie) histName = "histosDStarMBN";
    else histName = "histosD0MBN";
    // get analysis output file
    TString datafile;
    TFile *File;
    TDirectoryFile* dir;
    TList *histList;
    THnSparseF *sparse;

      datafile = dataFile;
      datafile += lhcprod;
      datafile += ".root";
      File = new TFile(datafile,"read");
      if(!File) { cout << "==== WRONG FILE WITH DATA =====\n\n"; return ;}
      dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");

      for(int i=0;i<2; i++){

        histList =  (TList*)dir->Get(Form("%s%dMCrec",histName.Data(),i));
          sparse = (THnSparseF*)histList->FindObject("hsDphiz");
          sparse->GetAxis(0)->SetRangeUser(zmin,zmax);
          sparse->GetAxis(1)->SetRangeUser(jetmin,jetmax);
          sparse->GetAxis(6)->SetRangeUser(-1,0);
          if(isEta) sparse->GetAxis(5)->SetRangeUser(-jetEta,jetEta);
          if(i==0) hInvMassptD=(TH3D*)sparse->Projection(3,1,2);
          else hInvMassptD->Add((TH3D*)sparse->Projection(3,1,2));
      }


    // --------------------------------------------------------
    // fit inv. mass in D pT bins and get raw jet pT spectra in the signal and side-bands regions
     Bool_t okSignalExt = rawJetSpectra(outdir,lhcprod+prod);
     if(!okSignalExt) { std::cout << "!!!!!! Something wrong in the raw signal extraction !!!!" << endl; return; }
    // --------------------------------------------------------

    // --------------------------------------------------------
   //------------------- draw output histos -------------------
  /*  if(savePlots){
      saveFitParams(outdir,lhcprod+prod);
      saveSpectraPlots(outdir,lhcprod+prod);
    }

    // --------------------------------------------------------
    // ----------- write to output file
    TFile *ofile = new TFile(Form("%s/JetPtSpectra_SB_%s.root",outdir.Data(), bEff ? "eff" : "noEff"),"RECREATE");
    hmean->Write();
    hsigma->Write();
    hsign->Write();
    hsb->Write();
    hSignal->Write();
    hrelErr->Write();

    hjetptspectrum->Write();
    hjetptspectrumReb->Write();
    hjetptspectrumRebUnc->Write();

    for(int i=0; i<ptDbinsN; i++){
        if(hjetpt[i]) hjetpt[i]->Write();
        if(hjetpt_sb[i]) hjetpt_sb[i]->Write();
        if(hjetptsub[i]) hjetptsub[i]->Write();
        if(hmass[i]) hmass[i]->Write();
        if(hmass_l[i]) hmass_l[i]->Write();
        if(hmass_u[i]) hmass_u[i]->Write();
        if(hmass_c[i]) hmass_c[i]->Write();
        if(fullfit[i]) fullfit[i]->Write();
        if(massfit[i]) massfit[i]->Write();
        if(bkgfit[i]) bkgfit[i]->Write();
        if(bkgRfit[i] && fUseRefl && fDmesonSpecie == 0) bkgRfit[i]->Write();
        if(hjetptcorr[i]) hjetptcorr[i]->Write();
    }

    ofile->Close();
    // --------------------------------------------------------
*/
}

Bool_t rawJetSpectra(TString outdir, TString prod){

    hmean = new TH1F("hmean","hmean",ptDbinsN,ptDbins);
    hsigma = new TH1F("hsigma","hsigma",ptDbinsN,ptDbins);
    hrelErr = new TH1F("hrelErr","hrelErr",ptDbinsN,ptDbins);
    hsign = new TH1F("hsign","hsign",ptDbinsN,ptDbins);
    hsb = new TH1F("hsb","hsb",ptDbinsN,ptDbins);
    hSignal = new TH1F("hSignal","hSignal",ptDbinsN,ptDbins);
    hSignal->Sumw2();
    hReflRS = new TH1F("hReflRS","hReflRS",ptDbinsN,ptDbins);

    if(fDmesonSpecie) hInvMassptD->GetXaxis()->SetTitle(Form("m(%s)(GeV/c^{2})","K#pi#pi-K#pi"));
    else hInvMassptD->GetXaxis()->SetTitle(Form("m(%s)(GeV/c^{2})","K#pi"));
    hInvMassptD->GetXaxis()->SetTitleSize(0.06);
    hInvMassptD->GetXaxis()->SetTitleOffset(0.9);
    hInvMassptD->GetYaxis()->SetTitle("p_{T}^{jet}");
    hInvMassptD->SetTitle();

    TPaveText *pvProd = new TPaveText(0.8,0.25,0.98,0.3,"brNDC");
    pvProd->SetFillStyle(0);
    pvProd->SetBorderSize(0);
    pvProd->AddText(Form("%s",prod.Data()));

    TPaveText *pvCuts = new TPaveText(0.8,0.3,0.98,0.35,"brNDC");
    pvCuts->SetFillStyle(0);
    pvCuts->SetBorderSize(0);
    pvCuts->AddText(Form("%s",outdir.Data()));

    TPaveText *pvEn= new TPaveText(0.2,0.80,0.8,0.85,"brNDC");
    pvEn->SetFillStyle(0);
    pvEn->SetBorderSize(0);
    pvEn->SetTextFont(42);
    pvEn->SetTextSize(0.075);
    pvEn->SetTextAlign(11);
    pvEn->AddText(Form("%s",fSystemS.Data()));

    double shift = 0.;
    TPaveText *pvD = new TPaveText(0.15,0.65-shift,0.9,0.7-shift,"brNDC");
    pvD->SetFillStyle(0);
    pvD->SetBorderSize(0);
    pvD->SetTextFont(42);
    pvD->SetTextSize(0.085);
    pvD->SetTextAlign(11);
    if(fDmesonSpecie) pvD->AddText("D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
    else pvD->AddText("D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

    TPaveText *pvJet = new TPaveText(0.15,0.55-shift,0.9,0.6-shift,"brNDC");
    pvJet->SetFillStyle(0);
    pvJet->SetBorderSize(0);
    pvJet->SetTextFont(42);
    pvJet->SetTextSize(0.085);
    pvJet->AddText(Form("in Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));
    pvJet->SetTextAlign(11);

    TPaveText *pvEta = new TPaveText(0.15,0.45-shift,0.8,0.5-shift,"brNDC");
    pvEta->SetFillStyle(0);
    pvEta->SetBorderSize(0);
    pvEta->SetTextFont(42);
    pvEta->SetTextSize(0.085);
    pvEta->SetTextAlign(11);
    pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

    int xnx = 3, xny=4;
    if(ptDbinsN>4 && ptDbinsN<7) { xnx = 2; xny=3; }
    else if(ptDbinsN>6 && ptDbinsN<10) { xnx = 3; xny=3; }
    else if(ptDbinsN>9 && ptDbinsN<13) { xnx = 3; xny=4; }
    else { xnx = 4; xny=4; }

    TCanvas *c2 = new TCanvas("c2","c2",1600,1600);
    c2->Divide(xnx,xny);

    int firstPtBin = 0;
    if(ptDbins[0] == 2) firstPtBin = 3;
    else if(ptDbins[0] == 3) firstPtBin = 4;
    else if(ptDbins[0] == 4) firstPtBin = 5;
    else if(ptDbins[0] == 5) firstPtBin = 6;
    else if(ptDbins[0] == 6) firstPtBin = 7;
    if(!firstPtBin) { std::cout << "==== Wrong first value of the D pT (should be 2,3 or 4) === \n"; return kFALSE; }
    Float_t RS = 0;

    for(int i=0; i<ptDbinsN; i++){

        TH1F *hh=(TH1F*)hInvMassptD->ProjectionX(Form("hh_%d",i),hInvMassptD->GetYaxis()->FindBin(jetmin), hInvMassptD->GetYaxis()->FindBin(jetmax)-1,hInvMassptD->GetZaxis()->FindBin(ptDbins[i]), hInvMassptD->GetZaxis()->FindBin(ptDbins[i+1])-1);
        hh->Rebin(fRebinMass);

        hh->GetXaxis()->SetRangeUser(minf,maxf);
        hh->SetTitle(Form("%.1lf < pt^{%s} < %.1lf",ptDbins[i],fDmesonS.Data(),ptDbins[i+1]));

        TH1F *hmassfit = (TH1F*)hh->Clone("hmassfit");
        if(fDmesonSpecie) hmassfit->SetMaximum(hmassfit->GetMaximum()*1.3);

        float hmin = TMath::Max(minf,hmassfit->GetBinLowEdge(2));
        float hmax = TMath::Min(maxf,hmassfit->GetBinLowEdge(hmassfit->GetNbinsX()));

        TF1 *fitG = new TF1("fitG","gaus",1.82,1.91);
        hh->Fit("fitG","0RM");
        fitG->SetLineColor(2);
          c2->cd(i+1);
          TVirtualPad *pad = (TVirtualPad*)c2->GetPad(i+1);
          hh->Draw();
          fitG->Draw("same");

          hsigma->SetBinContent(i+1,fitG->GetParameter(2));
          hsigma->SetBinError(i+1,fitG->GetParError(2));
          hmean->SetBinContent(i+1,fitG->GetParameter(1));
          hmean->SetBinError(i+1,fitG->GetParError(1));

      /*
        if(fUseRefl && fDmesonSpecie == 0) {
          SetReflection(fitterp,i+firstPtBin,hmin,hmax,RS);
        }

*/

    }

    hsigma->SetName("hsigma");
    hmean->SetName("hmean");
    TCanvas *cSigma = new TCanvas;
    hsigma->Draw();

    TCanvas *cMean = new TCanvas;
    hmean->Draw();

    TFile *oFile = new TFile("MCsignal.root","RECREATE");
    c2->Write();
    hsigma->Write();
    hmean->Write();
    oFile->Close();
    
    return kTRUE;

}

Bool_t SetReflection(AliHFInvMassFitter* &fitter, Int_t iBin, Float_t fLeftFitRange, Float_t fRightFitRange, Float_t &RS) {

  TFile *fileRefl = TFile::Open(fReflFilename.Data());
  if(!fileRefl){
    std::cout << "File " << fReflFilename << " (reflection templates) cannot be opened! check your file path!"; return kFALSE;
  }

  TString fHistnameRefl = "histRflFittedDoubleGaus_ptBin";
  TString fHistnameSign = "histSgn_";
  TH1F *histRefl = (TH1F*)fileRefl->Get(Form("%s%d",fHistnameRefl.Data(),iBin));
  TH1F *histSign = (TH1F*)fileRefl->Get(Form("%s%d",fHistnameSign.Data(),iBin));
  if(!histRefl || !histSign){
    std::cout << "Error in loading the template/signal histrograms! Exiting..." << endl; return kFALSE;
  }

  fitter->SetTemplateReflections(histRefl,"template",fLeftFitRange,fRightFitRange);
  Double_t RoverS = histRefl->Integral(histRefl->FindBin(fLeftFitRange),histRefl->FindBin(fRightFitRange))/histSign->Integral(histSign->FindBin(fLeftFitRange),histSign->FindBin(fRightFitRange));
  printf("R/S ratio in fit range for bin %d = %1.3f\n",iBin,RoverS);
  fitter->SetFixReflOverS(RoverS);

  RS = (Float_t)RoverS;
  return kTRUE;

}

void  saveSpectraPlots(TString outdir,TString prod){

      TPaveText *pvProd = new TPaveText(0.75,0.65,0.9,0.7,"brNDC");
      pvProd->SetFillStyle(0);
      pvProd->SetBorderSize(0);
      pvProd->AddText(Form("%s",prod.Data()));

      TPaveText *pvCuts = new TPaveText(0.75,0.6,0.9,0.65,"brNDC");
      pvCuts->SetFillStyle(0);
      pvCuts->SetBorderSize(0);
      pvCuts->AddText(Form("%s",outdir.Data()));

      TPaveText *pvEn= new TPaveText(0.2,0.80,0.8,0.85,"brNDC");
      pvEn->SetFillStyle(0);
      pvEn->SetBorderSize(0);
      pvEn->SetTextFont(42);
      pvEn->SetTextSize(0.045);
      pvEn->SetTextAlign(11);
      pvEn->AddText(Form("%s",fSystemS.Data()));

      double shift = -0.05;
      TPaveText *pvJet = new TPaveText(0.52,0.65-shift,0.9,0.7-shift,"brNDC");
      pvJet->SetFillStyle(0);
      pvJet->SetBorderSize(0);
      pvJet->SetTextFont(42);
      pvJet->SetTextSize(0.04);
      pvJet->SetTextAlign(11);
      pvJet->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

      shift+=0.07;
      TPaveText *pvD = new TPaveText(0.52,0.65-shift,0.9,0.7-shift,"brNDC");
      pvD->SetFillStyle(0);
      pvD->SetBorderSize(0);
      pvD->SetTextFont(42);
      pvD->SetTextSize(0.04);
      pvD->SetTextAlign(11);
      if(fDmesonSpecie) pvD->AddText("with D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
      else pvD->AddText("with D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

      shift+=0.07;
      TPaveText *pvEta = new TPaveText(0.52,0.65-shift,0.9,0.7-shift,"brNDC");
      pvEta->SetFillStyle(0);
      pvEta->SetBorderSize(0);
      pvEta->SetTextFont(42);
      pvEta->SetTextSize(0.04);
      pvEta->SetTextAlign(11);
      pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

      shift+=0.07;
      TPaveText *pv3 = new TPaveText(0.52,0.65-shift,0.9,0.7-shift,"brNDC");
      pv3->SetFillStyle(0);
      pv3->SetBorderSize(0);
      pv3->SetTextFont(42);
      pv3->SetTextSize(0.04);
      pv3->SetTextAlign(11);
      pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)ptDbins[0],fDmesonS.Data(),(Int_t)ptDbins[ptDbinsN]));

       // before eff. correction
      TCanvas *cRawSpectrum = new TCanvas("cRawSpectrum","cRawSpectrum",800,600);
      setHistoDetails(hrawjetptspectrum,0,kBlue+1,20,1.2);
      hrawjetptspectrum->GetXaxis()->SetTitle("p_{T}^{ch,jet} (GeV/c)");
      hrawjetptspectrum->GetYaxis()->SetTitle("Raw dN/dp_{T}");
      hrawjetptspectrum->GetXaxis()->SetRangeUser(jetplotmin,jetplotmax);
      hrawjetptspectrum->Draw();
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      pvEn->Draw("same");

      hjetptspectrum->GetYaxis()->SetTitle("dN/dp_{T}");
      hjetptspectrum->GetXaxis()->SetRangeUser(jetplotmin,jetplotmax);

      TCanvas *cSpectrum = new TCanvas("cSpectrum","cSpectrum",800,600);
      cSpectrum->SetLogy();
      setHistoDetails(hjetptspectrum,0,kRed,20,1.2);
      hjetptspectrum->SetMinimum(1);
      hjetptspectrum->GetXaxis()->SetTitle("p_{T,ch jet} (GeV/c)");
      hjetptspectrum->Draw();
      pv3->Draw("same");
      pvEn->Draw("same");
      pvD->Draw("same");
      pvJet->Draw("same");
      pvEta->Draw("same");
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      if(savePlots) SaveCanvas(cSpectrum,outdir+plotsDir+"/jetPtSpectrum_SB"+prod);

      TH1F *hjetptspectrumReb_tmp = (TH1F*)hjetptspectrum->Clone("hjetptspectrumReb_tmp");
      hjetptspectrumReb = (TH1F*)hjetptspectrumReb_tmp->Rebin(fptbinsJetMeasN,"hjetptspectrumReb",fptbinsJetMeasA);
      TH1F* hjetptspectrumReb2 = (TH1F*)hjetptspectrumReb_tmp->Rebin(fptbinsJetMeasN,"hjetptspectrumReb",fptbinsJetMeasA);
      setHistoDetails(hjetptspectrumReb,0,kBlue,20,1.2); // with bin width scaling
      setHistoDetails(hjetptspectrumReb2,1,kBlue,20,1.2); // with bin width scaling
      hjetptspectrumReb->GetXaxis()->SetTitle("p_{T,ch jet} (GeV/c)");
      hjetptspectrumReb2->GetXaxis()->SetTitle("p_{T,ch jet} (GeV/c)");
      TCanvas *cSpectrumRebin = new TCanvas("cSpectrumRebin","cSpectrumRebin",800,600);
      cSpectrumRebin->SetLogy();
      hjetptspectrumReb2->Draw();
      pvEn->Draw("same");
      pvD->Draw("same");
      pvJet->Draw("same");
      pvEta->Draw("same");
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      pv3->Draw("same");

      SaveCanvas(cSpectrumRebin,outdir+plotsDir+"/jetPtSpectrum_SB_Rebin"+prod);

      hjetptspectrumRebUnc = (TH1F*)hjetptspectrumReb->Clone("hjetptspectrumRebUnc");
      hjetptspectrumRebUnc->GetYaxis()->SetTitle("Rel. unc.");


      for(int j=1; j<=hjetptspectrumReb->GetNbinsX();j++){
                  double err;
                  if(hjetptspectrumReb->GetBinContent(j)) err = hjetptspectrumReb->GetBinError(j)/hjetptspectrumReb->GetBinContent(j);
                  else err = 0;
                  hjetptspectrumRebUnc->SetBinContent(j,err);
                  hjetptspectrumRebUnc->SetBinError(j,0);
      }

      hjetptspectrumRebUnc->SetMinimum(0);
      hjetptspectrumRebUnc->SetMaximum(hjetptspectrumRebUnc->GetMaximum()*1.2);

      double shift = 0;
      TPaveText *pvJet = new TPaveText(0.15,0.65-shift,0.9,0.7-shift,"brNDC");
      pvJet->SetFillStyle(0);
      pvJet->SetBorderSize(0);
      pvJet->SetTextFont(42);
      pvJet->SetTextSize(0.04);
      pvJet->SetTextAlign(11);
      if(fDmesonSpecie) pvJet->AddText("D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
      else pvJet->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

      TPaveText *pvD = new TPaveText(0.15,0.58-shift,0.9,0.63-shift,"brNDC");
      pvD->SetFillStyle(0);
      pvD->SetBorderSize(0);
      pvD->SetTextFont(42);
      pvD->SetTextSize(0.04);
      pvD->SetTextAlign(11);
      if(fDmesonSpecie) pvD->AddText("with D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
      else pvD->AddText("with D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

      TPaveText *pvEta = new TPaveText(0.15,0.51-shift,0.8,0.56-shift,"brNDC");
      pvEta->SetFillStyle(0);
      pvEta->SetBorderSize(0);
      pvEta->SetTextFont(42);
      pvEta->SetTextSize(0.04);
      pvEta->SetTextAlign(11);
      pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

      TPaveText *pv3 = new TPaveText(0.15,0.44,0.9,0.49,"brNDC");
      pv3->SetFillStyle(0);
      pv3->SetBorderSize(0);
      pv3->SetTextFont(42);
      pv3->SetTextSize(0.04);
      pv3->SetTextAlign(11);
      pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)ptDbins[0],fDmesonS.Data(),(Int_t)ptDbins[ptDbinsN]));

      TCanvas *cSpectrumRebinUnc = new TCanvas("cSpectrumRebinUnc","cSpectrumRebinUnc",800,500);

      //hjetptspectrumRebUnc->GetXaxis()->SetRangeUser(jetplotmin,jetplotmax);
      hjetptspectrumRebUnc->Draw();
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      pvEn->Draw("same");
      pv3->Draw("same");
      pvD->Draw("same");
      pvJet->Draw("same");
      pvEta->Draw("same");

      SaveCanvas(cSpectrumRebinUnc,outdir+plotsDir+"/jetPtSpectrumUnc_SB_Rebin"+prod);

}


void  saveFitParams(TString outdir,TString prod){

    TPaveText *pvProd = new TPaveText(0.78,0.75,0.9,0.8,"brNDC");
    pvProd->SetFillStyle(0);
    pvProd->SetBorderSize(0);
    pvProd->AddText(Form("%s",prod.Data()));

    TPaveText *pvCuts = new TPaveText(0.78,0.8,0.9,0.85,"brNDC");
    pvCuts->SetFillStyle(0);
    pvCuts->SetBorderSize(0);
    pvCuts->AddText(Form("%s",outdir.Data()));

    TPaveText *pvEn= new TPaveText(0.25,0.80,0.8,0.85,"brNDC");
    pvEn->SetFillStyle(0);
    pvEn->SetBorderSize(0);
    pvEn->SetTextFont(42);
    pvEn->SetTextSize(0.045);
    pvEn->SetTextAlign(11);
    pvEn->AddText(Form("%s",fSystemS.Data()));

    setHistoDetails(hmean,0,2,20);
    if(fDmesonSpecie) hmean->GetYaxis()->SetTitle("signal #mu (MeV/c^{2})");
    else hmean->GetYaxis()->SetTitle("signal #mu (GeV/c^{2})");
    setHistoDetails(hsigma,0,4,20);
    hsigma->GetYaxis()->SetTitle("signal #sigma (MeV/c^{2})");
    setHistoDetails(hsign,0,4,20);
    hsign->GetYaxis()->SetTitle("significance");
    setHistoDetails(hsb,0,8,20);
    hsb->GetYaxis()->SetTitle("S/B");
    setHistoDetails(hrelErr,0,6,20);
    hrelErr->GetYaxis()->SetTitle("rel. unc.");

    hmean->GetYaxis()->SetTitleOffset(1.8);


    if(fDmesonSpecie) {
      hmean->GetYaxis()->SetRangeUser(144.8,146);
      hsigma->GetYaxis()->SetRangeUser(0.35,0.8);
    }
    else {
      hmean->GetYaxis()->SetRangeUser(1.855,1.885);
      hsigma->GetYaxis()->SetRangeUser(9,20);
    }

    hSignal->SetName("hSignal");
    setHistoDetails(hSignal,0,2,20);
    hSignal->GetYaxis()->SetTitle("yield");
    hSignal->GetYaxis()->SetTitleOffset(1.6);
    hrelErr->GetYaxis()->SetTitleOffset(1.6);


    double mean = 145.421;
    TLine *lm = new TLine(ptDbins[0], mean, ptDbins[ptDbinsN], mean);
    lm->SetLineStyle(2);

    TCanvas *cMassFit = new TCanvas("cMassFit","cMassFit",1200,600);
    cMassFit->Divide(2,1);
    cMassFit->cd(1);
    gPad->SetLeftMargin(0.15);
    hmean->Draw();
    pvEn->Draw("same");
    if(isdetails) pvProd->Draw("same");
    if(isdetails) pvCuts->Draw("same");
    lm->Draw("same");
    cMassFit->cd(2);
    hsigma->Draw();
    if(isdetails) pvProd->Draw("same");
    if(isdetails) pvCuts->Draw("same");

     if(savePlots)  SaveCanvas(cMassFit,outdir+plotsDir+"/gaussianParams"+prod);

    TCanvas *cSignal = new TCanvas("cSignal","cSignal",1200,1200);
    cSignal->Divide(2,2);

    cSignal->cd(1);
    hSignal->Draw();
    if(isdetails) pvProd->Draw("same");
    if(isdetails) pvCuts->Draw("same");
    cSignal->cd(2);
    hsign->Draw();
    pvEn->Draw("same");
    if(isdetails) pvProd->Draw("same");
    if(isdetails) pvCuts->Draw("same");
    cSignal->cd(3);
    hrelErr->Draw();
    if(isdetails) pvProd->Draw("same");
    if(isdetails) pvCuts->Draw("same");
    cSignal->cd(4);
    hsb->Draw();
    if(isdetails) pvProd->Draw("same");
    if(isdetails) pvCuts->Draw("same");

    if(savePlots) SaveCanvas(cSignal,outdir+plotsDir+"/signalParams"+prod);

    if(hReflRS && fUseRefl && fDmesonSpecie == 0) {
      setHistoDetails(hReflRS,0,kGreen+2,20);
      hReflRS->GetYaxis()->SetTitle("R/S");
      hReflRS->SetMinimum(hReflRS->GetMinimum()*0.5);
      hReflRS->SetMaximum(hReflRS->GetMaximum()*1.2);
      TCanvas *cRS = new TCanvas("cRS","cRS",800,600);
      cRS->cd();
      hReflRS->Draw();
      if(savePlots) SaveCanvas(cRS,outdir+plotsDir+"/RefOverS"+prod);
    }

}
void setHistoDetails(TH1 *h, int scale, Color_t color, Style_t Mstyle, Size_t size = 0.9, Width_t width=2){

    if(scale)h->Scale(1,"width");
    h->SetMarkerStyle(Mstyle);
    h->SetMarkerColor(color);
    h->SetMarkerSize(size);
    h->SetLineColor(color);
    h->SetLineWidth(width);
    h->SetTitle(0);
    h->GetXaxis()->SetTitle(Form("p_{T,%s}(GeV/c)",fDmesonS.Data()));


    return;
}

void SaveCanvas(TCanvas *c, TString name = "tmp"){

    c->SaveAs(Form("%s_pTD%d.png",name.Data(),(int)ptDbins[0]));
    c->SaveAs(Form("%s_pTD%d.pdf",name.Data(),(int)ptDbins[0]));
}

void setStyle(){


}
