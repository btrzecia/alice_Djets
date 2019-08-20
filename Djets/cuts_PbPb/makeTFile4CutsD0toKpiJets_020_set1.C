#include <Riostream.h>
#include <TFile.h>
#include "AliRDHFCutsD0toKpi.h"
#include <TClonesArray.h>
#include <TParameter.h>


//Use:
//Set hard coded commentet with //set this!!
// gSystem->SetIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/ITS -I$ALICE_ROOT/TPC -I$ALICE_ROOT/CONTAINERS -I$ALICE_ROOT/STEER -I$ALICE_ROOT/TRD -I$ALICE_ROOT/macros -I$ALICE_ROOT/ANALYSIS -I$ALICE_ROOT/PWG3 -I$ALICE_ROOT/PWG3/vertexingHF -g");
//gSystem->SetIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/ITS -I$ALICE_ROOT/TPC -I$ALICE_ROOT/CONTAINERS -I$ALICE_ROOT/STEER -I$ALICE_ROOT/TRD -I$ALICE_ROOT/macros -I$ALICE_ROOT/ANALYSIS -I$ALICE_PHYSICS/PWGHF -I$ALICE_PHYSICS/PWGHF/vertexingHF -g");

// root[] .L makeInputD0tasks.C++
// root[] makeInputAliAnalysisTaskSED0Mass()
// root[] makeInputAliAnalysisTaskSESignificanceMaximization()
//similar macros for the other D mesons

//Author: Chiara Bianchin, cbianchi@pd.infn.it


//macro to make a .root file which contains an AliRDHFCutsD0toKpi for AliAnalysisTaskSED0Mass task

void makeInputAliAnalysisTaskSED0Mass(Float_t minc=0., Float_t maxc=20.){

  AliRDHFCutsD0toKpi* RDHFD0toKpi=new AliRDHFCutsD0toKpi();
  RDHFD0toKpi->SetName("D0toKpiCuts");
  RDHFD0toKpi->SetTitle("Cuts for D0 analysis");

  AliESDtrackCuts* esdTrackCuts=new AliESDtrackCuts();
  esdTrackCuts->SetRequireSigmaToVertex(kFALSE);
  //default
  esdTrackCuts->SetRequireTPCRefit(kTRUE);
  esdTrackCuts->SetRequireITSRefit(kTRUE);
  esdTrackCuts->SetEtaRange(-0.8,0.8);
  //esdTrackCuts->SetMinNClustersITS(4); // default is 5
  //esdTrackCuts->SetMinNClustersTPC(90);
  esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
					 AliESDtrackCuts::kAny); 
 // default is kBoth, otherwise kAny
  esdTrackCuts->SetMinDCAToVertexXY(0.);
  esdTrackCuts->SetPtRange(0.5,1.e10);
  esdTrackCuts->SetMaxDCAToVertexXY(1.);
  esdTrackCuts->SetMaxDCAToVertexZ(1.);
  esdTrackCuts->SetMinDCAToVertexXYPtDep("0.005*TMath::Max(0.,(1-TMath::Floor(TMath::Abs(pt)/2.)))");
 
  //esdTrackCuts->SetMinNCrossedRowsTPC(80);
  //esdTrackCuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.9);
  //esdTrackCuts->SetTPCNclsF(80);

  RDHFD0toKpi->AddTrackCuts(esdTrackCuts);
  RDHFD0toKpi->SetSelectCandTrackSPDFirst(kTRUE, 5);

  RDHFD0toKpi->SetTriggerClass("");
  //RDHFD0toKpi->ResetMaskAndEnableSemiCentralTrigger();
  //RDHFD0toKpi->EnableCentralTrigger();
  //RDHFD0toKpi->EnableMBTrigger();
  //RDHFD0toKpi->ResetMaskAndEnableMBTrigger();

  //RDHFD0toKpi->SetTriggerClass("CINT7");
  RDHFD0toKpi->SetTriggerMask(AliVEvent::kINT7);

  const Int_t nvars=11;

  const Int_t nptbins=32;
  Float_t* ptbins;
  ptbins=new Float_t[nptbins+1];
  ptbins[0]=0.;
  ptbins[1]=0.5;
  ptbins[2]=1.;
  ptbins[3]=1.5;
  ptbins[4]=2.;
  ptbins[5]=2.25;
  ptbins[6]=2.5;
  ptbins[7]=2.75;
  ptbins[8]=3.;
  ptbins[9]=3.25;
  ptbins[10]=3.5;
  ptbins[11]=3.75;
  ptbins[12]=4.;
  ptbins[13]=4.5;
  ptbins[14]=5.;
  ptbins[15]=5.5;
  ptbins[16]=6.;
  ptbins[17]=6.5;
  ptbins[18]=7.;
  ptbins[19]=7.5;
  ptbins[20]=8.;
  ptbins[21]=8.5;
  ptbins[22]=9.;
  ptbins[23]=10.;
  ptbins[24]=12.;
  ptbins[25]=14.;
  ptbins[26]=16.;
  ptbins[27]=20.;
  ptbins[28]=24.;
  ptbins[29]=36.;
  ptbins[30]=50.;
  ptbins[31]=100.;
  ptbins[32]=9999.;

  RDHFD0toKpi->SetPtBins(nptbins+1,ptbins);
  
  //setting my cut values
    //cuts order
    //       printf("    |M-MD0| [GeV]    < %f\n",fD0toKpiCuts[0]);
    //     printf("    dca    [cm]  < %f\n",fD0toKpiCuts[1]);
    //     printf("    cosThetaStar     < %f\n",fD0toKpiCuts[2]);
    //     printf("    pTK     [GeV/c]    > %f\n",fD0toKpiCuts[3]);
    //     printf("    pTpi    [GeV/c]    > %f\n",fD0toKpiCuts[4]);
    //     printf("    |d0K|  [cm]  < %f\n",fD0toKpiCuts[5]);
    //     printf("    |d0pi| [cm]  < %f\n",fD0toKpiCuts[6]);
    //     printf("    d0d0  [cm^2] < %f\n",fD0toKpiCuts[7]);
    //     printf("    cosThetaPoint    > %f\n",fD0toKpiCuts[8]);
    //     printf("    |cosThetaPointXY| < %f\n",fD0toKpiCuts[9]);
    //     printf("    NormDecayLenghtXY    > %f\n",fD0toKpiCuts[10]);

    const Int_t nvary=3;
    Float_t varyd0xd0[nptbins][nvary]=
					   {{-35000.*1E-8,-50000.*1E-8,-15000.*1E-8},/* pt<0.5*/
				       {-35000.*1E-8,-50000.*1E-8,-15000.*1E-8},/* 0.5<pt<1*/
				       {-30000.*1E-8,-43000.*1E-8,-30000.*1E-8},/* 1<pt<1.5 */
				       {-30000.*1E-8,-43000.*1E-8,-30000.*1E-8},/* 1.5<pt<2 */
				       {-30000.*1E-8,-43000.*1E-8,-36000.*1E-8},/* 2<pt<2.25 */
				       {-28000.*1E-8,-43000.*1E-8,-36000.*1E-8},/* 2.25<pt<2.5 */
				       {-28000.*1E-8,-43000.*1E-8,-36000.*1E-8},/* 2.5<pt<2.75 */
				       {-28000.*1E-8,-43000.*1E-8,-36000.*1E-8},/* 2.75<pt<3 */
				       {-28000.*1E-8,-34000.*1E-8,-36000.*1E-8},/* 3<pt<3.25 */
				       {-25000.*1E-8,-34000.*1E-8,-36000.*1E-8},/* 3.25<pt<3.5 */
				       {-25000.*1E-8,-34000.*1E-8,-36000.*1E-8},/* 3.5<pt<3.75 */
				       {-25000.*1E-8,-34000.*1E-8,-36000.*1E-8},/* 3.75<pt<4 */
				       {-12000.*1E-8,-25000.*1E-8,-36000.*1E-8},/* 4<pt<4.5 */
				       {-12000.*1E-8,-25000.*1E-8,-36000.*1E-8},/* 4.5<pt<5 */
				       {-5000.*1E-8,-20000.*1E-8,-23000.*1E-8},/* 5<pt<5.5 */
				       {-5000.*1E-8,-20000.*1E-8,-23000.*1E-8},/* 5.5<pt<6 */
				       {-5000.*1E-8,-10000.*1E-8,-23000.*1E-8},/* 6<pt<6.5 */
				       {-5000.*1E-8,-14000.*1E-8,-23000.*1E-8},/* 6.5<pt<7 */
				       {-5000.*1E-8,-14000.*1E-8,-23000.*1E-8},/* 7<pt<7.5 */
				       {-5000.*1E-8,-14000.*1E-8,-23000.*1E-8},/* 7.5<pt<8 */
				       {-0.*1E-8,-7000.*1E-8,-10000.*1E-8},/* 8<pt<8.5 */
				       {-0.*1E-8,-7000.*1E-8,-10000.*1E-8},/* 8.5<pt<9*/
				       {-0.*1E-8,-7000.*1E-8,-10000.*1E-8},/* 9<pt<10*/
				       {-0.*1E-8,-5000.*1E-8,-10000.*1E-8},/* 10<pt<12 */
				       {0.*1E-8,-1000.*1E-8,-10000.*1E-8},/* 12<pt<14 */
				       {0.*1E-8,-1000.*1E-8,-10000.*1E-8},/* 14<pt<16 */
				       {0.*1E-8,-1000.*1E-8,-10000.*1E-8},/* 16<pt<20 */
				       {5000.*1E-8,-1000.*1E-8,-5000.*1E-8},/* 20<pt<24 */
				       {5000.*1E-8,-1000.*1E-8,0.*1E-8},/* 24<pt<36 */
				       {5000.*1E-8,-1000.*1E-8,0.*1E-8},/* 36<pt<50 */
				       {5000.*1E-8,-1000.*1E-8,0.*1E-8},/* 50<pt<100 */
				       {5000.*1E-8,-1000.*1E-8,0.*1E-8}};/* pt>100 */
       

    Float_t varyCosPoint[nptbins][nvary]=
					  {{0.75,0.95,0.80},	/* 0<pt<0.5 */
					  {0.75,0.95,0.9},	/* 0.5<pt<1*/
					  {0.75,0.95,0.9},	/* 1<pt<1.5 */
					  {0.75,0.95,0.9},	/* 1.5<pt<2 */
					  {0.92,0.95,0.92},	/* 2<pt<2.25 */
					  {0.92,0.95,0.92},	/* 2.25<pt<2.5 */
					  {0.92,0.95,0.92},	/* 2.5<pt<2.75 */
					  {0.92,0.95,0.92},	/* 2.75<pt<3 */
					  {0.92,0.95,0.92},	/* 3<pt<3.25 */
					  {0.90,0.95,0.92},	/* 3.25<pt<3.5 */
					  {0.90,0.95,0.92},	/* 3.5<pt<3.75 */
					  {0.90,0.95,0.92},	/* 3.75<pt<4 */
					  {0.85,0.95,0.92},	/* 4<pt<4.5 */
					  {0.85,0.95,0.92},	/* 4.5<pt<5 */
					  {0.88,0.92,0.90},	/* 5<pt<5.5 */
					  {0.88,0.92,0.90},	/* 5.5<pt<6 */
					  {0.83,0.9,0.90},	/* 6<pt<6.5 */
					  {0.83,0.9,0.90},	/* 6.5<pt<7 */
					  {0.83,0.88,0.90},	/* 7<pt<7.5 */
					  {0.83,0.88,0.90},	/* 7.5<pt<8 */
					  {0.8,0.86,0.90},/* 8<pt<8.5 */
					  {0.8,0.86,0.90},/* 8.5<pt<9 */
					  {0.8,0.86,0.90},/* 9<pt<10 */
					  {0.8,0.85,0.90},/* 10<pt<12 */
					  {0.78,0.83,0.90},/* 12<pt<14 */
					  {0.78,0.83,0.90},/* 14<pt<16 */
					  {0.78,0.82,0.90},/* 16<pt<20 */
					  {0.76,0.82,0.90},/* 20<pt<24 */
					  {0.75,0.8,0.90},/*24<pt<36 */
					  {0.75,0.8,0.90},/* 36<pt<50 */
					  {0.75,0.8,0.90},/* 50<pt<100 */
					  {0.75,0.8,0.90}};/* pt>100 */


    Float_t d0MeasMinusExpCut[nptbins]={1.,/* 0<pt<0.5*/
					1.,/* 0.5<pt<1*/
					1.,/* 1<pt<1.5*/
					1.,/* 1.5<pt<2*/
					1.,/*2<pt<2.25*/
					1.,/*2.25<pt<2.5*/
					1.,/*2.5<pt<2.75*/
					1.,/*2.75<pt<3*/
					1.,/*3<pt<3.25*/
					1.,/*3.25<pt<3.5*/
					1.,/*3.5<pt<3.75*/
					1.,/*3.75<pt<4*/
					1.5,/*4<pt<4.5*/
					1.5,/*4.5<pt<5*/
					1.5,/*5<pt<5.5*/
					1.5,/*5.5<pt<6*/
					1.5,/*6<pt<6.5*/
					1.5,/*6.5<pt<7*/
					1.5,/*7<pt<7.5*/
					1.5,/*7.5<pt<8*/
					1.5,/*8<pt<8.5*/
					1.5,/*8.5<pt<9*/
					1.5,/*9<pt<10*/
					1.5,/*10<pt<12*/
					2.,/*12<pt<14*/
					2.,/*14<pt<16*/
					2.,/*16<pt<20*/
					2.,/*20<pt<24*/
					2.,/*24<pt<36*/
					2.,/*36<pt<50*/
					2.,/*50<pt<100*/
					2.,/*>100*/};

    
    Int_t varycuts = 22;
    //Int_t vcd0xd0=varycuts/10;
    Int_t vcd0xd0=1;
    //Int_t vccospoint=varycuts%10;
    Int_t vccospoint=1;
    Float_t cutsMatrixD0toKpiStand[nptbins][nvars]=
							{{0.400,400.*1E-4,0.6,0.5,0.5,1000.*1E-4,1000.*1E-4,varyd0xd0[0][vcd0xd0],varyCosPoint[0][vccospoint],0.999,10.},/* 0<pt<0.5*/
						    {0.400,400.*1E-4,0.6,0.5,0.5,1000.*1E-4,1000.*1E-4,varyd0xd0[1][vcd0xd0],varyCosPoint[1][vccospoint],0.999,10.},/* 0.5<pt<1*/
						    {0.400,400.*1E-4,0.8,0.5,0.5,1000.*1E-4,1000.*1E-4,varyd0xd0[2][vcd0xd0],varyCosPoint[2][vccospoint],0.998,8.},/* 1<pt<1.5 */
						    {0.400,400.*1E-4,0.8,0.5,0.5,1000.*1E-4,1000.*1E-4,varyd0xd0[3][vcd0xd0],varyCosPoint[3][vccospoint],0.998,8.},/* 1.5<pt<2 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[4][vcd0xd0],varyCosPoint[4][vccospoint],0.998,8.},/* 2<pt<2.25 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[5][vcd0xd0],varyCosPoint[5][vccospoint],0.998,8.},/* 2.25<pt<2.5 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[6][vcd0xd0],varyCosPoint[6][vccospoint],0.998,7.},/* 2.5<pt<2.75 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[7][vcd0xd0],varyCosPoint[7][vccospoint],0.998,7.},/* 2.75<pt<3 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[8][vcd0xd0],varyCosPoint[8][vccospoint],0.998,5.},/* 3<pt<3.25 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[9][vcd0xd0],varyCosPoint[9][vccospoint],0.998,5.},/* 3.25<pt<3.5 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[10][vcd0xd0],varyCosPoint[10][vccospoint],0.998,5.},/* 3.5<pt<3.75 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[11][vcd0xd0],varyCosPoint[11][vccospoint],0.998,5.},/* 3.75<pt<4 */
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[12][vcd0xd0],varyCosPoint[12][vccospoint],0.998,5.},/* 4<pt<4.5*/
						    {0.400,300.*1E-4,0.8,0.6,0.6,1000.*1E-4,1000.*1E-4,varyd0xd0[13][vcd0xd0],varyCosPoint[13][vccospoint],0.998,5.},/* 4.5<pt<5*/
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[14][vcd0xd0],varyCosPoint[14][vccospoint],0.998,5.},/* 5<pt<5.5 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[15][vcd0xd0],varyCosPoint[15][vccospoint],0.998,5.},/* 5.5<pt<6 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[16][vcd0xd0],varyCosPoint[16][vccospoint],0.998,5.},/* 6<pt<6.5 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[17][vcd0xd0],varyCosPoint[17][vccospoint],0.998,5.},/* 6.5<pt<7 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[18][vcd0xd0],varyCosPoint[18][vccospoint],0.998,5.},/* 7<pt<7.5 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[19][vcd0xd0],varyCosPoint[19][vccospoint],0.998,5.},/* 7.5<pt<8 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[20][vcd0xd0],varyCosPoint[20][vccospoint],0.998,5.},/* 8<pt<8.5 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[21][vcd0xd0],varyCosPoint[21][vccospoint],0.998,5.},/* 8.5<pt<9 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[22][vcd0xd0],varyCosPoint[22][vccospoint],0.998,5.},/* 9<pt<10 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[23][vcd0xd0],varyCosPoint[23][vccospoint],0.998,5.},/* 10<pt<12 */
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[24][vcd0xd0],varyCosPoint[24][vccospoint],0.998,5.},/*12< pt <14*/
						    {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[25][vcd0xd0],varyCosPoint[25][vccospoint],0.998,5.},/*14< pt <16*/
						    {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[26][vcd0xd0],varyCosPoint[26][vccospoint],0.995,5.},/*16< pt <20*/
						    {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[27][vcd0xd0],varyCosPoint[27][vccospoint],0.995,5.},/*20< pt <24*/
						    {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[28][vcd0xd0],varyCosPoint[28][vccospoint],0.995,5.},/*24< pt <36*/
						    {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[29][vcd0xd0],varyCosPoint[29][vccospoint],0.995,5.},/*36< pt <50*/
						    {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[30][vcd0xd0],varyCosPoint[30][vccospoint],0.995,5.},/*50< pt <100*/
						    {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,varyd0xd0[31][vcd0xd0],varyCosPoint[31][vccospoint],0.995,5.}};/* pt > 100*/

	
	Float_t **cutsMatrixTransposeStand=new Float_t*[nvars];
	for(Int_t iv=0;iv<nvars;iv++)cutsMatrixTransposeStand[iv]=new Float_t[nptbins];
	
	for (Int_t ibin=0;ibin<nptbins;ibin++){
		for (Int_t ivar = 0; ivar<nvars; ivar++){
			cutsMatrixTransposeStand[ivar][ibin]=cutsMatrixD0toKpiStand[ibin][ivar];
			//printf("cutsMatrixD0toKpi[%d][%d] = %f\n",ibin, ivar,cutsMatrixD0toKpiStand[ibin][ivar]);
		}
	}
	
 
  RDHFD0toKpi->SetCuts(nvars,nptbins,cutsMatrixTransposeStand);
  RDHFD0toKpi->Setd0MeasMinusExpCut(nptbins,d0MeasMinusExpCut);

  Bool_t pidflag=kTRUE;
  RDHFD0toKpi->SetUsePID(pidflag);
  if(pidflag) cout<<"PID is used"<<endl;
  else cout<<"PID is not used"<<endl;

    //pid settings
  AliAODPidHF* pidObj=new AliAODPidHF();
  //pidObj->SetName("pid4D0");
  Int_t mode=1;
  const Int_t nlims=2;
  Double_t plims[nlims]={0.6,0.8}; //TPC limits in momentum [GeV/c]
  Bool_t compat=kTRUE; //effective only for this mode
  Bool_t asym=kTRUE;
  Double_t sigmas[5]={2.,1.,0.,3.,0.}; //to be checked and to be modified with new implementation of setters by Rossella
  pidObj->SetAsym(asym);// if you want to use the asymmetric bands in TPC
  pidObj->SetMatch(mode);
  pidObj->SetPLimit(plims,nlims);
  pidObj->SetSigma(sigmas);
  pidObj->SetCompat(compat);
  pidObj->SetTPC(kTRUE);
  pidObj->SetTOF(kTRUE);

  pidObj->SetPCompatTOF(2.);
  pidObj->SetSigmaForTPCCompat(3.);
  pidObj->SetSigmaForTOFCompat(3.);

  pidObj->SetOldPid(kFALSE);
	
  RDHFD0toKpi->SetLowPt(kFALSE);
  RDHFD0toKpi->SetUseDefaultPID(kFALSE); //to use the AliAODPidHF
	
  //RDHFD0toKpi->SetMaximumPforPID(4.);
  RDHFD0toKpi->SetPidHF(pidObj);
	
  //activate pileup rejection (for pp)
  RDHFD0toKpi->SetOptPileup(AliRDHFCuts::kNoPileupSelection);
		
  RDHFD0toKpi->SetUseSpecialCuts(kFALSE);
  //Do not recalculate the vertex
  RDHFD0toKpi->SetRemoveDaughtersFromPrim(kFALSE); //activate for pp
	
  //RDHFD0toKpi->SetUseAOD049(kTRUE);
  RDHFD0toKpi->SetMaxVtxZ(10.);

  //RDHFD0toKpi->SetRemoveTrackletOutliers(kTRUE);
  RDHFD0toKpi->SetCutOnzVertexSPD(2);

  TString cent="";
  //centrality selection (Pb-Pb)
  
  RDHFD0toKpi->SetMinCentrality(minc);
  RDHFD0toKpi->SetMaxCentrality(maxc);
  cent=Form("%.0f%.0f",minc,maxc);
  RDHFD0toKpi->SetUseCentrality(AliRDHFCuts::kCentV0M); //kCentOff,kCentV0M,kCentTRK,kCentTKL,kCentCL1,kCentInvalid

//  RDHFD0toKpi->SetMaxDifferenceTRKV0Centraltity(5.);

  //temporary
  //RDHFD0toKpi->SetFixRefs();
	
  //RDHFD0toKpi->SetSelectCandTrackSPDFirst(kTRUE, 3.);

  cout<<"This is the odject I'm going to save:"<<endl;
  RDHFD0toKpi->PrintAll();
  //TFile* fout=new TFile(Form("cenrtalnewD0toKpi2011RefCuts%s%s%sRecVtx%sPileupRej_cent.root", RDHFD0toKpi->GetUseCentrality()==0 ? "pp" : "PbPb",cent.Data(),RDHFD0toKpi->GetIsPrimaryWithoutDaughters() ? "" : "No",RDHFD0toKpi->GetOptPileUp() ? "" : "No"),"recreate");   //set this!!
  TFile* fout=new TFile("D0toKpiCuts_PbPb2015_0_20_set1.root","recreate");   //set this!!

  fout->cd();
  RDHFD0toKpi->Write();
  fout->Close();

}
 
//macro to make a .root file (for significance maximization) which contains an AliRDHFCutsD0toKpi with loose set of cuts  and TParameter with the tighest value of these cuts

void makeInputAliAnalysisTaskSESignificanceMaximization(){

  AliRDHFCutsD0toKpi* RDHFD0toKpi=new AliRDHFCutsD0toKpi();
  RDHFD0toKpi->SetName("loosercuts");
  RDHFD0toKpi->SetTitle("Cuts for significance maximization");

  AliESDtrackCuts* esdTrackCuts=new AliESDtrackCuts();
  esdTrackCuts->SetRequireSigmaToVertex(kFALSE);
  //default
  esdTrackCuts->SetRequireTPCRefit(kTRUE);
  esdTrackCuts->SetRequireITSRefit(kTRUE);
  //esdTrackCuts->SetMinNClustersITS(4);
  //esdTrackCuts->SetMinNClustersTPC(120);

  esdTrackCuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
  esdTrackCuts->SetMinDCAToVertexXY(0.);
  esdTrackCuts->SetEtaRange(-0.8,0.8);
  esdTrackCuts->SetPtRange(0.8,1.e10);
  
  RDHFD0toKpi->AddTrackCuts(esdTrackCuts);

  const Int_t nvars=11;

  const Int_t nptbins=14; //change this when adding pt bins!
  Float_t ptbins[nptbins+1];
  ptbins[0]=0.;
  ptbins[1]=0.5;
  ptbins[2]=1.;
  ptbins[3]=2.;
  ptbins[4]=3.;
  ptbins[5]=4.;
  ptbins[6]=5.;
  ptbins[7]=6.;
  ptbins[8]=8.;
  ptbins[9]=10.;
  ptbins[10]=12.;
  ptbins[11]=16.;
  ptbins[12]=20.;
  ptbins[13]=24.;
  ptbins[14]=9999.;

  RDHFD0toKpi->SetPtBins(nptbins+1,ptbins);

  Float_t** rdcutsvalmine;
  rdcutsvalmine=new Float_t*[nvars];
  for(Int_t iv=0;iv<nvars;iv++){
    rdcutsvalmine[iv]=new Float_t[nptbins];
  }

  //setting my cut values
    //cuts order
    //       printf("    |M-MD0| [GeV]    < %f\n",fD0toKpiCuts[0]);
    //     printf("    dca    [cm]  < %f\n",fD0toKpiCuts[1]);
    //     printf("    cosThetaStar     < %f\n",fD0toKpiCuts[2]);
    //     printf("    pTK     [GeV/c]    > %f\n",fD0toKpiCuts[3]);
    //     printf("    pTpi    [GeV/c]    > %f\n",fD0toKpiCuts[4]);
    //     printf("    |d0K|  [cm]  < %f\n",fD0toKpiCuts[5]);
    //     printf("    |d0pi| [cm]  < %f\n",fD0toKpiCuts[6]);
    //     printf("    d0d0  [cm^2] < %f\n",fD0toKpiCuts[7]);
    //     printf("    cosThetaPoint    > %f\n",fD0toKpiCuts[8]);
    //     printf("    |cosThetaPointXY| < %f\n",fD0toKpiCuts[9]);
    //     printf("    NormDecayLenghtXY    > %f\n",fD0toKpiCuts[10]);


    Float_t cutsMatrixD0toKpiStand[nptbins][nvars]={{0.400,400.*1E-4,0.8,0.3,0.3,1000.*1E-4,1000.*1E-4,-40000.*1E-8,0.75,0.,2.},/* pt<0.5*/
						  {0.400,400.*1E-4,0.8,0.3,0.3,1000.*1E-4,1000.*1E-4,-40000.*1E-8,0.75,0.,2.},/* 0.5<pt<1*/
						  {0.400,400.*1E-4,0.8,0.4,0.4,1000.*1E-4,1000.*1E-4,-33000.*1E-8,0.75,0.,2.},/* 1<pt<2 */
						  {0.400,250.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-30000.*1E-8,0.85,0.994,2.},/* 2<pt<3 */
						  {0.400,250.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-26000.*1E-8,0.85,0.994,2.},/* 3<pt<4 */
						  {0.400,250.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-15000.*1E-8,0.85,0.994,2.},/* 4<pt<5 */
						  {0.400,250.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-11000.*1E-8,0.82,0.994,2.},/* 5<pt<6 */
						  {0.400,270.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-10000.*1E-8,0.78,0.994,2.},/* 6<pt<8 */
						  {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-1000.*1E-8,0.7,0.994,2.},/* 8<pt<10 */
						  {0.400,300.*1E-4,0.8,0.7,0.7,1000.*1E-4,1000.*1E-4,-1000.*1E-8,0.7,0.994,2.},/* 10<pt<12 */
						  {0.400,350.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,-500.*1E-8,0.7,0.994,2.},/* 12<pt<16 */
						  {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,-500.*1E-8,0.7,0.994,2.},/* 16<pt<20 */
						  {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,-500.*1E-8,0.7,0.994,2.},/* 20<pt<24 */
						  {0.400,400.*1E-4,1.0,0.7,0.7,1000.*1E-4,1000.*1E-4,-500.*1E-8,0.7,0.994,2.}};/* pt>24 */

  //CREATE TRANSPOSE MATRIX...REVERSE INDICES as required by AliRDHFCuts
  Float_t **cutsMatrixTransposeStand=new Float_t*[nvars];
  for(Int_t iv=0;iv<nvars;iv++)cutsMatrixTransposeStand[iv]=new Float_t[nptbins];
  for (Int_t ibin=0;ibin<nptbins;ibin++){
    for (Int_t ivar = 0; ivar<nvars; ivar++){
      cutsMatrixTransposeStand[ivar][ibin]=cutsMatrixD0toKpiStand[ibin][ivar];
    }
  }
  RDHFD0toKpi->SetCuts(nvars,nptbins,cutsMatrixTransposeStand);


  Bool_t stdvaropt=kFALSE;
  Int_t dim=4; //set this!!
  Bool_t *boolforopt;
  boolforopt=new Bool_t[nvars];
  if(stdvaropt){
    boolforopt=RDHFD0toKpi->GetVarsForOpt();
  }else{
    TString *names;
    names=new TString[nvars];
    TString answer="";
    Int_t checktrue=0;
    names=RDHFD0toKpi->GetVarNames();
    for(Int_t i=0;i<nvars;i++){
      cout<<names[i]<<" for opt? (y/n)"<<endl;
      cin>>answer;
      if(answer=="y") {
	boolforopt[i]=kTRUE;
	checktrue++;
      }
      else boolforopt[i]=kFALSE;
    }
    if (checktrue!=dim) {
      cout<<"Error! You set "<<checktrue<<" kTRUE instead of "<<dim<<endl;
      return;
    }
    RDHFD0toKpi->SetVarsForOpt(dim,boolforopt);
  }


  Float_t tighterval[dim][nptbins];
  //dca  
  //costhetastar
  //d0d0 <-this 
  //costhetapoint <-this 
  //cosThetaPointXY <-this 
  //NormDecayLength <-this 
  
  //number of steps for each variable is set in the AddTask arguments (default=8)
  //set this!!
  //0-0.5
  tighterval[0][0]=-0.00065;
  tighterval[1][0]=1.;
  tighterval[2][0]=0.4;
  tighterval[3][0]=6.;

  //0.5-1.
  tighterval[0][1]=-0.00065;
  tighterval[1][1]=1.;
  tighterval[2][1]=0.4;
  tighterval[3][1]=6.;

  //1-2
  tighterval[0][2]=-0.00065;
  tighterval[1][2]=1.;
  tighterval[2][2]=0.4;
  tighterval[3][2]=6.;
 
  //2-3
  tighterval[0][3]=-0.0006;
  tighterval[1][3]=1.;
  tighterval[2][3]=1.;
  tighterval[3][3]=6.;

  //3-4
  tighterval[0][4]=-0.00046;
  tighterval[1][4]=1.;
  tighterval[2][4]=1.;
  tighterval[3][4]=6.;
 
  //4-5
  tighterval[0][5]=-0.00045;
  tighterval[1][5]=1.;
  tighterval[2][5]=1.;
  tighterval[3][5]=6.;
 
  //5-6
  tighterval[0][6]=-0.00031;
  tighterval[1][6]=1.;
  tighterval[2][6]=1.;
  tighterval[3][6]=6.;

  //6-8
  tighterval[0][7]=-0.00021;
  tighterval[1][7]=0.98;
  tighterval[2][7]=1.;
  tighterval[3][7]=6.;

  //8-10
  tighterval[0][8]=-0.0001;
  tighterval[1][8]=0.98;
  tighterval[2][8]=1.;
  tighterval[3][8]=6.;

  //10-12
  tighterval[0][9]=-0.0001;
  tighterval[1][9]=0.9;
  tighterval[2][9]=1.;
  tighterval[3][9]=6.;
 
  //12-16
  tighterval[0][10]=-0.00005;
  tighterval[1][10]=0.9;
  tighterval[2][10]=1.;
  tighterval[3][10]=6.;

  //16-20
  tighterval[0][11]=-0.00005;
  tighterval[1][11]=0.9;
  tighterval[2][11]=1.;
  tighterval[3][11]=6.;

  //20-24
  tighterval[0][12]=-0.00005;
  tighterval[1][12]=0.9;
  tighterval[2][12]=1.;
  tighterval[3][12]=6.;

  //>24
  tighterval[0][13]=-0.00005;
  tighterval[1][13]=0.9;
  tighterval[2][13]=1.;
  tighterval[3][13]=6.;


  TString name=""; 
  Int_t arrdim=dim*nptbins;
  cout<<"Will save "<<arrdim<<" TParameter<float>"<<endl;
  TClonesArray max("TParameter<float>",arrdim);
  for(Int_t ival=0;ival<dim;ival++){
    for(Int_t jpt=0;jpt<nptbins;jpt++){
      name=Form("par%dptbin%d",ival,jpt);
      cout<<"Setting "<<name.Data()<<" to "<<tighterval[ival][jpt]<<endl;
      new(max[jpt*dim+ival])TParameter<float>(name.Data(),tighterval[ival][jpt]);
    }
  }

  Bool_t flagPID=kTRUE;
  RDHFD0toKpi->SetUsePID(flagPID);

  RDHFD0toKpi->PrintAll();
  printf("Use PID? %s\n",flagPID ? "yes" : "no");

  //pid settings
  AliAODPidHF* pidObj=new AliAODPidHF();
  //pidObj->SetName("pid4D0");
  Int_t mode=1;
  const Int_t nlims=2;
  Double_t plims[nlims]={0.6,0.8}; //TPC limits in momentum [GeV/c]
  Bool_t compat=kTRUE; //effective only for this mode
  Bool_t asym=kTRUE;
  Double_t sigmas[5]={2.,1.,0.,3.,0.}; //to be checked and to be modified with new implementation of setters by Rossella
  pidObj->SetAsym(asym);// if you want to use the asymmetric bands in TPC
  pidObj->SetMatch(mode);
  pidObj->SetPLimit(plims,nlims);
  pidObj->SetSigma(sigmas);
  pidObj->SetCompat(compat);
  pidObj->SetTPC(kTRUE);
  pidObj->SetTOF(kTRUE);
  RDHFD0toKpi->SetPidHF(pidObj);

  RDHFD0toKpi->SetUseDefaultPID(kFALSE); //to use the AliAODPidHF
  
  //activate pileup rejection (for pp)
  //RDHFD0toKpi->SetOptPileup(AliRDHFCuts::kRejectPileupEvent);

  //Do not recalculate the vertex
  RDHFD0toKpi->SetRemoveDaughtersFromPrim(kFALSE); //activate for pp

  TString cent="";
  //centrality selection (Pb-Pb)
  Float_t minc=20,maxc=80;
  RDHFD0toKpi->SetMinCentrality(minc);
  RDHFD0toKpi->SetMaxCentrality(maxc);
  cent=Form("%.0f%.0f",minc,maxc);
  RDHFD0toKpi->SetUseCentrality(AliRDHFCuts::kCentV0M); //kCentOff,kCentV0M,kCentTRK,kCentTKL,kCentCL1,kCentInvalid

  //temporary
  RDHFD0toKpi->SetFixRefs();

  TFile* fout=new TFile(Form("centralSignifMaxim%s%s%sRecVtx%sPileupRej.root", RDHFD0toKpi->GetUseCentrality()==0 ? "pp" : "PbPb",cent.Data(),RDHFD0toKpi->GetIsPrimaryWithoutDaughters() ? "" : "No",RDHFD0toKpi->GetOptPileUp() ? "" : "No"),"recreate");   //set this!!

  fout->cd();
  RDHFD0toKpi->Write();
  max.Write();
  fout->Close();
 
}

