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
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TF1.h"
#include "THnSparse.h"
#include "TDatabasePDG.h"
#include "TNtuple.h"

//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------

    const int Rpar = 4;
    const double fRpar = 0.4;

    enum DMesonSpecies {kD0toKpi=0, kDStarD0pi};
    DMesonSpecies fDmesonSpecie = 1;
    TString fDmesonDsc="Dstar";
    TString fDmesonS="D*";

    const int ND = 4; // number of D mesons to analyse in data
    const int NDMC = 2; // number of D mesons to analyse in MC

    //------- D pT bins
    //const int fptbinsDN = 12;
    //double fptbinsDA[fptbinsDN+1] = { 1,2,3,4,5,6,7,8,10,12,16,24,36 };
    //const int fptbinsDN = 11;
    //double fptbinsDA[fptbinsDN+1] = { 2,3,4,5,6,7,8,10,12,16,24,36 };
    const int fptbinsDN = 10;
    double fptbinsDA[fptbinsDN+1] = { 3,4,5,6,7,8,10,12,16,24,36 };

    //------- jet pT bins
    // pp bins
    const int fptbinsJetMeasN = 8;
    double fptbinsJetMeasA[fptbinsJetMeasN+1] = { 3,4,5,6,8,10,14,20,30 };
    const int fptbinsJetTrueN = 8;
    double fptbinsJetTrueA[fptbinsJetMeasN+1] = { 3,4,5,6,8,10,14,20,30 };
  /*  const int fptbinsJetMeasN = 10;
    double fptbinsJetMeasA[fptbinsJetMeasN+1] = { 2,3,4,5,6,8,10,14,20,30,50 };
    const int fptbinsJetTrueN = 7;
    double fptbinsJetTrueA[fptbinsJetMeasN+1] = { 4,5,6,8,10,14,20,30 };*/

    double zmin = -2, zmax = 2.;

    // THnSparse axis
    Int_t   fzMeasAxis = 0;
    Int_t   fJetPtMeasAxis = 1;

    //------- signal extraction config
    Bool_t  fUseRefl = 0; // no reflections for Dstar
    Int_t   fbkgtype = 5; // kExpo=0, kLin=1, kPol2=2, kNoBk=3, kPow=4, kPowEx=5
    Float_t fsigmaSignal = 3;
    Float_t fsigmaBkg[] = {-8,-5,5,13};
    Float_t fDmass = 0.1455, fDsigma = 0.00065;
    Float_t minf = 0.140, maxf = 0.155;
    Int_t   fRebinMass = 1;

    Int_t fColors[] = {1,2,8,4,kOrange-1,6,kGray+1,kCyan+1,kMagenta+2,kGreen+3,kViolet+5,kYellow+2};
    Int_t fMarkers[] = {20,21,22,23,24,25,26,27,28,29,30,32,33,34};

    //------- POWHEG simulations
    // old simulations
    const Int_t fBsimN = 9;
    TString fRunB[] = {
    "AnalysisResults_FastSim_powheg_beauty_1496995621",
    "AnalysisResults_FastSim_powheg_beauty_1497227464",
    "AnalysisResults_FastSim_powheg_beauty_1497228262",
    "AnalysisResults_FastSim_powheg_beauty_1497207414",
    "AnalysisResults_FastSim_powheg_beauty_1497173624",
    "AnalysisResults_FastSim_powheg_beauty_1497172782",
    "AnalysisResults_FastSim_powheg_beauty_1497132057",
    "AnalysisResults_FastSim_powheg_beauty_1497130041",
    "AnalysisResults_FastSim_powheg_beauty_1497121734"};

     // New simulations, with R=0.3,0.4
    /*const Int_t fBsimN = 10;
    TString fRunB[] = {
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519042424",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519293768",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519292550",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519275692",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519241992",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519127831",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519049991",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519079649",
    "AnalysisResults_FastSim_powheg+pythia6_beauty_1519124826",
    "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1520259013"};*/

    TString fDescB[] = {
    "central",
    "m_{b}=4.5",
    "m_{b}=5",
    "muR=2,muF=2",
    "muR=1,muF=2",
    "muR=2,muF=1",
    "muR=0.5,muF=0.5",
    "muR=1,muF=0.5" ,
    "muR=0.5,muF=1",
    "EvtGen" };

    const Int_t fCsimN = 9;
    TString fRunC[] = {
    "1496999831",
    "1497979153",
    "1497983041",
    "1497891573",
    "1497817179",
    "1497857391",
    "1497726041",
    "1497699194",
    "1497713588" };
    TString fDescC[] = {
    "central",
    "m_{c}=1.3",
    "m_{c}=1.7",
    "muR=2,muF=2",
    "muR=1,muF=2",
    "muR=2,muF=1",
    "muR=0.5,muF=0.5",
    "muR=1,muF=0.5",
    "muR=0.5,muF=1"};
