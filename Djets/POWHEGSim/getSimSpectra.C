//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------
//
// Macro to extract jet pt spectra from simulation, prompt or non-prompt D
//
#include "config.h"

TH1* GetInputSimHistD(TString inFile, bool isjetptcut);
TH1* GetInputSimHistJet(TString inFile, bool isEff, TString effFilePrompt, TString effFileNonPrompt,bool isDptcut, int isNPrompt, int SimNr);

//fDmesonSpecie = 1;
//fRpar = 0.4;
//Rpar = 4;

double jetptmin = 5, jetptmax = 30; // for D pT spectra
double jetEta = 0.9-fRpar;
int BFDsim;

double GetEfficiency(TH1 *hh, double Dpt);
void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, Width_t width, TString name);
void SaveCanvas(TCanvas *c, TString name = "tmp");

enum fHType {fHistDPt=1, fHistJetPt=2, fHistZ=3};

//quark: 1 = beauty, 0 = charm
void getSimSpectra(
TString simFile = "./",
int simNr = 0, int quark = 1, bool jet = 1,  bool isDptcut = 1, bool isEff = 0,
TString effFilePrompt = "$HOME/file.root",
TString effFileNonPrompt = "$HOME/file.root",
TString outFileDir = "outR03Test/",
bool isjetptcut = 0, double jetmin = 5, double jetmax = 30 )
{

    gSystem->Exec(Form("mkdir -p %s",outFileDir.Data()));

    jetptmin = jetmin;
    jetptmax = jetmax;
    BFDsim = quark;

    simFile += "/";
    if(quark == 1) simFile += fRunB[simNr];
    else if(quark == 0) simFile += fRunC[simNr];
    simFile += ".root";

    TH1D *hist;
    if(jet) hist = (TH1D*) GetInputSimHistJet(simFile.Data(), isEff, effFilePrompt.Data(), effFileNonPrompt.Data(),isDptcut,quark,simNr);
    else hist = (TH1D*) GetInputSimHistD(simFile.Data(),isjetptcut);

    TString out = outFileDir;
    if(jet) out += "/JetPt_";
    else out += "/DPt_";
    if(quark == 1) out += fRunB[simNr];
    else if(quark == 0) out += fRunC[simNr];

    if(jet){
        if(isDptcut) { out += "_Dpt"; out +=  fptbinsDA[0]; out += "_"; out += fptbinsDA[fptbinsDN];  }
    }
    else{
        if(isjetptcut){ out += "_Jetpt"; out +=  jetptmin; out += "_"; out += jetptmax; }
    }

    if(jet && isEff) out += "_effScaled";
    if(fDmesonSpecie) out += "_Dstar";
    else out += "_Dzero";
    out += ".root";

    TFile *ofile = new TFile( out.Data() ,"RECREATE");
    hist->Write();
    ofile->Close();

    return;
}

TH1* GetInputSimHist(TString inFile, bool isjetptcut){

    TH1 *hist;

    TFile *fileInput = new TFile(inFile,"read");
    if(!fileInput){
      std::cout << "File " << fileInput << " cannot be opened! check your file path!" << std::endl; return nullptr;
    }

    TList* dir=(TList*)fileInput->Get("AliAnalysisTaskDmesonJets_histos");
    if(!dir){
      std::cout << "Error in getting dir! Exiting..." << std::endl;
      return nullptr;
    }

    TH1D *hxsection = (TH1D*)dir->FindObject("fHistXsectionVsPtHard");
    if(!hxsection){
      std::cout << "Error in getting x-section hist! Exiting..." << std::endl;
      return nullptr;
    }
    double xsection = hxsection->GetMean(2);
    double events = (double)hxsection->GetEntries();
    double scaling = xsection/events;

    TTree *tree;
    if(!fDmesonSpecie) tree = (TTree*)fileInput->Get("AliAnalysisTaskDmesonJets_D0_MCTruth");
    else tree = (TTree*)fileInput->Get("AliAnalysisTaskDmesonJets_DStar_MCTruth");
    AliAnalysisTaskDmesonJets::AliDmesonInfoSummary *brD = nullptr;
    AliAnalysisTaskDmesonJets::AliJetInfoSummary *brJet = nullptr;
    tree->SetBranchAddress("DmesonJet",&brD);
    tree->SetBranchAddress(Form("Jet_AKTChargedR0%d0_pt_scheme",Rpar),&brJet);

    if(!tree || !brD || !brJet) {
      std::cout << "Error in setting the tree/branch names! Exiting..." << std::endl;
      return nullptr;
    }

    hist = new TH1D("hist","hDpt",100,0,100);
    // hist = new TH1D("hist","hDpt",20,-2,2);
    for (int k=0; k<tree->GetEntries(); k++) {
    tree->GetEntry(k);
    //if(isjetptcut) { if (brJet->fEta < -jetEta || brJet->fEta > jetEta) continue; }
    //if (brJet->fEta < -jetEta || brJet->fEta > jetEta) continue;
    if(isjetptcut) { if (brJet->fPt < jetptmin || brJet->fPt >= jetptmax) continue; }
    if (brD->fEta < -1 || brD->fEta > 1) continue;
      hist->Fill(brD->fPt);
      //hist->Fill(brD->fEta);
    }

    hist->Scale(scaling);

    if(!hist) {
      std::cout << "Error in extracting the mass plot! Exiting..." << std::endl;
      return nullptr;
    }

    return hist;
}


TH1* GetInputHist(TString inFile, TString histName,TH1 *hh){

    TFile *jetPtFile = new TFile(inFile,"read");
    hh = (TH1F*)jetPtFile->Get(histName.Data());

    //hh = (TH1F*)hh_tmp->Rebin(ptbinsJetN,"hh",ptbinsJet);
    return hh;
}

TH1* GetInputSimHistJet(TString inFile, bool isEff, TString effFilePrompt, TString effFileNonPrompt,bool isDptcut, int isNPrompt, int SimNr){

    TH1D *hist = new TH1D("hist","hjetpt",100,0,100);

    TFile *fileInput = new TFile(inFile,"read");
    if(!fileInput){
      std::cout << "File " << fileInput << " cannot be opened! check your file path!" << std::endl; return nullptr;
    }

    TList* dir=(TList*)fileInput->Get("AliAnalysisTaskDmesonJets_histos");
    if(!dir) {
      std::cout << "Error in getting dir! Exiting..." << std::endl;
      return nullptr;
    }

    TH1D *hxsection;
    if(!isNPrompt)	hxsection = (TH1D*)dir->FindObject("fHistXsection");
    else { 
          if(SimNr == 11)  hxsection = (TH1D*)dir->FindObject("fHistXsection");
          else hxsection = (TH1D*)dir->FindObject("fHistXsectionVsPtHard");
          //else hxsection = (TH1D*)dir->FindObject("fHistXsection");
    }   
 
    if(!hxsection) {
      std::cout << "Error in getting x-section hist! Exiting..." << std::endl;
      return nullptr;
    }
    double xsection = hxsection->GetMean(2);
    double events = (double)hxsection->GetEntries();
    double scaling = xsection/events;

    TTree *tree;
    if(!fDmesonSpecie) tree = (TTree*)fileInput->Get("AliAnalysisTaskDmesonJets_D0_MCTruth");
    else tree = (TTree*)fileInput->Get("AliAnalysisTaskDmesonJets_DStar_MCTruth");
    AliAnalysisTaskDmesonJets::AliDmesonMCInfoSummary *brD = nullptr;

    AliAnalysisTaskDmesonJets::AliJetInfoSummary *brJet = nullptr;
    tree->SetBranchAddress("DmesonJet",&brD);
    tree->SetBranchAddress(Form("Jet_AKTChargedR0%d0_pt_scheme",Rpar),&brJet);

    if(!tree || !brD || !brJet) {
      std::cout << "Error in setting the tree/branch names! Exiting..." << std::endl;
      return nullptr;
    }

    TH1D *hPromptEff = nullptr;
    if(isEff) hPromptEff = (TH1D*)GetInputHist(effFilePrompt,"hEff_reb",hPromptEff);
    TH1D *hNonPromptEff = nullptr;
    if(isEff) hNonPromptEff = (TH1D*)GetInputHist(effFileNonPrompt,"hEff_reb",hNonPromptEff);


    TH1D *hjetpt[fptbinsDN];
    for (int j=0; j<fptbinsDN; j++) {
        hjetpt[j] = new TH1D(Form("hjetpt_%d",j),"hjetpt",100,0,100);
        hjetpt[j]->Sumw2();
    }
    double effC, effB, eff;


    for (int k=0; k<tree->GetEntries(); k++) {
        tree->GetEntry(k);
        if (brJet->fEta < -jetEta || brJet->fEta > jetEta) continue;


        if(BFDsim){
          if(brD->fPartonType != 5) continue;
        }
        else if(brD->fPartonType != 4) continue;
        if(brD->fAncestorPDG == 2212) continue; // check if not coming from proton

        if(isDptcut){
          for (int j=0; j<fptbinsDN; j++) {
            if (brD->fPt < fptbinsDA[j] || brD->fPt >= fptbinsDA[j+1]) continue;
            hjetpt[j]->Fill(brJet->fPt);
          }//end of D-meson pT bin loop
        }
        else hist->Fill(brJet->fPt);
    }

    if(isDptcut){
      for (int j=0; j<fptbinsDN; j++){
        double pt = (fptbinsDA[j]+fptbinsDA[j+1])/2.;
        if(isEff)  {
            effC = GetEfficiency(hPromptEff,pt);
            effB = GetEfficiency(hNonPromptEff,pt);
            eff = effB / effC;
        }
        else eff = 1;
        if (!j){
            hist = (TH1D*)hjetpt[j]->Clone("hist");
            hist->Scale(eff);
        }
        else hist->Add(hjetpt[j],eff);
      }
    }

hist->Scale(scaling);

 if(!hist) {
   std::cout << "Error in extracting the mass plot! Exiting..." << std::endl;
   return nullptr;
 }

return hist;

}


TH1* GetInputSimHistD(TString inFile, bool isjetptcut){

    TH1D *hist = new TH1D("hist","hDpt",100,0,100);

    TFile *fileInput = new TFile(inFile,"read");
    if(!fileInput){
      std::cout << "File " << fileInput << " cannot be opened! check your file path!" << std::endl; return nullptr;
    }

    TList* dir=(TList*)fileInput->Get("AliAnalysisTaskDmesonJets_histos");
    if(!dir){
      std::cout << "Error in getting dir! Exiting..." << std::endl;
      return nullptr;
    }

    TH1D *hxsection = (TH1D*)dir->FindObject("fHistXsectionVsPtHard");
    if(!hxsection){
      std::cout << "Error in getting x-section hist! Exiting..." << std::endl;
      return nullptr;
    }
    double xsection = hxsection->GetMean(2);
    double events = (double)hxsection->GetEntries();
    double scaling = xsection/events;


    TTree *tree;
    if(!fDmesonSpecie) tree = (TTree*)fileInput->Get("AliAnalysisTaskDmesonJets_D0_MCTruth");
    else tree = (TTree*)fileInput->Get("AliAnalysisTaskDmesonJets_DStar_MCTruth");
    AliAnalysisTaskDmesonJets::AliDmesonInfoSummary *brD = nullptr;

    AliAnalysisTaskDmesonJets::AliJetInfoSummary *brJet = nullptr;
    tree->SetBranchAddress("DmesonJet",&brD);

    tree->SetBranchAddress(Form("Jet_AKTChargedR0%d0_pt_scheme",Rpar),&brJet);

    if(!tree || !brD || !brJet) {
      std::cout << "Error in setting the tree/branch names! Exiting..." << std::endl;
      return nullptr;
    }


    for (int k=0; k<tree->GetEntries(); k++) {
    tree->GetEntry(k);
    //if(isjetptcut) { if (brJet->fEta < -jetEta || brJet->fEta > jetEta) continue; }
    //if (brJet->fEta < -jetEta || brJet->fEta > jetEta) continue;
    if(isjetptcut) { if (brJet->fPt < jetptmin || brJet->fPt >= jetptmax) continue; }
    if (brD->fEta < -1 || brD->fEta > 1) continue;
      hist->Fill(brD->fPt);
      //hist->Fill(brD->fEta);
    }

    hist->Scale(scaling);

    if(!hist) {
      std::cout << "Error in extracting the mass plot! Exiting..." << std::endl;
      return nullptr;
    }

    return hist;
}

double GetEfficiency(TH1 *hh, double Dpt){
    return hh->GetBinContent(hh->GetXaxis()->FindBin(Dpt));
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
