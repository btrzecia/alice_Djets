//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------
//combined detector and bkg fluctuations matrxi for p-Pb


#include "config.h"

//====================== global =========================
TH2D *fMatrixPP;
TH2D *fMatrixDeltaPt;
TH1D *fRawSpectrum;
TH1D *fTrueSpectrum;
TH1D *fMeasSpectrum;

TH2D * hMtxPP = nullptr;
TH2D * hMtxDpt = nullptr;
TH2D * hMtxRe = nullptr;
TH2D * hMtxPro = nullptr;
TH2D * fMatrixProd = nullptr;

int LoadBackgroundMatrix(TString fn, TString mxname);
int LoadDetectorMatrix(TString fn, TString mxname, TString tsname, TString msname, bool norm = 1, TString spostfix="");
void WeightMatrixY(TH2D * Mtx, TH1D * h, bool divide);
int MtxPlots(TString outDir, TString outName);
TH2D * Rebin2D(const char* name, TH2D *h, int nx, const double *binx, int ny, const double *biny, bool crop);
TH2D * NormMatrixY(const char* name, TH2D * Mtx);
void NormMatrixY(TH2D * Mtx);
TH2D * ProductMatrix(TH2D * MtxA, TH2D * MtxB);
int plotSlice(TVirtualPad * p, TH2D * hMtxPP, TH2D * hMtxDpt, TH2D * hMtxRe, TH2D * hMtxPro, const double ptmin, const double ptmax);



void combineRM (
TString detRMFile,
bool isPrompt = 1,
TString outDir = "combinedMatrix",
bool useDeltaPt = 1,
TString bkgRMFile = "matrix.root",
bool fDoWeighting = 1,
bool fdivide = 1 )
{


    gStyle->SetOptStat(0000); //Mean and RMS shown
	gStyle->SetPadRightMargin(0.1);
	gSystem->Exec(Form("mkdir %s",outDir.Data()));
	gSystem->Exec(Form("mkdir %s/plots",outDir.Data()));


    if (useDeltaPt) {
      LoadBackgroundMatrix(bkgRMFile.Data(),"hBkgM");
      fMatrixDeltaPt->Sumw2();
        fMatrixDeltaPt->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
      fMatrixDeltaPt->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
        fMatrixDeltaPt->SetTitle("Bkg.Fluc. Matrix");
    }

    LoadDetectorMatrix(detRMFile.Data(),"hPtJet2d","hPtJetGen","hPtJetRec",0);


    if (!fTrueSpectrum) { Error("Unfold", "No true spectrum!");	return; }
    if (!fMeasSpectrum) { Error("Unfold", "No reconstructed spectrum!"); return; }


	fMatrixPP->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
    fMatrixPP->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
	fMatrixPP->SetTitle("Det.Res. Matrix");

	TCanvas *cMatrix = new TCanvas("cMatrix","cMatrix",1200,800);
    TH1D* priorhisto = (TH1D*) fTrueSpectrum->Clone("priorhisto");
    TH1D* hNormY;

	if(useDeltaPt) {
        TH2D *MatrixComb = ProductMatrix(fMatrixDeltaPt,fMatrixPP);
        fMatrixProd = (TH2D*)MatrixComb->Clone("fMatrixProd");
        // weighting the matrix
        if (fDoWeighting) {
                    hNormY=(TH1D*)fMatrixProd->ProjectionY("hNormY");
                    hNormY->Divide(priorhisto);
                    WeightMatrixY(fMatrixProd,hNormY,fdivide);
        }

        cMatrix->Divide(2,1);
        cMatrix->cd(1);
        gPad->SetLogz();
        fMatrixDeltaPt->Draw("colz");
        cMatrix->cd(2);
        gPad->SetLogz();
        fMatrixPP->Draw("colz");
    }
    else {
        fMatrixProd = (TH2D*)fMatrixPP->Clone("fMatrixProd");
    }

    if (!fMatrixProd) { Error("Unfold", "Error getting product matrix!"); return;	}

	fMatrixProd->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
    fMatrixProd->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
	fMatrixProd->SetTitle("Combined Matrix");

      if(fSystem) MtxPlots(outDir,"probability");

      hMtxPro = (TH2D*)fMatrixProd->Clone("hMtxPro");
      NormMatrixY(hMtxPro);
      hMtxPro->GetZaxis()->SetRangeUser(0.0001,1);

      TH1D* hProjYeff=(TH1D*)fMatrixProd->ProjectionY("hProjYeff");
      TH1D* hProjXeff=(TH1D*)fMatrixProd->ProjectionX("hProjXeff");

      TH2D *Matrix = Rebin2D("Matrix", fMatrixProd, fptbinsJetMeasN, fptbinsJetMeasA, fptbinsJetTrueN, fptbinsJetTrueA,0);
      Matrix->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
      Matrix->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
      Matrix->SetTitle("Combined Matrix");

      TH2D *MatrixProb = Rebin2D("MatrixProb", fMatrixProd, fptbinsJetMeasN, fptbinsJetMeasA, fptbinsJetTrueN, fptbinsJetTrueA,0);
      MatrixProb->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
      MatrixProb->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
      MatrixProb->SetTitle("Combined Prob Matrix");
      NormMatrixY(MatrixProb);
      MatrixProb->GetZaxis()->SetRangeUser(0.0001,1);

      hMtxPro->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
      hMtxPro->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
      hMtxPro->SetTitle("Combined Matrix");

/*  hMtxPP->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
  hMtxPP->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");
	hMtxPP->SetTitle("Det.Res. Matrix");*/

      TCanvas *cMatrixProb = new TCanvas("cMatrixProb","cMatrixProb",1200,800);
      if(useDeltaPt) {
            cMatrixProb->Divide(2,1);
            cMatrixProb->cd(1);
            gPad->SetLogz();
            hMtxDpt->Draw("colz");
            cMatrixProb->cd(2);
            gPad->SetLogz();
            hMtxPP->Draw("colz");
      }


  TPaveText *pvEn= new TPaveText(0.2,0.80,0.8,0.85,"brNDC");
  pvEn->SetFillStyle(0);
  pvEn->SetBorderSize(0);
  pvEn->SetTextFont(42);
  pvEn->SetTextSize(0.045);
  pvEn->SetTextAlign(11);
  pvEn->AddText(Form("%s",fSystemS.Data()));

  double shift = 0.35;
  TPaveText *pvJet = new TPaveText(0.55,0.66-shift,0.9,0.7-shift,"brNDC");
  pvJet->SetFillStyle(0);
  pvJet->SetBorderSize(0);
  pvJet->SetTextFont(42);
  pvJet->SetTextSize(0.03);
  pvJet->SetTextAlign(11);
  pvJet->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

  TPaveText *pvD = new TPaveText(0.55,0.61-shift,0.9,0.65-shift,"brNDC");
  pvD->SetFillStyle(0);
  pvD->SetBorderSize(0);
  pvD->SetTextFont(42);
  pvD->SetTextSize(0.03);
  pvD->SetTextAlign(11);
  if(isPrompt){
    if(fDmesonSpecie) pvD->AddText("With D^{*+} #rightarrow D^{0}#pi^{+}");
    else pvD->AddText("With D^{0} #rightarrow K^{-}#pi^{+}");
  }
  else {
    if(fDmesonSpecie) pvD->AddText("With B #rightarrowD^{*+} #rightarrow D^{0}#pi^{+}");
    else pvD->AddText("With B #rightarrow D^{0} #rightarrow K^{-}#pi^{+}");
  }

  TPaveText *pvEta = new TPaveText(0.55,0.56-shift,0.8,0.6-shift,"brNDC");
  pvEta->SetFillStyle(0);
  pvEta->SetBorderSize(0);
  pvEta->SetTextFont(42);
  pvEta->SetTextSize(0.03);
  pvEta->SetTextAlign(11);
  pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

  TPaveText *pv3 = new TPaveText(0.6,0.5-shift,0.9,0.54-shift,"brNDC");
  pv3->SetFillStyle(0);
  pv3->SetBorderSize(0);
  pv3->SetTextFont(42);
  pv3->SetTextSize(0.03);
  pv3->SetTextAlign(11);
  pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)fptbinsDA[0],fDmesonS.Data(),(Int_t)fptbinsDA[fptbinsDN]));

	TCanvas *cMatrixProd = new TCanvas();
	cMatrixProd->SetLogz();
	fMatrixProd->Draw("colz");

  pv3->Draw("same");
  pvEn->Draw("same");
  pvD->Draw("same");
  pvJet->Draw("same");
  pvEta->Draw("same");

  TCanvas *cMatrixProdProb = new TCanvas();
    cMatrixProdProb->SetLogz();
	hMtxPro->Draw("colz");

  pv3->Draw("same");
  pvEn->Draw("same");
  pvD->Draw("same");
  pvJet->Draw("same");
  pvEta->Draw("same");

  TFile *outMatrix;
  if(isPrompt) outMatrix = new TFile(Form("%s/combineMatrix.root",outDir.Data()),"recreate");
  else outMatrix = new TFile(Form("%s/combineMatrixFD.root",outDir.Data()),"recreate");
  fMatrixProd->Write();
  Matrix->Write();
  MatrixProb->Write();
  hMtxPro->Write();
  outMatrix->Close();

  Matrix->GetXaxis()->SetRangeUser(fptbinsJetMeasA[0],fptbinsJetMeasA[fptbinsJetMeasN]+5);
  Matrix->GetYaxis()->SetRangeUser(3,fptbinsJetTrueA[fptbinsJetTrueN]);

  MatrixProb->GetXaxis()->SetRangeUser(fptbinsJetMeasA[0],fptbinsJetMeasA[fptbinsJetMeasN]+5);
  MatrixProb->GetYaxis()->SetRangeUser(3,fptbinsJetTrueA[fptbinsJetTrueN]);

  shift = 0.41;
  pvJet = new TPaveText(0.5,0.66-shift,0.9,0.7-shift,"brNDC");
  pvJet->SetFillStyle(0);
  pvJet->SetBorderSize(0);
  pvJet->SetTextFont(42);
  pvJet->SetTextSize(0.03);
  pvJet->SetTextAlign(11);
  pvJet->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

  pvD = new TPaveText(0.5,0.61-shift,0.9,0.65-shift,"brNDC");
  pvD->SetFillStyle(0);
  pvD->SetBorderSize(0);
  pvD->SetTextFont(42);
  pvD->SetTextSize(0.03);
  pvD->SetTextAlign(11);
  if(isPrompt){
    if(fDmesonSpecie) pvD->AddText("With D^{*+} #rightarrow D^{0}#pi^{+}");
    else pvD->AddText("With D^{0} #rightarrow K^{-}#pi^{+}");
  }
  else {
    if(fDmesonSpecie) pvD->AddText("With B #rightarrowD^{*+} #rightarrow D^{0}#pi^{+}");
    else pvD->AddText("With B #rightarrow D^{0} #rightarrow K^{-}#pi^{+}");
  }

  pv3 = new TPaveText(0.5,0.56-shift,0.9,0.6-shift,"brNDC");
  pv3->SetFillStyle(0);
  pv3->SetBorderSize(0);
  pv3->SetTextFont(42);
  pv3->SetTextSize(0.03);
  pv3->SetTextAlign(11);
  pv3->AddText(Form("|#it{#eta}_{jet}| < 0.%d, %d < p_{T,%s} < %d GeV/#it{c}",9-Rpar,(Int_t)fptbinsDA[0],fDmesonS.Data(),(Int_t)fptbinsDA[fptbinsDN]));

  TCanvas *cMatrixProdReb = new TCanvas();
  cMatrixProdReb->SetLogz();
  Matrix->Draw("colz");
  pvEn->Draw("same");
  pvD->Draw("same");
  pvJet->Draw("same");
  pv3->Draw("same");

  TCanvas *cMatrixProdProbReb = new TCanvas();
  cMatrixProdProbReb->SetLogz();
  MatrixProb->Draw("colz");
  pvEn->Draw("same");
  pvD->Draw("same");
  pvJet->Draw("same");
  pv3->Draw("same");

    if(isPrompt){
        if(useDeltaPt) {
             cMatrix->SaveAs(Form("%s/plots/Matrices.png",outDir.Data()));
             cMatrixProb->SaveAs(Form("%s/plots/MatricesProb.png",outDir.Data()));
       }
        cMatrixProd->SaveAs(Form("%s/plots/ProdMatrix.png",outDir.Data()));
        cMatrixProdProb->SaveAs(Form("%s/plots/ProdMatrixProb.png",outDir.Data()));
        cMatrixProdReb->SaveAs(Form("%s/plots/ProdMatrixRebin.png",outDir.Data()));
        cMatrixProdProbReb->SaveAs(Form("%s/plots/ProdMatrixPropRebin.png",outDir.Data()));
    }
    else {
        if(useDeltaPt) {
            cMatrix->SaveAs(Form("%s/plots/MatricesFD.png",outDir.Data()));
            cMatrixProb->SaveAs(Form("%s/plots/MatricesProbFD.png",outDir.Data()));
        }
        cMatrixProd->SaveAs(Form("%s/plots/ProdMatrixFD.png",outDir.Data()));
        cMatrixProdProb->SaveAs(Form("%s/plots/ProdMatrixProbFD.png",outDir.Data()));
        cMatrixProdReb->SaveAs(Form("%s/plots/ProdMatrixRebinFD.png",outDir.Data()));
        cMatrixProdProbReb->SaveAs(Form("%s/plots/ProdMatrixPropRebinFD.png",outDir.Data()));
    }

	return;

}

int LoadDetectorMatrix(TString fn, TString mxname, TString tsname, TString msname, bool norm, TString spostfix) {
	TFile *f  = TFile::Open(fn);
	if (!f) { Error("LoadDetectorMatrix","Detector matrix file %s not found.",fn.Data()); return 0; }

    if(norm){
	     if (mxname == "") mxname = "Detectormatrix";
        //fMatrixPP = (TH2D*)f->Get(mxname);
        TH2D *matrix = (TH2D*)f->Get(mxname);
        //if (!fMatrixPP) {
        if (!matrix) {
            Error("LoadDetectorMatrix","Detector matrix %s could not be gotten from file.",mxname.Data());
            return 0;
        }
        fMatrixPP = NormMatrixY(mxname,matrix);
    }
    else {
        fMatrixPP = (TH2D*)f->Get(mxname);
        if (!fMatrixPP) {
            Error("LoadDetectorMatrix","Detector matrix %s could not be gotten from file.",mxname.Data());
            return 0;
        }

    }

    for(int i=0; i<=fMatrixPP->GetNbinsX()+1;i++){
        for(int j=0; j<=fMatrixPP->GetNbinsX()+1;j++){

            double cont = fMatrixPP->GetBinContent(i,j);
            if(i==0 && j==0)fMatrixPP->SetBinContent(i,j,0);
            else if(i==fMatrixPP->GetNbinsX()+1 && j==fMatrixPP->GetNbinsY()+1)fMatrixPP->SetBinContent(i,j,0);
            else fMatrixPP->SetBinContent(i,j,cont);

        }
    }

    /*
    TH1D *proj[50];
  fMatrixPP = new TH2D("fMatrixPP","fMatrixPP",50,0,50,50,0,50);
   for(int i=0; i<=50;i++){
       proj[i] = (TH1D*)matrix->ProjectionX(Form("proj%d",i), i, i+1 );
       proj[i]->SetName(Form("proj%d",i));
       proj[i]->Scale(1./proj[i]->Integral());

       for(int j=1;j<=50;j++){
            fMatrixPP->SetBinContent(j,i+1,proj[i]->GetBinContent(j));

        }
    }
    */

	if (tsname == "") tsname = "hTrue";
	fTrueSpectrum = (TH1D*) f->Get(tsname);
	//fTrueSpectrum = (TH1D*) fMatrixPP->ProjectionY("fTrueSpectrum");
	tsname+=spostfix;
	if (!fTrueSpectrum) {
		Error("LoadDetectorMatrix","True spectrum %s could not be gotten from file.",tsname.Data());
		return 0;
	}

	if (msname == "") msname = "hReco";
	msname+=spostfix;
	fMeasSpectrum = (TH1D*) f->Get(msname);
	//fMeasSpectrum = (TH1D*) fMatrixPP->ProjectionX("fMeasSpectrum");
    if (!fMeasSpectrum) {
		Error("LoadDetectorMatrix","True spectrum %s could not be gotten from file.",msname.Data());
		return 0;
	}

	Info("LoadDetectorMatrix", "mtx=%s, ts=%s, ms=%s loaded.",
		mxname.Data(), tsname.Data(), msname.Data());


	return 1;
}

/// load backround matrix
int LoadBackgroundMatrix(TString fn, TString mxname) {
	TFile *f  = TFile::Open(fn);
	if (!f) { Error("LoadBackgroundMatrix","Background matrix file %s not found.",fn.Data()); return 0; }

	fMatrixDeltaPt = (TH2D*)f->Get(mxname);
	if (!fMatrixDeltaPt) {
		Error("LoadBackgroundMatrix","Background matrix %s could not be gotten from file.",mxname.Data());
		return 0;
	}
	Info("LoadBackgroundMatrix", "%s loaded.", mxname.Data());

	for(int i=0; i<=fMatrixDeltaPt->GetNbinsX()+1;i++){
		 for(int j=0; j<=fMatrixDeltaPt->GetNbinsX()+1;j++){

				 double cont = fMatrixDeltaPt->GetBinContent(i,j);
				 if(i==0 && j==0)fMatrixDeltaPt->SetBinContent(i,j,0);
				 else if(i==fMatrixDeltaPt->GetNbinsX()+1 && j==fMatrixDeltaPt->GetNbinsY()+1)fMatrixDeltaPt->SetBinContent(i,j,0);
         else if(fMatrixDeltaPt->GetXaxis()->GetBinCenter(i) < fptbinsDA[0] || fMatrixDeltaPt->GetYaxis()->GetBinCenter(j) < fptbinsDA[0]) fMatrixDeltaPt->SetBinContent(i,j,0);
         else fMatrixDeltaPt->SetBinContent(i,j,cont);

		 }
 }

	return 1;


}

/// Plot probability matrices
int MtxPlots(TString outDir, TString outName) {

	TString tag = "tag";
    if (!fMatrixPP) { Error("MtxPlots","No unfolding matrix present."); return 0; }

	// Probabilities
	TCanvas *cMtx=new TCanvas("ProbMtx", "Probability matrices",50,50,800,800);
	cMtx->Divide(2,2);

	cMtx->cd(1)->SetLogz();
	cMtx->SetLogz();
	hMtxPP = (TH2D*)NormMatrixY("hMtxPP"+tag,fMatrixPP);
	hMtxPP->SetTitle("Detector prob. matrix");
	NormMatrixY(hMtxPP);
	hMtxPP->Draw("colz");

	if (fMatrixDeltaPt) {
		cMtx->cd(2)->SetLogz();
		cMtx->SetLogz();
		hMtxDpt = (TH2D*)NormMatrixY("hMtxDpt"+tag,fMatrixDeltaPt);
		hMtxDpt->SetTitle("Background prob. matrix");
		hMtxDpt->Draw("colz");

		cMtx->cd(3)->SetLogz();
		//if(!fMatrixProd)
	//	TH2D *fMatrixProd = getResponseMatrix( fMatrixDeltaPt );

        if (!fMatrixProd) { Error("MtxPlots", "Error getting product matrix!"); return 0; }

		hMtxRe = (TH2D*)NormMatrixY("hMtxRe"+tag,fMatrixProd);

		hMtxRe = (TH2D*)fMatrixProd->Clone("hMtRe");
		hMtxRe->SetTitle("Response prob. matrix");
		hMtxRe->Draw("colz");

		cMtx->cd(4)->SetLogz();
		hMtxPro = ProductMatrix(hMtxPP, hMtxDpt );
		hMtxPro->SetName("hMtxPro"+tag);
		hMtxPro->SetTitle("Product prob. matrix");
		NormMatrixY(hMtxPro);
		hMtxPro->Draw("colz");
	}

	cMtx->SaveAs(Form("%s/plots/%s_probMtx.pdf",outDir.Data(),outName.Data()));
	cMtx->SaveAs(Form("%s/plots/%s_probMtx.png",outDir.Data(),outName.Data()));

  if(fSystem){
	TCanvas *cSlices=new TCanvas("ProbSlice", "Probability slices",1000,1000);
	cSlices->Divide(3,2);
	plotSlice(cSlices->cd(1), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,5,10);
	plotSlice(cSlices->cd(2), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,10,15);
	plotSlice(cSlices->cd(3), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,15,20);
	plotSlice(cSlices->cd(4), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,20,25);
	plotSlice(cSlices->cd(5), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,25,30);

	cSlices->SaveAs(Form("%s/plots/%s_probSlices.pdf",outDir.Data(),outName.Data()));
	cSlices->SaveAs(Form("%s/plots/%s_probSlices.png",outDir.Data(),outName.Data()));
  }

	return 0;
}


/// Plot probability matrices
int plotSlice(TVirtualPad * p, TH2D * hMtxPP, TH2D * hMtxDpt, TH2D * hMtxRe, TH2D * hMtxPro, const double ptmin, const double ptmax) {

    TH1D * hSliceDpt = nullptr;
    TH1D * hSlicePP = nullptr;
    TH1D * hSliceRe = nullptr;
    TH1D * hSlicePro = nullptr;

	int imin = hMtxPP->GetYaxis()->FindBin(ptmin+0.0001);
	int imax = hMtxPP->GetYaxis()->FindBin(ptmax-0.0001);

	TString sname = "-"+TString::Itoa((int)(ptmin),10)+"-"+TString::Itoa((int)(ptmax),10);
	TString stitle = TString::Itoa((int)(ptmin),10)+"<p_{T}^{gen}<"+TString::Itoa((int)(ptmax),10)+" GeV/c";

	p->SetLogy();

	// takes width of 1st bin : only works for even binning!
	int binno = (fptbinsJetTrueA[1]-fptbinsJetTrueA[0]);

	if(hMtxDpt) {
		hSliceDpt = hMtxDpt->ProjectionX("hSliceDpt"+sname,imin,imax);
		hSliceDpt->SetTitle("probability matrix");
		hSliceDpt->SetMarkerColor(2);
		hSliceDpt->SetMarkerSize(0.8);
		hSliceDpt->SetMarkerStyle(20);
		hSliceDpt->SetLineColor(2);
		//setHistStyle(hSliceDpt, 2, 21);
		hSliceDpt->Rebin(binno);
		//hSliceDpt->Draw("p");
	}

	hSlicePP = hMtxPP->ProjectionX("hSlicePP"+sname,imin,imax);
	hSlicePP->SetMarkerColor(4);
	hSlicePP->SetMarkerStyle(20);
	hSlicePP->SetMarkerSize(0.8);
	hSliceDpt->SetLineColor(4);
	hSlicePP->Rebin(binno);
	/*if (hMtxDpt)
		hSlicePP->Draw("psame");
	else
		hSlicePP->Draw("HistP");*/

	if(hMtxRe) {
		hSliceRe = hMtxRe->ProjectionX("hSliceRe"+sname,imin,imax);
		//setHistStyle(hSliceRe, 4, 26);
		hSliceRe->SetMarkerColor(kMagenta);
		hSliceRe->SetMarkerStyle(20);
		hSliceRe->SetMarkerSize(0.8);
		hSliceRe->SetLineColor(1);
		hSliceRe->Rebin(binno);
		//hSliceRe->Draw("HistPSame");
	}

	if (hMtxPro) {
		hSlicePro = hMtxPro->ProjectionX("hSlicePro"+sname,imin,imax);
		//setHistStyle(hSlicePro, 6, 32);
		hSlicePro->SetMarkerColor(1);
		hSlicePro->SetMarkerStyle(20);
		hSlicePro->SetMarkerSize(0.8);
		hSlicePro->SetLineColor(1);
		hSlicePro->Rebin(binno);
		hSlicePro->Draw("HistPSame");
	}


	hSliceDpt->Draw("phist");
	hSlicePP->Draw("phistsame");
	//hSliceRe->Draw("psame");
	hSlicePro->Draw("phistsame");

	if(p) {
		TLegend* l=p->BuildLegend(0.5, 0.7, 0.89, 0.89, "");
		l->SetHeader(stitle);
		l->Draw();
	}

}

/// get response matrix. If no background, use just detector matrix, otherwise get product. Normalize if requested.
TH2D * getResponseMatrix(bool useDeltaPt) {

    TH2D *mtx;
    TH2D *mtx2, *fMatrixPP2, *fMatrixDeltaPt2;

	if (!useDeltaPt) {
        if (!fMatrixPP) { Error("getResponseMatrix","No unfolding matrix present."); return nullptr; }
 		mtx = (TH2D*) fMatrixPP->Clone();
 		//mtx = (TH2D*) fMatrixDeltaPt->Clone();
		mtx->Sumw2();
	}
	else {

		mtx = ProductMatrix(fMatrixPP, fMatrixDeltaPt);

	}

    TFile *outFileM = new TFile("outMatrix.root","recreate");
	outFileM->cd();
	fMatrixPP->Write();
	//fMatrixPP2->Write();
	//fMatrixDeltaPt->Write();
	//fMatrixDeltaPt2->Write();
	//mtx2->Write();
	mtx->Write();
	outFileM->Close();
	delete outFileM;

	return mtx;
}

/// get product of two matrices
TH2D * ProductMatrix(TH2D * MtxA, TH2D * MtxB) {
	// make sure the matrices exist
	if (!MtxA) {
        std::cerr << "Error in <AliHeavyUnfoldTools::ProductMatrix> : MtxA==0." << std::endl;
        return nullptr;
	}
	if (!MtxB) {
        std::cerr << "Error in <AliHeavyUnfoldTools::ProductMatrix> : MtxB==0." << std::endl;
        return nullptr;
	}

	Int_t binx_a=MtxA->GetNbinsX();
	Int_t biny_a=MtxA->GetNbinsY();
	Int_t binx_b=MtxB->GetNbinsX();
	Int_t biny_b=MtxB->GetNbinsY();

	// make sure the matrices are of the same size
	if (binx_b!=binx_a) {
        std::cerr << "Error in <AliHeavyUnfoldTools::ProductMatrix> : MtxA--MtxB dimension mismatch." << std::endl;
        return nullptr;
	}
  if (biny_b!=biny_a) {
        std::cerr << "Error in <AliHeavyUnfoldTools::ProductMatrix> : MtxA--MtxB dimension mismatch." << std::endl;
        return nullptr;
	}

	Double_t x_low = MtxA->GetXaxis()->GetBinLowEdge(1);
	Double_t x_up = MtxA->GetXaxis()->GetBinUpEdge(binx_a);
	Double_t y_low = MtxB->GetYaxis()->GetBinLowEdge(1);
	Double_t y_up = MtxB->GetYaxis()->GetBinUpEdge(biny_a);

	TH2D * MtxC=new TH2D("Matrix_prod","Product Matrix", binx_a, x_low, x_up, biny_b, y_low, y_up);
	Double_t c=0;
	for(Int_t k=0; k<=binx_a+1; k++){
		for(Int_t i=0; i<=biny_b+1; i++){
			c=0;
			for (Int_t j=0; j<=biny_a+1; j++){
				Double_t a = MtxB->GetBinContent(k,j);
				Double_t b = MtxA->GetBinContent(j,i);
				c+=a*b;
			}
			//MtxC->Fill(Float_t(k-0.5),Float_t(i-0.5),c);
			MtxC->SetBinContent(k,i,c);
		}
	}

	return MtxC;
}

/// Weight matrix along y axis by histo values
void WeightMatrixY(TH2D * Mtx, TH1D * h, bool divide) {

	if (!Mtx) {
        std::cerr << "Warning in <AliHeavyUnfoldTools::WeightMatrixY> : Mtx==0." << std::endl;
		return;
	}
	if (!h) {
        std::cerr << "Warning in <AliHeavyUnfoldTools::WeightMatrixY> : h==0." << std::endl;
		return;
	}

	for(int j=1; j<=Mtx->GetNbinsY()+1; j++) {

		double value = Mtx->GetYaxis()->GetBinCenter(j);
		double c = h->GetBinContent(h->GetXaxis()->FindBin(value));
		if (divide && c)  c = 1./c;
		//else c = 1.;
		for(int i=1; i<=Mtx->GetNbinsX()+1; i++) {
			Mtx->SetBinContent(i, j, Mtx->GetBinContent(i,j)*c);
			//Mtx->SetBinError(i, j, Mtx->GetBinError(i,j)*c);
		}
	}

}

/// rebin in 2d variable size - no such routine in Root
TH2D * Rebin2D(const char* name, TH2D *h, int nx, const double *binx, int ny, const double *biny, bool crop) {

	if (!h) {
        std::cerr << "Warning in <AliHeavyUnfoldTools::Rebin2D> : h==0." << std::endl;
        return nullptr;
	}

    TAxis *xaxis = h->GetXaxis();
    TAxis *yaxis = h->GetYaxis();

	TH2D * hre = new TH2D(name,name,nx,binx,ny,biny);
    hre->Sumw2();
    for (int i=1; i<=xaxis->GetNbins();i++) {
        for (int j=1; j<=yaxis->GetNbins();j++) {
            hre->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h->GetBinContent(i,j));
        }
    }

	/*
	const double epsilon = 0.00001;
	int ixmin = crop ? 0  : -1;
	int ixmax = crop ? nx : nx+1;
	int iymin = crop ? 0  : -1;
	int iymax = crop ? ny : ny+1;

	for(int ix=ixmin; ix<ixmax; ix++) {
		double xlo = (ix!=-1) ? binx[ix]   : h->GetXaxis()->GetBinCenter(0)-epsilon;
		double xhi = (ix!=nx) ? binx[ix+1] : h->GetXaxis()->GetBinCenter(h->GetfptbinsJetMeasN()+1)+epsilon;
		for(int iy=iymin; iy<iymax; iy++) {
			double ylo = (iy!=-1) ? binx[iy]   : h->GetYaxis()->GetBinCenter(0)-epsilon;
			double yhi = (iy!=ny) ? binx[iy+1] : h->GetYaxis()->GetBinCenter(h->GetfptbinsJetTrueN()+1)+epsilon;
			int k = 0;
			double hits = 0;
			double ersq = 0;
			for(int jx=0; jx<=h->GetfptbinsJetMeasN()+1; jx++) {
				if (h->GetXaxis()->GetBinCenter(jx) <  xlo) continue;
				if (h->GetXaxis()->GetBinCenter(jx) >= xhi) continue;
				for(int jy=0; jy<=h->GetfptbinsJetTrueN()+1; jy++) {
					if (h->GetYaxis()->GetBinCenter(jy) <  ylo) continue;
					if (h->GetYaxis()->GetBinCenter(jy) >= yhi) continue;

					hits += h->GetBinContent(jx,jy);
					ersq += pow(h->GetBinError(jx,jy)/h->GetBinContent(jx,jy),2);
					k++;
				}
			}

			hre->SetBinContent(ix+1, iy+1, hits);
			hre->SetBinError(ix+1, iy+1, hits*sqrt(ersq));
			//hre->SetBinError(ix+1, iy+1, 0);
		}
	}
*/
	for(int j=0;j<=hre->GetNbinsY()+1;j++){
		hre->SetBinContent(0,j,0);
		hre->SetBinError(0,j,0);
		hre->SetBinContent(hre->GetNbinsX()+1,j,0);
		hre->SetBinError(hre->GetNbinsX()+1,j,0);
	}

	return hre;
}

/// Prior for unfolding: try different prior functions wich best describe the raw specrum.
/// these shapes are from Gyulnara
TF1 * getPriorFunction(int prior, TH1D * spect, int priorType = 0) {

	if (!spect) {
		Error("getPriorFunction","Required spectrum does not exist.");
        return nullptr;
	}

	double fitlo = 3; // fFitPtMin;
	double fithi = 50; // fFitPtMax;

    TF1 *PriorFunction = nullptr;

	TString fitopt = "NR";
	//if (fLHfit) fitopt += "L";


   // PriorFunction = new TF1("PriorFunction","[0]*pow(1+(sqrt([1]*[1]+x*x)-[1])/([2]*[3]),-1*[2])",0,50);
   //PriorFunction->SetParameters(10,0.14,6.6,0.145);/
    PriorFunction = new TF1("PriorFunction","[0]* pow(x,-[1]) * exp(-[1]*[2]/x)",3,30);
    //PriorFunction->SetParameters(10,3,3);
    PriorFunction->FixParameter(2,6);
    if(priorType == 0) PriorFunction->FixParameter(1,3);
    else if(priorType == 1) PriorFunction->FixParameter(1,6);
    //PriorFunction->FixParameter(2,2);
    spect->Fit(PriorFunction, fitopt,"",fitlo,fithi);


   // PriorFunction = new TF1("PriorFunction","[0]*x+[1]",0,50);
   // PriorFunction->SetParameters(10,20);
    //spect->Fit(PriorFunction, fitopt,"",fitlo,fithi);

	PriorFunction->SetTitle("Chosen prior function");
	return PriorFunction;
}

/// Create a new, y-normalized matrix
TH2D * NormMatrixY(const char* name, TH2D * Mtx) {
	if (!Mtx) {
        std::cerr << "Warning in <AliHeavyUnfoldTools::NormMatrixY> : Mtx==0." << std::endl;
        return nullptr;
	}

	TH2D * Mre = (TH2D*)Mtx->Clone(name);
	NormMatrixY(Mre);

	return Mre;
}

/// Normalize matrix along y axis projection
void NormMatrixY(TH2D * Mtx) {
	if (!Mtx) {
        std::cerr << "Warning in <AliHeavyUnfoldTools::NormMatrixY> : Mtx==0." << std::endl;
		return;
	}

	TH1D * h = Mtx->ProjectionY();
	WeightMatrixY(Mtx, h, true);
}

/// get pearson coeffs from covariance matrix
TH2D * getPearsonCoeffs(const TMatrixD &covMatrix) {

	Int_t nrows = covMatrix.GetNrows();
	Int_t ncols = covMatrix.GetNcols();

	TH2D * PearsonCoeffs = new TH2D("PearsonCoeffs","Pearson Coefficients", nrows, 0, nrows, ncols, 0, ncols);
	for(Int_t row = 0; row<nrows; row++) {
		for(Int_t col = 0; col<ncols; col++) {
			Double_t pearson = 0.;
			if(covMatrix(row,row)!=0. && covMatrix(col,col)!=0.)
				pearson = covMatrix(row,col)/TMath::Sqrt(covMatrix(row,row)*covMatrix(col,col));
			PearsonCoeffs->SetBinContent(row+1,col+1, pearson);
		}
	}

	return PearsonCoeffs;
}
