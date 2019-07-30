//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------
//
// Macro to extract jet pt spectra from simulation, prompt or non-prompt D
//
#include "../config.h"

//fDmesonSpecie = 1;
//fRpar = 0.4;
//Rpar = 4;


double jetEta = 0.9-fRpar;
int BFDsim = 0;


enum fHType {fHistJetPt=1, fHistDPt=2, fHistZ=3};

void GetSimDJetHist(TString file,TH1* hist, fHType htype, bool powehgSim = 0, double jetmin=0, double jetmax=-1, double Dmin=0, double Dmax=-1, bool isEff=0, TString effFilePrompt="", TString effFileNonPrompt="");
TH1* GetInputHist(TString inFile, TString histName,TH1 *hh);
TH1 *CreateHisto(fHType htype);
double GetEfficiency(TH1 *hh, double Dpt);
void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, Width_t width, TString name);
void SaveCanvas(TCanvas *c, TString name = "tmp");


void simulationSpectra(
    TString inSimDir = "/media/basia/Disk2/Work/ALICE/Simulations/pp13TeV/powheg+pythia8_charm/",
    TString simFileName = "AnalysisResults_FastSim_powheg+pythia6_charm_1549014986",
    fHType htype = fHistZ,
    bool powhegsim = 0,
    double jetptmin = 15, double jetptmax = 50,
    double Dptmin = 2, double Dptmax = 36,
    TString outDir = "/media/basia/Disk2/Work/ALICE/Simulations/pp13TeV/simOutput/",
    bool isFD = 0,
    bool isEff = 0,
    TString effFilePrompt = "$HOME/file.root",
    TString effFileNonPrompt = "$HOME/file.root"
)
{

    gSystem->Exec(Form("mkdir -p %s",outDir.Data()));
    fDmesonSpecie = kD0toKpi;

    BFDsim = isFD;

    TString file = inSimDir;
    file += "/";
    file += simFileName;
    file += ".root";
    TH1D *hist;
    hist = dynamic_cast<TH1D*>(CreateHisto(htype));
    GetSimDJetHist(file.Data(),hist,htype,powhegsim,jetptmin,jetptmax,Dptmin,Dptmax,isEff,effFilePrompt,effFileNonPrompt);

    TString out = outDir;
    out += simFileName.Data();
    out += "_noShower";
    switch (htype) {
        case fHistJetPt: out += "_JetPt";
        if(Dptmin != -1) { out += "_DpTMin_"; out += Dptmin; }
        if(Dptmax != -1) { out += "_DpTMax_"; out += Dptmax; }
            break;
        case fHistDPt: out += "_DPt";
        if(jetptmin != -1) { out += "_JetpTMin_"; out += jetptmin; }
        if(jetptmax != -1) { out += "_JetpTMax_"; out += jetptmax; }
            break;
        case fHistZ: out += "_Z";
        if(Dptmin != -1) { out += "_DpTMin_"; out += Dptmin; }
        if(Dptmax != -1) { out += "_DpTMax_"; out += Dptmax; }
        if(jetptmin != -1) { out += "_JetpTMin_"; out += jetptmin; }
        if(jetptmax != -1) { out += "_JetpTMax_"; out += jetptmax; }
            break;
    }

    if(isEff) out += "_effScaled_";
    fDmesonSpecie ?  out +=  "_Dstar" : out +=  "_Dzero";

    out += "_R"; out += fRpar*10;
    out += ".root";

    TFile *ofile = new TFile( out.Data() ,"RECREATE");
    hist->Write();
    ofile->Close();

    return;
}

void GetSimDJetHist(TString file, TH1*hist, fHType htype, bool powhegSim, double jetptmin, double jetptmax, double Dptmin, double Dptmax, bool isEff, TString effFilePrompt, TString effFileNonPrompt){

//// no efficiency correction implemented so far - pure MC x-sections

    TFile *fileInput = new TFile(file,"read");
    if(!fileInput){
      std::cout << "File " << fileInput << " cannot be opened! check your file path!" << std::endl; return;
    }

    TList* dir = dynamic_cast<TList*>(fileInput->Get("AliAnalysisTaskDmesonJets_histos"));
    if(!dir){
      std::cout << "Error in getting dir! Exiting..." << std::endl;
      return ;
    }

    TH1D *hxsection = dynamic_cast<TH1D*>(dir->FindObject("fHistXsection"));
    if(!hxsection){
      std::cout << "Error in getting x-section hist! Exiting..." << std::endl;
      return ;
    }
    double xsection = hxsection->GetMean(2);
    double events = static_cast<double>(hxsection->GetEntries());
    double scaling = xsection/events;
    //if(!powhegSim) scaling = 1./events;

    TTree *tree;
    if(!fDmesonSpecie) tree = dynamic_cast<TTree*>(fileInput->Get("AliAnalysisTaskDmesonJets_D0_MCTruth"));
    else tree = dynamic_cast<TTree*>(fileInput->Get("AliAnalysisTaskDmesonJets_DStar_MCTruth"));
    AliAnalysisTaskDmesonJets::AliDmesonMCInfoSummary *brD = nullptr;
    AliAnalysisTaskDmesonJets::AliJetInfoSummary *brJet = nullptr;
    tree->SetBranchAddress("DmesonJet",&brD);
    tree->SetBranchAddress(Form("Jet_AKTChargedR0%d0_pt_scheme",static_cast<int>(fRpar*10)),&brJet);

    if(!tree || !brD || !brJet) {
      std::cout << "Error in setting the tree/branch names! Exiting..." << std::endl;
      return ;
    }


    TH1D *hPromptEff = nullptr,*hNonPromptEff = nullptr;
    if(isEff) {
        hPromptEff = dynamic_cast<TH1D*>(GetInputHist(effFilePrompt,"hEff_reb",hPromptEff));
        hNonPromptEff = dynamic_cast<TH1D*>(GetInputHist(effFileNonPrompt,"hEff_reb",hNonPromptEff));
    }

    for (int k=0; k<tree->GetEntries(); k++) {
        tree->GetEntry(k);
        if(BFDsim){
          if(brD->fPartonType != 5) continue;
        }
        else if(brD->fPartonType != 4) continue;
        if(brD->fAncestorPDG == 2212) continue; // check if not coming from proton ????, only for POWHEG

        if (brJet->fEta < -jetEta || brJet->fEta > jetEta) continue;
        if(jetptmin != -1 && brJet->fPt < jetptmin) continue;
        if(jetptmax != -1 && brJet->fPt > jetptmax) continue;
        if(Dptmin != -1 && brD->fPt < Dptmin)  continue;
        if(Dptmax != -1 && brD->fPt > Dptmax)  continue;

        switch (htype) {
        case fHistJetPt: hist->Fill(brJet->fPt); break;
        case fHistDPt: hist->Fill(brD->fPt);   break;
        case fHistZ:
            double z = brJet->fZ;
            if(!isEff && z==1) z-=0.001;
            hist->Fill(z);
            break;
        }

    }

    hist->Scale(scaling);

    if(!hist) {
      std::cout << "Error in extracting the histo! Exiting..." << std::endl;
      return ;
    }


}

TH1 *CreateHisto(fHType htype){

    double binWidth = 1;
    double binMin=0, binMax = 100;
    TString histName = "";
    switch (htype) {
        case fHistJetPt:{
            binMin = 0; binMax = 200;
            binWidth = 1;
            histName="JetPt";
            break;
        }
        case fHistDPt:{
            binMin = 0; binMax = 50;
            binWidth = 0.5;
            histName="DPt";
            break;
        }
        case fHistZ:{
            binMin = 0;
            binMax = BFDsim ? 1.02 : 1.;
            binWidth = 0.02;
            histName="Z";
            break;
        }

    }
    int nbins = (binMax-binMin)/binWidth;
    TH1D *hist = new TH1D("hist",histName.Data(),nbins,binMin,binMax);

    return hist;
}


TH1* GetInputHist(TString inFile, TString histName,TH1 *hh){

    TFile *File = new TFile(inFile,"read");
    hh = (TH1F*)File->Get(histName.Data());

    return hh;
}

void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, Width_t width, TString name){
    hh->SetMarkerColor(color);
    hh->SetMarkerStyle(Mstyle);;
    hh->SetLineColor(color);
    hh->SetLineWidth(2);
    hh->SetMarkerSize(1.1);
    hh->SetName(name.Data());
    hh->SetTitle("");
    hh->GetXaxis()->SetTitle("p_{T}^{ch,jet} (GeV/c)");
}

void SaveCanvas(TCanvas *c, TString name){
    c->SaveAs(Form("%s.png",name.Data()));
    c->SaveAs(Form("%s.pdf",name.Data()));
}
