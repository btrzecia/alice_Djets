#!/bin/bash

# B. Trzeciak (Utrecht University)
#-- script to run D-jets analysis

# ============= set up config =================

Dmeson=0 # 0: D0, 1: D*
if [ $Dmeson -eq 0 ]; then
  conffile=configDzero_pPb.h
else
  conffile=configDstar_pPb.h
fi

############## RooUnfold path
roounfoldpwd=$HOME/ALICE_HeavyFlavour/RooUnfold-1.1.1/libRooUnfold

currDir=`pwd`
outputdirectorybase=$HOME/Work/alice/analysis/pPb_run2/DzeroR03_
outputdirectory=${outputdirectorybase}BaseCuts
outputdirectorySignal=DefaultTest

# ========== input directories
anaoutfiledir=$HOME/Work/alice/analysis/pPb_run2/D0jet/outData
refoutfiledir=$HOME/Work/alice/analysis/pPb_run2/D0jet/outMC/reflections
MCoutfiledir=$HOME/Work/alice/analysis/pPb_run2/D0jet/outMC
simFilesDir=/media/basia/Disk2/Work/Djets/POWHEGSimulations/fastSim_pPb5TeV
bkgRMDir=$currDir/ResponseMatrix/BkgRM03

# ========== file names
analysisdatafile=AnalysisResults_LHC16R03.root
isMoreFiles=0   # 1 if there are more input files to be read
prod=kl         # if there are more input files to be read
reflfile=reflections_fitted_DoubleGaus.root
efficiencyfile=AnalysisResults_fast_R03_D0MCPythia_default.root
detRMpromptfile=AnalysisResults_fast_R03_D0MCPythia_default.root
detRMnonpromptfile=AnalysisResults_fast_R03_D0MCPythia_default.root
ispostfix=0     # if container in the analysis output file has different name than default you set here if and what is the postfix, this is set up in the signal, efficiency and RM extraction macros
postfix=Cut
ispostfixFD=1   # if container in the analysis output file has different name than default you set here if and what is the postfix, for the FD part wagons are usually configured with additional "FD" string in the container name, you should adjust this to yours configuration
postfixFD=FD

isRefl=1
isBkgRM=1

######## !!! POWHEG simulations config
nSimFilesB=10     # have to correspond to number of files defined in the config file
nSimFilesC=9      # have to correspond to number of files defined in the config file

unfType=$1
regPar=$2
isPrior=$3
priorType=$4
bkgRMtype=$5
doRawSpectra=$6

###############  for p-Pb 2016 ###############
############### bkg fluctuations matrix
case $bkgRMtype in
0)
  bkgRMfileName=RandCones_BkgM_Djet5Excl.root
  ;;
1)
  bkgRMfileName=RandCones_BkgM_Djet5Excl_Dlead.root
  ;;
2)
  bkgRMfileName=RandCones_BkgM_Djet5Perp.root
  ;;
3)
  bkgRMfileName=RandCones_BkgM_Djet5Perp_Dlead.root
  ;;
4)
  bkgRMfileName=RandCones_BkgM_Djet10Excl.root
  ;;
5)
  bkgRMfileName=RandCones_BkgM_Djet10Excl_Dlead.root
  ;;
6)
  bkgRMfileName=RandCones_BkgM_Djet10Perp.root
  ;;
7)
  bkgRMfileName=RandCones_BkgM_Djet10Perp_Dlead.root
  ;;
8)
  bkgRMfileName=RandCones_BkgM_Alljet5Excl.root
  ;;
9)
  bkgRMfileName=RandCones_BkgM_Alljet5Perp.root
  ;;
10)
  bkgRMfileName=RandCones_BkgM_Alljet10Excl.root
  ;;
11)
  bkgRMfileName=RandCones_BkgM_Alljet10Perp.root
  ;;
*)
  echo "!!! Wrong bkg RM typ !!!"
  exit 1
  ;;
esac
############### END of p-Pb 2016 bkg ###############


############### RUN ###############
./run.csh $outputdirectory $outputdirectorySignal $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile $detRMnonpromptfile $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 $doRawSpectra $nSimFilesB $nSimFilesC $roounfoldpwd


################################################
############### Additional
################################################

#### systematics  -- these are additional settings - in order to use them you need to configure this and (if needed) further scripts
doCutVar=${7}
doRawCutVar=${8}
doJESSys=${9}
doFDSys=${10}
doSystematics=${11}

################################################
############### B FD systematics -- this will work w/o additinal modifications
################################################
if [ $doFDSys -eq 1 ]; then

  ./run.csh $outputdirectory $outputdirectorySignal $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile $detRMnonpromptfile $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 1 0 0 $nSimFilesB $nSimFilesC $roounfoldpwd

  ./run.csh $outputdirectory $outputdirectorySignal $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile $detRMnonpromptfile $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 1 0 $nSimFilesB $nSimFilesC $roounfoldpwd

fi

################################################
############### JES systematics -- configure below names for the responce matrices
################################################

if [ $doJESSys -eq 1 ]; then

    detRMpromptfile1=AnalysisResults_fast_R03_D0MCPythia_JES96_1.root
  	detRMnonpromptfile1=AnalysisResults_fast_R03_D0MCPythia_JES96_1.root
  	eff=96
  	outputdirectorySignal1=$outputdirectorySignal$eff

    ./run.csh $outputdirectory $outputdirectorySignal1 $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile1 $detRMnonpromptfile1 $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 0 $nSimFilesB $nSimFilesC $roounfoldpwd

    detRMpromptfile1=AnalysisResults_fast_R03_D0MCPythia_JES95_1.root
    detRMnonpromptfile1=AnalysisResults_fast_R03_D0MCPythia_JES95_1.root
    eff=95
    outputdirectorySignal1=$outputdirectorySignal$eff

    ./run.csh $outputdirectory $outputdirectorySignal1 $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile1 $detRMnonpromptfile1 $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 0 $nSimFilesB $nSimFilesC $roounfoldpwd

    detRMpromptfile1=AnalysisResults_fast_R03_D0MCPythia_JES90_1.root
    detRMnonpromptfile1=AnalysisResults_fast_R03_D0MCPythia_JES90_1.root
    eff=90
    outputdirectorySignal1=$outputdirectorySignal$eff

    ./run.csh $outputdirectory $outputdirectorySignal1 $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile1 $detRMnonpromptfile1 $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 0 $nSimFilesB $nSimFilesC $roounfoldpwd

fi

################################################
############### Cut Variation -- configure this !!!
################################################

if [ $doCutVar -eq 1 ]; then

  # ========== file names
  analysisdatafileCut=AnalysisResults_LHC16R03_CutVariation.root
  efficiencyfileCut=AnalysisResults_fast_R03_D0MCPythia_CutVariation_1.root
  detRMpromptfileCut=AnalysisResults_fast_R03_D0MCPythia_CutVariation_1.root
  detRMnonpromptfileCut=AnalysisResults_fast_R03_D0MCPythia_CutVariation_1.root
  isReflCut=0
  outputdirectorySignalCut=Default

  ispostfixCut=0
  ispostfixFDCut=1
  postfixCut=Cut
  postfixFDCut=FD
  outputdirectoryCut=${outputdirectorybase}Cuts_VarDef
	./run.csh $outputdirectoryCut $outputdirectorySignalCut $conffile $Dmeson $anaoutfiledir $analysisdatafileCut $refoutfiledir $reflfile $isReflCut $isMoreFiles $prod $ispostfixCut $postfixCut $ispostfixFDCut $postfixFDCut $MCoutfiledir $efficiencyfileCut $detRMpromptfileCut $detRMnonpromptfileCut $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 $doRawSpectra $nSimFilesB $nSimFilesC $roounfoldpwd

arraySize
declare -a cutarray=( L0 L1 L2 L3 T0 T1 T2 ) #declare postfixes

for i in "${cutarray[@]}"
do
  ispostfixCut=1
  ispostfixFDCut=1
  postfixCut=${cutarray[$i]}
  postfixFDCut=${cutarray[$i]}FD
  outputdirectoryCut=${outputdirectorybase}Cuts_${cutarray[$i]}
  ./run.csh $outputdirectoryCut $outputdirectorySignalCut $conffile $Dmeson $anaoutfiledir $analysisdatafileCut $refoutfiledir $reflfile $isReflCut $isMoreFiles $prod $ispostfixCut $postfixCut $ispostfixFDCut $postfixFDCut $MCoutfiledir $efficiencyfileCut $detRMpromptfileCut $detRMnonpromptfileCut $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 $doRawSpectra $nSimFilesB $nSimFilesC $roounfoldpwd
done

  # cut variation systematics
#  cd systematics
  #after unfolding
#  root -l -b -q cutsSystematics.C'('$regPar',"'$outputdirectorybase'","'$outputdirectorySignal'",'$doRawCutVar',1,1)'
  #before unfolding
#  root -l -b -q cutsSystematics.C'('$regPar',"'$outputdirectorybase'","'$outputdirectorySignal'",'$doRawCutVar',0,1)'
  cd $currDir

fi

exit 1

################################################
############### Get systematic uncertanties
################################################

#if [ $doSystematics -eq 1 ]; then
#	cd systematics
#		root -l -b -q JetSpectrumSys.C'('$regPar',"'$outputdirectory'","'$outputdirectorySignal'",1)'
#	cd $currDir
#fi

exit 1
