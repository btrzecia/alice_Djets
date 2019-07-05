//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include "config.h"

double jetEta = 0.9 - fRpar;
double dy = 2*jetEta;

double *sysCutVar, *systuncP;
double DTrackEff = 0.03;
double globalUnc = 0.038;
Double_t *xAxis;
double plotmin = 5, plotmax=50;

Int_t colors[] = {1,8,4,2,6,kOrange-1,kGray+2,kCyan+1,kMagenta+2,kViolet+5,kYellow+2};
Int_t markers[] = {20,21,22,23,24,25,26,27,28,29,30,32,33,34};
Int_t linestyle[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};

TGraphAsymmErrors *grsystheory, *grsys, *grsysRatio, *grsystheoryratio;
TH1D *hData_binned, *hData_binned_ratio;
TH1D *hPrompt_central_binned, *hPrompt_up, *hPrompt_down;


TH1* GetUpSys(TH1D **hh, const int nFiles, TH1D *hh_up);
TH1* GetDownSys(TH1D **hh, const int nFiles, TH1D *hh_down);
TH1* GetInputHist(TString inFile, TString histName,TH1 *hh);
void ScaleHist(TH1 *hh, int full = 0);
void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, int Msize = 1.1, Width_t Lwidth = 2, Style_t Lstyle = 1);
void SaveCanvas(TCanvas *c, TString name = "tmp");
void drawFinal(TString outPlotDir);

TH1D *CentralPointsStatisticalUncertainty__4;
TH1D *GeneratorLevel_JetPtSpectrum__3;

bool isSimSys = 1, isSys = 1, isSim = 1;
TString sysUncDir;

void finalJetSpectra(
TString dataFile = "/home/basia/Work/alice/analysis/pPb_run2/DzeroR03_RefDPt3PythiaEff_BaseCuts/Default_jetMeas3_50_jetTrue3_50_ppbinning/unfolding_Bayes_3/unfoldedSpectrum_unfoldedJetSpectrum.root",
TString dataAnalysisFile = "/home/basia/Work/alice/analysis/pPb_run2/D0jet/outData/AnalysisResults_LHC16R03.root",
TString simDir = "/home/basia/Work/alice/analysis/pPb_run2/DzeroR03_RefDPt3PythiaEff_BaseCuts/Simulations/Prompt",
TString outSpectraDir = "/home/basia/Work/alice/analysis/pPb_run2/DzeroR03_RefDPt3PythiaEff_BaseCuts/Default_jetMeas3_50_jetTrue3_50_ppbinning/unfolding_Bayes_3/finalSpectra",
TString sysDir = "/home/basia/Work/alice/analysis/pPb_run2/DzeroR03_RefDPt3PythiaEff_BaseCuts/Default_jetMeas3_50_jetTrue3_50_ppbinning/systematics",
bool issys = 1,
bool issim = 1,
bool simsys = 1,
TString listName = "",
bool oldCounter = 0,
TString histBase = "unfoldedSpectrum"

)
{
    isSimSys = simsys;
    isSys = issys;
    isSim = issim;
    sysUncDir = sysDir;
    TString outPlotDir = outSpectraDir;
    outPlotDir+="/plots";
    gSystem->Exec(Form("mkdir %s",outSpectraDir.Data()));
    gSystem->Exec(Form("mkdir %s",outPlotDir.Data()));

    xAxis = new double[fptbinsJetFinalN+1];
    for(int k=0; k<fptbinsJetFinalN+1; k++) xAxis[k] = fptbinsJetFinalA[k];
    systuncP = new double[fptbinsJetFinalN];
//    for(int k=0; k<fptbinsJetFinalN; k++) systuncP[k] = 0.15;
//    systuncErr_pp[ptbinsN] = {0.104599235179,0.0886904729946,0.114415907985,0.111763142404,0.15895282319,0.180252600536,0.199602104197};
//    sysUncErr_pp[ptbinsN] = {0.104599395179,0.0886906329946,0.114416067985,0.111763302404,0.15895298319,0.180252760536,0.199602264197};//with BR 0.04% incl
systuncP[0]=0.104599395179;
systuncP[1]=0.0886906329946;
systuncP[2]=0.114416067985;
systuncP[3]=0.111763302404;
systuncP[4]=0.15895298319;
systuncP[5]=0.180252760536;
systuncP[6]=0.199602264197;
    sysCutVar = new double[fptbinsJetFinalN];
    for(int k=0; k<fptbinsJetFinalN; k++) sysCutVar[k] = 0.05;

    TFile *File = new TFile(dataAnalysisFile,"read");
    TDirectoryFile* dir;
    TList *histList;
    double nEv;

    if(oldCounter) {
      dir = (TDirectoryFile*)File->Get("DmesonsForJetCorrelations");
      if(fDmesonSpecie) histList = (TList*)dir->Get("histosDStarMBN0");
      else histList = (TList*)dir->Get("histosD0MBN0");
      TH1F* hEvents = (TH1F*)histList->FindObject("hstat");
      double nEvSel = hEvents->GetBinContent(2);
      double nEvAna = hEvents->GetBinContent(1);
      nEv = nEvScale*nEvSel;
    }
    else {
      //dir = (TDirectoryFile*)File->Get(Form("PWG3_D2H_DmesonsForJetCorrelations%sMBN0",listName.Data()));
      dir = (TDirectoryFile*)File->Get("PWG3_D2H_DmesonsForJetCorrelationsMBN0");
      AliNormalizationCounter *c = (AliNormalizationCounter*)dir->Get("NormalizationCounter");
      nEv = c->GetNEventsForNorm();
    }

    double dataLum = nEv/(sigma_in*1000) ;//Luminosity in mbar
    double simScaling = 1;
    if(fSystem) simScaling = APb/2.;
    else simScaling = 0.5;
    double dataScaling;
    if(fDmesonSpecie) dataScaling = 1. /(BRDstar * dataLum)/2.;
    else dataScaling = 1. /(BRDzero * dataLum)/2.;

    //if(isSys) getSystematics(sysDir,outPlotDir);

    // ----------------- prompt simulation ---------------------
    if(isSim){
      int simNr = 0; // 0 - central value
      int nFiles = fCsimN;
      TH1D *hPrompt[fCsimN];
      TH1D *hPrompt_binned[fCsimN];

      for (int nr=simNr; nr<fCsimN; nr++){
          TString file = simDir;
          file += "/JetPt_";
          file += fRunC[nr];
          file += "_Dpt"; file += fptbinsDA[0]; file += "_"; file += fptbinsDA[fptbinsDN];
          if(fDmesonSpecie) file += "_Dstar";
          else file += "_Dzero";
          file += ".root";
          TH1D *htmp;
          htmp = (TH1D*) GetInputHist(file, "hPt", htmp);
          htmp->GetYaxis()->SetTitle("d#sigma/dp_{T} (mb)");
          hPrompt[nr] = (TH1D*)htmp->Clone(Form("hPrompt_%d",nr));
          hPrompt_binned[nr] = (TH1D*)htmp->Rebin(fptbinsJetFinalN,Form("hPrompt_binned_%d",nr),xAxis);
      }

      TH1D *htmp = (TH1D*)(hPrompt[simNr]->Clone("htmp"));
      TH1D *hPrompt_central = (TH1D*)htmp->Clone("hPrompt_central");
      hPrompt_central_binned = (TH1D*)htmp->Rebin(fptbinsJetFinalN,"hPrompt_central_binned",xAxis);

      setHistoDetails(hPrompt_central,4,24);
      setHistoDetails(hPrompt_central_binned,4,24);

      hPrompt_central->Scale(simScaling);
      hPrompt_central_binned->Scale(simScaling);
      hPrompt_central_binned->Scale(1,"width");
      hPrompt_central_binned->Scale(1./dy);

      if(isSimSys){
      // ----------------- prompt syst. (rebinned)---------------------
        // get up unc
        hPrompt_up = (TH1D*)hPrompt_central_binned->Clone("hPrompt_up");
        hPrompt_up = (TH1D*)GetUpSys(hPrompt_binned,nFiles,hPrompt_up);
        setHistoDetails(hPrompt_up,4,24,0,2,2);
        hPrompt_up->Scale(simScaling);
        hPrompt_up->Scale(1,"width");
        hPrompt_up->Scale(1./dy);
        // get down unc
        hPrompt_down = (TH1D*)hPrompt_central_binned->Clone("hPrompt_down");
        hPrompt_down = (TH1D*)GetDownSys(hPrompt_binned,nFiles,hPrompt_down);
        setHistoDetails(hPrompt_down,4,24,0,2,2);
        hPrompt_down->Scale(simScaling);
        hPrompt_down->Scale(1,"width");
        hPrompt_down->Scale(1./dy);
      }
    }

    // ----------------- data ---------------------
    TH1D *hData_binned2;
    hData_binned2 = (TH1D*)GetInputHist(dataFile, histBase, hData_binned2);
    hData_binned = (TH1D*)hData_binned2->Rebin(fptbinsJetFinalN,"hData_binned", xAxis);
    hData_binned->Scale(1,"width");
    hData_binned->Scale(dataScaling);
    hData_binned->Scale(1./dy);
    hData_binned->SetTitle("");
    //hData_binned->SetMinimum(1);
    hData_binned->SetMaximum(hData_binned->GetMaximum()*2);
    hData_binned->GetYaxis()->SetTitle("d^{2}#sigma/dp_{T}d#it{#eta} (mb)");

    Double_t sysunc[fptbinsJetFinalN];
    Double_t sysuncAbs[fptbinsJetFinalN];
    Double_t statunc[fptbinsJetFinalN];
    Double_t value[fptbinsJetFinalN];
    Double_t ptval[fptbinsJetFinalN];
    Double_t ptvalunc[fptbinsJetFinalN];
    if(isSys) {
      for(int j=0; j<fptbinsJetFinalN; j++){
              ptval[j] = (xAxis[j]+xAxis[j+1]) / 2.;
              ptvalunc[j] = (xAxis[j+1]-xAxis[j]) / 2.;
              value[j] = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(ptval[j]));
              Double_t error = hData_binned->GetBinError(hData_binned->GetXaxis()->FindBin(ptval[j]));
              sysunc[j] =  systuncP[j];
              sysuncAbs[j] = value[j] * systuncP[j];
              statunc[j] = error/ value[j] *100;
            //  cout << j << " sys: " << sysunc[j] << "\t\t stat: " << statunc[j] << endl;
      }
      grsys = new TGraphAsymmErrors(fptbinsJetFinalN,ptval,value,ptvalunc,ptvalunc,sysuncAbs,sysuncAbs);
    }

    Double_t sysuncTheory[fptbinsJetFinalN];
    Double_t ptvaltheory[fptbinsJetFinalN];
    Double_t ptvalunctheory[fptbinsJetFinalN];
    Double_t valuetheory[fptbinsJetFinalN];
    Double_t valuetheoryerrup[fptbinsJetFinalN];
    Double_t valuetheoryerrdown[fptbinsJetFinalN];
    if(isSim && isSimSys){
      for(int j=0; j<fptbinsJetFinalN; j++){
              ptvaltheory[j] = (xAxis[j]+xAxis[j+1]) / 2.;
              ptvalunctheory[j] = (xAxis[j+1]-xAxis[j]) / 2.;
              valuetheory[j] = hPrompt_central_binned->GetBinContent(hPrompt_central_binned->GetXaxis()->FindBin(ptvaltheory[j]));
              valuetheoryerrup[j] = hPrompt_up->GetBinContent(hPrompt_up->GetXaxis()->FindBin(ptvaltheory[j])) - valuetheory[j];
              valuetheoryerrdown[j] = valuetheory[j] - hPrompt_down->GetBinContent(hPrompt_up->GetXaxis()->FindBin(ptvaltheory[j]));

      }
      grsystheory = new TGraphAsymmErrors(fptbinsJetFinalN,ptvaltheory,valuetheory,ptvalunctheory,ptvalunctheory,valuetheoryerrdown,valuetheoryerrup);
    }

   //======= Ratio to powheg ======
    TH1D *hPrompt_central_binned_ratio;
    TH1D *hPrompt_down_ratio;
    TH1D *hPrompt_up_ratio;
     if(isSim){
       hPrompt_central_binned_ratio  = (TH1D*)hPrompt_central_binned->Clone("hPrompt_central_binned_ratio");
       hPrompt_central_binned_ratio->Divide(hPrompt_central_binned);
       if(isSimSys){
         hPrompt_up_ratio = (TH1D*)hPrompt_up->Clone("hPrompt_up_ratio");
         hPrompt_down_ratio = (TH1D*)hPrompt_down->Clone("hPrompt_down_ratio");
         hPrompt_up_ratio->Divide(hPrompt_central_binned);
         hPrompt_down_ratio->Divide(hPrompt_central_binned);
       }
     }
       hData_binned_ratio = (TH1D*)hData_binned->Clone("hData_binned_ratio");
       if(isSys){
         double *sysuncRatio = new double[fptbinsJetFinalN];
         double *valRatio = new double[fptbinsJetFinalN];
         for(int j=0; j<fptbinsJetFinalN; j++){
                 double pt = (xAxis[j]+xAxis[j+1]) / 2.;
                 double val = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
                 double valPred;
                 if(isSim) valPred = hPrompt_central_binned->GetBinContent(hPrompt_central_binned->GetXaxis()->FindBin(pt));
                 else valPred = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
                 valRatio[j] = val / valPred;
                 double err = hData_binned->GetBinError(hData_binned->GetXaxis()->FindBin(pt)) / valPred;
                 //err = err * valRatio[j];
                 sysuncRatio[j] = sysunc[j]*valRatio[j];

                 hData_binned_ratio->SetBinContent(hData_binned_ratio->GetXaxis()->FindBin(pt),valRatio[j]);
                 hData_binned_ratio->SetBinError(hData_binned_ratio->GetXaxis()->FindBin(pt),err);
         }
         grsysRatio = new TGraphAsymmErrors(fptbinsJetFinalN,ptval,valRatio,ptvalunc,ptvalunc,sysuncRatio,sysuncRatio);
       }
       hData_binned_ratio->SetMaximum(2);

       double *sysuncTheoryratio = new double[fptbinsJetFinalN];
       double *ptvaltheoryratio = new double[fptbinsJetFinalN];
       double *ptvalunctheoryratio = new double[fptbinsJetFinalN];
       double *valuetheoryratio = new double[fptbinsJetFinalN];
       double *valuetheoryerrupratio = new double[fptbinsJetFinalN];
       double *valuetheoryerrdownratio = new  double[fptbinsJetFinalN];
       if(isSimSys){

        for(int j=0; j<fptbinsJetFinalN; j++){
              ptvaltheoryratio[j] = (xAxis[j]+xAxis[j+1]) / 2.;
              ptvalunctheoryratio[j] = (xAxis[j+1]-xAxis[j]) / 2.;
              valuetheoryratio[j] = hPrompt_central_binned_ratio->GetBinContent(hPrompt_central_binned_ratio->GetXaxis()->FindBin(ptvaltheory[j]));
              valuetheoryerrupratio[j] = hPrompt_up_ratio->GetBinContent(hPrompt_up_ratio->GetXaxis()->FindBin(ptvaltheory[j])) - valuetheoryratio[j];
              valuetheoryerrdownratio[j] = valuetheoryratio[j] - hPrompt_down_ratio->GetBinContent(hPrompt_down_ratio->GetXaxis()->FindBin(ptvaltheory[j]));
        }
        grsystheoryratio = new TGraphAsymmErrors(fptbinsJetFinalN,ptvaltheoryratio,valuetheoryratio,ptvalunctheoryratio,ptvalunctheoryratio,valuetheoryerrdownratio,valuetheoryerrupratio);
      }


drawFinal(outPlotDir);

TFile *ofile = new TFile(Form("%s/JetPtSpectrum_final.root",outSpectraDir.Data()),"RECREATE");
hData_binned->Write();
hData_binned_ratio->Write();
if(isSim){
  hPrompt_central_binned->Write();
  if(isSimSys){
    hPrompt_up->Write();
    hPrompt_down->Write();
    grsystheory->Write();
    grsystheoryratio->Write();
  }
}
if(isSys) grsysRatio->Write();
if(isSys) grsys->Write();

ofile->Close();

return;

}

void getSystematics(TString inDir, TString outPlotDir) {

  const int nfiles = 9;
  TString files[nfiles] = {
    "CutVariationSyst_reg0.root",
    "YieldExtraction.root",
    "RawYield_reflections.root",
    "SBRangesComparison.root",
    "FD_reg3.root",
    "JES_reg3.root",
    "PriorComparison_reg3.root",
    "UnfoldingRangesComparison.root",
    "BkgComparison_reg3.root"
  };
  TString histName[nfiles] = {
    "cutSysRMS",
    "sysUnc",
    "hsys",
    "hsys",
    "hDFUnc",
    "hratiof_0",
    "hsys",
    "hsys",
    "hsys"
  };

  TString desc[nfiles+1] = {
    "Cut variation",
    "Raw Yield Extraction",
    "Reflections",
    "SB,Signal ranges",
    "B Feed-down",
    "Track. Eff. (JES)",
    "Unfolding: priors",
    "Unfolding: ranges,SVD",
    "Bkg. Fluctuation Matrix",
    "Track. Eff. (D meson)"
  };

  TCanvas *cUnc = new TCanvas("cUnc","cUnc",1200,800);
  TH1F *hist[nfiles+1];
  double **sysUnc = new double*[fptbinsJetFinalN];
  for(int i=0; i<fptbinsJetFinalN; i++)  sysUnc[i] = new double[nfiles+1];

  hist[nfiles] = new TH1F("histUncN","Systematic uncertanties",fptbinsJetFinalN, xAxis);
  hist[0] = new TH1F("histUnc0","Systematic uncertanties",fptbinsJetFinalN, xAxis);
//  hist[1] = new TH1F("histUnc1","Systematic uncertanties",fptbinsJetFinalN, fptbinsJetTrueFinalpp);

  for(int k=0; k<fptbinsJetFinalN; k++) hist[nfiles]->SetBinContent(k+1,DTrackEff*100);
  hist[nfiles]->SetMarkerColor(colors[nfiles+1]);
  hist[nfiles]->SetLineColor(colors[nfiles+1]);
  hist[nfiles]->SetLineStyle(linestyle[nfiles]);

  for(int j=0; j<fptbinsJetFinalN; j++) hist[0]->SetBinContent(j+1,sysCutVar[j]);
  //for(int j=0; j<fptbinsJetFinalN; j++) hist[1]->SetBinContent(j+1,sysRawYield[j]);

  TLegend  *leg = new TLegend(0.45,0.55,0.85,0.85);
  for(int i=0; i<nfiles; i++){
    TFile *fileIn;
//    if(i != 0 && i != 1) { fileIn = new TFile(Form("%s/%s", inDir.Data(),files[i].Data())); hist[i] = (TH1F*)fileIn->Get(Form("%s",histName[i].Data())); }
    if(i != 0 ) { fileIn = new TFile(Form("%s/%s", inDir.Data(),files[i].Data())); hist[i] = (TH1F*)fileIn->Get(Form("%s",histName[i].Data())); }

    //hist[i] = (TH1F*)fileIn->Get(Form("%s",histName[i].Data()));
    hist[i]->SetMarkerColor(colors[i+1]);
    hist[i]->SetLineColor(colors[i+1]);
    hist[i]->SetLineStyle(linestyle[i+1]);
    hist[i]->GetYaxis()->SetTitle("Syst. Uncertainty [%]");
    hist[i]->GetXaxis()->SetRangeUser(5,50);
    hist[i]->GetYaxis()->SetRangeUser(0,30);
    hist[i]->SetFillColor(0);
    hist[i]->SetFillStyle(0);
    if(!i) hist[i]->Draw("hist");
    else hist[i]->Draw("histsame");
    std::cout << "systematics: " << desc[i] << std::endl;
    for(int j=0; j<fptbinsJetFinalN; j++){
      double pt = (xAxis[j] + xAxis[j+1])/2.;
      int bin = hist[i]->GetXaxis()->FindBin(pt);
      if(bin) sysUnc[j][i] = hist[i]->GetBinContent(bin)*0.01;
      else sysUnc[j][i] = 0;
      std::cout << "pt: " << pt << "\tsys: " << sysUnc[j][i]*100 << std::endl;
    }
    std::cout << "=============END" << std::endl;
      leg->AddEntry(hist[i],Form("%s",desc[i].Data()),"l");
  }
  leg->AddEntry(hist[nfiles],Form("%s",desc[nfiles].Data()),"l");
  hist[nfiles]->Draw("histsame");
  leg->Draw("same");

  cUnc->SaveAs(Form("%s/JetPtSpectra_allUnc.png",outPlotDir.Data()));
  cUnc->SaveAs(Form("%s/JetPtSpectra_allUnc.pdf",outPlotDir.Data()));

  TCanvas *cUnc2 = new TCanvas("cUnc2","cUnc2",1200,800);
  TH1D *histUnc = new TH1D("histUnc","Systematic uncertanties",fptbinsJetFinalN, xAxis);
  histUnc->SetLineColor(2);
  histUnc->SetLineWidth(2);
  histUnc->SetLineStyle(2);
  histUnc->SetTitle("");
  histUnc->GetYaxis()->SetTitle("Final systematic uncertanties");
  histUnc->GetXaxis()->SetTitle("p_{T}^{ch,jet} (GeV/c)");
  histUnc->GetXaxis()->SetRangeUser(plotmin,plotmax);
  for(int i=0; i<fptbinsJetFinalN; i++){
    double value = 0;
    for(int j=0; j<nfiles+1; j++){
      value += sysUnc[i][j]*sysUnc[i][j];
    }
    value += DTrackEff*DTrackEff;
    systuncP[i] = TMath::Sqrt(value);
    histUnc->SetBinContent(i+1,systuncP[i]);
    value += globalUnc*globalUnc;
    systuncP[i] = TMath::Sqrt(value);
  }
  histUnc->GetYaxis()->SetRangeUser(0,0.4);
  histUnc->GetXaxis()->SetRangeUser(plotmin,plotmax);
  histUnc->Draw("hist");

  cUnc2->SaveAs(Form("%s/JetPtSpectra_finalUnc.png",outPlotDir.Data()));
  cUnc2->SaveAs(Form("%s/JetPtSpectra_finalUnc.pdf",outPlotDir.Data()));

}


TH1* GetUpSys(TH1D **hh, const int nFiles, TH1D *hh_up){

        double bin = 0, binerr = 0;
        double max = 0, maxerr = 0;
        for(int j=1; j<fptbinsJetFinalN+1; j++ ){
            max = hh[0]->GetBinContent(j);
            for(int i=1;i<nFiles;i++){
                if(hh[i]->GetBinContent(j) > max){
                        max = hh[i]->GetBinContent(j);
                        maxerr = hh[i]->GetBinError(j);
                }
            }
            hh_up->SetBinContent(j,max);
            hh_up->SetBinError(j,0);
        }

    return hh_up;
}

TH1* GetDownSys(TH1D **hh, const int nFiles, TH1D *hh_down){
        double bin = 0, binerr = 0;
        double max = 0, maxerr = 0;

        for(int j=1; j<fptbinsJetFinalN+1; j++ ){
      //for(int j=1; j<hh[0]->GetNbinsX()+1; j++ ){
            max = hh[0]->GetBinContent(j);
            for(int i=1;i<nFiles;i++){
                if(hh[i]->GetBinContent(j) < max){
                        max = hh[i]->GetBinContent(j);
                        maxerr = hh[i]->GetBinError(j);
                }
            }
            hh_down->SetBinContent(j,max);
            hh_down->SetBinError(j,0);
        }

    return hh_down;
}



TH1* GetInputHist(TString inFile, TString histName,TH1 *hh){

	TFile *jetPtFile = new TFile(inFile,"read");
  hh = (TH1F*)jetPtFile->Get(histName.Data());

  return hh;
}



void ScaleHist(TH1 *hh, int full){
    if(full){
        //hh->Scale(1,"width");
        //hh->Scale(pPbscaling);
        //hh->Scale(scalingF);
         hh->Scale(1,"width");
        hh->GetYaxis()->SetTitle("d^{2}#sigma/d#it{#eta}dp_{T} (mb #it{c}/GeV)");
    }
    else {
        hh->Scale(1,"width");
        hh->GetYaxis()->SetTitle("dN/dp_{T} (#it{c}/GeV)");
    }
}

void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, int Msize, Width_t Lwidth, Style_t Lstyle){
    hh->SetMarkerColor(color);
    hh->SetMarkerStyle(Mstyle);;
    hh->SetLineColor(color);
    hh->SetLineWidth(Lwidth);
    hh->SetMarkerSize(Msize);
    hh->SetLineStyle(Lstyle);
   // hh->SetName(name.c_str());
    hh->SetTitle("");
    hh->GetXaxis()->SetTitle("p_{T}^{ch,jet} (GeV/c)");
}

void SaveCanvas(TCanvas *c, TString name){
    c->SaveAs(Form("%s.png",name.Data()));
    c->SaveAs(Form("%s.pdf",name.Data()));
    //c->SaveAs(Form("%s.png",name.c_str()));
    //c->SaveAs(Form("%s.pdf",name.c_str()));
}

void drawFinal(TString outPlotDir){

  //=========Macro generated from canvas: FinalSpectrum/FinalSpectrum
//=========  (Mon Jun 12 17:00:30 2017) by ROOT version5.34/30
   TCanvas *FinalSpectrum = new TCanvas("FinalSpectrum", "FinalSpectrum",0,45,700,700);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   FinalSpectrum->SetHighLightColor(2);
   FinalSpectrum->Range(0,0,1,1);
   FinalSpectrum->SetFillColor(0);
   FinalSpectrum->SetBorderMode(0);
   FinalSpectrum->SetBorderSize(2);
   FinalSpectrum->SetFrameBorderMode(0);

// ------------>Primitives in pad: FinalSpectrum_1
   TPad *FinalSpectrum_1 = new TPad("FinalSpectrum_1", "FinalSpectrum_1",0,0.35,1,1);
   FinalSpectrum_1->Draw();
   FinalSpectrum_1->cd();
   FinalSpectrum_1->Range(-1.986821e-07,-4.69897,33.33333,0.3499945);
   FinalSpectrum_1->SetFillColor(0);
   FinalSpectrum_1->SetBorderMode(0);
   FinalSpectrum_1->SetBorderSize(2);
   FinalSpectrum_1->SetLogy();
   FinalSpectrum_1->SetTickx(1);
   FinalSpectrum_1->SetTicky(1);
   FinalSpectrum_1->SetLeftMargin(0.15);
   FinalSpectrum_1->SetBottomMargin(0);
   FinalSpectrum_1->SetFrameBorderMode(0);
   FinalSpectrum_1->SetFrameBorderMode(0);

   TH1D *CentralPointsStatisticalUncertainty__1 = new TH1D("CentralPointsStatisticalUncertainty__1","Central Values",fptbinsJetFinalN, xAxis);
   //TH1D *CentralPointsStatisticalUncertainty__1 = new TH1D("CentralPointsStatisticalUncertainty__1","Central Values",fptbinsJetFinalN2, xAxis2);
   if(fSystem){
     CentralPointsStatisticalUncertainty__1->SetMinimum(2.e-04);
     CentralPointsStatisticalUncertainty__1->SetMaximum(500);
   }
   else{
     CentralPointsStatisticalUncertainty__1->SetMinimum(2.e-06);
     CentralPointsStatisticalUncertainty__1->SetMaximum(0.5);
  }
   CentralPointsStatisticalUncertainty__1->SetEntries(8);
   CentralPointsStatisticalUncertainty__1->SetDirectory(0);
   CentralPointsStatisticalUncertainty__1->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__1->SetLineColor(ci);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__1->SetMarkerColor(ci);
   CentralPointsStatisticalUncertainty__1->SetMarkerStyle(20);
   CentralPointsStatisticalUncertainty__1->SetMarkerSize(0.9);
   CentralPointsStatisticalUncertainty__1->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   CentralPointsStatisticalUncertainty__1->GetXaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__1->GetXaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__1->GetXaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__1->GetXaxis()->SetTitleFont(42);
   //CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} [mb (GeV/#it{c})^{-1}]");
   CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} mb (GeV/#it{c})^{-1}");
   CentralPointsStatisticalUncertainty__1->GetYaxis()->SetLabelFont(43);
   CentralPointsStatisticalUncertainty__1->GetYaxis()->SetLabelSize(22);
   CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitleSize(26);
   CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitleOffset(1.6);
   CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitleFont(43);
   CentralPointsStatisticalUncertainty__1->GetZaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__1->GetZaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__1->GetZaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__1->GetZaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__1->Draw("axis");

   // dat syst. unc.
   TGraphAsymmErrors *grae;
if(isSys){
   grae = (TGraphAsymmErrors*) grsys->Clone("grae"); // = new TGraphAsymmErrors(6);
   grae->SetName("CentralPointsSystematicUncertainty_copy");
   grae->SetTitle("Bayes, iter=4, prior=ResponseTruth Systematics");

   ci = TColor::GetColor("#cccccc");
   grae->SetFillColor(ci);
   grae->SetLineColor(ci);

   //=== data uncertantity from grae
   TH1F *Graph_central_syst_unc1 = new TH1F("Graph_central_syst_unc1","Bayes, iter=4, prior=ResponseTruth Systematics",100,2.5,52.5);
   Graph_central_syst_unc1->SetMinimum(4.779682e-05);
   Graph_central_syst_unc1->SetMaximum(0.02142993);
   Graph_central_syst_unc1->SetDirectory(0);
   Graph_central_syst_unc1->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_central_syst_unc1->SetLineColor(ci);
   Graph_central_syst_unc1->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   Graph_central_syst_unc1->GetXaxis()->SetLabelFont(42);
   Graph_central_syst_unc1->GetXaxis()->SetLabelSize(0.035);
   Graph_central_syst_unc1->GetXaxis()->SetTitleSize(0.035);
   Graph_central_syst_unc1->GetXaxis()->SetTitleFont(42);
   Graph_central_syst_unc1->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} [mb (GeV/#it{c})^{-1}]");
   Graph_central_syst_unc1->GetYaxis()->SetLabelFont(42);
   Graph_central_syst_unc1->GetYaxis()->SetLabelSize(0.035);
   Graph_central_syst_unc1->GetYaxis()->SetTitleSize(0.035);
   Graph_central_syst_unc1->GetYaxis()->SetTitleFont(42);
   Graph_central_syst_unc1->GetZaxis()->SetLabelFont(42);
   Graph_central_syst_unc1->GetZaxis()->SetLabelSize(0.035);
   Graph_central_syst_unc1->GetZaxis()->SetTitleSize(0.035);
   Graph_central_syst_unc1->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_central_syst_unc1);
   grae->Draw("2");
 }

   // Central data
   TH1D *CentralPointsStatisticalUncertainty__2 = (TH1D*) hData_binned->Clone("CentralPointsStatisticalUncertainty__2"); // = new
   CentralPointsStatisticalUncertainty__2->SetMinimum(2e-05);
   CentralPointsStatisticalUncertainty__2->SetMaximum(4);
   CentralPointsStatisticalUncertainty__2->SetEntries(8);
   CentralPointsStatisticalUncertainty__2->SetDirectory(0);
   CentralPointsStatisticalUncertainty__2->SetStats(0);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__2->SetLineColor(ci);
   CentralPointsStatisticalUncertainty__2->SetMarkerColor(ci);
   CentralPointsStatisticalUncertainty__2->SetMarkerStyle(20);
   CentralPointsStatisticalUncertainty__2->SetMarkerSize(0.9);
   CentralPointsStatisticalUncertainty__2->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   CentralPointsStatisticalUncertainty__2->GetXaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__2->GetXaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__2->GetXaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__2->GetXaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__2->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} [mb (GeV/#it{c})^{-1}]");
   CentralPointsStatisticalUncertainty__2->GetYaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__2->GetYaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__2->GetYaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__2->GetYaxis()->SetTitleOffset(1.4);
   CentralPointsStatisticalUncertainty__2->GetYaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__2->GetZaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__2->GetZaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__2->GetZaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__2->GetZaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__2->Draw("same p e0 x0");

   // central theory
if(isSim){
   TH1D *GeneratorLevel_JetPtSpectrum__3 = (TH1D*)hPrompt_central_binned->Clone("GeneratorLevel_JetPtSpectrum__3"); //  new
   GeneratorLevel_JetPtSpectrum__3->SetEntries(316731);
   GeneratorLevel_JetPtSpectrum__3->SetDirectory(0);
   GeneratorLevel_JetPtSpectrum__3->SetStats(0);

   ci = TColor::GetColor("#000099");
   GeneratorLevel_JetPtSpectrum__3->SetLineColor(ci);

   ci = TColor::GetColor("#000099");
   GeneratorLevel_JetPtSpectrum__3->SetMarkerColor(ci);
   GeneratorLevel_JetPtSpectrum__3->SetMarkerStyle(24);
   GeneratorLevel_JetPtSpectrum__3->SetMarkerSize(1.2);
   GeneratorLevel_JetPtSpectrum__3->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   GeneratorLevel_JetPtSpectrum__3->GetXaxis()->SetLabelFont(42);
   GeneratorLevel_JetPtSpectrum__3->GetXaxis()->SetLabelSize(0.035);
   GeneratorLevel_JetPtSpectrum__3->GetXaxis()->SetTitleSize(0.035);
   GeneratorLevel_JetPtSpectrum__3->GetXaxis()->SetTitleFont(42);
   GeneratorLevel_JetPtSpectrum__3->GetYaxis()->SetTitle("#frac{d#sigma}{d#it{p}_{T}} #times #Delta#it{p}_{T} (mb)");
   GeneratorLevel_JetPtSpectrum__3->GetYaxis()->SetLabelFont(42);
   GeneratorLevel_JetPtSpectrum__3->GetYaxis()->SetLabelSize(0.035);
   GeneratorLevel_JetPtSpectrum__3->GetYaxis()->SetTitleSize(0.035);
   GeneratorLevel_JetPtSpectrum__3->GetYaxis()->SetTitleFont(42);
   GeneratorLevel_JetPtSpectrum__3->GetZaxis()->SetLabelFont(42);
   GeneratorLevel_JetPtSpectrum__3->GetZaxis()->SetLabelSize(0.035);
   GeneratorLevel_JetPtSpectrum__3->GetZaxis()->SetTitleSize(0.035);
   GeneratorLevel_JetPtSpectrum__3->GetZaxis()->SetTitleFont(42);
   GeneratorLevel_JetPtSpectrum__3->Draw("same p e0 x0");

   // theory syst unc
   if(isSimSys){
     grae = (TGraphAsymmErrors*) grsystheory->Clone("grae"); //  new TGraphAsymmErrors(6);
     grae->SetName("theorySyst_copy");
     grae->SetTitle("Graph");
     grae->SetFillColor(1);
     grae->SetFillStyle(0);

     ci = TColor::GetColor("#000099");
     grae->SetLineColor(ci);
     grae->SetLineWidth(2);
     TH1F *Graph_theorySyst_copy2 = new TH1F("Graph_theorySyst_copy2","Graph",100,2.5,52.5);
     Graph_theorySyst_copy2->SetMinimum(2.864129e-05);
     Graph_theorySyst_copy2->SetMaximum(0.01825038);
     Graph_theorySyst_copy2->SetDirectory(0);
     Graph_theorySyst_copy2->SetStats(0);

     ci = TColor::GetColor("#000099");
     Graph_theorySyst_copy2->SetLineColor(ci);
     Graph_theorySyst_copy2->GetXaxis()->SetLabelFont(42);
     Graph_theorySyst_copy2->GetXaxis()->SetLabelSize(0.035);
     Graph_theorySyst_copy2->GetXaxis()->SetTitleSize(0.035);
     Graph_theorySyst_copy2->GetXaxis()->SetTitleFont(42);
     Graph_theorySyst_copy2->GetYaxis()->SetLabelFont(42);
     Graph_theorySyst_copy2->GetYaxis()->SetLabelSize(0.035);
     Graph_theorySyst_copy2->GetYaxis()->SetTitleSize(0.035);
     Graph_theorySyst_copy2->GetYaxis()->SetTitleFont(42);
     Graph_theorySyst_copy2->GetZaxis()->SetLabelFont(42);
     Graph_theorySyst_copy2->GetZaxis()->SetLabelSize(0.035);
     Graph_theorySyst_copy2->GetZaxis()->SetTitleSize(0.035);
     Graph_theorySyst_copy2->GetZaxis()->SetTitleFont(42);
     grae->SetHistogram(Graph_theorySyst_copy2);
     grae->Draw("2");
  }
}
   TLegend *leg = new TLegend(0.5,0.32,0.8,0.57,NULL,"NB NDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(43);
   leg->SetTextSize(23);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("CentralPointsStatisticalUncertainty","Data","p");
   ci = TColor::GetColor("#990000");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(0.9);
   entry->SetTextFont(43);

   if(isSys){

     entry=leg->AddEntry("CentralPointsSystematicUncertainty_copy","Syst. unc. (data)","f");
     ci = TColor::GetColor("#cccccc");
     entry->SetFillColor(ci);
     entry->SetFillStyle(1001);
  }

  if(isSim){
      ci = TColor::GetColor("#000099");
     entry->SetLineColor(ci);
     entry->SetLineStyle(1);
     entry->SetLineWidth(1);
     entry->SetMarkerColor(1);
     entry->SetMarkerStyle(21);
     entry->SetMarkerSize(1);
     entry->SetTextFont(43);
     if(fSystem) entry=leg->AddEntry("GeneratorLevel_JetPtSpectrum","POWHEG+PYTHIA6 #times A","p");
     else entry=leg->AddEntry("GeneratorLevel_JetPtSpectrum","POWHEG+PYTHIA6","p");
     entry->SetLineColor(1);
     entry->SetLineStyle(1);
     entry->SetLineWidth(1);
     ci = TColor::GetColor("#000099");
     entry->SetMarkerColor(ci);
     entry->SetMarkerStyle(24);
     entry->SetMarkerSize(1.2);
     entry->SetTextFont(43);
     if(isSimSys) {
       entry=leg->AddEntry("theorySyst_copy","Syst. unc. (theory)","f");
       entry->SetFillColor(1);
       entry->SetLineColor(ci);
       entry->SetLineStyle(1);
       entry->SetLineWidth(2);
       entry->SetMarkerColor(1);
       entry->SetMarkerStyle(21);
       entry->SetMarkerSize(1);
       entry->SetTextFont(43);
    }
 }
   leg->Draw();

   TPaveText *pt = new TPaveText(0.16,0.64,0.55,0.9,"NB NDC");
   pt->SetBorderSize(0);
   pt->SetFillStyle(0);
   pt->SetTextAlign(13);
   pt->SetTextFont(43);
   pt->SetTextSize(22);
   //TText *text = pt->AddText("ALICE Preliminary");
   TText *text = new TText;
   text = pt->AddText("ALICE Preliminary");
   if(fSystem) text = pt->AddText("p-Pb, #sqrt{#it{s}_{NN}} = 5.02 TeV");
   else text = pt->AddText("pp, #sqrt{#it{s}} = 5.02 TeV");
   text = pt->AddText(Form("charged jets, anti-#it{k}_{T}, #it{R} = 0.%d, |#it{#eta}_{lab}^{jet}| < 0.%d",Rpar,9-Rpar));
   text = pt->AddText(Form ("with D^{0}, %d < #it{p}_{T,D^{0}} < %d GeV/#it{c}",(int)fptbinsDA[0],(int)fptbinsDA[fptbinsDN]));
   pt->Draw();

   // does nothing
   TH1D *CentralPointsStatisticalUncertainty__4 = new TH1D("CentralPointsStatisticalUncertainty__4","Central Values",fptbinsJetFinalN, xAxis);
   CentralPointsStatisticalUncertainty__4->SetMinimum(2e-05);
   CentralPointsStatisticalUncertainty__4->SetMaximum(0.7);
   CentralPointsStatisticalUncertainty__4->SetEntries(8);
   CentralPointsStatisticalUncertainty__4->SetDirectory(0);
   CentralPointsStatisticalUncertainty__4->SetStats(0);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__4->SetLineColor(ci);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__4->SetMarkerColor(ci);
   CentralPointsStatisticalUncertainty__4->SetMarkerStyle(20);
   CentralPointsStatisticalUncertainty__4->SetMarkerSize(0.9);
   CentralPointsStatisticalUncertainty__4->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   CentralPointsStatisticalUncertainty__4->GetXaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__4->GetXaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__4->GetXaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__4->GetXaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__4->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} [mb (GeV/#it{c})^{-1}]");
   CentralPointsStatisticalUncertainty__4->GetYaxis()->SetLabelFont(43);
   CentralPointsStatisticalUncertainty__4->GetYaxis()->SetLabelSize(22);
   CentralPointsStatisticalUncertainty__4->GetYaxis()->SetTitleSize(26);
   CentralPointsStatisticalUncertainty__4->GetYaxis()->SetTitleOffset(1.6);
   CentralPointsStatisticalUncertainty__4->GetYaxis()->SetTitleFont(43);
   CentralPointsStatisticalUncertainty__4->GetZaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__4->GetZaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__4->GetZaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__4->GetZaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__4->Draw("sameaxis");
   FinalSpectrum_1->Modified();
   FinalSpectrum->cd();

// ------------>Primitives in pad: FinalSpectrum_2
   TPad *FinalSpectrum_2 = new TPad("FinalSpectrum_2", "FinalSpectrum_2",0,0,1,0.35);
   FinalSpectrum_2->Draw();
   FinalSpectrum_2->cd();
   FinalSpectrum_2->Range(-1.986821e-07,-0.9209589,33.33333,2.49);
   FinalSpectrum_2->SetFillColor(0);
   FinalSpectrum_2->SetBorderMode(0);
   FinalSpectrum_2->SetBorderSize(2);
   FinalSpectrum_2->SetGridy();
   FinalSpectrum_2->SetTickx(1);
   FinalSpectrum_2->SetTicky(1);
   FinalSpectrum_2->SetLeftMargin(0.15);
   FinalSpectrum_2->SetTopMargin(0);
   FinalSpectrum_2->SetBottomMargin(0.27);
   FinalSpectrum_2->SetFrameBorderMode(0);
   FinalSpectrum_2->SetFrameBorderMode(0);

   // central points data (values don't really needed)
   TH1D *CentralPointsStatisticalUncertainty__5 = new TH1D("CentralPointsStatisticalUncertainty__5","Central Values",fptbinsJetFinalN, xAxis);
   CentralPointsStatisticalUncertainty__5->SetMinimum(0);
   CentralPointsStatisticalUncertainty__5->SetMaximum(2.49);
   CentralPointsStatisticalUncertainty__5->SetEntries(8);
   CentralPointsStatisticalUncertainty__5->SetDirectory(0);
   CentralPointsStatisticalUncertainty__5->SetStats(0);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__5->SetLineColor(ci);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__5->SetMarkerColor(ci);
   CentralPointsStatisticalUncertainty__5->SetMarkerStyle(20);
   CentralPointsStatisticalUncertainty__5->SetMarkerSize(0.9);
   CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   CentralPointsStatisticalUncertainty__5->GetXaxis()->SetLabelFont(43);
   CentralPointsStatisticalUncertainty__5->GetXaxis()->SetLabelSize(22);
   CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitleSize(26);
   CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitleOffset(2.9);
   CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitleFont(43);
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitle("data / theory");
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetNdivisions(509);
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetLabelFont(43);
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetLabelSize(22);
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitleSize(26);
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitleOffset(1.4);
   CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitleFont(43);
   CentralPointsStatisticalUncertainty__5->GetZaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__5->GetZaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__5->GetZaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__5->GetZaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__5->Draw("axis");

if(isSys) {
   // data syst unc ratio
   grae =  (TGraphAsymmErrors*) grsysRatio->Clone("grae"); //   new TGraphAsymmErrors(6);
   grae->SetName("ratioSyst");
   grae->SetTitle("Bayes, iter=4, prior=ResponseTruth Systematics");

   ci = TColor::GetColor("#cccccc");
   grae->SetFillColor(ci);
   grae->SetLineColor(ci);
   grae->SetMarkerColor(ci);

   TH1F *Graph_ratioSyst3 = new TH1F("Graph_ratioSyst3","Bayes, iter=4, prior=ResponseTruth Systematics",100,2.5,52.5);
   Graph_ratioSyst3->SetMinimum(0.5839274);
   Graph_ratioSyst3->SetMaximum(2.28012);
   Graph_ratioSyst3->SetDirectory(0);
   Graph_ratioSyst3->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_ratioSyst3->SetLineColor(ci);
   Graph_ratioSyst3->GetXaxis()->SetLabelFont(42);
   Graph_ratioSyst3->GetXaxis()->SetLabelSize(0.035);
   Graph_ratioSyst3->GetXaxis()->SetTitleSize(0.035);
   Graph_ratioSyst3->GetXaxis()->SetTitleFont(42);
   Graph_ratioSyst3->GetYaxis()->SetLabelFont(42);
   Graph_ratioSyst3->GetYaxis()->SetLabelSize(0.035);
   Graph_ratioSyst3->GetYaxis()->SetTitleSize(0.035);
   Graph_ratioSyst3->GetYaxis()->SetTitleFont(42);
   Graph_ratioSyst3->GetZaxis()->SetLabelFont(42);
   Graph_ratioSyst3->GetZaxis()->SetLabelSize(0.035);
   Graph_ratioSyst3->GetZaxis()->SetTitleSize(0.035);
   Graph_ratioSyst3->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_ratioSyst3);

   grae->Draw("2");
}

   // data central ratio
   TH1D *CentralPointsStatisticalUncertainty__6 =  (TH1D*) hData_binned_ratio->Clone("CentralPointsStatisticalUncertainty__6");
   CentralPointsStatisticalUncertainty__6->SetMinimum(1.122659e-05);
   CentralPointsStatisticalUncertainty__6->SetMaximum(0.06069752);
   CentralPointsStatisticalUncertainty__6->SetEntries(14);
   CentralPointsStatisticalUncertainty__6->SetDirectory(0);
   CentralPointsStatisticalUncertainty__6->SetStats(0);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__6->SetLineColor(ci);
   CentralPointsStatisticalUncertainty__6->SetMarkerColor(ci);
   CentralPointsStatisticalUncertainty__6->SetMarkerStyle(20);
   CentralPointsStatisticalUncertainty__6->SetMarkerSize(0.9);
   CentralPointsStatisticalUncertainty__6->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   CentralPointsStatisticalUncertainty__6->GetXaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__6->GetXaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__6->GetXaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__6->GetXaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__6->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} [mb (GeV/#it{c})^{-1}]");
   CentralPointsStatisticalUncertainty__6->GetYaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__6->GetYaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__6->GetYaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__6->GetYaxis()->SetTitleOffset(1.4);
   CentralPointsStatisticalUncertainty__6->GetYaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__6->GetZaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__6->GetZaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__6->GetZaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__6->GetZaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__6->Draw("same p e0 x0");

   // theory syst ratio
   if(isSim && isSimSys) {
     grae = (TGraphAsymmErrors*)  grsystheoryratio->Clone("grae"); // new TGraphAsymmErrors(6);
     grae->SetName("ratioTheorySyst");
     grae->SetTitle("Graph");
     grae->SetFillColor(1);
     grae->SetFillStyle(0);

     ci = TColor::GetColor("#000099");
     grae->SetLineColor(ci);
     grae->SetLineWidth(2);

     TH1F *Graph_ratioTheorySyst4 = new TH1F("Graph_ratioTheorySyst4","Graph",100,2.5,32.5);
     Graph_ratioTheorySyst4->SetMinimum(0.2058133);
     Graph_ratioTheorySyst4->SetMaximum(1.968171);
     Graph_ratioTheorySyst4->SetDirectory(0);
     Graph_ratioTheorySyst4->SetStats(0);

     ci = TColor::GetColor("#000099");
     Graph_ratioTheorySyst4->SetLineColor(ci);
     Graph_ratioTheorySyst4->GetXaxis()->SetLabelFont(42);
     Graph_ratioTheorySyst4->GetXaxis()->SetLabelSize(0.035);
     Graph_ratioTheorySyst4->GetXaxis()->SetTitleSize(0.035);
     Graph_ratioTheorySyst4->GetXaxis()->SetTitleFont(42);
     Graph_ratioTheorySyst4->GetYaxis()->SetLabelFont(42);
     Graph_ratioTheorySyst4->GetYaxis()->SetLabelSize(0.035);
     Graph_ratioTheorySyst4->GetYaxis()->SetTitleSize(0.035);
     Graph_ratioTheorySyst4->GetYaxis()->SetTitleFont(42);
     Graph_ratioTheorySyst4->GetZaxis()->SetLabelFont(42);
     Graph_ratioTheorySyst4->GetZaxis()->SetLabelSize(0.035);
     Graph_ratioTheorySyst4->GetZaxis()->SetTitleSize(0.035);
     Graph_ratioTheorySyst4->GetZaxis()->SetTitleFont(42);
     grae->SetHistogram(Graph_ratioTheorySyst4);
     grae->Draw("2");
  }
   // just for graphics, data not really needed
   TH1D *CentralPointsStatisticalUncertainty__7 = new TH1D("CentralPointsStatisticalUncertainty__7","Central Values",fptbinsJetFinalN, xAxis);
   CentralPointsStatisticalUncertainty__7->SetMinimum(0);
   CentralPointsStatisticalUncertainty__7->SetMaximum(2.49);
   CentralPointsStatisticalUncertainty__7->SetEntries(8);
   CentralPointsStatisticalUncertainty__7->SetDirectory(0);
   CentralPointsStatisticalUncertainty__7->SetStats(0);

   ci = TColor::GetColor("#990000");
   CentralPointsStatisticalUncertainty__7->SetLineColor(ci);
   CentralPointsStatisticalUncertainty__7->SetMarkerColor(ci);
   CentralPointsStatisticalUncertainty__7->SetMarkerStyle(20);
   CentralPointsStatisticalUncertainty__7->SetMarkerSize(0.9);
   CentralPointsStatisticalUncertainty__7->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
   CentralPointsStatisticalUncertainty__7->GetXaxis()->SetLabelFont(43);
   CentralPointsStatisticalUncertainty__7->GetXaxis()->SetLabelSize(22);
   CentralPointsStatisticalUncertainty__7->GetXaxis()->SetTitleSize(26);
   CentralPointsStatisticalUncertainty__7->GetXaxis()->SetTitleOffset(2.9);
   CentralPointsStatisticalUncertainty__7->GetXaxis()->SetTitleFont(43);
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetTitle("data / theory");
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetNdivisions(509);
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetLabelFont(43);
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetLabelSize(22);
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetTitleSize(26);
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetTitleOffset(1.4);
   CentralPointsStatisticalUncertainty__7->GetYaxis()->SetTitleFont(43);
   CentralPointsStatisticalUncertainty__7->GetZaxis()->SetLabelFont(42);
   CentralPointsStatisticalUncertainty__7->GetZaxis()->SetLabelSize(0.035);
   CentralPointsStatisticalUncertainty__7->GetZaxis()->SetTitleSize(0.035);
   CentralPointsStatisticalUncertainty__7->GetZaxis()->SetTitleFont(42);
   CentralPointsStatisticalUncertainty__7->Draw("sameaxig");

   FinalSpectrum_2->Modified();
   FinalSpectrum->cd();
   FinalSpectrum->Modified();
   FinalSpectrum->cd();
   FinalSpectrum->SetSelected(FinalSpectrum);

   FinalSpectrum->SaveAs(Form("%s/JetPtSpectra_final.png",outPlotDir.Data()));
   FinalSpectrum->SaveAs(Form("%s/JetPtSpectra_final.pdf",outPlotDir.Data()));
   FinalSpectrum->SaveAs(Form("%s/JetPtSpectra_final.eps",outPlotDir.Data()));

}
