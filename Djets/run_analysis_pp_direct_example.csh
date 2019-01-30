#!/bin/bash
#
#//-----------------------------------------------------------------------
#//  Author B.Trzeciak
#//  Utrecht University
#//  barbara.antonina.trzeciak@cern.ch
#//-----------------------------------------------------------------------
#-- script to run D-jets analysis
# examples from D0-jet p-Pb analysis

# ============= set up config =================

Dmeson=0 # 0: D0, 1: D*
if [ $Dmeson -eq 0 ]; then
  conffile=configDzero_pp.h
else
  conffile=configDstar.h
fi

############## RooUnfold path
roounfoldpwd=$HOME/Work/alice/RooUnfold-1.1.1/libRooUnfold.so

currDir=`pwd`
outputdirectorybase=$HOME/Work/alice/analysis/pp5TeV/D0jet/results/DzeroR03_
outputdirectory=${outputdirectorybase}pPbCuts
#outputdirectory=${outputdirectorybase}cuts2
outputdirectorySignal=Default_directExtraction

# ========== input directories
anaoutfiledir=$HOME/Work/alice/analysis/pp5TeV/D0jet/outData
MCoutfiledir=$HOME/Work/alice/analysis/pp5TeV/D0jet/outMC
refoutfiledir=$HOME/Work/alice/analysis/pp5TeV/D0jet/outMC/reflections
simFilesDir=/media/basia/Disk2/Work/Djets/POWHEGSimulations/fastSim_pp5TeV/ # POWHEG sim
bkgRMDir=$currDir/ResponseMatrix/BkgRM03

# ========== file names
analysisdatafile=AnalysisResults_LHC17pq_FASTwoSDD.root
isMoreFiles=0                                     # 1 if there are more input files to be read
prod=kl                                           # if there are more input files to be read
reflfile=reflectionTemplates_pPb.root
MCfile=AnalysisResults_fast_R03_D0MC_def.root
efficiencyfile=$MCfile
detRMpromptfile=$MCfile
detRMnonpromptfile=$MCfile
ispostfix=0                                       # if container in the analysis output file has different name than default you set here if and what is the postfix, this is set up in the signal, efficiency and RM extraction macros
postfix=cut2
ispostfixFD=0                                     # if container in the analysis output file has different name than default you set here if and what is the postfix, for the FD part wagons are usually configured with additional "FD" string in the container name, you should adjust this to yours configuration
postfixFD=FD
#postfixFD=FDcut2

isRefl=0
isBkgRM=0

######## !!! POWHEG simulations config
nSimFilesB=4                                    # have to correspond to number of files defined in the config file
nSimFilesC=1                                    # have to correspond to number of files defined in the config file

unfType=$1
regPar=$2
isPrior=$3
priorType=$4
bkgRMtype=$5
doRawSpectra=$6

############### bkg fluctuations matrix
bkgRMfileName=tmp.root

############### RUN ###############
./run_direct.csh $outputdirectory $outputdirectorySignal $conffile $Dmeson $anaoutfiledir $analysisdatafile $refoutfiledir $reflfile $isRefl $isMoreFiles $prod $ispostfix $postfix $ispostfixFD $postfixFD $MCoutfiledir $efficiencyfile $detRMpromptfile $detRMnonpromptfile $isBkgRM $bkgRMtype $bkgRMDir $bkgRMfileName $simFilesDir $unfType $regPar $isPrior $priorType 0 0 $doRawSpectra $nSimFilesB $nSimFilesC $roounfoldpwd


exit 1
