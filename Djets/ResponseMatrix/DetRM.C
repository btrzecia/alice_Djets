//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include "config.h"

void DetRM(bool isPrompt = 1, TString datafile = "../outMC/AnalysisResults_fast_D0MCPythia_SMQcorr2.root", TString outDir = "plots",
bool postfix = 0, TString listName = "FD", bool isprefix=0 )
{

    gStyle->SetOptStat(0000); //Mean and RMS shown
    gStyle->SetPadRightMargin(0.1);
    gSystem->Exec(Form("mkdir %s",outDir.Data()));
    gSystem->Exec(Form("mkdir %s/plots",outDir.Data()));

    TFile *File = new TFile(datafile,"read");
    TDirectoryFile* dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");
        TString histName;
        if(!isprefix){
                if(fDmesonSpecie) histName = "histosDStarMBN";
                else histName = "histosD0MBN";}
        else{
                if(fDmesonSpecie) histName = "histosDStarMBN";
                else histName = "histosD0";}


    float jetmin = 0, jetmax = 60;
    float Dptmin = fptbinsDA[0], Dptmax = fptbinsDA[fptbinsDN];

    TPaveText *pv2 = new TPaveText(0.15,0.8,0.25,0.9,"brNDC");
    pv2->SetFillStyle(0);
    pv2->SetBorderSize(0);
    pv2->AddText(Form("R=0.%d",Rpar));

    TH1F *hMCpt;
	  TH1F *hMCpt_reco;
    TH2F *hPtJet[NDMC];
    TH1F *hPtG[NDMC];
    TH1F *hPtR[NDMC];

	  TList *histList[NDMC];
	  THnSparseF *sparseMC[NDMC];

    TH2F *hPtJet2d;
    TH1F *hPtJetGen;
    TH1F *hPtJetRec;


        for(int i=0; i<NDMC; i++){
           if(!isprefix){
                if(postfix) { 
			histList[i] =  (TList*)dir->Get(Form("%s%d%sMCrec",histName.Data(),i,listName.Data())); }
                else {
                         if(isPrompt) histList[i] =  (TList*)dir->Get(Form("%s%dMCrec",histName.Data(),i));
                         else histList[i] =  (TList*)dir->Get(Form("%s%dFDMCrec",histName.Data(),i));
                }
           }
           else{
                if(postfix) {
                        if(isPrompt){ histList[i] =  (TList*)dir->Get(Form("%s%sMBN%dMCrec",histName.Data(),listName.Data(),i)); }
                        else{    histList[i] =  (TList*)dir->Get(Form("%s%sMBN%dFDMCrec",histName.Data(),listName.Data(),i)); }
                }
                else { cout<<"-----postfix has to be true if prefix is true!! check again----------------"<<endl; return;       }
           }


        sparseMC[i] = (THnSparseF*)histList[i]->FindObject("ResponseMatrix");

        sparseMC[i]->GetAxis(2)->SetRangeUser(Dptmin,Dptmax);
        sparseMC[i]->GetAxis(1)->SetRangeUser(jetmin,jetmax);

        if(fDmesonSpecie) sparseMC[i]->GetAxis(6)->SetRangeUser(Dptmin,Dptmax);
        else sparseMC[i]->GetAxis(7)->SetRangeUser(Dptmin,Dptmax);

        if(fDmesonSpecie) sparseMC[i]->GetAxis(5)->SetRangeUser(jetmin,jetmax); // Dstar tmp
        else sparseMC[i]->GetAxis(6)->SetRangeUser(jetmin,jetmax);

	if(!fDmesonSpecie) {
		sparseMC[i]->GetAxis(4)->SetRangeUser(-(0.9-fRpar),0.9-fRpar);
		sparseMC[i]->GetAxis(9)->SetRangeUser(-(0.9-fRpar),0.9-fRpar);
	}

        if(fDmesonSpecie) hPtJet[i] = (TH2F*)sparseMC[i]->Projection(5,1,"E"); //Dstar tmp
        else hPtJet[i] = (TH2F*)sparseMC[i]->Projection(6,1,"E");

        hPtJet[i]->Sumw2();
        hPtJet[i]->SetName(Form("hPtJet_%d",i));

        if(fDmesonSpecie) hPtG[i] = (TH1F*)sparseMC[i]->Projection(5); //Dstar tmp
        else   hPtG[i] = (TH1F*)sparseMC[i]->Projection(6);

        hPtG[i]->SetName(Form("hPtG_%d",i));
        hPtR[i] = (TH1F*)sparseMC[i]->Projection(1);
        hPtR[i]->SetName(Form("hPtR_%d",i));
        hPtG[i]->Sumw2();
        hPtR[i]->Sumw2();

		    if (!i){
  			     hPtJet2d = (TH2F*)hPtJet[0]->Clone("hPtJet2d");
  			     hPtJetGen = (TH1F*)hPtG[0]->Clone("hPtJetGen");
  			     hPtJetRec = (TH1F*)hPtR[0]->Clone("hPtJetRec");
        }
        else {
            hPtJet2d->Add(hPtJet[i]);
      			hPtJetGen->Add(hPtG[i]);
      			hPtJetRec->Add(hPtR[i]);
        }

	}


    hPtJet2d->SetTitle();
    hPtJet2d->SetName("hPtJet2d");
    hPtJet2d->GetXaxis()->SetTitle("p_{T,ch jet}^{rec.} (GeV/#it{c})");
    hPtJet2d->GetYaxis()->SetTitle("p_{T,ch jet}^{gen.} (GeV/#it{c})");

    hPtJetGen->SetName("hPtJetGen");
    hPtJetRec->SetName("hPtJetRec");
    hPtJetGen->SetLineColor(kBlue+2);
    hPtJetRec->SetLineColor(kRed+2);


    TCanvas *cjetPt = new TCanvas("cjetPt","cjetPt",800,600);
    cjetPt->SetLogy();
    hPtJetGen->Draw();
    hPtJetRec->Draw("same");
    //cjetPt->SaveAs(Form("%s/pTdist_Dpt%d_%d.png",outDir, (int)Dptmin, (int)Dptmax));

    TCanvas *cjetPt2d = new TCanvas("cjetPt2d","cjetPt2d",800,600);
    cjetPt2d->SetLogz();
    hPtJet2d->Draw("colz");
    pv2->Draw("same");


    cjetPt2d->SaveAs(Form("%s/plots/DetMatrix_%s_Dpt%d_%d.png",outDir.Data(), isPrompt ? "prompt" : "nonPrompt", (int)Dptmin, (int)Dptmax));

    //TFile *ofile = new TFile(Form("%s/DetMatrix_Dpt%d_%d.root",outDir, (int)Dptmin, (int)Dptmax),"RECREATE");
    TFile *ofile = new TFile(Form("%s/DetMatrix_%s.root",outDir.Data(), isPrompt ? "prompt" : "nonPrompt" ),"RECREATE");

    hPtJetGen->Write();
    hPtJetRec->Write();
    hPtJet2d->Write();
    ofile->Close();

   return;

/*
    TH1D *proj[nJetBins];
    for(int i=0; i<nJetBins; i++){
            proj[i] = (TH1D*)hPtJet2d->ProjectionX(Form("proj_%d",i),hPtJet2d->GetYaxis()->FindBin(ptJetbins[i]), hPtJet2d->GetYaxis()->FindBin(ptJetbins[i+1]) -1);
            proj[i]->Scale(1./proj[i]->Integral());
            //proj[i-1]->SetMarkerStyle(20);
            proj[i]->SetMarkerColor(2);
            proj[i]->SetLineColor(2);

    }
*/

}
