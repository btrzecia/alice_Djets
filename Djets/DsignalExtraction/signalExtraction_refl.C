 //-----------------------------------------------------------------------
 //  Author B.Trzeciak
 //  Utrecht University
 //  barbara.antonina.trzeciak@cern.ch
 //-----------------------------------------------------------------------

#include <iostream>
#include "TMath.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TList.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "THnSparse.h"
#include "TDatabasePDG.h"

using namespace std;

 const int     fptbinsDN = 12;
 double        fptbinsDA[fptbinsDN+1] = { 1,2,3,4,5,6,7,8,10,12,16,24,36 };


void setHistoDetails(TH1 *h, int scale, Color_t color, Style_t Mstyle, Size_t size){
    Width_t width=2;
    if(scale)h->Scale(1,"width");
    h->SetMarkerStyle(Mstyle);
    h->SetMarkerColor(color);
    h->SetMarkerSize(size);
    h->SetLineColor(color);
    h->SetLineWidth(width);
    h->SetTitle(0);
    h->GetXaxis()->SetTitle("p_{T,D}(GeV/c)");


    return;
}

void SaveCanvas(TCanvas *c, TString name = "tmp"){

    c->SaveAs(Form("%s_pTD%d.png",name.Data(),(int)fptbinsDA[0]));
    c->SaveAs(Form("%s_pTD%d.pdf",name.Data(),(int)fptbinsDA[0]));
}


void signalExtraction_refl(
  TString data = "$HOME//Work/ALICE/Analysis/Results/pp13tev/Jakub/MC/sub/AnalysisResults_MC_LHC18p_hadronPID.root",
  TString out = "$HOME/Work/ALICE/Analysis/Results/pp13tev/reflections/LHC18p_binRefl",
  TString outName = "LHC18o",
  bool postfix = 0, TString listName = "FD",
  bool isMoreFiles = 0,
  TString prod = "kl"   // for more than 1 file, for one file leave it empty)
)
{

    const int nDMC = 2;
  int fRebinMass = 2;
  double jetmin = -20, jetmax = 200;
  double zmin = -2, zmax = 2;
  double minf = 1.65, maxf = 2.2;
  double minRS = 1.71, maxRS = 2.1;
    
    TString plotsDir = "/plots";
    TString outdir = out;
    gSystem->Exec(Form("mkdir %s",outdir.Data()));
    gSystem->Exec(Form("mkdir %s%s",outdir.Data(),plotsDir.Data()));

    if(!isMoreFiles) prod="";
    int nFiles = (int)prod.Length();

    TString histName = "histosD0MBN";
    // get analysis output file
    TString datafile;
    TFile *File;
    TDirectoryFile* dir;
    TList *histList;
    THnSparseF *sparse;

    TH2D *hInvMassptDSig, *hInvMassptDRefl;
    if(!isMoreFiles) {
      datafile = data;
      File = new TFile(datafile,"read");
      if(!File) { std::cout << "==== WRONG FILE WITH DATA =====\n\n"; return ;}
      dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");

      for(int i=0;i<nDMC; i++){
          if(postfix) histList =  (TList*)dir->Get(Form("%s%d%sMCrec",histName.Data(),i,listName.Data()));
          else histList =  (TList*)dir->Get(Form("%s%dMCrec",histName.Data(),i));
          sparse = (THnSparseF*)histList->FindObject("hsDphiz");
          sparse->GetAxis(1)->SetRangeUser(jetmin,jetmax);
          if(i==0) {
            hInvMassptDSig = (TH2D*)sparse->Projection(2,6);
            hInvMassptDRefl = (TH2D*)sparse->Projection(2,7);
          }
          else {
            hInvMassptDSig->Add((TH2D*)sparse->Projection(2,6));
            hInvMassptDRefl->Add((TH2D*)sparse->Projection(2,7));
          }
      }
      /*for(int i=0;i<nDMC; i++){
          histList =  (TList*)dir->Get(Form("%s%d%sMCrec",histName.Data(),i,listName.Data()));
          sparse = (THnSparseF*)histList->FindObject("hsDphiz");
          sparse->GetAxis(1)->SetRangeUser(jetmin,jetmax);

          hInvMassptDSig->Add((TH2D*)sparse->Projection(2,6));
          hInvMassptDRefl->Add((TH2D*)sparse->Projection(2,7));
          
      }*/

    }
    else {
      for (int j=0;j<nFiles;j++){
          datafile = data;
          datafile += prod.Data()[j];
          datafile += ".root";
          File = new TFile(datafile,"read");
          if(!File) { cout << "==== WRONG FILE WITH DATA =====\n\n"; return ;}
          dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");

          for(int i=0;i<nDMC; i++){
              if(postfix) histList =  (TList*)dir->Get(Form("%s%d%sMCrec",histName.Data(),i,listName.Data()));
              else histList =  (TList*)dir->Get(Form("%s%dMCrec",histName.Data(),i));
              sparse = (THnSparseF*)histList->FindObject("hsDphiz");
              sparse->GetAxis(1)->SetRangeUser(jetmin,jetmax);
              if(i==0 && j==0) {
                hInvMassptDSig = (TH2D*)sparse->Projection(2,6);
                hInvMassptDRefl = (TH2D*)sparse->Projection(2,7);
              }
              else {
                hInvMassptDSig->Add((TH2D*)sparse->Projection(2,6));
                hInvMassptDRefl->Add((TH2D*)sparse->Projection(2,7));
              }
          }
      }
    }

    TH1D *hsig[fptbinsDN], *hrefl[fptbinsDN];
    TH1D *hFitReflNewTemp[fptbinsDN], *ratio[fptbinsDN];
    TH1D *hReflRS = new TH1D("hReflRS","hReflRS",fptbinsDN,fptbinsDA);
    TString formulaSig = "[0]/([2]*TMath::Sqrt(2*TMath::Pi()))*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))";
    TString formulaRef = "[0]/(TMath::Sqrt(2.*TMath::Pi())*[2])*TMath::Exp(-(x-[1])*(x-[1])/(2.*[2]*[2]))+[3]/( TMath::Sqrt(2.*TMath::Pi())*[5])*TMath::Exp(-(x-[4])*(x-[4])/(2.*[5]*[5]))";

    TCanvas *cSig = new TCanvas("cSig","cSig",1600,1800);
    cSig->Divide(4,3);
    TCanvas *cRefl = new TCanvas("cRefl","cRefl",1600,1800);
    cRefl->Divide(4,3);
    TCanvas *cRefl2 = new TCanvas("cRefl2","cRefl2",1600,1800);
    cRefl2->Divide(4,3);
    TCanvas *cRatio = new TCanvas("cRatio","cRatio",1600,1800);
    cRatio->Divide(4,3);

    for(int i=0; i<fptbinsDN; i++){
      hsig[i] = (TH1D*)hInvMassptDSig->ProjectionX(Form("histSgn_%d_%d",(int)fptbinsDA[i],(int)fptbinsDA[i+1]),hInvMassptDSig->GetYaxis()->FindBin(fptbinsDA[i]), hInvMassptDSig->GetYaxis()->FindBin(fptbinsDA[i+1])-1);
      hrefl[i] = (TH1D*)hInvMassptDRefl->ProjectionX(Form("hrefl_%d_%d",(int)fptbinsDA[i],(int)fptbinsDA[i+1]),hInvMassptDRefl->GetYaxis()->FindBin(fptbinsDA[i]), hInvMassptDRefl->GetYaxis()->FindBin(fptbinsDA[i+1])-1);
      hsig[i]->Rebin(fRebinMass);
      hrefl[i]->Rebin(fRebinMass);

      hsig[i]->GetXaxis()->SetRangeUser(minf,maxf);
      hrefl[i]->GetXaxis()->SetRangeUser(minf,maxf);
      hsig[i]->SetTitle(Form("%.1lf < pt^{D} < %.1lf",fptbinsDA[i],fptbinsDA[i+1]));
      hrefl[i]->SetTitle(Form("%.1lf < pt^{D} < %.1lf",fptbinsDA[i],fptbinsDA[i+1]));

     hFitReflNewTemp[i] = (TH1D*)hrefl[i]->Clone(Form("hFitReflNewTemp%d_%d", (int)fptbinsDA[i],(int)fptbinsDA[i+1]));
     ratio[i] = (TH1D*)hrefl[i]->Clone(Form("ratioRelDistr_pt%d_%d", (int)fptbinsDA[i],(int)fptbinsDA[i+1]));

      
      TF1 *gaussMCSignal=new TF1("gaussMCSig",formulaSig.Data(),minf,maxf);
      gaussMCSignal->SetParName(0,"IntegralSgn");
      gaussMCSignal->SetParName(1,"Mean");
      gaussMCSignal->SetParName(2,"Sigma");
      gaussMCSignal->SetParameter(0,1);
      gaussMCSignal->SetParameter(1,1.864);
      gaussMCSignal->SetParameter(2,0.010);
      gaussMCSignal->SetLineColor(kOrange+2);
      cSig->cd(i+1);
      //gStyle->SetOptFit(11111);
      hsig[i]->Fit("gaussMCSig","RI","",1.65,2.15);
      hsig[i]->Draw();

      cRefl->cd(i+1);
      TF1 *doublegaussMCRefl=new TF1("doublegaussMCRefl",formulaRef.Data(),minf,maxf);
      doublegaussMCRefl->SetParName(0,"IntegralRefl");
    //  doublegaussMCRefl->SetParName(1,"Mean");
    //  doublegaussMCRefl->SetParName(2,"Sigma");
    //  doublegaussMCRefl->SetParameter(0,1);
    //  doublegaussMCRefl->SetParameter(1,1.864);
    //  doublegaussMCRefl->SetParameter(2,0.010);
      doublegaussMCRefl->SetLineColor(kRed+2);
      doublegaussMCRefl->SetParameter(0, 1);
      //doublegaussMCRefl->SetParameter(1, 1);
      //doublegaussMCRefl->SetParameter(1, 1.82);
      doublegaussMCRefl->SetParLimits(1, 1.76,1.87);
      doublegaussMCRefl->SetParameter(2, 1);
      doublegaussMCRefl->SetParameter(3, 1);
      //doublegaussMCRefl->SetParameter(4, 1);
      //doublegaussMCRefl->SetParameter(4, 1.93);
      doublegaussMCRefl->SetParLimits(4, 1.87,2.1);
      doublegaussMCRefl->SetParameter(5, 1);
      hrefl[i]->Fit("doublegaussMCRefl", "MLFR");
      hrefl[i]->Draw();

     
      TF1 *fFitRefl = hrefl[i]->GetFunction("doublegaussMCRefl");
      //fFitReflection->cd();
  

     for(Int_t iBin2=1; iBin2<=hrefl[i]->GetNbinsX(); iBin2++){
        hFitReflNewTemp[i]->SetBinContent(iBin2, 0.);
        ratio[i]->SetBinContent(iBin2, 0.);

        //hFitReflNewTemp[i]->SetBinContent(iBin2, fFitRefl->Eval(hrefl[i]->GetBinCenter(iBin2)));
        //ratio[i]->SetBinContent(iBin2, (hrefl[i]->GetBinContent(iBin2) / fFitRefl->Eval(hrefl[i]->GetBinCenter(iBin2))));

        hFitReflNewTemp[i]->SetBinContent(iBin2, doublegaussMCRefl->Eval(hrefl[i]->GetBinCenter(iBin2)));
        ratio[i]->SetBinContent(iBin2, (hrefl[i]->GetBinContent(iBin2) / doublegaussMCRefl->Eval(hrefl[i]->GetBinCenter(iBin2))));

      }

     cRefl2->cd(i+1);
      hFitReflNewTemp[i]->Draw();

      cRatio->cd(i+1);
      ratio[i]->GetYaxis()->SetRangeUser(-1.5, 3.);
      ratio[i]->Draw();
      ratio[i]->Fit("pol0", "FM");


        //Float_t RoverS = hFitReflNewTemp[i]->Integral(hFitReflNewTemp[i]->FindBin(minRS),hFitReflNewTemp[i]->FindBin(maxRS))/hsig[i]->Integral(hsig[i]->FindBin(minRS),hsig[i]->FindBin(maxRS));
        Float_t RoverS = hrefl[i]->Integral(hrefl[i]->FindBin(minRS),hrefl[i]->FindBin(maxRS))/hsig[i]->Integral(hsig[i]->FindBin(minRS),hsig[i]->FindBin(maxRS));

    //cout << "\n\n\ Signal: " << hsig[i]->Integral() << "\t ref: " <<  hFitReflNewTemp[i]->Integral() / hFitReflNewTemp[i]->GetBinWidth(1) <<  endl;

//Double_t RoverS = hFitReflNewTemp[i]->Integral()/hsig[i]->Integral();

          hReflRS->SetBinContent(i+1,RoverS);
          hReflRS->SetBinError(i+1,0);

    }


      setHistoDetails(hReflRS,0,kGreen+2,20,0.9);
      hReflRS->GetYaxis()->SetTitle("R/S");
      hReflRS->SetMinimum(hReflRS->GetMinimum()*0.5);
      hReflRS->SetMaximum(hReflRS->GetMaximum()*1.2);
      TCanvas *cRS = new TCanvas("cRS","cRS",800,600);
      cRS->cd();
      hReflRS->Draw();

      SaveCanvas(cRS,outdir+"/plots/"+outName+"_refRatio");
     SaveCanvas(cSig,outdir+"/plots/"+outName+"_signal");
     SaveCanvas(cRefl,outdir+"/plots/"+outName+"_reflections");
     SaveCanvas(cRefl2,outdir+"/plots/"+outName+"_reflTemp");


    // --------------------------------------------------------
    // ----------- write to output file
    TFile *ofile = new TFile(Form("%s/reflectionTemplates_%s.root",outdir.Data(), postfix ? listName.Data() : outName.Data() ),"RECREATE");
    hReflRS->Write();    
    for(int i=0; i<fptbinsDN; i++){
      hsig[i]->Write();
      hrefl[i]->Write();
      hFitReflNewTemp[i]->Write();
      ratio[i]->Write();
       
    }
    ofile->Close();
    // --------------------------------------------------------

}



