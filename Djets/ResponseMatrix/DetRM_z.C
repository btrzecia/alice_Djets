//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include "../DsignalExtraction/configDzero_ppz.h"

void DetRM_z(
bool isPrompt = 1, 
TString datafile = "/home/jackbauer/Work/alice/analysis/pp5TeV/D0jet/outMC/AnalysisResults_414_z.root", 
TString outDir = "plots",
bool postfix = 0, 
TString listName = "FD", 
bool isprefix=0 )
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


    float zmin = 0, zmax = 1.0;
    float Dptmin = fptbinsDA[0], Dptmax = fptbinsDA[fptbinsDN];

    TPaveText *pv2 = new TPaveText(0.15,0.8,0.25,0.9,"brNDC");
    pv2->SetFillStyle(0);
    pv2->SetBorderSize(0);
    pv2->AddText(Form("R=0.%d",Rpar));

    TH1F *hMCpt;
    TH1F *hMCpt_reco;
    TH2D* hZ[NDMC];
    TH1D* hZG[NDMC];
    TH1D* hZR[NDMC];

	  TList *histList[NDMC];
	  THnSparseF *sparseMC[NDMC];
	  THnSparseF *sparsereco[NDMC];

    TH2D* hZ2d;
    TH1D* hZGen;
    TH1D* hZRec;


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




//	  for(int i=0; i<NDMC; i++){
//        if(postfix) { histList[i] =  (TList*)dir->Get(Form("%s%d%sMCrec",histName.Data(),i,listName.Data())); }
//        else {
//    			 if(isPrompt) histList[i] =  (TList*)dir->Get(Form("%s%dMCrec",histName.Data(),i));
//    			 else histList[i] =  (TList*)dir->Get(Form("%s%dFDMCrec",histName.Data(),i));
//    		}

        sparseMC[i] = (THnSparseF*)histList[i]->FindObject("ResponseMatrix");

        sparseMC[i]->GetAxis(2)->SetRangeUser(Dptmin,Dptmax);
        sparseMC[i]->GetAxis(0)->SetRangeUser(zmin,zmax);

        if(fDmesonSpecie) sparseMC[i]->GetAxis(6)->SetRangeUser(Dptmin,Dptmax);
        else sparseMC[i]->GetAxis(7)->SetRangeUser(Dptmin,Dptmax);

        if(fDmesonSpecie) sparseMC[i]->GetAxis(4)->SetRangeUser(zmin,zmax); // Dstar tmp
        else sparseMC[i]->GetAxis(5)->SetRangeUser(zmin,zmax);

        if(fDmesonSpecie) hZ[i] = (TH2D*)sparseMC[i]->Projection(4,0,"E"); //Dstar tmp
        else hZ[i] = (TH2D*)sparseMC[i]->Projection(5,0,"E");

        hZ[i]->Sumw2();
        hZ[i]->SetName(Form("hZ_%d",i));

        if(fDmesonSpecie) hZG[i] = (TH1D*)sparseMC[i]->Projection(4); //Dstar tmp
        else   hZG[i] = (TH1D*)sparseMC[i]->Projection(5);

        hZG[i]->SetName(Form("hZG_%d",i));
        hZR[i] = (TH1D*)sparseMC[i]->Projection(0);
        hZR[i]->SetName(Form("hZR_%d",i));
        hZG[i]->Sumw2();
        hZR[i]->Sumw2();

	if (!i){
  	        hZ2d  = (TH2D*)hZ[0] ->Clone("hZ2d");
  	        hZGen = (TH1D*)hZG[0]->Clone("hZGen");
  	        hZRec = (TH1D*)hZR[0]->Clone("hZRec");
        }
        else {
		hZ2d ->Add(hZ[i]);
		hZGen->Add(hZG[i]);
		hZRec->Add(hZR[i]);
        }

	}


    hZ2d->SetTitle("hZ2d");
    hZ2d->SetName("hZ2d");
    hZ2d->GetXaxis()->SetTitle("z_{||}^{rec.} ");
    hZ2d->GetYaxis()->SetTitle("z_{||}^{gen.} ");

    hZGen->SetName("hZGen");
    hZRec->SetName("hZRec");
    hZGen->SetLineColor(kBlue+2);
    hZRec->SetLineColor(kRed+2);


    TCanvas *cZ = new TCanvas("cZ","cZ",800,600);
    cZ->SetLogy();
    hZGen->Draw();
    hZRec->Draw("same");
    //cZ->SaveAs(Form("%s/Zdist_Dpt%d_%d.png",outDir, (int)Dptmin, (int)Dptmax));


    TCanvas *cZ2d = new TCanvas("cZ2d","cZ2d",800,600);
    cZ2d->SetLogz();
    hZ2d->Draw("colz");
    pv2->Draw("same");


    cZ2d->SaveAs(Form("%s/plots/DetMatrix_%s_Dpt%d_%d.png",outDir.Data(), isPrompt ? "prompt" : "nonPrompt", (int)Dptmin, (int)Dptmax));

    TFile *ofile = new TFile(Form("%s/DetMatrix_%s.root",outDir.Data(), isPrompt ? "prompt" : "nonPrompt" ),"RECREATE");

    hZGen->Write();
    hZRec->Write();
    hZ2d->Write();
    ofile->Close();

   return;

}
