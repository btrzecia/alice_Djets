//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------
#include <iostream>
#include "TObject.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TList.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "THnSparse.h"
#include "TDatabasePDG.h"
//#include "AliHFInvMassFitter.h"
/*
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldBinByBin.h"*/

enum DMesonSpecies {kD0toKpi=0, kDStarD0pi};

    // ========================== Prepare your config ============================================
    int           fSystem = 2;            //-----! 0: pp, 1: p-Pb, 2: Pb-Pb -- set up system
    TString       fSystemS = "Pb--Pb, #sqrt{#it{s_{NN}}} = 5.02 TeV";
    DMesonSpecies fDmesonSpecie = 0;
    TString       fDmesonDsc = "Dzero";
    TString       fDmesonS = "D^{0}";
    const double  fRpar = 0.3;           //-----! jet R parameter
    const int     Rpar = 3;
    const int     ND = 3;                //-----!  change these numbers based on how many D mesons you analyse in data !
    const int     NDMC = 1;              //-----!  change these numbers based on how many D mesons you analyse in MC !

    const double  sigma_in = 54.e9;       //-----! inelastic x-section in bars
    const double  BRDstar = 0.0257;
    const double  BRDzero = 0.0389;
    const int     APb = 208;
    const int	    Ncoll = 1; // !!! Check this
    const int     Taa = 18.8;
    double        fRaapTBins[] = { 1,2,3,4,5,6,7,8,10,12,16,24,36 };
    double        fRaaD0_010[] = { 0.936,0.835,0.592,0.376,0.235,0.211,0.212,0.210,0.220,0.218,0.321,0.373 };
    double        fRaaD0_3050[] = { 0.894,0.693,0.595,0.480,0.408,0.378,0.415,0.425,0.385,0.470,0.539,0.398 };

    //====== D pT bins ---- set up your D pT bins ======
    const int     fptbinsDN = 10;
    double        fptbinsDA[fptbinsDN+1] = { 3,4,5,6,7,8,10,12,16,24,36 };
    //====== jet pT bins ---- set up your jet pT bins ======
    const int     fptbinsJetTrueN = 9;
    double        fptbinsJetTrueA[fptbinsJetTrueN+1] = { 3,4,5,6,8,10,14,20,30,50 };
    const int     fptbinsJetMeasN = 11;
    double        fptbinsJetMeasA[fptbinsJetMeasN+1] = { -20,-10,-5,0,5,8,12,15,20,25,30,50 };
    const int     fptbinsJetFinalN = 7;
    double        fptbinsJetFinalA[fptbinsJetFinalN+1] = { 5,6,8,10,14,20,30,50 };
    //====== z range ---- set up your min, max z ======
    double        zmin = -2, zmax = 2.; // for D-jet pT spectrum

    //====== signal extraction config ======
    Bool_t        fUseRefl = 0;                      //-----! if to use reflections (for D0, you must have reflections files ready)
    Int_t         fbkgtype = 0;                      //-----! kExpo=0, kLin=1, kPol2=2, kNoBk=3, kPow=4, kPowEx=5
    Float_t       fsigmaSignal = 2;                  //-----! sigma for the signal region
    Float_t       fsigmaBkg[] = {-9,-4,4,9};         //-----! sigma for the SB region (both left and right side from the fit)
    Float_t       fDmass = 1.864, fDsigma = 0.010;   //-----! initial values for D mass and sigma
    Float_t       fDsigmafix[fptbinsDN] = {0.01,0.011,0.01175,0.0125,0.013,0.0135,0.0145,0.016,0.0175,0.0185};   //-----! initial values for D mass and sigma
    Float_t       minf = 1.71, maxf = 2.1;           //-----! min/mass of the inv. mass distributions
    Int_t         fRebinMass = 2;                    //-----! rebining of the inv. mass distributions

    Int_t fColors[] = {1,2,8,4,kOrange-1,6,kGray+1,kCyan+1,kMagenta+2,kGreen+3,kViolet+5,kYellow+2};
    Int_t fMarkers[] = {20,21,22,23,24,25,26,27,28,29,30,32,33,34};


    ///============== POWHEG simulations ============================
    //======= set up here names of your simulation files =======
    TString fRunB[] = {
      "AnalysisResults_FastSim_powheg+pythia6_beauty_150593961473"	//central
    };

/*
    TString fDescB[] = {
      "central",
      "muR=1,muF=0.5" ,
      "muR=0.5,muF=1",
      "muR=2,muF=1"
    };
*/
    TString fDescB[] = {
      "central"
,      "muR=1,muF=0.5"
,      "muR=0.5,muF=1"
,      "muR=2,muF=1"
,      "muR=1,muF=2"
,      "muR=0.5,muF=0.5"
,      "muR=2,muF=2"
,      "m_{b}=5"
,      "m_{b}=4.5"
,      "PDF 21200"
,      "PDF 10800"
,
      "Evt gen"
    };

    TString fRunC[] = {
      "AnalysisResults_FastSim_powheg+pythia6_charm_central"
    };
    TString fDescC[] = {
      "central"
,      "m_{c}=1.3"
,      "m_{c}=1.7"
,      "muR=2,muF=2"
,      "muR=1,muF=2"
,      "muR=2,muF=1"
,      "muR=0.5,muF=0.5"
,      "muR=1,muF=0.5"
,      "muR=0.5,muF=1"
    };
