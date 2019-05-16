//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include <string>
#include <sstream>
#include <iostream>

#include "config.h"

int promptColor = kRed+1;
int nonpromptColor = kBlue+1;

//void drawEff(TString promptFile = "$HOME/Work/ALICE/Analysis/Results/pp13tev/efficincies/LHC18defghiklmnop/DjetEff_prompt_jetpt5_50.root", TString nonpromptFile = "$HOME/Work/ALICE/Analysis/Results/pp13tev/efficincies/LHC18defghiklmnop/DjetEff_nonPrompt_jetpt5_50.root",
//TString outDir = "$HOME/Work/ALICE/Analysis/Results/pp13tev/efficincies/LHC18defghiklmnop/plots",
//double jetptmin = 5, double jetptmax = 50,
//double plotmin = 2, double plotmax = 36)

void drawEff(
TString promptFile = "$HOME/Work/ALICE/Analysis/Results/pp13tev/efficiencies/LHC18/DjetEff_prompt_jetpt5_50.root", 
TString nonpromptFile = "$HOME/Work/ALICE/Analysis/Results/pp13tev/efficiencies/LHC18/DjetEff_nonPrompt_jetpt5_50.root",
TString outDir = "$HOME/Work/ALICE/Analysis/Results/pp13tev/efficiencies/LHC18/plots",
double jetptmin = 5, double jetptmax = 50,
double plotmin = 2, double plotmax = 36)


{

	gROOT->ForceStyle();
    gStyle->SetOptStat(000);
    gSystem->Exec(Form("mkdir -p %s",outDir.Data()));

    plotmin = fptbinsDA[0];

    TFile *inFilePrompt = new TFile(promptFile.Data(),"read");
    TFile *inFileFD = new TFile(nonpromptFile.Data(),"read");

    TH1F *hEffPrompt = (TH1F*)inFilePrompt->Get("hEff_reb");
    TH1F *hEffNonPrompt = (TH1F*)inFileFD->Get("hEff_reb");

    hEffPrompt->SetTitle("");
    hEffPrompt->SetMarkerColor(promptColor);
    hEffPrompt->SetLineColor(promptColor);
    hEffPrompt->SetMarkerStyle(20);
    hEffPrompt->SetMarkerSize(1.2);
    hEffPrompt->GetXaxis()->SetTitle(Form("#it{p}_{T,%s} (GeV/#it{c})",fDmesonS.Data()));
    hEffPrompt->GetYaxis()->SetTitle("Acceptance #times Efficiency");
    hEffPrompt->GetXaxis()->SetLabelSize(0.04);
    hEffPrompt->GetXaxis()->SetTitleSize(0.04);
    hEffPrompt->GetXaxis()->SetTitleOffset(1.);
    hEffPrompt->GetYaxis()->SetTitleOffset(1.);
    hEffPrompt->GetYaxis()->SetLabelSize(0.045);
    hEffPrompt->GetYaxis()->SetTitleSize(0.05);
    hEffPrompt->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hEffPrompt->SetMaximum(hEffPrompt->GetMaximum()*1.6);
    //if(fSystem) hEffPrompt->SetMaximum(hEffPrompt->GetMaximum()*1.6);
    //else hEffPrompt->SetMaximum(hEffNonPrompt->GetMaximum()*1.6);

    hEffNonPrompt->SetTitle("");
    hEffNonPrompt->SetMarkerColor(nonpromptColor);
    hEffNonPrompt->SetLineColor(nonpromptColor);
    hEffNonPrompt->SetMarkerStyle(21);
    hEffNonPrompt->SetMarkerSize(1.2);
    hEffNonPrompt->GetXaxis()->SetTitle(Form("#it{p}_{T,%s} (GeV/#it{c})",fDmesonS.Data()));
    hEffNonPrompt->GetYaxis()->SetTitle("Acceptance #times Efficiency");
    hEffNonPrompt->GetXaxis()->SetLabelSize(0.04);
    hEffNonPrompt->GetXaxis()->SetTitleSize(0.05);
    hEffNonPrompt->GetXaxis()->SetTitleOffset(1.);
    hEffNonPrompt->GetYaxis()->SetTitleOffset(1.);
    hEffNonPrompt->GetYaxis()->SetLabelSize(0.045);
    hEffNonPrompt->GetYaxis()->SetTitleSize(0.05);
    hEffNonPrompt->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hEffNonPrompt->SetMaximum(hEffNonPrompt->GetMaximum()*1.5);


    TLegend *leg = new TLegend(0.5,0.22,0.85,0.35);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.045);
    leg->AddEntry(hEffPrompt,Form("Prompt %s",fDmesonS.Data()),"p");
    leg->AddEntry(hEffNonPrompt,Form("Feed-down %s",fDmesonS.Data()),"p");

    TPaveText *pvALICE = new TPaveText(0.15,0.85,0.8,0.9,"brNDC");
    pvALICE->SetFillStyle(0);
    pvALICE->SetBorderSize(0);
    pvALICE->SetTextFont(42);
    pvALICE->SetTextSize(0.045);
    pvALICE->SetTextAlign(11);
    pvALICE->AddText("ALICE Preliminary");

    TPaveText *pvEn= new TPaveText(0.15,0.80,0.8,0.85,"brNDC");
    pvEn->SetFillStyle(0);
    pvEn->SetBorderSize(0);
    pvEn->SetTextFont(42);
    pvEn->SetTextSize(0.045);
    pvEn->SetTextAlign(11);
    pvEn->AddText(Form("%s",fSystemS.Data()));
   // pvEn->AddText("PYTHIA6+HIJING, p-Pb, #sqrt{#it{s}_{NN}} = 5.02 TeV");

    double shift = 0.1;
    TPaveText *pvD = new TPaveText(0.42,0.66-shift,0.9,0.7-shift,"brNDC");
    pvD->SetFillStyle(0);
    pvD->SetBorderSize(0);
    pvD->SetTextFont(42);
    pvD->SetTextSize(0.045);
    pvD->SetTextAlign(11);
    if(fDmesonSpecie) pvD->AddText("D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
    else pvD->AddText("D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

    TPaveText *pvJet = new TPaveText(0.42,0.61-shift,0.9,0.65-shift,"brNDC");
    pvJet->SetFillStyle(0);
    pvJet->SetBorderSize(0);
    pvJet->SetTextFont(42);
    pvJet->SetTextSize(0.045);
    pvJet->SetTextAlign(11);
    pvJet->AddText(Form("in Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

    TPaveText *pvEta = new TPaveText(0.42,0.56-shift,0.8,0.6-shift,"brNDC");
    pvEta->SetFillStyle(0);
    pvEta->SetBorderSize(0);
    pvEta->SetTextFont(42);
    pvEta->SetTextSize(0.045);
    pvEta->SetTextAlign(11);
    pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

    TPaveText *pvJetPt = new TPaveText(0.42,0.48-shift,0.8,0.52-shift,"brNDC");
    pvJetPt->SetFillStyle(0);
    pvJetPt->SetBorderSize(0);
    pvJetPt->SetTextFont(42);
    pvJetPt->SetTextSize(0.045);
    pvJetPt->SetTextAlign(11);
    pvJetPt->AddText(Form("%.0f < p_{T.ch jet} < %.0f GeV/#it{c}",jetptmin,jetptmax));

    TCanvas *cEff = new TCanvas("cEff","cEff",1400,1000);
    cEff->SetLogy();
    //hEffPrompt->Draw();
    //hEffNonPrompt->Draw("same");

    hEffNonPrompt->Draw();
    hEffPrompt->Draw("same");

        //pvALICE->Draw("same");
        pvEn->Draw("same");
        pvJet->Draw("same");
        pvD->Draw("same");
        pvEta->Draw("same");
        pvJetPt->Draw("same");
        leg->Draw("same");


        cEff->SaveAs(Form("%s/DjetEff_Sim_log.pdf",outDir.Data()));
        cEff->SaveAs(Form("%s/DjetEff_Sim_log.png",outDir.Data()));



    TCanvas *cEff2 = new TCanvas("cEff2","cEff2",1400,1000);
    //hEffPrompt->SetMaximum(0.4);
    hEffPrompt->Draw();
    hEffNonPrompt->Draw("same");


    cEff2->SaveAs(Form("%s/DjetEff_Sim.pdf",outDir.Data()));
    cEff2->SaveAs(Form("%s/DjetEff_Sim.png",outDir.Data()));

}
