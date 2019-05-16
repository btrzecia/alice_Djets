    const int     fptbinsDN = sizeof(fptbinsDA)/sizeof(fptbinsDA[0])-1;

    //====== jet pT bins --- set up your jet pT bins ======
    //const int 	  fptbinsJetN = 3;
    //double	  fptbinsJetA[fptbinsJetN+1] = {5.0, 7.0, 10.0, 16.0, 36.0, 50.0};
    //double	  fptbinsJetA[fptbinsJetN+1] = {5.0, 7.0, 10.0, 15.0, 36.0, 50.0};
    //double	  fptbinsJetA[] = {5.0, 15.0, 30.0};
    double	  fptbinsJetA[] = {5.0, 7.0, 10.0, 15.0, 36.0, 5.0, 15.0, 30.0, 15.0, 50.0, 10.0, 16.0, 36.0};
    
    const int     fptbinsJetN = sizeof(fptbinsJetA)/sizeof(fptbinsJetA[0])-1;

//---------new suggested binning end
//---------//---------//---------//---------//---------//---------
    //====== z bins ---- set up your z (momentum fraction) bins ======
    const int     fptbinsZTrueN = 9;
    double        fptbinsZTrueA[fptbinsZTrueN+1] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    const int     fptbinsZMeasN = 9;
    double        fptbinsZMeasA[fptbinsZMeasN+1] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    const int     fptbinsZFinalN = 9;
    double        fptbinsZFinalA[fptbinsZFinalN+1] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    //====== z range ---- set up your min, max z ======
    double        zmin = -2., zmax = 2.; // for D-jet pT spectrum

    //====== signal extraction config ======
    Bool_t        fUseRefl = 1;                      //-----! if to use reflections (for D0, you must have reflections files ready)
    Int_t         fbkgtype = 0;                      //-----! kExpo=0, kLin=1, kPol2=2, kNoBk=3, kPow=4, kPowEx=5
    Float_t       fsigmaSignal = 2;                  //-----! sigma for the signal region
    Float_t       fsigmaBkg[] = {-9,-4,4,9};         //-----! sigma for the SB region (both left and right side from the fit)
    Float_t       fDmass = 1.864, fDsigma = 0.010;   //-----! initial values for D mass and sigma
    //Float_t       fDsigmafix[fptbinsDN] = {0.01,0.011,0.01175,0.0125,0.013,0.0135,0.0145,0.016,0.0175,0.0185};   //-----! initial values for D mass and sigma
    double       minf = 1.71, maxf = 2.1;           //-----! min/mass of the inv. mass distributions
    Int_t         fRebinMass = 4;                    //-----! rebining of the inv. mass distributions

    Int_t fColors[] = {1,2,8,4,kOrange-1,6,kGray+1,kCyan+1,kMagenta+2,kGreen+3,kViolet+5,kYellow+2};
    Int_t fMarkers[] = {20,21,22,23,24,25,26,27,28,29,30,32,33,34};


    ///============== POWHEG simulations ============================
    //======= set up here names of your simulation files =======
/*
    TString fRunB[] = {
      "AnalysisResults_FastSim_powheg+pythia6_beauty_150593961473",
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504284947",
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504259653",
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1506803374"
    };
*/
    TString fRunB[] = {
      "AnalysisResults_FastSim_powheg+pythia6_beauty_150593961473"	//central
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504284947"	//R1F05
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504259653"	//R05F1
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1506803374"	//R2F1
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504296768"	//R1F2
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504212024"	//R05F05
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504318569"	//R2F2
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504202511"	//mb5
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504197966"	//mb4.5
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504197460"	//pdf 21200
,      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504199953"	//pdf 10800
,
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1536649348"//evtgen
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
//      "RAW_CHARM_POWHEG"
      "AnalysisResults_FastSim_powheg+pythia6_charm_central"
,"AnalysisResults_FastSim_powheg+pythia6_charm_m13_1536595965"
,"AnalysisResults_FastSim_powheg+pythia6_charm_m17_1536655729"
,"AnalysisResults_FastSim_powheg+pythia6_charm_F2R2_1535895146"
,"AnalysisResults_FastSim_powheg+pythia6_charm_F1R2_1536594271"
,"AnalysisResults_FastSim_powheg+pythia6_charm_F2R1_1535916012"
,"AnalysisResults_FastSim_powheg+pythia6_charm_F05R05_1535894261"
,"AnalysisResults_FastSim_powheg+pythia6_charm_F1R05_1536598175"
,"AnalysisResults_FastSim_powheg+pythia6_charm_F05R1_1536604800"
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
const Int_t fBsimN = 12;
const Int_t fCsimN = 9;
TString OUTDIRECTORY="/home/jackbauer/Work/alice/analysis/pp5TeV/D0jet/results_cutTight/DzeroR03_def_437_old0";
