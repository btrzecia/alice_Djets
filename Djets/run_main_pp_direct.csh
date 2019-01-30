#!/bin/bash

#//-----------------------------------------------------------------------
#//  Author B.Trzeciak
#//  Utrecht University
#//  barbara.antonina.trzeciak@cern.ch
#//-----------------------------------------------------------------------
#-- script to run D-jets analysis

################################################
############### FULL with sys
################################################

unfType=0     # unfolding type, 0: bayes. 1: SVD
regPar=4      # regularization parameter for unfolding
isPrior=0     # if 0 deafult prior (MC true) is used, otherwise a prior defined in the macro
priorType=0   # if isPrior != 0, define which prior function you want to use, you can define new ones in the unfolding macro
bkgRMtype=0   # 0: default, if you use external bkg. fluctuation matrix define which one should be used, based on this name of a root file with a corresponding bkg. fluctuation matrix is set in the run_analysis.csh script (you can change this so it corresponds to your file)

#systematics -- these are additional settings - in order to use them you need to configure your run_analysis_*.csh script
doBkg=0
doPrior=0
doCutVar=0
doJESSys=0
doFDSys=0
doSys=0
doRawCutVarSys=0

############### Default
doRawSignal=0
# run your run_analysis_*.csh script
./run_analysis_pp_direct.csh $unfType $regPar $isPrior $priorType $bkgRMtype $doRawSignal $doCutVar $doRawCutVarSys $doJESSys $doFDSys $doSys

exit 1
