//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include "config.h"

using namespace std;

void setHistoDetails(TH1 *h, Color_t color, Style_t Mstyle, Size_t size = 0.9, Width_t width=2){

    h->SetMarkerStyle(Mstyle);
    h->SetMarkerColor(color);
    h->SetMarkerSize(size);
    h->SetLineColor(color);
    h->SetLineWidth(width);
    h->SetTitle("");
    h->GetXaxis()->SetTitle("p_{T,D^{0}}(GeV/c)");

    return;

}

void SaveCanvas(TCanvas *c, TString name = "tmp"){

    c->SaveAs(Form("%s.png",name.Data()));
    c->SaveAs(Form("%s.pdf",name.Data()));

}


float ptmin = fptbinsDA[0], ptmax = fptbinsDA[fptbinsDN];

void DjetEfficiency(bool isPrompt = 1, 
TString effFile = "/home/basia/Work/ALICE/Analysis/Results/pp5TeV/outEffNew/AnalysisResults_R03Prel.root",
TString outDir = "out", float jetptmin = 5, float jetptmax = 50, bool recoPt = 0, bool postfix = 0, TString listName = "", bool isprefix = 0 )
{

	gSystem->Exec(Form("mkdir -p %s",outDir.Data()));

    TString outFileName = "DjetEffPrelYReco";
	// get analysis output file
	TFile *File = new TFile(effFile,"read");
	TDirectoryFile* dir = dynamic_cast<TDirectoryFile*>(File->Get("DmesonsForJetCorrelations"));

	TString histName;
	if(!isprefix){
		if(fDmesonSpecie) histName = "histosDStarMBN";
		else histName = "histosD0MBN";}
	else{
		if(fDmesonSpecie) histName = "histosDStarMBN";
		else histName = "histosD0";}

	TH1F *hMCpt;
	TH1F *hMCpt_reco;

	TH1F *hMC[NDMC];
	TH1F * hreco[NDMC];
	TList *histList[NDMC];
	THnSparseF *sparseMC[NDMC];
	THnSparseF *sparsereco[NDMC];
    TH2D *hreco2D = nullptr, *hMC2D = nullptr;

	for(int i=0; i<NDMC; i++){

		if(!isprefix){
			if(postfix) { histList[i] =  (TList*)dir->Get(Form("%s%d%sMCrec",histName.Data(),i,listName.Data())); }
			else {
			 	if(isPrompt) histList[i] =  (TList*)dir->Get(Form("%s%dMCrec",histName.Data(),i));
			 	else histList[i] =  (TList*)dir->Get(Form("%s%dFDMCrec",histName.Data(),i));
			}
	   	}
	   	else{
			if(postfix) {
				if(isPrompt){ histList[i] =  (TList*)dir->Get(Form("%s%sMBN%dMCrec",histName.Data(),listName.Data(),i)); }
				else{ histList[i] =  (TList*)dir->Get(Form("%s%sMBN%dFDMCrec",histName.Data(),listName.Data(),i)); }
			}
			else { cout<<"-----postfix has to be true if prefix is true!! check again----------------"<<endl; return;	}
	   	}

		THnSparseF* sMC = (THnSparseF*)histList[i]->FindObject("ResponseMatrix");
		sparseMC[i] = (THnSparseF*)sMC->Clone(Form("sparseMC_%d",i));
		if(fDmesonSpecie) sparseMC[i]->GetAxis(5)->SetRangeUser(jetptmin,jetptmax); // Dstar tmp
        else {
            sparseMC[i]->GetAxis(6)->SetRangeUser(jetptmin,jetptmax); // jet pT gen
            sparseMC[i]->GetAxis(9)->SetRangeUser(-(0.9-fRpar),0.9-fRpar); // MC jet eta
		}
        if(fDmesonSpecie) hMC[i] = (TH1F*)sparseMC[i]->Projection(6); // Dstar tmp
        else hMC[i] = (TH1F*)sparseMC[i]->Projection(7); // Dpt gen
		hMC[i]->SetName(Form("hMC_%d",i));

		THnSparseF* sreco = (THnSparseF*)histList[i]->FindObject("ResponseMatrix");
        sparsereco[i] = (THnSparseF*)sreco->Clone(Form("sparsereco_%d",i));
   	 	if(recoPt) {
      			sparsereco[i]->GetAxis(1)->SetRangeUser(ptmin,ptmax); // jet pT reco
        }
        else {
            if(fDmesonSpecie)sparsereco[i]->GetAxis(5)->SetRangeUser(jetptmin,jetptmax); // Dstar tmp
            else sparsereco[i]->GetAxis(6)->SetRangeUser(jetptmin,jetptmax); // jet pT gen
            sparsereco[i]->GetAxis(1)->SetRangeUser(0,200); // jet pT reco
        }
        if(!fDmesonSpecie) {
            sparsereco[i]->GetAxis(4)->SetRangeUser(-(0.9-fRpar),0.9-fRpar); // reco jet eta
            //sparsereco[i]->GetAxis(9)->SetRangeUser(-(0.9-fRpar),0.9-fRpar); // MC jet eta
        }

        if(fDmesonSpecie)hreco[i] = (TH1F*)sparsereco[i]->Projection(6); // Dstar tmp
        else hreco[i] = (TH1F*)sparsereco[i]->Projection(7); // Dpt gen
		hreco[i]->SetName(Form("hreco_%d",i));

        if(!i){
            hreco2D = sparsereco[i]->Projection(7,8);
            hMC2D = sparseMC[i]->Projection(7,8);
        }

        if (!i){
			hMCpt = (TH1F*)hMC[0]->Clone("hMCpt");
			hMCpt_reco = (TH1F*)hreco[0]->Clone("hMCpt_reco");
		}
		else {
			hMCpt->Add(hMC[i]);
			hMCpt_reco->Add(hreco[i]);
		}
	}

	hMCpt->Sumw2();
	hMCpt_reco->Sumw2();
	hMCpt->SetLineColor(4);
	hMCpt->SetMarkerColor(4);
	hMCpt->SetMarkerStyle(20);

	hMCpt_reco->SetLineColor(2);
	hMCpt_reco->SetMarkerColor(2);
	hMCpt_reco->SetMarkerStyle(20);
	
	TH1D * hEff = (TH1D*)hMCpt_reco->Clone("hEff");
	hEff -> Divide(hMCpt_reco,hMCpt,1,1,"b");
	//hEff->GetXaxis()->SetRangeUser(ptmin,ptmax);

	hMCpt->SetMinimum(10);
	TCanvas *cPt = new TCanvas();
	hMCpt->Draw();
	hMCpt_reco->Draw("same");

	if(isPrompt) setHistoDetails(hEff,2,20,1.2);
	else setHistoDetails(hEff,4,25,1.2);

	TCanvas *cEff = new TCanvas();
	hEff->Draw("ep");

	TH1D *hpt_mc_reb = (TH1D*)hMCpt->Rebin(fptbinsDN,"hpt_mc_reb",fptbinsDA);
	TH1D *hpt_reco_reb = (TH1D*)hMCpt_reco->Rebin(fptbinsDN,"hpt_reco_reb",fptbinsDA);

	TH1D * hEff_reb = (TH1D*)hpt_reco_reb->Clone("hEff_reb");
	hEff_reb -> Divide(hpt_reco_reb,hpt_mc_reb,1,1,"b");
	//hEff_reb->GetXaxis()->SetRangeUser(ptmin,ptmax);
	hEff_reb->SetTitle(Form("|#eta_{jet}|<%.1f",0.9-fRpar));
	hEff_reb->GetXaxis()->SetTitle(Form("p_{T,%s} (GeV/$it{c})",fDmesonS.Data()));
	hEff_reb->GetYaxis()->SetTitle("Acc #times Eff");

	if(isPrompt) setHistoDetails(hEff_reb,2,20,1.2);
	else setHistoDetails(hEff_reb,4,25,1.2);

	TCanvas *cEffReb = new TCanvas();
	hEff_reb->Draw("ep");

    TString out = Form("%s/DjetEffRaw_%s_jetpt%d_%d", outDir.Data(), isPrompt ? "prompt" : "nonPrompt", (int)jetptmin,(int)jetptmax );
	SaveCanvas(cEff,out);

    out = Form("%s/DjetEff_%s_jetpt%d_%d", outDir.Data(), isPrompt ? "prompt" : "nonPrompt", (int)jetptmin,(int)jetptmax );
	SaveCanvas(cEffReb,out);

    TFile *outFile = new TFile(Form("%s/%s_%s_jetpt%d_%d.root", outDir.Data(), outFileName.Data(), isPrompt ? "prompt" : "nonPrompt", (int)jetptmin, (int)jetptmax ),"RECREATE");
    hEff->Write();
	hEff_reb->Write();
	hMCpt->Write();
	hMCpt_reco->Write();
	hpt_mc_reb->Write();
	hpt_reco_reb->Write();
    hreco2D->Write();
    hMC2D->Write();
	outFile->Close();

return;

}


