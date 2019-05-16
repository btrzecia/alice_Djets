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

 //-----------------------------------------------------------------------
 //  Author B.Trzeciak
 //  Utrecht University
 //  barbara.antonina.trzeciak@cern.ch
 //-----------------------------------------------------------------------

 // Extraction of raw (or efficiency-corrected) D-jet pT spectrum
 // inv. mass method: side-band


#include "signalExtraction_z.h"
//--------------------------

bool isRefSys=0;
double refScale = 1.5;

void signalExtraction_SBz(
//  TString data = "$HOME/Work/alice/analysis/out/AnalysisResults.root",
  TString data = "/home/jackbauer/Work/alice/analysis/pp5TeV/D0jet/outData/trial_437.root",
  bool isEff = 0, 
  TString efffile ="",// "/home/jackbauer/Work/alice/analysis/pp5TeV/D0jet/results/DzeroR03_pPbCuts/Default/efficiency/DjetEff_prompt_jetpt5_50.root",
  bool isRef = 0, 
  TString refFile = "test.root",
  bool postfix = 0, 
  TString listName = "Cut",
//  TString out = "signalExtraction",
  TString out = "/media/jackbauer/data/z_out/",
  bool save = 1,
  bool isMoreFiles = 0,
  TString prod = "kl",   // for more than 1 file, for one file leave it empty)
  bool isprefix=0,
  TString saveDir="Feb28"
)
{

    fUseRefl = isRef;
    if(fUseRefl) fReflFilename = refFile;

    savePlots = save;
    bEff = isEff;
    TString plotsDir;
    if(bEff)plotsDir=Form("/plots/%s_jetbin_%d_%d", saveDir.Data(),(int)fptbinsJetA[(int)zjetbin-1], (int)fptbinsJetA[(int)zjetbin]);
    else plotsDir = Form("/plotsNoEff/%s_jetbin_%d_%d",saveDir.Data(),(int)fptbinsJetA[(int)zjetbin-1], (int)fptbinsJetA[(int)zjetbin]);
    TString outdir = out;
    gSystem->Exec(Form("mkdir %s",outdir.Data()));
    gSystem->Exec(Form("mkdir %s%s",outdir.Data(),plotsDir.Data()));

    if(!isMoreFiles) prod="";
    int nFiles = (int)prod.Length();

    TString histName;
		if(!isprefix){
		    if(fDmesonSpecie) histName = "histosDStarMBN";
		    else histName = "histosD0MBN";
		}
		else{
		    if(fDmesonSpecie) histName = "histosDStarMBN";
		    else histName = "histosD0";
		}
    // get analysis output file
    TString datafile;
    TFile *File;
    TDirectoryFile* dir;
    TList *histList;
    THnSparseF *sparse;

cout<<"-------------------------------------"<<endl;
cout<<"-------------------------------------"<<endl;
cout<<"-------------"<<zjetbin <<"--------------"<<endl;
cout<<"-------------------------------------"<<endl;
cout<<"-------------------------------------"<<endl;
cout<<fptbinsJetA[(int)zjetbin-1]<<endl;
cout<<fptbinsJetA[(int)zjetbin]<<endl;
cout<<"-------------------------------------"<<endl;
cout<<"-------------------------------------"<<endl;

    if(!isMoreFiles) {
      datafile = data;
      File = new TFile(datafile,"read");
      if(!File) { cout << "==== WRONG FILE WITH DATA =====\n\n"; return ;}
      dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");

      for(int i=0;i<ND; i++){
		if(!isprefix){
		          if(postfix) histList =  (TList*)dir->Get(Form("%s%d%s",histName.Data(),i,listName.Data()));
		          else histList =  (TList*)dir->Get(Form("%s%d",histName.Data(),i));
		}
		else{    if(postfix){ histList =  (TList*)dir->Get(Form("%s%sMBN%d",histName.Data(),listName.Data(),i));}
		          else {cout<<postfix<<"----dude! something's wrong, postfix has to be true if prefix is, check again-----"<<endl; return;}
		}
          sparse = (THnSparseF*)histList->FindObject("hsDphiz");
          sparse->GetAxis(0)->SetRangeUser(zmin,zmax);
          sparse->GetAxis(1)->SetRangeUser(jetmin,jetmax);
//		for (int j = 0; j < fptbinsJetN+1; j++){
//			if(zjet==j) sparse->GetAxis(1)->SetRangeUser(fptbinsJetA[0+j],fptbinsJetA[1+j]);
//			}
          if(zjetbin)sparse->GetAxis(1)->SetRangeUser(fptbinsJetA[(int)zjetbin-1],fptbinsJetA[(int)zjetbin]);
          if(isEta) sparse->GetAxis(5)->SetRangeUser(-jetEta,jetEta);
          if(i==0) hInvMassptD=(TH3D*)sparse->Projection(3,0,2);
          else hInvMassptD->Add((TH3D*)sparse->Projection(3,0,2));
      }
    }
    else {
      for (int j=0;j<nFiles;j++){
          datafile = data;
          datafile += prod.Data()[j];
          datafile += ".root";
          File = new TFile(datafile,"read");
          if(!File) { cout << "==== WRONG FILE WITH DATA =====\n\n"; return ;}
          dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");

          for(int i=0;i<ND; i++){
if(!isprefix){          if(postfix) histList =  (TList*)dir->Get(Form("%s%d%s",histName.Data(),i,listName.Data()));
          else histList =  (TList*)dir->Get(Form("%s%d",histName.Data(),i));
}
else {          if(postfix) histList =  (TList*)dir->Get(Form("%s%sMBN%d",histName.Data(),listName.Data(),i));
		          else {cout<<postfix<<"----dude! something's wrong, again! postfix has to be true if prefix is, check again-----"<<endl; return;}
}
              sparse = (THnSparseF*)histList->FindObject("hsDphiz");
              sparse->GetAxis(0)->SetRangeUser(zmin,zmax);
              sparse->GetAxis(1)->SetRangeUser(jetmin,jetmax);
              if(isEta) sparse->GetAxis(5)->SetRangeUser(-jetEta,jetEta);
              if(j==0 && i==0) hInvMassptD=(TH3D*)sparse->Projection(3,0,2);
              else hInvMassptD->Add((TH3F*)sparse->Projection(3,0,2));
          }
      }
    }

    efficiency = new double[fptbinsDN];
    if(bEff){
        TFile *FileEff = new TFile(efffile.Data(),"read");
        TH1F* hEff = (TH1F*)FileEff->Get("hEff_reb");
        for(int i=0;i<fptbinsDN;i++){
            double pt = (fptbinsDA[i]+fptbinsDA[i+1]) / 2.;
            efficiency[i] = hEff->GetBinContent(hEff->GetXaxis()->FindBin(pt));
        }
    }
    else {
        for(int i=0;i<fptbinsDN;i++){
            efficiency[i] = 1;
        }
    }

    // --------------------------------------------------------
    // fit inv. mass in D pT bins and get raw jet pT spectra in the signal and side-bands regions
     Bool_t okSignalExt = rawJetSpectra(outdir,prod);
     if(!okSignalExt) { std::cout << "!!!!!! Something wrong in the raw signal extraction !!!!" << endl; return; }
    // --------------------------------------------------------

    // --------------------------------------------------------
   //------------------- draw output histos -------------------
    if(savePlots){
      saveFitParams(outdir,prod);
      saveSpectraPlots(outdir,prod);
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
    hjetptspectrumRebScaled->Write();
    hjetptspectrumRebUnc->Write();

    for(int i=0; i<fptbinsDN; i++){
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
        if(bkgRfit[i] && fUseRefl && fDmesonSpecie == 0) {   bkgRfit[i]->Write(); }
        if(hjetptcorr[i]) hjetptcorr[i]->Write();
    }

    ofile->Close();
    // --------------------------------------------------------

}

Bool_t rawJetSpectra(TString outdir, TString prod){

    hmean = new TH1F("hmean","hmean",fptbinsDN,fptbinsDA);
    hsigma = new TH1F("hsigma","hsigma",fptbinsDN,fptbinsDA);
    hrelErr = new TH1F("hrelErr","hrelErr",fptbinsDN,fptbinsDA);
    hsign = new TH1F("hsign","hsign",fptbinsDN,fptbinsDA);
    hsb = new TH1F("hsb","hsb",fptbinsDN,fptbinsDA);
    hSignal = new TH1F("hSignal","hSignal",fptbinsDN,fptbinsDA);
    hSignal->Sumw2();
    hReflRS = new TH1F("hReflRS","hReflRS",fptbinsDN,fptbinsDA);

    if(fDmesonSpecie) hInvMassptD->GetXaxis()->SetTitle(Form("m(%s)(GeV/c^{2})","K#pi#pi-K#pi"));
    else hInvMassptD->GetXaxis()->SetTitle(Form("m(%s)(GeV/c^{2})","K#pi"));
    hInvMassptD->GetXaxis()->SetTitleSize(0.06);
    hInvMassptD->GetXaxis()->SetTitleOffset(0.9);
    hInvMassptD->GetYaxis()->SetTitle("z = p_{T, D}/p_{T,ch jet}");
    hInvMassptD->SetTitle("hInvMassptD");

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
    pvJet->SetTextSize(0.075);
    pvJet->AddText(Form("#splitline{in Charged Jets,}{Anti-#it{k}_{T}, #it{R} = 0.%d}",Rpar));
    pvJet->SetTextAlign(11);

    TPaveText *pvEta = new TPaveText(0.15,0.45-shift,0.8,0.5-shift,"brNDC");
    pvEta->SetFillStyle(0);
    pvEta->SetBorderSize(0);
    pvEta->SetTextFont(42);
    pvEta->SetTextSize(0.085);
    pvEta->SetTextAlign(11);
    pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

    int xnx = 3, xny=4;
//    if(fptbinsDN<3) { xnx = 3; xny=1; }
//    if(fptbinsDN>4 && fptbinsDN<7) { xnx = 2; xny=3; }
//    else if(fptbinsDN>6 && fptbinsDN<10) { xnx = 3; xny=3; }
//    else if(fptbinsDN>9 && fptbinsDN<13) { xnx = 3; xny=4; }
//    else { xnx = 4; xny=4; }

    TCanvas *c2 = new TCanvas("c2","c2",1200,1200);
    c2->Divide(xnx,xny);
    TCanvas *c2jet = new TCanvas("c2jet","c2jet",1200,1200);
    c2jet->Divide(xnx,xny);
    TCanvas *c2jetcorr = new TCanvas("c2jetcorr","c2jetcorr",1200,1200);
    c2jetcorr->Divide(xnx,xny);

    int firstPtBin = 0;
    if(fptbinsDA[0] == 1) firstPtBin = 2; //new
    else if(fptbinsDA[0] == 2) firstPtBin = 3;
    else if(fptbinsDA[0] == 3) firstPtBin = 4;
    else if(fptbinsDA[0] == 4) firstPtBin = 5;
    else if(fptbinsDA[0] == 5) firstPtBin = 6;
    else if(fptbinsDA[0] == 6) firstPtBin = 7;
    //if(!firstPtBin) { std::cout << "==== Wrong first value of the D pT (should be 2,3 or 4) === \n"; return kFALSE; }
    Float_t RS = 0;

    for(int i=0; i<fptbinsDN; i++){

        TH1D *hh=(TH1D*)hInvMassptD->ProjectionX(
		Form("hh_%d",i),
		hInvMassptD->GetYaxis()->FindBin(jetmin), 
		hInvMassptD->GetYaxis()->FindBin(jetmax)-1,
		hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i]), 
		hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i+1])-1
	);
        hh->Rebin(fRebinMass);

        hh->GetXaxis()->SetRangeUser(minf,maxf);
        hh->SetTitle(Form("%.1lf < pt^{%s} < %.1lf",fptbinsDA[i],fDmesonS.Data(),fptbinsDA[i+1]));

        TH1F* hmassfit = (TH1F*)hh->Clone("hmassfit");
        //TH1F* hmassfitD = (TH1F*)hh->Clone("hmassfit");
        //TH1F* hmassfit; hmassfitD->Copy(hmassfit);// = (TH1F*)hh->Clone("hmassfit");
        if(fDmesonSpecie) hmassfit->SetMaximum(hmassfit->GetMaximum()*1.3);

        float hmin = TMath::Max(minf,hmassfit->GetBinLowEdge(2));
        float hmax = TMath::Min(maxf,hmassfit->GetBinLowEdge(hmassfit->GetNbinsX()));
       // AliHFMassFitter* fitterp=new AliHFMassFitter((TH1F*)hmassfit,hmin,hmax,1,fbkgtype,0);
        AliHFInvMassFitter* fitterp = new AliHFInvMassFitter((TH1F*)hmassfit,hmin,hmax,fbkgtype,0);
        fitterp->SetInitialGaussianMean(fDmass);
        fitterp->SetInitialGaussianSigma(fDsigma);
        //fitterp->SetFixGaussianSigma(fDsigmafix[i]);

        if(fUseRefl && fDmesonSpecie == 0) {
          if(fSystem) SetReflection(fitterp,hmin,hmax,RS,i+firstPtBin); // older way from Fabio's templates for p-Pb
          else SetReflection(fitterp,hmin,hmax,RS,(Int_t)fptbinsDA[i],(Int_t)fptbinsDA[i+1]); // new for pp (new templates from D-jet code)
        }

        fitterp->MassFitter(kFALSE);
        //fitterp->PrintFunctions();

        TH1F* h=fitterp->GetHistoClone();
        massfit[i] = fitterp->GetMassFunc();
        massfit[i]->SetRange(hmin,hmax);
        massfit[i]->SetLineColor(4);
        fullfit[i] = h->GetFunction("funcmass");
        if(fullfit[i]) fullfit[i]->SetName(Form("fullfit_%d",i));
        hmass[i] = (TH1F*)h->Clone(Form("hmass_%d",i));
        hmass[i]->SetName(Form("hmass_%d",i));
        hmass[i]->GetYaxis()->SetTitle("Entries");
        hmass[i]->GetXaxis()->SetTitle("Invariant Mass (GeV/c^{2})");
        bkgfit[i] = fitterp->GetBackgroundRecalcFunc();
        bkgfit[i]->SetRange(hmin,hmax);
        bkgfit[i]->SetLineColor(2);
        bkgfit[i]->SetName(Form("bkgFit_%d",i));

        // bkg+reflection function
        if(fUseRefl && fDmesonSpecie == 0) {
          bkgRfit[i] = fitterp->GetBkgPlusReflFunc();
          bkgRfit[i]->SetName(Form("bkgFitWRef_%d",i));
          bkgRfit[i]->SetRange(hmin,hmax);
          bkgRfit[i]->SetLineColor(15);
          hReflRS->SetBinContent(i+1,RS);
          hReflRS->SetBinError(i+1,0);
        }

        TVirtualPad *pad = (TVirtualPad*)c2->GetPad(i+1);
        fitterp->DrawHere(pad,3,0);

        float Dsigma = 0, Dmean = 0, DmeanUnc = 0, DsigmaUnc = 0;

        if(!fullfit[i]) { std::cout << "======= Fit failed for bin: " << i << endl; continue; }

        Dsigma = fitterp->GetSigma();
        DsigmaUnc  = fitterp->GetSigmaUncertainty();
        Dmean = fitterp->GetMean();
        DmeanUnc = fitterp->GetMeanUncertainty();

        Double_t signal_c_min = Dmean-fsigmaSignal*Dsigma;
        Double_t signal_c_max = Dmean+fsigmaSignal*Dsigma;
        Double_t signal_l_min = Dmean+fsigmaBkg[0]*Dsigma;
        Double_t signal_l_max = Dmean+fsigmaBkg[1]*Dsigma;
        Double_t signal_u_min = Dmean+fsigmaBkg[2]*Dsigma;
        Double_t signal_u_max = Dmean+fsigmaBkg[3]*Dsigma;
        if(signal_l_min<hmin) signal_l_min = hmin;
        if(signal_u_max>hmax) signal_u_max = hmax;

        Double_t binwidth = hmass[i]->GetXaxis()->GetBinWidth(1)*0.5;
        // signal
        Int_t binmin = hmass[i]->GetXaxis()->FindBin(signal_c_min);
        Int_t binmax = hmass[i]->GetXaxis()->FindBin(signal_c_max);
        Double_t min = hmass[i]->GetXaxis()->GetBinCenter(binmin)-binwidth;
        Double_t max = hmass[i]->GetXaxis()->GetBinCenter(binmax-1)+binwidth;
        // side-bands
        Int_t binmin_sb1 = hmass[i]->GetXaxis()->FindBin(signal_l_min);
        Int_t binmax_sb1 = hmass[i]->GetXaxis()->FindBin(signal_l_max);
        Double_t min_sb1 = hmass[i]->GetXaxis()->GetBinCenter(binmin_sb1)-binwidth;
        Double_t max_sb1 = hmass[i]->GetXaxis()->GetBinCenter(binmax_sb1-1)+binwidth;
        // side-bands
        Int_t binmin_sb2 = hmass[i]->GetXaxis()->FindBin(signal_u_min);
        Int_t binmax_sb2 = hmass[i]->GetXaxis()->FindBin(signal_u_max);
        Double_t min_sb2 = hmass[i]->GetXaxis()->GetBinCenter(binmin_sb2)-binwidth;
        Double_t max_sb2 = hmass[i]->GetXaxis()->GetBinCenter(binmax_sb2-1)+binwidth;


        Double_t s=0, serr=0, srelerr=0, bkg=0, bkgerr=0, bkgref=0, ref=0;
        Double_t sb1=0, sb2=0, ref1=0, ref2=0;
        fitterp->Signal(fsigmaSignal, s, serr);
        fitterp->Background(min, max ,bkg, bkgerr);
        //fitterp->Background(fsigmaSignal,b,berr);
        sb1 = bkgfit[i]->Integral(min_sb1,max_sb1)/(Double_t)hmass[i]->GetBinWidth(1);
        sb2 = bkgfit[i]->Integral(min_sb2,max_sb2)/(Double_t)hmass[i]->GetBinWidth(1);
        if(fUseRefl && fDmesonSpecie == 0) {
          bkgref = bkgRfit[i]->Integral(min,max)/(Double_t)hmass[i]->GetBinWidth(1);
          ref = bkgref - bkg;
          ref1 = (bkgRfit[i]->Integral(min_sb1,max_sb1)/(Double_t)hmass[i]->GetBinWidth(1)) - sb1;
          ref2 = (bkgRfit[i]->Integral(min_sb2,max_sb2)/(Double_t)hmass[i]->GetBinWidth(1)) - sb2;
        }

        Double_t signf=0,signferr=0,sob=0,soberr=0;
        fitterp->Significance(fsigmaSignal,signf,signferr);
        if(s) srelerr = serr/s;
        if(bkg) sob = s/bkg;
        else sob = s;
        if(bkg && bkgerr) soberr = TMath::Sqrt((serr/bkg)*(serr/bkg) + (s/bkg/bkg*bkgerr)*(s/bkg/bkg*bkgerr));
        else soberr = serr;

        TPaveText *pvSig;
        if(fDmesonSpecie) pvSig = new TPaveText(0.55,0.47,0.95,0.75,"brNDC");
        else pvSig = new TPaveText(0.15,0.55,0.47,0.9,"brNDC");
        pvSig->SetFillStyle(0);
        pvSig->SetBorderSize(0);
        Bool_t twodigits=kTRUE;
        if(soberr*100. > 35.) twodigits=kFALSE;
        //if(twodigits) pv->AddText(Form("S/B (3#sigma) = (%.2f #pm %.2f)", sob,soberr));
        //else pv->AddText(Form("S/B (3#sigma) = (%.1f #pm %.1f)", sob,soberr));
        if(twodigits) pvSig->AddText(Form("S (2#sigma) = %.2f #pm %.2f", s,serr));
        else pvSig->AddText(Form("S (2#sigma) = %.1f #pm %.1f", s,serr));
        if(twodigits) pvSig->AddText(Form("B (2#sigma) = %.2f #pm %.2f", bkg,bkgerr));
        else pvSig->AddText(Form("B (2#sigma) = %.1f #pm %.1f", bkg,bkgerr));
        pvSig->AddText(Form("Signif.(2#sigma) = %.1f #pm %.1f", signf,signferr));
        pvSig->AddText(Form("S/B(2#sigma) = %.2f #pm %.2f", sob,soberr));
        if(fUseRefl && fDmesonSpecie == 0) pvSig->AddText(Form("R/S = %.2f", RS));
        pvSig->Draw("same");
        //if(isdetails) pvProd->Draw("same");
        //if(isdetails) pvCuts->Draw("same");

        TPaveText *pv;
        if(fDmesonSpecie) pv = new TPaveText(0.55,0.77,0.95,0.9,"brNDC");
        else pv = new TPaveText(0.57,0.77,0.95,0.9,"brNDC");
        pv->SetFillStyle(0);
        pv->SetBorderSize(0);
        if(fDmesonSpecie) pv->AddText(Form("#mu = (%.2f #pm %.2f) MeV/#it{c}^{2}", Dmean*1000,DmeanUnc*1000));
        else pv->AddText(Form("#mu = (%.2f #pm %.2f) GeV/#it{c}^{2}", Dmean,DmeanUnc));
        pv->AddText(Form("#sigma = (%.2f #pm %.2f) MeV/#it{c}^{2}", Dsigma*1000,DsigmaUnc*1000));
        pv->Draw("same");

        // ---------------- fitting results
        if(fDmesonSpecie) {
          hmean->SetBinContent(i+1,Dmean*1000);
          hmean->SetBinError(i+1,DmeanUnc*1000);
        }
        else {
          hmean->SetBinContent(i+1,Dmean);
          hmean->SetBinError(i+1,DmeanUnc);
        }
        hsigma->SetBinContent(i+1,Dsigma*1000);
        hsigma->SetBinError(i+1,DsigmaUnc*1000);

        hrelErr->SetBinContent(i+1,srelerr);
        hsign->SetBinContent(i+1,signf);
        hsign->SetBinError(i+1,signferr);
        hsb->SetBinContent(i+1,sob);
        hsb->SetBinError(i+1,soberr);
        hSignal->SetBinContent(i+1,s);
        hSignal->SetBinError(i+1,serr);

        // ---------------- side-band drawing
        hmass_l[i] = (TH1F*)hmass[i]->Clone("hmass_l");
        hmass_l[i]->GetXaxis()->SetRangeUser(signal_l_min,signal_l_max);
        hmass_l[i]->SetName(Form("hmass_l_%d",i));
        hmass_u[i] = (TH1F*)hmass[i]->Clone("hmass_u");
        hmass_u[i]->GetXaxis()->SetRangeUser(signal_u_min,signal_u_max);
        hmass_u[i]->SetName(Form("hmass_u_%d",i));
        hmass_c[i] = (TH1F*)hmass[i]->Clone("hmass_c");
        hmass_c[i]->GetXaxis()->SetRangeUser(signal_c_min,signal_c_max);
        hmass_c[i]->SetName(Form("hmass_c_%d",i));

        hmass_l[i]->SetFillColor(kBlue+2);
        hmass_u[i]->SetFillColor(kBlue+2);
        hmass_c[i]->SetFillColor(kRed+2);
        hmass_l[i]->SetFillStyle(3004);
        hmass_u[i]->SetFillStyle(3004);
        hmass_c[i]->SetFillStyle(3005);

        hmass_l[i]->Draw("hsame");
        hmass_u[i]->Draw("hsame");
        hmass_c[i]->Draw("hsame");

        //----------------------------------
        //-------- jet pt spectrum - signal
        hjetpt[i]=(TH1F*)hInvMassptD->ProjectionY(Form("hjetpt_%d",i),hInvMassptD->GetXaxis()->FindBin(signal_c_min), hInvMassptD->GetXaxis()->FindBin(signal_c_max)-1,hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i]), hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i+1])-1);
        hjetpt[i]->GetXaxis()->SetRangeUser(zplotmin,zplotmax);
        hjetpt[i]->SetTitle(Form("%.1lf < pt^{%s} < %.1lf",fptbinsDA[i],fDmesonS.Data(),fptbinsDA[i+1]));
        hjetpt[i]->SetMarkerColor(kRed+2);
        hjetpt[i]->SetLineColor(kRed+2);

        //------ jet pt spectrum - side bands
        TH1F* hjetpt_sb1=(TH1F*)hInvMassptD->ProjectionY(Form("hjetpt_sb1%d",i),hInvMassptD->GetXaxis()->FindBin(signal_l_min), hInvMassptD->GetXaxis()->FindBin(signal_l_max)-1,hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i]), hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i+1])-1);
        hjetpt_sb1->SetTitle(Form("%.1lf < pt^{%s} < %.1lf",fptbinsDA[i],fDmesonS.Data(),fptbinsDA[i+1]));
        hjetpt_sb1->SetMarkerColor(kBlue+2);
        hjetpt_sb1->SetLineColor(kBlue+2);
        TH1F* hjetpt_sb2=(TH1F*)hInvMassptD->ProjectionY(Form("hjetpt_sb2%d",i),hInvMassptD->GetXaxis()->FindBin(signal_u_min), hInvMassptD->GetXaxis()->FindBin(signal_u_max)-1,hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i]), hInvMassptD->GetZaxis()->FindBin(fptbinsDA[i+1])-1);
        hjetpt_sb2->SetTitle(Form("%.1lf < pt^{%s} < %.1lf",fptbinsDA[i],fDmesonS.Data(),fptbinsDA[i+1]));
        hjetpt_sb2->SetMarkerColor(kBlue+2);
        hjetpt_sb2->SetLineColor(kBlue+2);
        hjetpt_sb[i] = (TH1F*)hjetpt_sb1->Clone(Form("hjetpt_sb_%d",i));
        hjetpt_sb[i]->Add(hjetpt_sb2);

        hjetpt[i]->GetXaxis()->SetRangeUser(zplotmin,zplotmax);
        hjetpt[i]->GetXaxis()->SetTitle("z = p_{D}/p_{ch jet}");
        hjetpt[i]->GetXaxis()->SetTitleOffset(1.);
        hjetpt_sb[i]->GetXaxis()->SetRangeUser(zplotmin,zplotmax);

        // scale background from side bands to the background under the peak
        //double scalingB = bkg / hjetpt_sb[i]->Integral();

      //  cout << "====================== scaling: " << sb2 << endl;
        double scalingB = bkg/(sb1+sb2);
        double scalingS;

        if(fUseRefl && fDmesonSpecie == 0) {
          scalingS = s / ( s+ref - ( (ref1+ref2)*bkg ) / (sb1+sb2) );
        }

        hjetpt_sb[i]->Scale(scalingB);

        //------- subtract background from signal jet
        hjetptsub[i] = (TH1F*)hjetpt[i]->Clone(Form("hjetptsub_%d",i));
       	hjetptsub[i]->Add(hjetpt_sb[i],-1);
        if(fUseRefl && fDmesonSpecie == 0) {
          hjetptsub[i]->Scale(scalingS);
        }
	//hjetptsub[i]->Add(hjetpt_sb[i],-1);

	if(fsigmaSignal==2) hjetptsub[i]->Scale(1./0.9545); 
        hjetptsub[i]->SetMarkerColor(kGreen+3);
        hjetptsub[i]->SetLineColor(kGreen+3);


        c2jet->cd(i+1);
        gPad->SetLogy();
        hjetpt[i]->Draw("ep");
        hjetpt_sb[i]->Draw("epsame");
        hjetptsub[i]->Draw("epsame");
        if(isdetails) pvProd->Draw("same");
        if(isdetails) pvCuts->Draw("same");
        TLegend *l1 = new TLegend(0.65,0.65,0.88,0.85);
        l1->AddEntry(hjetpt[i],"signal","l");
        l1->AddEntry(hjetpt_sb[i],"SB","l");
        l1->Draw("same");

        TLegend *l2 = new TLegend(0.6,0.75,0.9,0.9);
        l2->AddEntry(hjetptsub[i],"bkg subtracted","l");
        l1->AddEntry(hjetptsub[i],"sig-SB","l");
        //l2->Draw("same");
        if(!hrawjetptspectrum) hrawjetptspectrum = (TH1F*)hjetptsub[i]->Clone("hrawjetptspectrum");
        else hrawjetptspectrum->Add(hjetptsub[i]);

        //------- correct for D* efficiency
        hjetptcorr[i] = (TH1F*)hjetptsub[i]->Clone(Form("hjetptcorr_%d",i));
        if(bEff) hjetptcorr[i]->Scale(1./efficiency[i]); // D efficiency
        hjetptcorr[i]->SetMarkerColor(kBlue+3);
        hjetptcorr[i]->SetLineColor(kBlue+3);

        c2jetcorr->cd(i+1);
        hjetptcorr[i]->Draw("ep");
        if(isdetails) pvProd->Draw("same");
        if(isdetails) pvCuts->Draw("same");
        TLegend *l3 = new TLegend(0.6,0.7,0.9,0.9);
        l3->AddEntry(hjetptcorr[i],"#splitline{corrected jet p_{T}}{spectrum}","l");
        l3->Draw("same");

        //----- add corrected jet pt spectrum distributions in each D pt bin into a one distribution
        if(!hjetptspectrum) hjetptspectrum = (TH1F*)hjetptcorr[i]->Clone("hjetptspectrum");
        else hjetptspectrum->Add(hjetptcorr[i]);

    }

    c2->cd(fptbinsDN+1);
    pvEn->Draw();
    pvD->Draw("same");
    pvJet->Draw("same");
    pvEta->Draw("same");
    c2jet->cd(fptbinsDN+1);
    pvEn->Draw();
    pvD->Draw("same");
    pvJet->Draw("same");
    pvEta->Draw("same");

    if(savePlots) SaveCanvas(c2,outdir+plotsDir+"/invMass"+prod);
    if(savePlots) SaveCanvas(c2jet,outdir+plotsDir+"/jetRawSpectrum"+prod);

    return kTRUE;

}

Bool_t SetReflection(AliHFInvMassFitter* &fitter, Float_t fLeftFitRange, Float_t fRightFitRange, Float_t &RS, Int_t iBin) {

  TFile *fileRefl = TFile::Open(fReflFilename.Data());
  if(!fileRefl){
    std::cout << "File " << fReflFilename << " (reflection templates) cannot be opened! check your file path!"; return kFALSE;
  }

  TString fHistnameRefl = "histRflFittedDoubleGaus_ptBin";
  TString fHistnameSign = "histSgn_";
  TH1F* histRefl = (TH1F*)fileRefl->Get(Form("%s%d",fHistnameRefl.Data(),iBin));
  TH1F* histSign = (TH1F*)fileRefl->Get(Form("%s%d",fHistnameSign.Data(),iBin));
  if(!histRefl || !histSign){
    std::cout << "Error in loading the template/signal histrograms! Exiting..." << endl; return kFALSE;
  }

  fitter->SetTemplateReflections(histRefl,"template",fLeftFitRange,fRightFitRange);
  Double_t RoverS = histRefl->Integral(histRefl->FindBin(fLeftFitRange),histRefl->FindBin(fRightFitRange))/histSign->Integral(histSign->FindBin(fLeftFitRange),histSign->FindBin(fRightFitRange));
  if(isRefSys) RoverS*=refScale;
  printf("R/S ratio in fit range for bin %d = %1.3f\n",iBin,RoverS);
  fitter->SetFixReflOverS(RoverS);

  RS = (Float_t)RoverS;
  return kTRUE;

}


Bool_t SetReflection(AliHFInvMassFitter* &fitter, Float_t fLeftFitRange, Float_t fRightFitRange, Float_t &RS, Int_t ptmin, Int_t ptmax) {

  TFile *fileRefl = TFile::Open(fReflFilename.Data());
  if(!fileRefl){
    std::cout << "File " << fReflFilename << " (reflection templates) cannot be opened! check your file path!"; return kFALSE;
  }

  TString fHistnameRefl = "histRflFittedDoubleGaus_pt";
  TString fHistnameSign = "histSgn_";
  TH1F* histRefl = (TH1F*)fileRefl->Get(Form("%s%d_%d",fHistnameRefl.Data(),ptmin,ptmax));
  TH1F* histSign = (TH1F*)fileRefl->Get(Form("%s%d_%d",fHistnameSign.Data(),ptmin,ptmax));
  if(!histRefl || !histSign){
    std::cout << "Error in loading the template/signal histrograms! Exiting..." << endl; return kFALSE;
  }

  fitter->SetTemplateReflections(histRefl,"template",fLeftFitRange,fRightFitRange);
  Double_t RoverS = histRefl->Integral(histRefl->FindBin(fLeftFitRange),histRefl->FindBin(fRightFitRange))/histSign->Integral(histSign->FindBin(fLeftFitRange),histSign->FindBin(fRightFitRange));
  if(isRefSys) RoverS*=refScale;
  //printf("R/S ratio in fit range for bin %d = %1.3f\n",iBin,RoverS);
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

      TPaveText *pvEn= new TPaveText(0.12,0.82,0.5,0.87,"brNDC");
      pvEn->SetFillStyle(0);
      pvEn->SetBorderSize(0);
      pvEn->SetTextFont(42);
      pvEn->SetTextSize(0.04);
      pvEn->SetTextAlign(11);
      pvEn->AddText(Form("%s",fSystemS.Data()));

      double shift = -0.10;
      TPaveText *pvJet = new TPaveText(0.12,0.65-shift,0.5,0.7-shift,"brNDC");
      pvJet->SetFillStyle(0);
      pvJet->SetBorderSize(0);
      pvJet->SetTextFont(42);
      pvJet->SetTextSize(0.04);
      pvJet->SetTextAlign(11);
      pvJet->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

      shift+=0.07;
      TPaveText *pvD = new TPaveText(0.12,0.65-shift,0.5,0.7-shift,"brNDC");
      pvD->SetFillStyle(0);
      pvD->SetBorderSize(0);
      pvD->SetTextFont(42);
      pvD->SetTextSize(0.04);
      pvD->SetTextAlign(11);
      if(fDmesonSpecie) pvD->AddText("with D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
      else pvD->AddText("with D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

      shift+=0.07;
      TPaveText *pvEta = new TPaveText(0.12,0.65-shift,0.5,0.7-shift,"brNDC");
      pvEta->SetFillStyle(0);
      pvEta->SetBorderSize(0);
      pvEta->SetTextFont(42);
      pvEta->SetTextSize(0.04);
      pvEta->SetTextAlign(11);
      pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

      shift+=0.07;
      TPaveText *pv3 = new TPaveText(0.12,0.65-shift,0.5,0.7-shift,"brNDC");
      pv3->SetFillStyle(0);
      pv3->SetBorderSize(0);
      pv3->SetTextFont(42);
      pv3->SetTextSize(0.04);
      pv3->SetTextAlign(11);
      pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)fptbinsDA[0],fDmesonS.Data(),(Int_t)fptbinsDA[fptbinsDN]));

       // before eff. correction
      TCanvas *cRawSpectrum = new TCanvas("cRawSpectrum","cRawSpectrum",800,600);
      setHistoDetails(hrawjetptspectrum,0,kBlue+1,20,1.2);
      hrawjetptspectrum->GetXaxis()->SetTitle("z = p_{D}/p_{ch,jet}");
      hrawjetptspectrum->GetYaxis()->SetTitle("Raw dN/dp_{T}");
      hrawjetptspectrum->GetXaxis()->SetRangeUser(zplotmin,zplotmax);
      hrawjetptspectrum->Draw();
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      pvEn->Draw("same");

      hjetptspectrum->GetYaxis()->SetTitle("dN/dp_{T}");
      hjetptspectrum->GetXaxis()->SetRangeUser(zplotmin,zplotmax);

      TCanvas *cSpectrum = new TCanvas("cSpectrum","cSpectrum",800,600);
      cSpectrum->SetLogy();
      setHistoDetails(hjetptspectrum,0,kRed,20,1.2);
      hjetptspectrum->SetMinimum(1);
      hjetptspectrum->GetXaxis()->SetTitle("z = p_{D}/p_{ch jet}");
      hjetptspectrum->GetXaxis()->SetRangeUser(zplotmin,zplotmax-0.01);
      hjetptspectrum->Draw();
      pv3->Draw("same");
      pvEn->Draw("same");
      pvD->Draw("same");
      pvJet->Draw("same");
      pvEta->Draw("same");
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      if(savePlots) SaveCanvas(cSpectrum,outdir+plotsDir+"/jetPtSpectrum_SB"+prod);

      TH1F* hjetptspectrumReb_tmp = (TH1F*)hjetptspectrum->Clone("hjetptspectrumReb_tmp");
      hjetptspectrumReb = (TH1F*)hjetptspectrumReb_tmp->Rebin(fptbinsZMeasN,"hjetptspectrumReb",fptbinsZMeasA);
//      hjetptspectrumRebScaled = (TH1F*)hjetptspectrumReb_tmp->Rebin(fptbinsZMeasN,"hjetptspectrumRebScaled",fptbinsZMeasA);
hjetptspectrumRebScaled = (TH1F*)hjetptspectrumReb->Clone("hjetptspectrumRebScaled");
      setHistoDetails(hjetptspectrumReb,0,kBlue,20,1.2); // with bin width scaling
      setHistoDetails(hjetptspectrumRebScaled,1,kBlue,20,1.2); // with bin width scaling
      hjetptspectrumReb->GetXaxis()->SetTitle("z = p_{D}/p_{ch jet}");
      hjetptspectrumRebScaled->GetXaxis()->SetTitle("z = p_{D}/p_{ch jet}");
      TCanvas *cSpectrumRebin = new TCanvas("cSpectrumRebinProb","cSpectrumRebinProb",800,600);
      cSpectrumRebin->SetLogy();
      //hjetptspectrumRebScaled->GetYaxis()->SetRangeUser(1,1000000);
      hjetptspectrumRebScaled->SetTitle(Form("p_{T,%s} > %d GeV/#it{c}",fDmesonS.Data(),(int)fptbinsDA[0]));
      //TH1F* hjetptspectrumRebScaledProb=(TH1F*)hjetptspectrumRebScaled->Clone("hjetptspectrumRebScaledProb");
      //hjetptspectrumRebScaledProb->Scale(1.0/hjetptspectrumRebScaled->Integral());
      hjetptspectrumRebScaled->Draw();
      //pvEn->Draw("same");
      //pvD->Draw("same");
      //pvJet->Draw("same");
      //pvEta->Draw("same");
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      //pv3->Draw("same");

      //SaveCanvas(cSpectrumRebin,outdir+plotsDir+"/jetPtSpectrum_SB_RebinProbLinear"+prod);
      SaveCanvas(cSpectrumRebin,outdir+plotsDir+"/jetPtSpectrum_SB_RebinProb"+prod);

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

      shift = 0;
      TPaveText *pvJet2 = new TPaveText(0.15,0.65-shift,0.9,0.7-shift,"brNDC");
      pvJet2->SetFillStyle(0);
      pvJet2->SetBorderSize(0);
      pvJet2->SetTextFont(42);
      pvJet2->SetTextSize(0.04);
      pvJet2->SetTextAlign(11);
      if(fDmesonSpecie) pvJet2->AddText("D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
      else pvJet2->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

      TPaveText *pvD2 = new TPaveText(0.15,0.58-shift,0.9,0.63-shift,"brNDC");
      pvD2->SetFillStyle(0);
      pvD2->SetBorderSize(0);
      pvD2->SetTextFont(42);
      pvD2->SetTextSize(0.04);
      pvD2->SetTextAlign(11);
      if(fDmesonSpecie) pvD2->AddText("with D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
      else pvD2->AddText("with D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

      TPaveText *pvEta2 = new TPaveText(0.15,0.51-shift,0.8,0.56-shift,"brNDC");
      pvEta2->SetFillStyle(0);
      pvEta2->SetBorderSize(0);
      pvEta2->SetTextFont(42);
      pvEta2->SetTextSize(0.04);
      pvEta2->SetTextAlign(11);
      pvEta2->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

      TPaveText *pv32 = new TPaveText(0.15,0.44,0.9,0.49,"brNDC");
      pv32->SetFillStyle(0);
      pv32->SetBorderSize(0);
      pv32->SetTextFont(42);
      pv32->SetTextSize(0.04);
      pv32->SetTextAlign(11);
      pv32->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)fptbinsDA[0],fDmesonS.Data(),(Int_t)fptbinsDA[fptbinsDN]));

      TCanvas *cSpectrumRebinUnc = new TCanvas("cSpectrumRebinUnc","cSpectrumRebinUnc",800,500);

      //hjetptspectrumRebUnc->GetXaxis()->SetRangeUser(jetplotmin,jetplotmax);
      hjetptspectrumRebUnc->Draw();
      if(isdetails) pvProd->Draw("same");
      if(isdetails) pvCuts->Draw("same");
      pvEn->Draw("same");
      pv32->Draw("same");
      pvD2->Draw("same");
      pvJet2->Draw("same");
      pvEta2->Draw("same");

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

    setHistoDetails(hmean,0,2,20,0.9);
    if(fDmesonSpecie) hmean->GetYaxis()->SetTitle("signal #mu (MeV/c^{2})");
    else hmean->GetYaxis()->SetTitle("signal #mu (GeV/c^{2})");
    setHistoDetails(hsigma,0,4,20,0.9);
    hsigma->GetYaxis()->SetTitle("signal #sigma (MeV/c^{2})");
    setHistoDetails(hsign,0,4,20,0.9);
    hsign->GetYaxis()->SetTitle("significance");
    setHistoDetails(hsb,0,8,20,0.9);
    hsb->GetYaxis()->SetTitle("S/B");
    setHistoDetails(hrelErr,0,6,20,0.9);
    hrelErr->GetYaxis()->SetTitle("rel. unc.");

    hmean->GetYaxis()->SetTitleOffset(1.8);


    if(fDmesonSpecie) {
      hmean->GetYaxis()->SetRangeUser(144.8,146);
      hsigma->GetYaxis()->SetRangeUser(0.35,0.8);
    }
    else {
      hmean->GetYaxis()->SetRangeUser(1.855,1.885);
      hsigma->GetYaxis()->SetRangeUser(9,35);
    }

    hSignal->SetName("hSignal");
    setHistoDetails(hSignal,0,2,20,0.9);
    hSignal->GetYaxis()->SetTitle("yield");
    hSignal->GetYaxis()->SetTitleOffset(1.6);
    hrelErr->GetYaxis()->SetTitleOffset(1.6);


    double mean = 145.421;
    TLine *lm = new TLine(fptbinsDA[0], mean, fptbinsDA[fptbinsDN], mean);
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
      setHistoDetails(hReflRS,0,kGreen+2,20,0.9);
      hReflRS->GetYaxis()->SetTitle("R/S");
      hReflRS->SetMinimum(hReflRS->GetMinimum()*0.5);
      hReflRS->SetMaximum(hReflRS->GetMaximum()*1.2);
      TCanvas *cRS = new TCanvas("cRS","cRS",800,600);
      cRS->cd();
      hReflRS->Draw();
    if(savePlots) SaveCanvas(cRS,outdir+plotsDir+"/RefOverS"+prod);
    }

}
void setHistoDetails(TH1 *h, int scale, Color_t color, Style_t Mstyle, Size_t size){
    Width_t width=2;
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

    c->SaveAs(Form("%s_pTD%d.png",name.Data(),(int)fptbinsDA[0]));
    c->SaveAs(Form("%s_pTD%d.pdf",name.Data(),(int)fptbinsDA[0]));
}

