//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------
//https://arxiv.org/pdf/1105.1160.pdf
//http://hep.physics.utoronto.ca/~orr/wwwroot/Unfolding/d-agostini.pdf

#include "config.h"

//====================== global =========================
TH2D* fMatrixPP;
TH2D* fMatrixProd;
TH2D* fMatrixDeltaPt;
TH1D* fRawSpectrum;
TH1D* fTrueSpectrum;
TH1D* fMeasSpectrum;

int LoadBackgroundMatrix(TString fn, TString mxname);
int LoadDetectorMatrix(TString fn, TString mxname, TString tsname, TString msname, bool norm = 1, TString spostfix="");
int LoadRawSpectrum(TString fn, TString sname, int rebin = 0, TString spostfix="");
int MtxPlots(TString outDir, TString outName);
TH2D* NormMatrixY(const char* name, TH2D* Mtx);
void NormMatrixY(TH2D * Mtx);
int plotSlice(TVirtualPad * p, TH2D * hMtxPP, TH2D * hMtxDpt, TH2D * hMtxRe, TH2D * hMtxPro, const double ptmin, const double ptmax);
TH2D * ProductMatrix(TH2D * MtxA, TH2D * MtxB);
TH2D * Rebin2D(const char* name, TH2D *h, int nx, const double *binx, int ny, const double *biny, bool crop);
void WeightMatrixY(TH2D * Mtx, TH1D * h, bool divide);
TF1* getPriorFunction(int prior, TH1D* spect, int priorType = 0, TH1D* rawspectrum = nullptr);
TH2D * getPearsonCoeffs(const TMatrixD &covMatrix);

	/***********************************
	############# define your bins #####################
	************************************/

int colortable[] = {kMagenta, kViolet, kBlue, kCyan+2, kGreen+4, kGreen+1, kYellow+1, kOrange+1, kRed, kRed+2};
int linesytle[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
	 // Int_t fColors[] = {1,2,8,4,kOrange-1,6,kGray+1,kCyan+1,kMagenta+2,kGreen+3,kViolet+5,kYellow+2};

/***********************************
############# begining of the macro ##################
************************************/
void unfold_Bayes
(
TString roounfoldpwd = "",
TString datafile = "file.root",
TString detRMfile = "detRM.root",
TString bkgRMfile = "bkgRM.root",
TString outDir = "out", // output directory
const int regBayes = 5,  // default reg. parameter for the bayes unfolding
bool isPrior = 0,  // if to use prior different than the true spectrum from the sim
int priorType = 1,   // if isPrior == 1, choose type of the prior
bool useDeltaPt = 1,  // if to use a separate bkg. fluctuation matrix
bool isFDUpSpec = 0,
bool isFDDownSpec = 0,
bool fDoWeighting = 1,
bool fdivide = 1,
bool overflow = 1,  // if to use overflow in the unfolding
const int NTrials = 10,//10,  //number of total trials
bool debug = 0
)
{

gStyle->SetOptStat(0000); //Mean and RMS shown
gSystem->Exec(Form("mkdir  %s",outDir.Data()));
gSystem->Exec(Form("mkdir  %s/plots",outDir.Data()));

	double plotmin = fptbinsJetTrueA[0] ;
	double plotmax = fptbinsJetTrueA[fptbinsJetTrueN];


	TString outName = "unfoldedSpectrum";
	//outName += "Bayes";
	//outName += regBayes;
	//if(fDoWeighting) outName += "_weight";
	//if(isPrior) { outName += "_"; outName += priorName; }
	//outName += "_PtMeas"; outName += fptbinsJetMeasA[0];	outName += "_"; outName += fptbinsJetMeasA[fptbinsJetMeasN];
	//outName += "_PtTrue"; outName += fptbinsJetTrueA[0];	outName += "_"; outName += fptbinsJetTrueA[fptbinsJetTrueN];


/***********************************
############# load raw spectrum and response matrices ##################
************************************/

if(isFDUpSpec) LoadRawSpectrum(datafile.Data(),"hData_binned_sub_up",0);
else if(isFDDownSpec) LoadRawSpectrum(datafile.Data(),"hData_binned_sub_down",0);
else LoadRawSpectrum(datafile.Data(),"hData_binned_sub",0);
if(useDeltaPt) LoadBackgroundMatrix(bkgRMfile.Data(),"hBkgM");
LoadDetectorMatrix(detRMfile.Data(),"hPtJet2d","hPtJetGen","hPtJetRec",0);

    if (!fRawSpectrum)  { Error("Unfold", "No raw spectrum!"); return;	}
    if (!fTrueSpectrum) { Error("Unfold", "No true spectrum!");	return; }
    if (!fMeasSpectrum) { Error("Unfold", "No reconstructed spectrum!"); return; }


	TH1D* truespec = (TH1D*)fTrueSpectrum->Clone("truespec"); //fTrueSpectrum is an (!un)initialized 1D histogram. 		//They are initialised by the LoadR/B/D....() functions above.
	truespec->Scale(1./truespec->Integral());
	TH2D* MatrixDeltaReb;
	TH2D* MatrixComb;

	if(useDeltaPt){
        fMatrixDeltaPt->Sumw2();
        //WeightMatrixY(fMatrixDeltaPt,fTrueSpectrum,0);
        MatrixDeltaReb = Rebin2D("MatrixDeltaReb", fMatrixDeltaPt, fptbinsJetMeasN, fptbinsJetMeasA, fptbinsJetTrueN, fptbinsJetTrueA,0);
        MatrixComb = ProductMatrix(fMatrixDeltaPt,fMatrixPP); //product of two matrices
	}

	if(useDeltaPt) fMatrixProd = (TH2D*)MatrixComb->Clone("fMatrixProd");
	else fMatrixProd = (TH2D*)fMatrixPP->Clone("fMatrixProd");
    if (!fMatrixProd) { Error("Unfold", "Error getting product matrix!"); return;	}

	TH1D* priorhisto = (TH1D*) fTrueSpectrum->Clone("priorhisto");
	TH1D *rawspectrum = (TH1D*)fRawSpectrum->Clone("rawspectrum");
	//rawspectrum->Scale(1./rawspectrum->Integral());
	rawspectrum->Scale(1,"width");

	//if(isPrior && priorType == 0) priorhisto = (TH1D*) rawspectrum->Clone("priorhisto");
	//if(isPrior && priorType == 0) priorhisto = (TH1D*) fRawSpectrum->Clone("priorhisto");

        TF1* fPriorFunction;
        if(isPrior) {
					fPriorFunction = getPriorFunction(isPrior, priorhisto,priorType, rawspectrum);
                    priorhisto->SetTitle("");
					priorhisto->GetXaxis()->SetTitle("p_{T, ch.jet}");
					TCanvas* cPrior = new TCanvas("cPrior0", "cPrior0", 800, 600);
					cPrior->SetLogy();
					TH1D* histoPrior=(TH1D*)priorhisto->Clone();
					histoPrior->Draw();
					if(priorType == 8) rawspectrum->Draw();
					fPriorFunction->Draw("same");

					cPrior->SaveAs(Form("%s/plots/%s_prior.pdf",outDir.Data(),outName.Data()));
					cPrior->SaveAs(Form("%s/plots/%s_prior.png",outDir.Data(),outName.Data()));
				}

    TH1D* hNormY;
    // weighting the matrix
		if (fDoWeighting) {
        std::cout << "==== weighting ==== " << std::endl;
				hNormY=(TH1D*)fMatrixProd->ProjectionY("hNormY");
				if (isPrior){
            std::cout << "=== using prior function ====" << std::endl;
                    if (! hNormY->Divide(fPriorFunction) ) { std::cout << "\"divide\" failed "; return; }
        }
				else{
            std::cout << "==== dividing ==== " << std::endl;
					hNormY->Divide(priorhisto);
        }
				WeightMatrixY(fMatrixProd,hNormY,fdivide);
	}

    TH1D* hProjYeff=(TH1D*)fMatrixProd->ProjectionY("hProjYeff");
    TH1D* hProjXeff=(TH1D*)fMatrixProd->ProjectionX("hProjXeff");

    TH2D* Matrix = Rebin2D("Matrix", fMatrixProd, fptbinsJetMeasN, fptbinsJetMeasA, fptbinsJetTrueN, fptbinsJetTrueA,0);
    TH1D* hProjXeffRebin=(TH1D*)hProjXeff->Rebin(fptbinsJetMeasN, "hProjXeffRebin", fptbinsJetMeasA);
    TH1D* hProjYeffRebin=(TH1D*)hProjYeff->Rebin(fptbinsJetTrueN, "hProjYeffRebin", fptbinsJetTrueA);
    TH1D* fRawRebin=(TH1D*)fRawSpectrum->Rebin(fptbinsJetMeasN, "fRawRebin", fptbinsJetMeasA);


/**************************************************
############# unfolding settings ##################
**************************************************/

    RooUnfold::ErrorTreatment errorTreatment = RooUnfold::kCovariance;
    RooUnfoldResponse response(hProjXeffRebin,hProjYeffRebin, Matrix, "response","response");
    response.UseOverflow(overflow);

    TH1D* fUnfoldedBayes[NTrials];
    TH1D* folded[NTrials];
    TH2D* fPearsonCoeffs[NTrials];
    TH1D* hRatioSpectrum[NTrials];
    TH1D* hRatio[NTrials];

    TCanvas* cUnfolded = new TCanvas("cUnfolded","cUnfolded",800,600);
    cUnfolded->SetLogy();
    TLegend* leg =  new TLegend(0.6,0.4,0.85,0.85);
    leg->SetBorderSize(0);

//------------ do unfolding NTrials times ------------
	for(Int_t ivar=0; ivar<NTrials; ivar++){//changes
/***********************************
############# unfolding ##################
************************************/
		RooUnfoldBayes unfold (&response, fRawRebin, ivar+1);
		fUnfoldedBayes[ivar] = (TH1D*)unfold.Hreco();
		folded[ivar] = (TH1D*)response.ApplyToTruth(fUnfoldedBayes[ivar]);

		// ------------ Get Person coefficient ------------
		fPearsonCoeffs[ivar] = getPearsonCoeffs( unfold.Ereco(RooUnfold::kCovariance) );
		fPearsonCoeffs[ivar]->SetName(Form("PearsonCoeffs%d",ivar));

		fUnfoldedBayes[ivar]->SetLineColor(colortable[ivar]);
		fUnfoldedBayes[ivar]->SetLineStyle(1);
		fUnfoldedBayes[ivar]->SetMarkerColor(colortable[ivar]);
		fUnfoldedBayes[ivar]->SetMarkerStyle(fMarkers[ivar]);
		fUnfoldedBayes[ivar]->SetLineWidth(2);
		fUnfoldedBayes[ivar]->GetXaxis()->SetTitle("p_{T,jet}^{ch} (GeV/#it{c})");
		fUnfoldedBayes[ivar]->GetYaxis()->SetTitle("dN/dp_{T}");
		fUnfoldedBayes[ivar]->SetTitle("Bayes unfolding");
		fUnfoldedBayes[ivar]->GetXaxis()->SetRangeUser(plotmin,plotmax);

		if(ivar == 0) { fUnfoldedBayes[ivar]->Draw(); }
		fUnfoldedBayes[ivar]->Draw("same");
		leg->AddEntry(fUnfoldedBayes[ivar],Form("Reg=%d",ivar+1),"p");

	}
	leg->Draw("same");

	cUnfolded->SaveAs(Form("%s/plots/%s_unfSpectra.pdf",outDir.Data(),outName.Data()));
	cUnfolded->SaveAs(Form("%s/plots/%s_unfSpectra.png",outDir.Data(),outName.Data()));

	TCanvas *cPearson = new TCanvas("cPearson","cPearson",1800,1800);
	cPearson->Divide(3,3);
	for(Int_t ivar=1; ivar<NTrials; ivar++)
	{
		cPearson->cd(ivar);
		fPearsonCoeffs[ivar]->SetTitle(Form("k=%d",ivar+1));
		fPearsonCoeffs[ivar]->SetMaximum(1);
		fPearsonCoeffs[ivar]->SetMinimum(-1);
		fPearsonCoeffs[ivar]->Draw("colz");
	}

	cPearson->SaveAs(Form("%s/plots/%s_Pearson.pdf",outDir.Data(),outName.Data()));
	cPearson->SaveAs(Form("%s/plots/%s_Pearson.png",outDir.Data(),outName.Data()));

    TCanvas *cRatio2 = new TCanvas("cRatio2","cRatio2",800,600);
    TLegend *legRatio2 =  new TLegend(0.4,0.15,0.6,0.5);//0.6,0.5,0.65,0.85
    legRatio2->SetBorderSize(0);
		TH1D* hBaseMeasure = (TH1D*)fRawRebin->Clone("hBaseSpectrum");

		// =============== Refolded / Measured
    THStack *hRatioS = new THStack("hRatioS","refolded/measured");
    for(Int_t ivar=0; ivar<NTrials; ivar++){
        hRatio[ivar] = (TH1D*) folded[ivar]->Clone(Form("hRatio%d",ivar));
        hRatio[ivar]->Divide(hBaseMeasure);
        hRatio[ivar]->GetYaxis()->SetTitle("refolded/measured");
				hRatio[ivar]->SetTitle(Form("Reg=%d",ivar+1));
        //hRatio[ivar]->GetYaxis()->SetRangeUser(0,3.5);
        //hRatio[ivar]->SetTitle();

        hRatio[ivar]->SetMarkerStyle(fMarkers[ivar]);
        hRatio[ivar]->SetMarkerColor(colortable[ivar]);
        hRatio[ivar]->SetMarkerSize(0);
        hRatio[ivar]->SetLineColor(colortable[ivar]);
				hRatio[ivar]->SetLineStyle(linesytle[ivar]);
				hRatio[ivar]->SetLineWidth(2);

				int counter = 0;
        for(int kk=1; kk<fptbinsJetMeasN; kk++){
                double val = fabs(1-hRatio[ivar]->GetBinContent(kk));
                //std::cout << "==val: " << val << std::endl;
                if(val<0.1) counter++;
        }
       //if(counter == fptbinsJetMeasN-2) std::cout << "!!!!!! bin found: " << ivar+1 << std::endl;
        hRatio[ivar]->GetXaxis()->SetRangeUser(fptbinsJetMeasA[0],fptbinsJetMeasA[fptbinsJetMeasN]);

				if(ivar==0){ //hRatio[ivar]->GetXaxis()->SetRangeUser(plotmin,plotmax);
						continue;
				}
				else hRatioS->Add(hRatio[ivar]);
			//	else if(ivar==1)  hRatio[ivar]->Draw("hist");
			//	else hRatio[ivar]->Draw("samehist");

			//	legRatio2->AddEntry(hRatio[ivar],Form("Reg=%d",ivar+1),"l");

    }

		hRatioS->SetMinimum(hRatioS->GetMinimum("nostack"));
		hRatioS->Draw("nostackhist");
//		hRatioS->GetXaxis()->SetTitle("p_{T, ch.jet}");
		gPad->BuildLegend(0.55,0.65,0.9,0.9,"");
    //legRatio2->Draw("same");

    TLine *line = new TLine(fptbinsJetMeasA[0],1,fptbinsJetMeasA[fptbinsJetMeasN],1);
    line->SetLineStyle(2);
    line->SetLineWidth(2);
    line->Draw("same");

    //cRatio2->SaveAs(Form("%s/%s_foldedRatio.png",outDir.Data(),outName.Data()));
    cRatio2->SaveAs(Form("%s/plots/%s_foldedRatio.pdf",outDir.Data(),outName.Data()));
		cRatio2->SaveAs(Form("%s/plots/%s_foldedRatio.png",outDir.Data(),outName.Data()));

		// =============== Unfolded
    THStack *hRatioUnfS = new THStack("hRatioUnfS",Form("Unfolded RegX/Reg%d",regBayes));
    TCanvas *cRatio = new TCanvas("cRatio","cRatio",800,600);
    TH1D *hBaseSpectrum = (TH1D*)fUnfoldedBayes[regBayes-1]->Clone("hBaseSpectrum");

    for(Int_t ivar=0; ivar<NTrials; ivar++){
        hRatioSpectrum[ivar] = (TH1D*) fUnfoldedBayes[ivar]->Clone(Form("hRatioSpectrum%d",ivar));
        hRatioSpectrum[ivar]->Divide(hBaseSpectrum);
        hRatioSpectrum[ivar]->GetYaxis()->SetTitle(Form("RegX/Reg%d",regBayes));
        hRatioSpectrum[ivar]->SetTitle(Form("Reg=%d",ivar+1));
        hRatioSpectrum[ivar]->SetMarkerStyle(fMarkers[ivar]);//[iter]);
        hRatioSpectrum[ivar]->SetMarkerColor(colortable[ivar]);//[iter]);//(colortable[iter+1]);
        hRatioSpectrum[ivar]->SetMarkerSize(0);
        hRatioSpectrum[ivar]->SetLineColor(colortable[ivar]);//[iter]);//(colortable[iter+1]);
				hRatioSpectrum[ivar]->SetLineStyle(linesytle[ivar]);
				hRatioSpectrum[ivar]->SetLineWidth(2);

        if((ivar == 0)  || (ivar == regBayes-1)) continue;
        hRatioUnfS->Add(hRatioSpectrum[ivar]);

        //if(ivar==1) { hRatioSpectrum[ivar]->GetYaxis()->SetRangeUser(0.8,1.3); hRatioSpectrum[ivar]->Draw("hist"); }
        //else hRatioSpectrum[ivar]->Draw("samehist");

    }
    hRatioUnfS->SetMinimum(hRatioUnfS->GetMinimum("nostack"));
    //hRatioUnfS->SetMaximum(hRatioUnfS->GetMaximum("nostack")*0.8);
    hRatioUnfS->Draw("nostackhist");
//	hRatioUnfS->GetXaxis()->SetTitle("p_{T, ch.jet}");
    gPad->BuildLegend(0.55,0.65,0.9,0.9,"");
    line->Draw("same");

    cRatio->SaveAs(Form("%s/plots/%s_unfRatio.pdf",outDir.Data(),outName.Data()));
    cRatio->SaveAs(Form("%s/plots/%s_unfRatio.png",outDir.Data(),outName.Data()));

    fRawRebin->SetLineColor(kBlue+1);
    fRawRebin->SetMarkerColor(kBlue+1);
    fRawSpectrum->SetLineColor(kBlue+1);
    fRawSpectrum->SetMarkerColor(kBlue+1);
    fUnfoldedBayes[regBayes-1]->SetLineColor(kRed+1);
    fUnfoldedBayes[regBayes-1]->SetMarkerColor(kRed+1);
    fUnfoldedBayes[regBayes-1]->SetMarkerStyle(21);
		fUnfoldedBayes[regBayes-1]->SetLineStyle(1);
    folded[regBayes-1]->SetLineColor(kGreen+1);//(kRed+1);
    folded[regBayes-1]->SetMarkerColor(kGreen+1);//(kRed+1);

    fUnfoldedBayes[regBayes-1]->SetName("unfoldedSpectrum");
    folded[regBayes-1]->SetName("foldedSpectrum");
    fRawRebin->SetName("fRawRebin");

		TH1F *hUnfolded_Unc = (TH1F*)fUnfoldedBayes[regBayes-1]->Clone("hUnfolded_Unc");
		hUnfolded_Unc->GetYaxis()->SetTitle("Rel. unc.");
		hUnfolded_Unc->SetLineColor(kGreen+1);
		hUnfolded_Unc->SetMarkerColor(kGreen+1);

		for(int j=1; j<=fUnfoldedBayes[regBayes-1]->GetNbinsX();j++){
								double err;
								if(fUnfoldedBayes[regBayes-1]->GetBinContent(j)) err = fUnfoldedBayes[regBayes-1]->GetBinError(j)/fUnfoldedBayes[regBayes-1]->GetBinContent(j);
								else err = 0;
								hUnfolded_Unc->SetBinContent(j,err);
								hUnfolded_Unc->SetBinError(j,0);
		}

        hUnfolded_Unc->SetTitle("");
		hUnfolded_Unc->SetMaximum(hUnfolded_Unc->GetMaximum()*1.2);
		hUnfolded_Unc->SetMinimum(0);

    TFile *outSpectra = new TFile(Form("%s/%s_unfoldedJetSpectrum.root",outDir.Data(),outName.Data()),"recreate");
		//TFile *outSpectra = new TFile(Form("%s/%s_unfoldedJetSpectrum.root",outDir.Data(),outName.Data()),"recreate");
    hProjYeff ->Write();
    hProjXeff ->Write();
    hProjYeffRebin ->Write();
    hProjXeffRebin ->Write();
    fMatrixProd->Write();
    Matrix->Write();

    fRawRebin->SetTitle("");
//    fUnfoldedBayes[regBayes-1]->SetLineColor(kRed+1);
//    fUnfoldedBayes[regBayes-1]->SetMarkerColor(kRed+1);
    fRawRebin ->Write();
    fUnfoldedBayes[regBayes-1]->Write();
    folded[regBayes-1]->Write();
		hUnfolded_Unc->Write();

    outSpectra->Close();

/* ==============================
 * Drawing, etc., below
 ================================*/
 /* ==================
	* Scale with the bin width
	================== */
		 fRawRebin->Scale(1,"width");
		 for(Int_t ivar=0; ivar<NTrials; ivar++){ fUnfoldedBayes[ivar]->Scale(1,"width"); }
		 folded[regBayes-1]->Scale(1,"width");
         fUnfoldedBayes[regBayes-1]->SetTitle("");

		 TPaveText *pvEn= new TPaveText(0.2,0.80,0.8,0.85,"brNDC");
		 pvEn->SetFillStyle(0);
		 pvEn->SetBorderSize(0);
		 pvEn->SetTextFont(42);
		 pvEn->SetTextSize(0.045);
		 pvEn->SetTextAlign(11);
		 pvEn->AddText(Form("%s",fSystemS.Data()));

		 double shift = 0.052;
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
		 pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)fptbinsDA[0],fDmesonS.Data(),(Int_t)fptbinsDA[fptbinsDN]));

    TCanvas* cSpectra = new TCanvas("cSpectra","cSpectra",800,600);
    cSpectra->SetLogy();
    //fRawSpectrum ->Draw();
    fUnfoldedBayes[regBayes-1]->Draw();
    folded[regBayes-1]->Draw("same");
    fRawRebin ->Draw("same");
    pv3->Draw("same");
    pvEn->Draw("same");
    pvD->Draw("same");
    pvJet->Draw("same");
    pvEta->Draw("same");

    TLegend* ls = new TLegend(0.55,0.67,0.88,0.87);
    ls->SetBorderSize(0);
    ls->AddEntry(fRawRebin,"Measured","p");
    ls->AddEntry(fUnfoldedBayes[regBayes-1], Form("Unfolded, Bayes reg=%d",regBayes,"p"));
    ls->AddEntry(folded[regBayes-1],"Refolded","l");
    ls->Draw("same");

    cSpectra->SaveAs(Form("%s/plots/%s_UnfSpectrum.pdf",outDir.Data(),outName.Data()));
    cSpectra->SaveAs(Form("%s/plots/%s_UnfSpectrum.png",outDir.Data(),outName.Data()));

    pv3 = new TPaveText(0.2,0.65,0.6,0.75,"brNDC");
    pv3->SetFillStyle(0);
    pv3->SetBorderSize(0);
    pv3->SetTextFont(42);
    pv3->SetTextSize(0.04);
    pv3->SetTextAlign(11);
    pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",(Int_t)fptbinsDA[0],fDmesonS.Data(),(Int_t)fptbinsDA[fptbinsDN]));

    TCanvas *cSpectrumRebinUnc = new TCanvas("cSpectrumRebinUnc","cSpectrumRebinUnc",800,500);
    hUnfolded_Unc->Draw();
    pvEn->Draw("same");
    pv3->Draw("same");

    cSpectrumRebinUnc->SaveAs(Form("%s/plots/%s_UnfSpectrum_unc.pdf",outDir.Data(),outName.Data()));
    cSpectrumRebinUnc->SaveAs(Form("%s/plots/%s_UnfSpectrum_unc.png",outDir.Data(),outName.Data()));

    TH1F* hratio = (TH1F*)fUnfoldedBayes[regBayes-1]->Rebin(fptbinsJetTrueN,"hratio",fptbinsJetTrueA);
    TH1D* fRawRebinClone;
		if (fptbinsJetTrueN != fptbinsJetMeasN){
            fRawRebinClone = new TH1D("fRawRebinClone","Measured hist, True rebinned",fptbinsJetTrueN,fptbinsJetTrueA);
			int istart = 0;
			while (fptbinsJetTrueA[0] != fptbinsJetMeasA[istart]){
			istart++;
			}
      for(int j=0; j<=fRawRebinClone->GetNbinsX()+1;j++){
        double cont = fRawRebin->GetBinContent(j+istart);
        double err = fRawRebin->GetBinError(j+istart);
        fRawRebinClone->SetBinContent(j, cont);
        fRawRebinClone->SetBinError(j, err);
      }
	}
    else    fRawRebinClone = (TH1D*)fRawRebin->Clone("fRawRebinClone");
	hratio->Divide(fRawRebinClone);
	hratio->SetLineColor(kMagenta+1);
	hratio->SetMarkerColor(kMagenta+1);

    TCanvas* cr= new TCanvas("cr","cr",800,600);
	hratio->GetYaxis()->SetTitle(Form("dN/dp_{T} Unfolded(Reg=%d)/Measured",regBayes));
	hratio->SetTitle(Form("Unfolded(Reg=%d)/Measured",regBayes));
	hratio->Draw("hist");
	line->Draw("same");
	cr->SaveAs(Form("%s/plots/%s_UnfMeasRatio.pdf",outDir.Data(),outName.Data()));
    cr->SaveAs(Form("%s/plots/%s_UnfMeasRatio.png",outDir.Data(),outName.Data()));

    hProjXeff->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hProjYeff->GetXaxis()->SetRangeUser(plotmin,plotmax);

    TCanvas* cProjMatrix= new TCanvas("cProjMatrix","cProjMatrix",800,600);
    cProjMatrix->SetLogz();
    fMatrixProd->Draw("colz");
		cProjMatrix->SaveAs(Form("%s/plots/%s_MatrixProd.pdf",outDir.Data(),outName.Data()));
    cProjMatrix->SaveAs(Form("%s/plots/%s_MatrixProd.png",outDir.Data(),outName.Data()));

    TCanvas* cMatrix= new TCanvas("cMatrix","cMatrix",800,600);
    cMatrix->SetLogz();
    Matrix->Draw("colz");
    cMatrix->SaveAs(Form("%s/plots/%s_Matrix.pdf",outDir.Data(),outName.Data()));
    cMatrix->SaveAs(Form("%s/plots/%s_Matrix.png",outDir.Data(),outName.Data()));

    TCanvas* cProjMReb= new TCanvas("cProjMReb","cProjMReb",800,600);
    cProjMReb->SetLogy();
    hProjYeffRebin->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hProjXeffRebin->GetXaxis()->SetRangeUser(plotmin,plotmax);

    hProjYeffRebin->SetLineColor(2);
    hProjYeffRebin->Draw();
    hProjXeffRebin->SetLineColor(4);
    hProjXeffRebin->Draw("same");

    cProjMReb->SaveAs(Form("%s/plots/%s_MatrixProdProjReb.pdf",outDir.Data(),outName.Data()));
    cProjMReb->SaveAs(Form("%s/plots/%s_MatrixProdProjReb.png",outDir.Data(),outName.Data()));

    if(fSystem) MtxPlots(outDir,outName);
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
        for(int j=0; j<=fMatrixPP->GetNbinsY()+1;j++){

            double cont = fMatrixPP->GetBinContent(i,j);
            if(i==0 && j==0)fMatrixPP->SetBinContent(i,j,0);
            else if(i==fMatrixPP->GetNbinsX()+1 && j==fMatrixPP->GetNbinsY()+1)fMatrixPP->SetBinContent(i,j,0);
            else fMatrixPP->SetBinContent(i,j,cont);

        }
    }

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
        for(int j=0; j<=fMatrixDeltaPt->GetNbinsY()+1;j++){

            double cont = fMatrixDeltaPt->GetBinContent(i,j);
            if(i==0 && j==0)fMatrixDeltaPt->SetBinContent(i,j,0);
            else if(i==fMatrixDeltaPt->GetNbinsX()+1 && j==fMatrixDeltaPt->GetNbinsY()+1)fMatrixDeltaPt->SetBinContent(i,j,0);
						else if(fMatrixDeltaPt->GetXaxis()->GetBinCenter(i) < fptbinsDA[0] || fMatrixDeltaPt->GetYaxis()->GetBinCenter(j) < fptbinsDA[0]) fMatrixDeltaPt->SetBinContent(i,j,0);
 	         else fMatrixDeltaPt->SetBinContent(i,j,cont);

        }
    }
	return 1;

}


/// load raw spectrum (that is to be unfolded)
/// if sname is not specified, try to load according to default naming conventions
int LoadRawSpectrum(TString fn, TString sname, int rebin, TString spostfix) {
	TFile *f  = TFile::Open(fn);
	if (!f) { Error("LoadRawSpectrum","Raw spectrum file %s not found.",fn.Data());	return 0; }

	if (sname == "") sname = "hReco";
	sname += spostfix;


	TH1D *spectrum = (TH1D*)f->Get(sname);
	if (!spectrum) {
		Error("LoadRawSpectrum","Raw spectrum %s could not be gotten from file.",sname.Data());
		return 0;
	}
	Info("LoadRawSpectrum", "%s loaded.", sname.Data());

	if(rebin)  fRawSpectrum = (TH1D*)spectrum->Rebin(fptbinsJetMeasN,"fRawSpectrum",fptbinsJetMeasA);
	else fRawSpectrum = (TH1D*)spectrum->Clone("fRawSpectrum");


    fRawSpectrum->Sumw2();

	return 1;
}


/// get response matrix. If no background, use just detector matrix, otherwise get product. Normalize if requested.
TH2D * getResponseMatrix(bool useDeltaPt) {

    TH2D *mtx;
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


/// Plot probability matrices
int MtxPlots(TString outDir, TString outName) {

	TString tag = "tag";
	if (!fMatrixPP) { Error("MtxPlots","No unfolding matrix present."); return kErr; }

    TH2D * hMtxPP = nullptr;
    TH2D * hMtxDpt = nullptr;
    TH2D * hMtxRe = nullptr;
    TH2D * hMtxPro = nullptr;

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
		//	fMatrixProd = getResponseMatrix( fMatrixDeltaPt );

		if (!fMatrixProd) { Error("MtxPlots", "Error getting product matrix!"); return kErr; }

		//hMtxRe = (TH2D*)NormMatrixY("hMtxRe"+tag,fMatrixProd);

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

	TCanvas *cSlices=new TCanvas("ProbSlice", "Probability slices",1000,1000);
	cSlices->Divide(3,2);
	plotSlice(cSlices->cd(1), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,5,10);
	plotSlice(cSlices->cd(2), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,10,15);
	plotSlice(cSlices->cd(3), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,15,20);
	plotSlice(cSlices->cd(4), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,20,25);
	plotSlice(cSlices->cd(5), hMtxPP,hMtxDpt,hMtxRe,hMtxPro,25,30);

	cSlices->SaveAs(Form("%s/plots/%s_probSlices.pdf",outDir.Data(),outName.Data()));
	cSlices->SaveAs(Form("%s/plots/%s_probSlices.png",outDir.Data(),outName.Data()));


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
		double xhi = (ix!=nx) ? binx[ix+1] : h->GetXaxis()->GetBinCenter(h->GetNbinsX()+1)+epsilon;
		for(int iy=iymin; iy<iymax; iy++) {
			double ylo = (iy!=-1) ? binx[iy]   : h->GetYaxis()->GetBinCenter(0)-epsilon;
			double yhi = (iy!=ny) ? binx[iy+1] : h->GetYaxis()->GetBinCenter(h->GetNbinsY()+1)+epsilon;
			int k = 0;
			double hits = 0;
			double ersq = 0;
			for(int jx=0; jx<=h->GetNbinsX()+1; jx++) {
				if (h->GetXaxis()->GetBinCenter(jx) <  xlo) continue;
				if (h->GetXaxis()->GetBinCenter(jx) >= xhi) continue;
				for(int jy=0; jy<=h->GetNbinsY()+1; jy++) {
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

    //for(int i=0;i<=hre->GetNbinsX();i++){
        for(int j=0;j<=hre->GetNbinsY()+1;j++){
            hre->SetBinContent(0,j,0);
            hre->SetBinError(0,j,0);

						hre->SetBinContent(hre->GetNbinsX()+1,j,0);
            hre->SetBinError(hre->GetNbinsX()+1,j,0);

    }
    //}

	return hre;
}


/// Prior for unfolding: try different prior functions wich best describe the raw specrum.
TF1* getPriorFunction(int prior, TH1D* spect, int priorType, TH1D* rawspectrum) {

	if (!spect) {
		Error("getPriorFunction","Required spectrum does not exist.");
        return nullptr;
	}

	double fitlo = fptbinsJetMeasA[0]; // fFitPtMin;
	double fithi = fptbinsJetMeasA[fptbinsJetMeasN]; // fFitPtMax;
	//fithi = 30;

    TF1 *fPriorFunction = nullptr;
	TString fitopt = "MRN";
	//if (fLHfit) fitopt += "L";

	//PriorFunction = new TF1("PriorFunction","[0]*pow(1+(sqrt([1]*[1]+x*x)-[1])/([2]*[3]),-1*[2])",0,50);
	//PriorFunction->SetParameters(10,0.14,6.6,0.145);/
    fPriorFunction = new TF1("PriorFunction","[0]* pow(x,-[1]) * exp(-[1]*[2]/x)",fitlo,fithi);

        fPriorFunction->SetParLimits(1,2,8);
            if(priorType == 0) 	{ fPriorFunction->FixParameter(1,4.6); fPriorFunction->FixParameter(2,4);}
        else if(priorType == 1) { fPriorFunction->FixParameter(1,3);   fPriorFunction->FixParameter(2,4);}
        else if(priorType == 2) { fPriorFunction->FixParameter(1,4);   fPriorFunction->FixParameter(2,4);}
        else if(priorType == 3) { fPriorFunction->FixParameter(1,5);   fPriorFunction->FixParameter(2,4);}
        else if(priorType == 4) { fPriorFunction->FixParameter(1,6);   fPriorFunction->FixParameter(2,4);}
        else if(priorType == 5) { fPriorFunction->FixParameter(1,7);   fPriorFunction->FixParameter(2,4);}
        else if(priorType == 6) { fPriorFunction->FixParameter(1,4.5); fPriorFunction->FixParameter(2,3);}
        else if(priorType == 7) { fPriorFunction->FixParameter(1,4.5); fPriorFunction->FixParameter(2,5);}
        else if(priorType == 8) { fPriorFunction->SetParLimits(1,2,8); fPriorFunction->SetParLimits(2,2,8); fitlo = fptbinsJetMeasA[0]; }
        if(priorType == 8) { rawspectrum->Fit(fPriorFunction, fitopt,"",fitlo,fithi); }
        else spect->Fit(fPriorFunction, fitopt,"",fitlo,fithi);

	//PriorFunction = new TF1("PriorFunction","[0]*x+[1]",0,50);
	//PriorFunction->SetParameters(10,20);
	//spect->Fit(PriorFunction, fitopt,"",fitlo,fithi);


    fPriorFunction->SetTitle("Chosen prior function");
    return fPriorFunction;
}

/// Create a new, y-normalized matrix
TH2D* NormMatrixY(const char* name, TH2D* Mtx) {
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

	TH1D* h = Mtx->ProjectionY();
	WeightMatrixY(Mtx, h, true);
}


/// get pearson coeffs from covariance matrix
TH2D * getPearsonCoeffs(const TMatrixD &covMatrix) {

	Int_t nrows = covMatrix.GetNrows();
	Int_t ncols = covMatrix.GetNcols();

	TH2D* PearsonCoeffs = new TH2D("PearsonCoeffs","Pearson Coefficients", nrows, 0, nrows, ncols, 0, ncols);
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
