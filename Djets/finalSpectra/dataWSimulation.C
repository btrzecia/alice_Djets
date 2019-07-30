//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

#include "config.h"

//double jetEta = 0.9 - fRpar;
double fJetR, fJetEta, dy;


enum fHType {fHistJetPt=1, fHistZ=2, fHistDPt=3};

TH1* GetUpSys(TH1D **hh, const int nFiles, TH1D *hh_up);
TH1* GetDownSys(TH1D **hh, const int nFiles, TH1D *hh_down);
TH1* GetInputHist(TString inFile, TString histName);
void ScaleHist(TH1 *hh, int full = 0);
void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, double Msize = 1.1, Width_t Lwidth = 2, Style_t Lstyle = 1);
void SaveCanvas(TCanvas *c, TString name = "tmp");
void setCanvasDetails(TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, TPad *FinalSpectrum_2, TLegend *leg, fHType hType=fHistJetPt, bool isData=1, bool isProb=0, double jetptmin = 5, double jetptmax = 50);

void drawData(TH1D* data, TGraphAsymmErrors *grsys, fHType hType, TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, bool isSys=1);
void drawSim(TH1D* sim, TGraphAsymmErrors *grsys, TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, bool isSys=1);
void drawRatio(TH1D* sim, TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, bool isSys=0, TGraphAsymmErrors *grsys=nullptr);


bool isSimSys = 1, isSim = 1;
TString sysUncDir;

int     fbinsJetFinalN;
Double_t  *fbinsJetFinalA;
Double_t *xAxis;
Double_t plotmin = 5, plotmax=50;

Int_t colors[] = {kRed+2,kBlue+1,kBlue-1,kGreen+2, kViolet+5,kOrange-1,kGray+2,kCyan+1,kMagenta+2,kYellow+2,8,4,2,6};
Int_t markers[] = {20,21,25,22,23,26,24,25,26,27,28,29,30,32,33,34};
Double_t markersSize[] = {1.8,1.8,2,2,2,2,2,1.5,1.5,1.5,};
Int_t linestyle[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};


TString simFiles[] = {
  "AnalysisResults_FastSim_powheg+pythia6_charm_1549014986_noShower",
  "AnalysisResults_FastSim_powheg+pythia6_charm_1549014986",
  "AnalysisResults_FastSim_powheg+pythia8_charm_1562747037",
  "AnalysisResults_FastSim_pythia6_charm_full",
  "AnalysisResults_FastSim_pythia8_charm_full",
    "",
    "",
    ""
};

TString simPOWHEG6Files[12] {
    "AnalysisResults_FastSim_powheg+pythia6_charm_1556529849",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1556531679",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557130640",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557131019",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557314618",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557315004",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557483108",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557488527",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557603973",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557651402",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557694660",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557829599"
};


void dataWSimulation(
        bool isauto = 0,
    TString dataFile = "~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final2.root",
    //TString dataFile = "~/Work/ALICE/Analysis/Results/pp13tev/JetPtSpectrum_final_PRELIMINARY.root",
    bool isData = 1,
    bool isProb = 0,
    fHType   hType = fHistZ, //fHistZ, //fHistJetPt, //fHistDPt
    double JetPtMin = 7, double JetPtMax = 10,
    double DPtMin = 2, double DPtMax = 36,
    double Rpar = 0.4,
    TString histBase = "hData_binned",
    bool isDataSpectrum = 1,
    TString simDir = "/media/basia/Disk2/Work/ALICE/Simulations/pp13TeV/simOutput/",
    TString outSpectraDir = "/home/basia/Work/ALICE/Analysis/Results/pp13tev/simulations/plots/",
    TString sysDir = "./",
    bool isSys = 1,
    bool simsys = 1
)
{

    fJetR = Rpar;
    fJetEta = 0.9-fJetR;
    dy = 2*fJetEta;

    std::map<std::string, bool> simulators =
    {
         { "Powheg6_noshower", false }, { "Powheg6", false }, { "Powheg8", false }
    };
       /* {
             { "Powheg6_noshower", false }, { "Powheg6", false }, { "Powheg8", false }, { "Pythia6", false }, { "Pythia8", false }, { "Powheg6_diJet", false }, { "HerwigCharm", false }, { "HerwigMB", false }
        };*/


    std::cout << "Available simulators:"  << std::endl;
    std::cout << "All"  << std::endl;
           for ( const auto &p : simulators )
           {
                std::cout << p.first << std::endl;
           }
    std::cout << "Type simulators you want to use" << std::endl;

        std::string sim;
        bool isSim = false;

        if(isauto) {
            for ( auto &p : simulators )
            {
                p.second = true;
                isSim = true;

            }
        }
        else {
            while ( std::getline(std::cin, sim ) )
            {
                if ( sim == "Exit" | sim == "exit" ) break;

                if ( sim == "All" | sim == "all" ) {
                    for ( auto &p : simulators )
                    {
                        p.second = true;
                        isSim = true;
                    }
                    break;
                }

                if ( isSim = ( simulators.count( sim ) != 0 ) )
                {
                    simulators[sim] = true;
                }
                else
                {
                    std::cout << "Invalid simulator." << std::endl;
                }

                std::cout << "Type another if you have any more simulators else type Exit" << std::endl;
            }
        }


    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    isSimSys = simsys;
    sysUncDir = sysDir;
    //gSystem->Exec(Form("mkdir %s",outSpectraDir.Data()));


    double jetptbins[9] = { 5,6,8,10,12,14,20,30,50 };
    double zbinsSim[11] = { 0.,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1. };
    double zbinsData1[8] = { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1. };
    double zbinsData2[9] = { 0.,0.2,0.4,0.5,0.6,0.7,0.8,0.9,1. };
    double Dptbins[16] = { 0,1,2,3,4,5,6,8,10,12,14,16,18,20,30,40 };


    switch (hType) {
    case fHistJetPt:
        fbinsJetFinalN = 8;
        fbinsJetFinalA = new double[fbinsJetFinalN+1];
        for(int i=0; i<fbinsJetFinalN+1; i++) fbinsJetFinalA[i] = jetptbins[i];
        break;
    case fHistDPt:
        fbinsJetFinalN = 15;
        fbinsJetFinalA = new double[fbinsJetFinalN+1];
        for(int i=0; i<fbinsJetFinalN+1; i++) fbinsJetFinalA[i] = Dptbins[i];
        break;
    case fHistZ:
        if(isData){
            if(JetPtMin == 10 || JetPtMin == 15) fbinsJetFinalN = 8;
            else fbinsJetFinalN = 7;
        }
        else fbinsJetFinalN = 10;
        fbinsJetFinalA = new double[fbinsJetFinalN+1];
        for(int i=0; i<fbinsJetFinalN+1; i++) {
            if(isData){
                if(JetPtMin == 10 || JetPtMin == 15)  fbinsJetFinalA[i] = zbinsData2[i];
                else  fbinsJetFinalA[i] = zbinsData1[i];
            }
            else fbinsJetFinalA[i] = zbinsSim[i];
        }
        break;
    }

    xAxis = new double[fbinsJetFinalN+1];
    for(int k=0; k<fbinsJetFinalN+1; k++) xAxis[k] = fbinsJetFinalA[k];
    double* systuncP = new double[fbinsJetFinalN];
    for(int k=0; k<fbinsJetFinalN; k++) systuncP[k] = 0.1;

    double sysJetPtSpectrum[8] = {  11.9, 12.3, 13.5, 14.2, 16.0, 17.7, 25.4, 27.6 };
    if(hType==fHistJetPt) for(int k=0; k<fbinsJetFinalN; k++) systuncP[k] = sysJetPtSpectrum[k]*0.01;

    double nEv = 1.55444E+9;
    double dataLum = nEv/(sigma_in*1000) ;//Luminosity in mbar
    double simScaling = 1;
    if(fSystem) simScaling = APb/2.;
    else simScaling = 0.5;
    double dataScaling;
    if(fDmesonSpecie) dataScaling = 1. /(BRDstar * dataLum)/2.;
    else dataScaling = 1. /(BRDzero * dataLum)/2.;

    TCanvas *FinalSpectrum = new TCanvas("FinalSpectrum", "FinalSpectrum",0,45,1400,1500);
    TPad *FinalSpectrum_1 = new TPad("FinalSpectrum_1", "FinalSpectrum_1",0,0.35,1,1);
    TPad *FinalSpectrum_2 = new TPad("FinalSpectrum_2", "FinalSpectrum_2",0,0,1,0.35);
    TLegend *leg;
    if (hType == fHistZ) {
       if(isData) leg = new TLegend(0.12,0.65,0.5,0.88,NULL,"NB NDC");
       else leg = new TLegend(0.55,0.12,0.9,0.35,NULL,"NB NDC");
    }
    else{
        if(isData) leg = new TLegend(0.5,0.55,0.9,0.85,NULL,"NB NDC");
        else leg = new TLegend(0.5,0.55,0.9,0.85,NULL,"NB NDC");
    }

    setCanvasDetails(FinalSpectrum, FinalSpectrum_1, FinalSpectrum_2,leg,hType,isData,isProb, JetPtMin,JetPtMax);


    // ----------------- data ---------------------
    TH1D *hData_binned2 = (TH1D*)GetInputHist(dataFile, histBase);
    TH1D *hData_binned;
    if(!isDataSpectrum) {
        hData_binned = (TH1D*)hData_binned2->Rebin(fbinsJetFinalN,"hData_binned", xAxis);
        hData_binned->Sumw2();
        hData_binned->Scale(1,"width");
        hData_binned->Scale(dataScaling);
        hData_binned->Scale(1./dy);
    }
    else hData_binned = (TH1D*)hData_binned2->Clone("hData_binned");
    if(isData) leg->AddEntry(hData_binned,"data","p");


    if(isProb){
        for(int j=1; j<hData_binned->GetNbinsX()+1;j++){
            double val = hData_binned->GetBinContent(j) * hData_binned->GetBinWidth(j);
            double err = hData_binned->GetBinError(j) * hData_binned->GetBinWidth(j);
            hData_binned->SetBinContent(j,val);
            hData_binned->SetBinError(j,err);
        }
        hData_binned->Scale(1./hData_binned->Integral());
        hData_binned->Scale(1,"width");
    }


    double *sysunc = new double[fbinsJetFinalN], *sysuncAbs = new double[fbinsJetFinalN];
    double *value = new double[fbinsJetFinalN], *ptval = new double[fbinsJetFinalN], *ptvalunc = new double[fbinsJetFinalN];
    TGraphAsymmErrors *grsys = nullptr;
    if(isData && isSys) {
          for(int j=0; j<fbinsJetFinalN; j++){
                  ptval[j] = (xAxis[j]+xAxis[j+1]) / 2.;
                  ptvalunc[j] = (xAxis[j+1]-xAxis[j]) / 2.;
                  value[j] = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(ptval[j]));
                  sysuncAbs[j] = value[j] * systuncP[j];
          }
          grsys = new TGraphAsymmErrors(fbinsJetFinalN,ptval,value,ptvalunc,ptvalunc,sysuncAbs,sysuncAbs);

    }

    if(isData) drawData(hData_binned,grsys,hType,FinalSpectrum,FinalSpectrum_1,isSys);

    TGraphAsymmErrors *grsysRatio;
    TH1D *hData_binned_ratio = (TH1D*)hData_binned->Clone("hData_binned_ratio");
    if(isData && isSys){
      double *sysuncRatio = new double[fbinsJetFinalN];
      double *valRatio = new double[fbinsJetFinalN];
      for(int j=0; j<fbinsJetFinalN; j++){
              double pt = (xAxis[j]+xAxis[j+1]) / 2.;
              ptvalunc[j] = (xAxis[j+1]-xAxis[j]) / 2.;
              double val = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
              double valPred = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
              valRatio[j] = val / valPred;
              double err = hData_binned->GetBinError(hData_binned->GetXaxis()->FindBin(pt)) / valPred;
              sysuncRatio[j] = systuncP[j]*valRatio[j];
              hData_binned_ratio->SetBinContent(hData_binned_ratio->GetXaxis()->FindBin(pt),valRatio[j]);
              hData_binned_ratio->SetBinError(hData_binned_ratio->GetXaxis()->FindBin(pt),err);
      }
      grsysRatio = new TGraphAsymmErrors(fbinsJetFinalN,ptval,valRatio,ptvalunc,ptvalunc,sysuncRatio,sysuncRatio);
      Int_t ci = TColor::GetColor("#cccccc");
      grsysRatio->SetFillColor(ci);
      grsysRatio->SetLineColor(ci);
    }


    if(isData) drawRatio(hData_binned_ratio,FinalSpectrum,FinalSpectrum_2,1,grsysRatio);

    TH1D *hSim0 = nullptr;
    if(!isData){
        int simNr = 0;
        TString file = simDir;
        file += simFiles[simNr];
        if(hType == fHistJetPt) file += "_JetPt_DpTMin_";
        else if(hType == fHistDPt) file += "_DPt_JetpTMin_";
        else if(hType == fHistZ) file += "_Z_DpTMin_";
        if(hType == fHistDPt) { file += JetPtMin; file += "_JetpTMax_"; file += JetPtMax; }
        else { file += DPtMin; file += "_DpTMax_"; file += DPtMax; }
        if(hType == fHistZ) {
            file += "_JetpTMin_"; file += JetPtMin; file += "_JetpTMax_"; file += JetPtMax;
        }
        file += "_Dzero_R";
        file += Rpar*10;
        file += ".root";

        TH1D *htmp = (TH1D*) GetInputHist(file, "hist");
        TH1D *hSim = (TH1D*)htmp->Rebin(fbinsJetFinalN,"hSim",xAxis);
        hSim->Scale(simScaling);
        hSim->Scale(1./dy);
        if(isProb) hSim->Scale(1./hSim->Integral());
        hSim->Scale(1,"width");
        hSim->SetLineColor(colors[simNr+1]);
        hSim->SetMarkerColor(colors[simNr+1]);
        hSim->SetMarkerStyle(markers[simNr+1]);
        hSim->SetMarkerSize(markersSize[simNr+1]);
        hSim0 = dynamic_cast<TH1D*>(hSim->Clone("hSim0"));

    }

    // ----------------- simulations ---------------------
    if(isSim){
       for ( auto &p : simulators ) {
           if(p.second){
               TH1D *hSim = nullptr;
               TH1D *hSimRatio = nullptr;
               int simNr = -1;
               TString legName;
               if(p.first == "Powheg6_noshower") {
                   simNr = 0;
                   legName = "POWHEG+PYTHIA6";
               }
               else if(p.first == "Powheg6") {
                   simNr = 1;
                   legName = "POWHEG+PYTHIA6_full";
               }
               else if(p.first == "Powheg8") {
                   simNr = 2;
                   legName = "POWHEG+PYTHIA8";
               }
               else if(p.first == "Pythia6") {
                   simNr = 3;
                   legName = "PYTHIA6";
               }
               else if(p.first == "Pythia8") {
                   simNr = 4;
                   legName = "PYTHIA8";
               }
               else continue;

               std::cout << simNr << std::endl;
               TString file = simDir;
               file += simFiles[simNr];
               if(hType == fHistJetPt) file += "_JetPt_DpTMin_";
               else if(hType == fHistDPt) file += "_DPt_JetpTMin_";
               else if(hType == fHistZ) file += "_Z_DpTMin_";
               if(hType == fHistDPt) { file += JetPtMin; file += "_JetpTMax_"; file += JetPtMax; }
               else { file += DPtMin; file += "_DpTMax_"; file += DPtMax; }
               if(hType == fHistZ) {
                   file += "_JetpTMin_"; file += JetPtMin; file += "_JetpTMax_"; file += JetPtMax;
               }
               file += "_Dzero_R";
               file += Rpar*10;
               file += ".root";

               TH1D *htmp = (TH1D*) GetInputHist(file, "hist");
               hSim = (TH1D*)htmp->Rebin(fbinsJetFinalN,"hSim",xAxis);
               hSim->Scale(simScaling);
               hSim->Scale(1./dy);
               if(isProb) hSim->Scale(1./hSim->Integral());
               hSim->Scale(1,"width");
               hSim->SetLineColor(colors[simNr+1]);
               hSim->SetMarkerColor(colors[simNr+1]);
               hSim->SetMarkerStyle(markers[simNr+1]);
               hSim->SetMarkerSize(markersSize[simNr+1]);

               leg->AddEntry(hSim,legName.Data(),"p");
               //======= Ratio to data ======
               if(isData) {
                   hSimRatio = (TH1D*)hSim->Clone("hSimRatio");
                   hSimRatio->Divide(hData_binned);
               }
               else {
                   hSimRatio = (TH1D*)hSim->Clone("hSimRatio");
                   hSimRatio->Divide(hSim0);
               }

               TGraphAsymmErrors *grsystheory;
               TGraphAsymmErrors *grsysTheoryRatio;
               if(isSimSys && !simNr){
                   TH1D *hPrompt[12], *hPrompt_binned[12];
                   for (int nr=0; nr<12; nr++){
                       TString file = simDir;
                       file += simPOWHEG6Files[nr];
                       file += "_noShower";
                       if(hType == fHistJetPt) file += "_JetPt_DpTMin_";
                       else if(hType == fHistDPt) file += "_DPt_JetpTMin_";
                       else if(hType == fHistZ) file += "_Z_DpTMin_";
                       if(hType == fHistDPt) { file += JetPtMin; file += "_JetpTMax_"; file += JetPtMax; }
                       else { file += DPtMin; file += "_DpTMax_"; file += DPtMax; }
                       if(hType == fHistZ) {
                           file += "_JetpTMin_"; file += JetPtMin; file += "_JetpTMax_"; file += JetPtMax;
                       }
                       file += "_Dzero_R";
                       file += Rpar*10;
                       file += ".root";

                       TH1D *htmp = (TH1D*) GetInputHist(file, "hist");
                       hPrompt_binned[nr] = (TH1D*)htmp->Rebin(fbinsJetFinalN,"hSim",xAxis);

                   }

                   TH1D *hPrompt_up = (TH1D*)hSim->Clone("hPrompt_up");
                   hPrompt_up = (TH1D*)GetUpSys(hPrompt_binned,12,hPrompt_up);
                   hPrompt_up->Scale(simScaling);
                   hPrompt_up->Scale(1./dy);
                   if(isProb) hPrompt_up->Scale(1./hPrompt_up->Integral());
                   hPrompt_up->Scale(1,"width");
                   hPrompt_up->SetLineColor(colors[simNr+1]);
                   hPrompt_up->SetMarkerColor(colors[simNr+1]);
                   hPrompt_up->SetMarkerStyle(markers[simNr+1]);
                   hPrompt_up->SetMarkerSize(markersSize[simNr+1]);

                   TH1D *hPrompt_down = (TH1D*)hSim->Clone("hPrompt_down");
                   hPrompt_down = (TH1D*)GetDownSys(hPrompt_binned,12,hPrompt_down);
                   hPrompt_down->Scale(simScaling);
                   hPrompt_down->Scale(1./dy);
                   if(isProb) hPrompt_down->Scale(1./hPrompt_down->Integral());
                   hPrompt_down->Scale(1,"width");
                   hPrompt_down->SetLineColor(colors[simNr+1]);
                   hPrompt_down->SetMarkerColor(colors[simNr+1]);
                   hPrompt_down->SetMarkerStyle(markers[simNr+1]);
                   hPrompt_down->SetMarkerSize(markersSize[simNr+1]);

                   Double_t ptvaltheory[fbinsJetFinalN];
                   Double_t ptvalunctheory[fbinsJetFinalN];
                   Double_t valuetheory[fbinsJetFinalN];
                   Double_t valuetheoryerrup[fbinsJetFinalN];
                   Double_t valuetheoryerrdown[fbinsJetFinalN];

                   for(int j=0; j<fbinsJetFinalN; j++){
                             ptvaltheory[j] = (xAxis[j]+xAxis[j+1]) / 2.;
                             ptvalunctheory[j] = (xAxis[j+1]-xAxis[j]) / 2.;
                             valuetheory[j] = hSim->GetBinContent(hSim->GetXaxis()->FindBin(ptvaltheory[j]));
                             valuetheoryerrup[j] = hPrompt_up->GetBinContent(hPrompt_up->GetXaxis()->FindBin(ptvaltheory[j])) - valuetheory[j];
                             valuetheoryerrdown[j] = valuetheory[j] - hPrompt_down->GetBinContent(hPrompt_up->GetXaxis()->FindBin(ptvaltheory[j]));
//                             std::cout << ptvaltheory[j] << '\t' << valuetheory[j] << '\t' << valuetheoryerrdown[j] << '\t' << valuetheoryerrup[j] << std::endl;

                     }
                     grsystheory = new TGraphAsymmErrors(fbinsJetFinalN,ptvaltheory,valuetheory,ptvalunctheory,ptvalunctheory,valuetheoryerrdown,valuetheoryerrup);
                     grsystheory->SetLineColor(colors[simNr+1]);
                     grsystheory->SetLineWidth(2);
                     grsystheory->SetFillStyle(0);

                      TH1D *hSimRatioUp = nullptr, *hSimRatioDown = nullptr;
                     if(isData) {
                         hSimRatioUp = (TH1D*)hPrompt_up->Clone("hSimRatioUp");
                         hSimRatioUp->Divide(hData_binned);
                         hSimRatioDown = (TH1D*)hPrompt_down->Clone("hSimRatioDown");
                         hSimRatioDown->Divide(hData_binned);
                     }
                     else {
                         hSimRatioUp = (TH1D*)hPrompt_up->Clone("hSimRatioUp");
                         hSimRatio->Divide(hSim0);
                         hSimRatioDown = (TH1D*)hPrompt_down->Clone("hSimRatioDown");
                         hSimRatioDown->Divide(hSim0);
                     }

                    double *valRatioSim = new double[fbinsJetFinalN];
                    double *valuetheoryerrdownratio = new double[fbinsJetFinalN];
                    double *valuetheoryerrupratio = new double[fbinsJetFinalN];

                     for(int j=0; j<fbinsJetFinalN; j++){
                             double pt = (xAxis[j]+xAxis[j+1]) / 2.;
                             ptvalunc[j] = (xAxis[j+1]-xAxis[j]) / 2.;
                             double val = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
                             double valPred = hSim->GetBinContent(hSim->GetXaxis()->FindBin(pt));
                             valRatioSim[j] = hSimRatio->GetBinContent(hSim->GetXaxis()->FindBin(pt));

                             valuetheoryerrupratio[j] = hSimRatioUp->GetBinContent(hSimRatioUp->GetXaxis()->FindBin(ptvaltheory[j])) - valRatioSim[j];
                             valuetheoryerrdownratio[j] = valRatioSim[j] - hSimRatioDown->GetBinContent(hSimRatioDown->GetXaxis()->FindBin(ptvaltheory[j]));

                     }
                     grsysTheoryRatio = new TGraphAsymmErrors(fbinsJetFinalN,ptval,valRatioSim,ptvalunc,ptvalunc,valuetheoryerrdownratio,valuetheoryerrupratio);
                     grsysTheoryRatio->SetLineColor(colors[simNr+1]);
                     grsysTheoryRatio->SetLineWidth(2);
                     grsysTheoryRatio->SetFillStyle(0);

               }

               if(!simNr && isSimSys) drawSim(hSim,grsystheory,FinalSpectrum,FinalSpectrum_1,1);
               else drawSim(hSim,grsys,FinalSpectrum,FinalSpectrum_1,0);
               if(!simNr && isSimSys) drawRatio(hSimRatio,FinalSpectrum,FinalSpectrum_2,1,grsysTheoryRatio);
               else drawRatio(hSimRatio,FinalSpectrum,FinalSpectrum_2,0);
           }

       }

    }

    FinalSpectrum_1->cd();
    leg->Draw("same");

    TString outName = outSpectraDir;
    if(hType == fHistJetPt) outName += "JetPt_DpTMin_";
    else if(hType == fHistZ) outName += "Z_DpTMin_";
    else if(hType == fHistDPt) outName += "DPt";
    if(!(hType == fHistDPt)) { outName += DPtMin; outName += "_DpTMax_"; outName += DPtMax; }
    if(hType == fHistZ || hType == fHistDPt) {
        outName += "_JetpTMin_"; outName += JetPtMin; outName += "_JetpTMax_"; outName += JetPtMax;
    }
    outName += "_Dzero_R";
    outName += Rpar*10;
    if(!isData) outName += "_MConly";
    if(isProb) outName += "_prob";
    outName += ".png";

    FinalSpectrum->SaveAs(outName.Data());


return;

/*
    Double_t ptvaltheory[fbinsJetFinalN];
    Double_t ptvalunctheory[fbinsJetFinalN];
    Double_t valuetheory[fbinsJetFinalN];
    Double_t valuetheoryerrup[fbinsJetFinalN];
    Double_t valuetheoryerrdown[fbinsJetFinalN];
    if(isSim && isSimSys){
      for(int j=0; j<fbinsJetFinalN; j++){
              ptvaltheory[j] = (xAxis[j]+xAxis[j+1]) / 2.;
              ptvalunctheory[j] = (xAxis[j+1]-xAxis[j]) / 2.;
              valuetheory[j] = hPrompt_central_binned->GetBinContent(hPrompt_central_binned->GetXaxis()->FindBin(ptvaltheory[j]));
              valuetheoryerrup[j] = hPrompt_up->GetBinContent(hPrompt_up->GetXaxis()->FindBin(ptvaltheory[j])) - valuetheory[j];
              valuetheoryerrdown[j] = valuetheory[j] - hPrompt_down->GetBinContent(hPrompt_up->GetXaxis()->FindBin(ptvaltheory[j]));

      }
      grsystheory = new TGraphAsymmErrors(fbinsJetFinalN,ptvaltheory,valuetheory,ptvalunctheory,ptvalunctheory,valuetheoryerrdown,valuetheoryerrup);
    }

   //======= Ratio to powheg ======
    TH1D *hPrompt_central_binned_ratio;
    TH1D *hPrompt_down_ratio;
    TH1D *hPrompt_up_ratio;
     if(isSim){
       hPrompt_central_binned_ratio  = (TH1D*)hPrompt_central_binned->Clone("hPrompt_central_binned_ratio");
       hPrompt_central_binned_ratio->Divide(hPrompt_central_binned);
       if(isSimSys){
         hPrompt_up_ratio = (TH1D*)hPrompt_up->Clone("hPrompt_up_ratio");
         hPrompt_down_ratio = (TH1D*)hPrompt_down->Clone("hPrompt_down_ratio");
         hPrompt_up_ratio->Divide(hPrompt_central_binned);
         hPrompt_down_ratio->Divide(hPrompt_central_binned);
       }
     }
       hData_binned_ratio = (TH1D*)hData_binned->Clone("hData_binned_ratio");
       if(isSys){
         double *sysuncRatio = new double[fbinsJetFinalN];
         double *valRatio = new double[fbinsJetFinalN];
         for(int j=0; j<fbinsJetFinalN; j++){
                 double pt = (xAxis[j]+xAxis[j+1]) / 2.;
                 double val = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
                 double valPred;
                 if(isSim) valPred = hPrompt_central_binned->GetBinContent(hPrompt_central_binned->GetXaxis()->FindBin(pt));
                 else valPred = hData_binned->GetBinContent(hData_binned->GetXaxis()->FindBin(pt));
                 valRatio[j] = val / valPred;
                 double err = hData_binned->GetBinError(hData_binned->GetXaxis()->FindBin(pt)) / valPred;
                 //err = err * valRatio[j];
                 sysuncRatio[j] = sysunc[j]*valRatio[j];

                 hData_binned_ratio->SetBinContent(hData_binned_ratio->GetXaxis()->FindBin(pt),valRatio[j]);
                 hData_binned_ratio->SetBinError(hData_binned_ratio->GetXaxis()->FindBin(pt),err);
         }
         grsysRatio = new TGraphAsymmErrors(fbinsJetFinalN,ptval,valRatio,ptvalunc,ptvalunc,sysuncRatio,sysuncRatio);
       }
       hData_binned_ratio->SetMaximum(2);

       double *sysuncTheoryratio = new double[fbinsJetFinalN];
       double *ptvaltheoryratio = new double[fbinsJetFinalN];
       double *ptvalunctheoryratio = new double[fbinsJetFinalN];
       double *valuetheoryratio = new double[fbinsJetFinalN];
       double *valuetheoryerrupratio = new double[fbinsJetFinalN];
       double *valuetheoryerrdownratio = new  double[fbinsJetFinalN];
       if(isSimSys){

            for(int j=0; j<fbinsJetFinalN; j++){
                  ptvaltheoryratio[j] = (xAxis[j]+xAxis[j+1]) / 2.;
                  ptvalunctheoryratio[j] = (xAxis[j+1]-xAxis[j]) / 2.;
                  valuetheoryratio[j] = hPrompt_central_binned_ratio->GetBinContent(hPrompt_central_binned_ratio->GetXaxis()->FindBin(ptvaltheory[j]));
                  valuetheoryerrupratio[j] = hPrompt_up_ratio->GetBinContent(hPrompt_up_ratio->GetXaxis()->FindBin(ptvaltheory[j])) - valuetheoryratio[j];
                  valuetheoryerrdownratio[j] = valuetheoryratio[j] - hPrompt_down_ratio->GetBinContent(hPrompt_down_ratio->GetXaxis()->FindBin(ptvaltheory[j]));
            }
            grsystheoryratio = new TGraphAsymmErrors(fbinsJetFinalN,ptvaltheoryratio,valuetheoryratio,ptvalunctheoryratio,ptvalunctheoryratio,valuetheoryerrdownratio,valuetheoryerrupratio);
      }




TFile *ofile = new TFile(Form("%s/JetPtSpectrum_final.root",outSpectraDir.Data()),"RECREATE");
hData_binned->Write();
hData_binned_ratio->Write();
if(isSim){
  hPrompt_central_binned->Write();
  if(isSimSys){
    hPrompt_up->Write();
    hPrompt_down->Write();
    grsystheory->Write();
    grsystheoryratio->Write();
  }
}
if(isSys) grsysRatio->Write();
if(isSys) grsys->Write();

ofile->Close();
*/


return;

}

void drawData(TH1D* hData_binned, TGraphAsymmErrors* grsys, fHType hType, TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, bool isSys){

      //=========Macro generated from canvas: FinalSpectrum/FinalSpectrum
    //=========  (Mon Jun 12 17:00:30 2017) by ROOT version5.34/30

    FinalSpectrum->cd();
    FinalSpectrum_1->Draw();
    FinalSpectrum_1->cd();

    Int_t ci;
       // dat syst. unc.
    TGraphAsymmErrors *grae;
    if(isSys){
       grae = (TGraphAsymmErrors*) grsys->Clone("grae"); // = new TGraphAsymmErrors(6);
       grae->SetName("CentralPointsSystematicUncertainty_copy");
       ci = TColor::GetColor("#cccccc");
       grae->SetFillColor(ci);
       grae->SetLineColor(ci);

       //=== data uncertantity from grae
       TH1F *Graph_central_syst_unc1 = new TH1F("Graph_central_syst_unc1","Graph_central_syst_unc1",100,5,45);
       grae->SetHistogram(Graph_central_syst_unc1);
       grae->Draw("2");
     }

    // Central data
    hData_binned->SetTitle("");
    hData_binned->SetMaximum(hData_binned->GetMaximum()*2);
    if(hType==fHistJetPt || hType==fHistDPt) {
        hData_binned->GetYaxis()->SetTitle("d^{2}#sigma/dp_{T}d#it{#eta} (mb)");
        hData_binned->GetXaxis()->SetTitle("p_{T}^{ch,jet} (GeV/c)");
    }
    else if(hType==fHistZ) {
        hData_binned->GetYaxis()->SetTitle("d^{2}#sigma/dzd#it{#eta} (mb)");
        hData_binned->GetXaxis()->SetTitle("z_{||}");
    }

    hData_binned->SetLineColor(colors[0]);
    hData_binned->SetMarkerColor(colors[0]);
    hData_binned->SetMarkerStyle(markers[0]);
    hData_binned->SetMarkerSize(markersSize[0]);
    hData_binned->Draw("same p e0 x0");
}


void drawSim(TH1D* hSim, TGraphAsymmErrors* grsys,TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, bool isSys){

    FinalSpectrum->cd();
    FinalSpectrum_1->Draw();
    FinalSpectrum_1->cd();

    // theory syst unc
    TGraphAsymmErrors *grae;
    if(isSys){
        grae = (TGraphAsymmErrors*) grsys->Clone("grae"); // = new TGraphAsymmErrors(6);
        grae->SetName("grae");
        grae->SetFillColor(1);
        grae->SetFillStyle(0);

        //=== sim uncertantity from grae
        TH1F *gr = new TH1F("gr","Graph_central_syst_unc1",100,5,45);
        grae->SetHistogram(gr);
        grae->Draw("2");

   }

    TH1D *GeneratorLevel_JetPtSpectrum__3 = (TH1D*)hSim->Clone("GeneratorLevel_JetPtSpectrum__3");
    GeneratorLevel_JetPtSpectrum__3->Draw("same p e0 x0");

}


void drawRatio(TH1D* hratio, TCanvas *FinalSpectrum, TPad *FinalSpectrum_2, bool isSys, TGraphAsymmErrors *grsys) {

    FinalSpectrum->cd();
    FinalSpectrum_2->Draw();
    FinalSpectrum_2->cd();

    TGraphAsymmErrors *grae;
       // theory syst. unc.
    if(isSys){
        grae = (TGraphAsymmErrors*) grsys->Clone("grae"); // = new TGraphAsymmErrors(6);
        grae->SetName("grae");

        //=== sim uncertantity from grae
        TH1F *Graph_central_syst_unc1 = new TH1F("Graph_central_syst_unc1","Graph_central_syst_unc1",100,5,45);
        grae->SetHistogram(Graph_central_syst_unc1);
        grae->Draw("2");

   }

    TH1D *hRatioSimToData =  (TH1D*) hratio->Clone("hRatioSimToData");
    hRatioSimToData->Draw("same p e0 x0");

    FinalSpectrum_2->Modified();
    FinalSpectrum->cd();
    FinalSpectrum->Modified();
    //FinalSpectrum->cd();
    //FinalSpectrum->SetSelected(FinalSpectrum);

}


TH1* GetUpSys(TH1D **hh, const int nFiles, TH1D *hh_up){

        double bin = 0, binerr = 0;
        double max = 0, maxerr = 0;
        for(int j=1; j<fbinsJetFinalN+1; j++ ){
            max = hh[0]->GetBinContent(j);
            for(int i=1;i<nFiles;i++){
                if(hh[i]->GetBinContent(j) > max){
                        max = hh[i]->GetBinContent(j);
                        maxerr = hh[i]->GetBinError(j);
                }
            }
            hh_up->SetBinContent(j,max);
            hh_up->SetBinError(j,0);
        }

    return hh_up;
}

TH1* GetDownSys(TH1D **hh, const int nFiles, TH1D *hh_down){
        double bin = 0, binerr = 0;
        double max = 0, maxerr = 0;

        for(int j=1; j<fbinsJetFinalN+1; j++ ){
      //for(int j=1; j<hh[0]->GetNbinsX()+1; j++ ){
            max = hh[0]->GetBinContent(j);
            for(int i=1;i<nFiles;i++){
                if(hh[i]->GetBinContent(j) < max){
                        max = hh[i]->GetBinContent(j);
                        maxerr = hh[i]->GetBinError(j);
                }
            }
            hh_down->SetBinContent(j,max);
            hh_down->SetBinError(j,0);
        }

    return hh_down;
}



TH1* GetInputHist(TString inFile, TString histName){

	TFile *jetPtFile = new TFile(inFile,"read");
    TH1F *hh = (TH1F*)jetPtFile->Get(histName.Data());

  return hh;
}


void setCanvasDetails(TCanvas *FinalSpectrum, TPad *FinalSpectrum_1, TPad *FinalSpectrum_2, TLegend *leg, fHType hType, bool isData, bool isProb, double jetptmin, double jetptmax){

    FinalSpectrum->SetHighLightColor(2);
    FinalSpectrum->Range(0,0,1,1);
    FinalSpectrum->SetFillColor(0);
    FinalSpectrum->SetBorderMode(0);
    FinalSpectrum->SetBorderSize(4);
    FinalSpectrum->SetFrameBorderMode(0);

 // ------------>Primitives in pad: FinalSpectrum_1

    FinalSpectrum->cd();
    FinalSpectrum_1->Draw();
    FinalSpectrum_1->cd();
    FinalSpectrum_1->Range(-1.986821e-07,-4.69897,33.33333,0.3499945);
    FinalSpectrum_1->SetFillColor(0);
    FinalSpectrum_1->SetBorderMode(0);
    FinalSpectrum_1->SetBorderSize(4);
    FinalSpectrum_1->SetLogy();
    FinalSpectrum_1->SetTickx(1);
    FinalSpectrum_1->SetTicky(1);
    FinalSpectrum_1->SetLeftMargin(0.12);
    FinalSpectrum_1->SetBottomMargin(0);
    FinalSpectrum_1->SetFrameBorderMode(0);
    FinalSpectrum_1->SetFrameBorderMode(0);

    double min, max;
    TH1D *CentralPointsStatisticalUncertainty__1 = new TH1D("CentralPointsStatisticalUncertainty__1","Central Values",fbinsJetFinalN, xAxis);
    if(fSystem){
      CentralPointsStatisticalUncertainty__1->SetMinimum(2.e-04);
      CentralPointsStatisticalUncertainty__1->SetMaximum(500);
    }
    else{
        if(hType == fHistZ) {
            if(isData) {
                min = 4.e-04;
                max = 0.62;
                if(jetptmin == 10 || jetptmin == 15) {
                    min = 1.1e-04;
                    max = 0.06;
                }
                if(isProb) min = 5.e-02;
            }
            else {
                min = 1.1e-03;
                max = 0.22;

                if(jetptmin == 10 || jetptmin == 15) {
                    min = 1.1e-04;
                    max = 0.06;
                }
            }
        }
        else {
            min = 2.e-06;
            max = 0.05;

            if(!isData){
                min = 5.2e-06;
                max = 0.02;
                if(hType==fHistDPt) max = 0.2;
                if(hType==fHistDPt && jetptmin == 5) max = 0.05;
            }
        }

        if(isProb) max = 10;
    }


        CentralPointsStatisticalUncertainty__1->SetMinimum(min);
        CentralPointsStatisticalUncertainty__1->SetMaximum(max);


    CentralPointsStatisticalUncertainty__1->SetEntries(8);
    CentralPointsStatisticalUncertainty__1->SetDirectory(0);
    CentralPointsStatisticalUncertainty__1->SetStats(0);

    CentralPointsStatisticalUncertainty__1->GetXaxis()->SetTitle("#it{p}_{T,ch jet} (GeV/#it{c})");
    CentralPointsStatisticalUncertainty__1->GetXaxis()->SetLabelFont(42);
    CentralPointsStatisticalUncertainty__1->GetXaxis()->SetLabelSize(0.055);
    CentralPointsStatisticalUncertainty__1->GetXaxis()->SetTitleSize(0.055);
    CentralPointsStatisticalUncertainty__1->GetXaxis()->SetTitleFont(42);
    if(hType == fHistZ) CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{z}_{||}d#it{#eta}} mb (GeV/#it{c})^{-1}");
    else CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitle("#frac{d^{2}#sigma}{d#it{p}_{T}d#it{#eta}} mb (GeV/#it{c})^{-1}");
    if(isProb)CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitle("Probability density");
    CentralPointsStatisticalUncertainty__1->GetYaxis()->SetLabelFont(43);
    CentralPointsStatisticalUncertainty__1->GetYaxis()->SetLabelSize(30);
    CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitleSize(40);
    CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitleOffset(1.8);
    CentralPointsStatisticalUncertainty__1->GetYaxis()->SetTitleFont(43);
    CentralPointsStatisticalUncertainty__1->GetZaxis()->SetLabelFont(42);
    CentralPointsStatisticalUncertainty__1->GetZaxis()->SetLabelSize(0.055);
    CentralPointsStatisticalUncertainty__1->GetZaxis()->SetTitleSize(0.055);
    CentralPointsStatisticalUncertainty__1->GetZaxis()->SetTitleFont(42);
    CentralPointsStatisticalUncertainty__1->Draw("axis");


    TPaveText *pt;
    if(hType == fHistZ){
        if(isData) pt = new TPaveText(0.42,0.05,0.7,0.3,"NB NDC");
        else  pt = new TPaveText(0.15,0.55,0.55,0.85,"NB NDC");
    }
    else {
        if(isData) pt = new TPaveText(0.15,0.1,0.45,0.3,"NB NDC");
        else  pt = new TPaveText(0.15,0.1,0.45,0.3,"NB NDC");
    }

    pt->SetBorderSize(0);
    pt->SetFillStyle(0);
    pt->SetTextAlign(13);
    pt->SetTextFont(43);
    pt->SetTextSize(36);
    TText *text = new TText;
    if(isData) text = pt->AddText("ALICE Preliminary");
    text = pt->AddText("pp, #sqrt{#it{s}} = 13 TeV");
    text = pt->AddText(Form("charged jets, anti-#it{k}_{T}, #it{R} = %.1lf, |#it{#eta}_{lab}^{jet}| < %.1lf",fJetR,fJetEta));
    if(hType == fHistZ || hType == fHistDPt) text = pt->AddText(Form("%.0lf < #it{p}_{T, ch jet} < %.0lf GeV/#it{c}",jetptmin,jetptmax));
    int ptmin = 2;
    if(jetptmin == 15) ptmin = 3;
    if(hType!=fHistDPt) text = pt->AddText(Form ("with D^{0}, %d < #it{p}_{T,D^{0}} < %d GeV/#it{c}",ptmin,36));
    pt->Draw();

    FinalSpectrum->cd();
    FinalSpectrum_2->Draw();
    FinalSpectrum_2->cd();
    FinalSpectrum_2->Range(-1.986821e-07,-0.9209589,33.33333,2.49);
    FinalSpectrum_2->SetFillColor(0);
    FinalSpectrum_2->SetBorderMode(0);
    FinalSpectrum_2->SetBorderSize(4);
    FinalSpectrum_2->SetGridy();
    FinalSpectrum_2->SetTickx(1);
    FinalSpectrum_2->SetTicky(1);
    FinalSpectrum_2->SetLeftMargin(0.12);
    FinalSpectrum_2->SetTopMargin(0);
    FinalSpectrum_2->SetBottomMargin(0.25);
    FinalSpectrum_2->SetFrameBorderMode(0);
    FinalSpectrum_2->SetFrameBorderMode(0);

    double ratiomin, ratiomax;
    if(isData) {
        ratiomin = 0.12;
        if(jetptmin == 5) ratiomin = 0.12;
        ratiomax = 1.29;
        if(isProb) ratiomax = 1.89;
        if(hType==fHistJetPt) ratiomax = 1.39;

    }
    else {
        ratiomin = 0.62;
        ratiomax = 2.09;
        if(hType == fHistJetPt) {
            ratiomin = 0.82;
            ratiomax = 1.29;
        }
        if(hType == fHistDPt) {
            ratiomin = 0.82;
            ratiomax = 1.29;
        }
        if(hType==fHistDPt && jetptmin == 5) ratiomax = 1.92;

    }

    // central points data (values don't really needed)
    TH1D *CentralPointsStatisticalUncertainty__5 = new TH1D("CentralPointsStatisticalUncertainty__5","Central Values",fbinsJetFinalN, xAxis);
    CentralPointsStatisticalUncertainty__5->SetMinimum(ratiomin);
    CentralPointsStatisticalUncertainty__5->SetMaximum(ratiomax);
    CentralPointsStatisticalUncertainty__5->SetEntries(8);
    CentralPointsStatisticalUncertainty__5->SetDirectory(0);
    CentralPointsStatisticalUncertainty__5->SetStats(0);

    Int_t ci = TColor::GetColor("#990000");
    if(hType==fHistJetPt) {
        CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitle("p_{T}^{ch,jet} (GeV/c)");
    }
    else if(hType==fHistZ) {
        CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitle("z_{||}");
    }
    else if(hType==fHistDPt){
        CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitle("p_{T,D} (GeV/c)");
    }

    CentralPointsStatisticalUncertainty__5->GetXaxis()->SetLabelFont(43);
    CentralPointsStatisticalUncertainty__5->GetXaxis()->SetLabelSize(40);
    CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitleSize(40);
    CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitleOffset(3.2);
    CentralPointsStatisticalUncertainty__5->GetXaxis()->SetTitleFont(43);
    if(isData) CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitle("MC / data");
    else CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitle("MC ratio");
    CentralPointsStatisticalUncertainty__5->GetYaxis()->SetNdivisions(509);
    CentralPointsStatisticalUncertainty__5->GetYaxis()->SetLabelFont(43);
    CentralPointsStatisticalUncertainty__5->GetYaxis()->SetLabelSize(40);
    CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitleSize(40);
    CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitleOffset(2);
    CentralPointsStatisticalUncertainty__5->GetYaxis()->SetTitleFont(43);
    CentralPointsStatisticalUncertainty__5->GetZaxis()->SetLabelFont(42);
    CentralPointsStatisticalUncertainty__5->GetZaxis()->SetLabelSize(0.055);
    CentralPointsStatisticalUncertainty__5->GetZaxis()->SetTitleSize(0.055);
    CentralPointsStatisticalUncertainty__5->GetZaxis()->SetTitleFont(42);
    CentralPointsStatisticalUncertainty__5->Draw("axis");
    TH1D *CentralPointsStatisticalUncertainty__7 = new TH1D("CentralPointsStatisticalUncertainty__7","Central Values",fptbinsJetFinalN, xAxis);
    CentralPointsStatisticalUncertainty__7->Draw("sameaxig");


    leg->SetBorderSize(0);
    leg->SetTextFont(43);
    leg->SetTextSize(32);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);

}

void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, double Msize, Width_t Lwidth, Style_t Lstyle){
    hh->SetMarkerColor(color);
    hh->SetMarkerStyle(Mstyle);;
    hh->SetLineColor(color);
    hh->SetLineWidth(Lwidth);
    hh->SetMarkerSize(Msize);
    hh->SetLineStyle(Lstyle);
   // hh->SetName(name.c_str());
    hh->SetTitle("");

}

void SaveCanvas(TCanvas *c, TString name){
    c->SaveAs(Form("%s.png",name.Data()));
    c->SaveAs(Form("%s.pdf",name.Data()));
    //c->SaveAs(Form("%s.png",name.c_str()));
    //c->SaveAs(Form("%s.pdf",name.c_str()));
}
