#!/bin/bash

#//-----------------------------------------------------------------------
#//  Author B.Trzeciak
#//  Utrecht University
#//  barbara.antonina.trzeciak@cern.ch
#//-----------------------------------------------------------------------
#-- script to run D-jets analysis

############## RooUnfold path
roounfoldpwd=${34}
############### set up the output directory name (the whole analysis output will be saved there)
currDir=`pwd`
outdirBase=$1
outdir=$2
conffile=$3
Dmeson=$4           # 0: D0, 1: D*

###### !!!! CONFIGURE THE FLAGS BELOW !!!!!!!!
############### analysis flags (you can switch some of the flags to run on a part of the analysis chain)
# usually you don't need to change this part
isSignal=1          # if to extract signal from the inv. mass
isEffPrompt=1       # if to extract prompt D-jet efficiency
isEffNonPrompt=1    # if to extract non-prompt D-jet efficiency
isDetRMPrompt=1     # if to extract prompt D-jet response matrix
isDetRMNonPrompt=1  # if to extract non-prompt D-jet response matrix
isUnfolding=1       # if to perform unfolding
isFDSub=1           # if to subtract feed-down
isFinalSpectra=1    # if to extract the final x-section

isCsim=1            # switch this flag on if you haven't prepared output of the simulations yet, it takes time -- if the simulation output directory is empty the simulations will be run anyway
isBsim=0            # switch this flag on if you haven't prepared output of the simulations yet with a current efficiencies, it takes time -- if the simulation output directory is empty the simulations will be run anyway
isBsimNoEff=0       # switch one if you want non-prompt simulations without scaling for non-prompt/prompt efficiency (shouldn't be used in a standard analysis chain)


######## !!! Efficiency extraction -- configure this if you need different jet pT range
jetpteffmin=5     # min jet pT for the D-jet efficiency extraction
jetpteffmax=50    # max jet pT for the D-jet efficiency extraction
recoPt=0          # if numinator is cut on gen or reco jet pT


################################################
############### in/out directories config
################################################

############### D-jet signal config
anaoutfiledir=${5}
analysisfile=${6}
refoutfiledir=${7}
reflfile=${8}
isRefl=${9}            # reflections for D0
analysisDataFile=$anaoutfiledir/$analysisfile
reflTemplatesFile=$refoutfiledir/$reflfile
### if more than one file to analyse
isMoreFiles=${10}
production=${11}

ispostfix=${12}
postfix=${13}
ispostfixFD=${14}
postfixFD=${15}

########## efficiency config
MCoutfiledir=${16}
efficiencyfile=${17}
efffile=$MCoutfiledir/$efficiencyfile

############### response matrices config
detRMpromptfile=${18}
detRMnonpromptfile=${19}
isBkgRM=${20}           # if to use external bkg. fluctuation matrix
bkgRMtype=${21}
bkgRMDir=${22}
bkgRMfileName=${23}
bkgRMFile=$bkgRMDir/$bkgRMfileName
detRMPrompt=$MCoutfiledir/$detRMpromptfile
detRMNonPrompt=$MCoutfiledir/$detRMnonpromptfile

############### POWHEG simulations config
simFilesDir=${24}

############### unfolding config
unfType=${25}       # 0:bayes, 1:SVD
regPar=${26}        # regularization parameter for unfolding
isPrior=${27}       # if prior different than MC true is needed
priorType=${28}     # then provide also the prior type, based on what is implemented in the unfolding macro

############### analysis steps
isFDUpSys=${29}
isFDDownSys=${30}
isRawSpectra=${31}

######## !!! POWHEG simulations config
nSimFilesB=${32}     # have to correspond to number of files defined in the config file
nSimFilesC=${33}      # have to correspond to number of files defined in the config file

isOnlyFinal=0
if [ $isOnlyFinal -eq 1 ]; then
  isSignal=0
  isRefl=0        #reflections for D0
  isEffPrompt=0
  isEffNonPrompt=0
  isDetRMPrompt=0
  isDetRMNonPrompt=0
  isUnfolding=0
fi

if [ $bkgRMtype -gt 0 ]; then
    outdir=$outdir\_BkgM$bkgRMtype
fi

######### config file
mkdir -p $outdirBase/$outdir
cat $conffile > config.h
echo "const Int_t fBsimN = ${nSimFilesB};"  >> config.h
echo "const Int_t fCsimN = ${nSimFilesC};"  >> config.h
echo "TString OUTDIRECTORY=\"$outdirBase\";"  >> config.h


################################################
############### in/out directories config
################################################

############### analysis code dirs
signalDir=DsignalExtraction
effDir=efficiency
FDDir=FDsubtraction
RMDir=ResponseMatrix
SimDir=POWHEGSim
unfoldingDir=unfolding
finalDir=finalSpectra

############### out dirs
signalDirOut=$outdirBase/$outdir/signalExtraction
effDirOut=$outdirBase/$outdir/efficiency
FDSubDirOut=$outdirBase/$outdir/FDsubtraction
RMDirOut=$outdirBase/$outdir/ResponseMatrix
unfoldingDirOut=$outdirBase/$outdir/unfolding
if [ $unfType -eq  0 ]; then
  unfoldingDirOut=$unfoldingDirOut\_Bayes\_
fi
if [ $unfType -eq  1 ]; then
  unfoldingDirOut=$unfoldingDirOut\_SVD\_
fi
unfoldingDirOut=$unfoldingDirOut$regPar
if [ $isPrior -eq 1 ]; then
  unfoldingDirOut=$unfoldingDirOut\_priorType$priorType
fi

if [ $isFDUpSys -eq 1 ]; then
  unfoldingDirOut=$unfoldingDirOut\_FDsysUp
fi
if [ $isFDDownSys -eq 1 ]; then
  unfoldingDirOut=$unfoldingDirOut\_FDsysDown
fi

finalDirOut=$unfoldingDirOut/finalSpectra
BFDSimDirOut=$outdirBase/Simulations/BFeedDown
PromptSimDirOut=$outdirBase/Simulations/Prompt

if [ $isRawSpectra -eq 1 ]; then
  isEffPrompt=0
fi

################################################
############### D-jet efficiency
################################################
cd $effDir
if [ $isEffPrompt -eq 1 ]; then
    aliroot -l -b -q DjetEfficiency.C'(1,"'$efffile'","'$effDirOut'",'$jetpteffmin','$jetpteffmax','$recoPt','$ispostfix',"'$postfix'")'
fi

if [ $isEffNonPrompt -eq 1 ]; then
    aliroot -l -b -q DjetEfficiency.C'(0,"'$efffile'","'$effDirOut'",'$jetpteffmin','$jetpteffmax','$recoPt','$ispostfixFD',"'$postfixFD'")'
fi

effFilePrompt=$effDirOut/DjetEff_prompt_jetpt$jetpteffmin\_$jetpteffmax.root
effFileNonPrompt=$effDirOut/DjetEff_nonPrompt_jetpt$jetpteffmin\_$jetpteffmax.root

if [[ $isEffPrompt -eq 1 && $isEffNonPrompt -eq 1 ]]; then
  aliroot -l -b -q drawEff.C'("'$effFilePrompt'","'$effFileNonPrompt'","'$effDirOut'",'$jetpteffmin','$jetpteffmax',2,36)'
fi

cd $currDir

################################################
############### D-jet signal
################################################

if [ $isSignal -eq 1 ]; then

if [ $isEffPrompt -eq 1 ]; then
  if [ ! -f $effFilePrompt ]; then
    echo "!!! Prompt efficiency file: \"$effFilePrompt\"  does not exist !!!! "
    exit 1
  fi
fi

cd $signalDir
./getYields_direct.csh $analysisDataFile $isEffPrompt $effFilePrompt $isRefl $reflTemplatesFile $ispostfix $postfix $signalDirOut $isMoreFiles $production

if [ $isEffPrompt -eq 0 ]; then
  signalFile=$signalDirOut/JetPtSpectra_SB_noEff.root
else
  signalFile=$signalDirOut/JetPtSpectra_SB_eff.root
fi

cd $currDir
fi

if [ ! -f $signalFile ]; then
  echo "!!! D-jet signal extraction file: \"$signalFile\"  does not exist !!!! "
  exit 1
fi

if [ $isRawSpectra -eq 1 ]; then
  exit 1
fi

exit 1
################################################
############### Detector Response Matrix
################################################
cd $RMDir

if [ $isDetRMPrompt -eq 1 ]; then
    aliroot -l -b -q DetRM.C'(1,"'$detRMPrompt'","'$RMDirOut'",'$ispostfix',"'$postfix'")'
fi

if [ $isDetRMNonPrompt -eq 1 ]; then
  aliroot -l -b -q DetRM.C'(0,"'$detRMNonPrompt'","'$RMDirOut'",'$ispostfixFD',"'$postfixFD'")'
fi

detRMFilePrompt=$RMDirOut/DetMatrix_prompt.root
detRMFileNonPrompt=$RMDirOut/DetMatrix_nonPrompt.root

cd $currDir

################################################
############### B feed-down simulations
################################################

if [ ! -d "$BFDSimDirOut" ] || [ $isBsim -eq 1 ]; then

cd $SimDir
# 1: number of simulation files (as defined in the config file)
# 2: simulation file directory;
# 3: quark type: 0: charm, 1: beauty
# 4: D-jet pT spectrum: 1, D-meson pT spectrum: 0
# 5: if D meson pT cut applied (for D-jet pT spectrum case), if yes the lower and upper values from the D-meson pT bins from the config file are taken
# 6: if efficiency applied (for B simulations, ratio of non-prompt/prompt efficiency)
# 7: prompt efficiency file
# 8: non-prompt efficiency file
# 9: directory for the output files

 ./doGetSimOut.csh $nSimFilesB $simFilesDir 1 1 1 1 $effFilePrompt $effFileNonPrompt $BFDSimDirOut
#plot all the variations
  aliroot -l -b -q plotSimSpectra.C'(1,1,1,1,"'$BFDSimDirOut'","'$BFDSimDirOut'")'

cd $currDir
fi

if [ $isBsimNoEff -eq 1 ]; then
cd $SimDir

  #no eff scaling
  if [ $isBsimNoEff -eq 1 ]; then
    ./doGetSimOut.csh $nSimFilesB $simFilesDir 1 1 1 0 $effFilePrompt $effFileNonPrompt $BFDSimDirOut
    aliroot -l -b -q plotSimSpectra.C'(1,1,1,0,"'$BFDSimDirOut'","'$BFDSimDirOut'")'
  fi
cd $currDir
fi


################################################
############### B feed-down subtraction
################################################

if [ $isFDSub -eq 1 ]; then
  if [ ! -f $detRMFileNonPrompt ]; then
    echo "!!! Non-prompt det RM file: \"$detRMFileNonPrompt\"  does not exist !!!! "
    exit 1
  fi

  cd $RMDir
  aliroot -l -b -q combineRM.C'(0,"'$RMDirOut'","'$detRMFileNonPrompt'",'$isBkgRM',"'$bkgRMFile'")'
  combRMFDFile=$RMDirOut/combineMatrixFD.root
  if [ ! -f $combRMFDFile ]; then
    echo "!!! Non-prompt combined RM file: \"$combRMFDFile\"  does not exist !!!! "
    exit 1
  fi
  cd $currDir

  cd $FDDir
  aliroot -l -b -q subtractFD.C'("'$roounfoldpwd'","'$signalFile'","'$analysisDataFile'","'$BFDSimDirOut'","'$combRMFDFile'","'$FDSubDirOut'")'

  cd $currDir
fi

if [ $isFDSub -eq 1 ]; then
  signalBCorrFile=$FDSubDirOut/JetPtSpectrum_FDsub.root
fi
if [ $isFDSub -eq 0 ]; then
  signalBCorrFile=$signalFile
fi


################################################
############### Unfolding
################################################
if [ $isUnfolding -eq 1 ]; then

  if [ ! -f $detRMFilePrompt ]; then
    echo "!!! Non-prompt det RM file: \"$detRMFilePrompt\"  does not exist !!!! "
    exit 1
  fi

  cd $RMDir
    aliroot -l -b -q combineRM.C'(1,"'$RMDirOut'","'$detRMFilePrompt'",'$isBkgRM',"'$bkgRMFile'")'
    combRMFile=$RMDirOut/combineMatrix.root
  cd $currDir

  cd $unfoldingDir
      if [ $unfType -eq  0 ]; then
          aliroot -l -b -q unfold_Bayes.C'("'$roounfoldpwd'","'$signalBCorrFile'","'$detRMFilePrompt'","'$bkgRMFile'","'$unfoldingDirOut'",'$regPar','$isPrior','$priorType','$isBkgRM','$isFDUpSys','$isFDDownSys')'
      fi
      if [ $unfType -eq  1 ]; then
          aliroot -l -b -q unfold_SVD.C'("'$roounfoldpwd'","'$signalBCorrFile'","'$detRMFilePrompt'","'$bkgRMFile'","'$unfoldingDirOut'",'$regPar','$isPrior','$priorType','$isBkgRM','$isFDUpSys','$isFDDownSys')'
      fi

  cd $currDir
fi

cd $currDir

if [ $isUnfolding -eq 1 ]; then
  signalUnfoldedFile=$unfoldingDirOut/unfoldedSpectrum_unfoldedJetSpectrum.root
fi
if [ $isUnfolding -eq 0 ]; then
  signalUnfoldedFile=$signalBCorrFile
fi


################################################
############### prompt D-jet simulations
################################################

if [ ! -d "$PromptSimDirOut" ] && [ $isCsim -eq 1 ]; then
cd $SimDir
  ./doGetSimOut.csh $nSimFilesC $simFilesDir 0 1 1 0 $effFilePrompt $effFileNonPrompt $PromptSimDirOut
  #plot all the variations
  aliroot -l -b -q plotSimSpectra.C'(0,1,1,0,"'$PromptSimDirOut'","'$PromptSimDirOut'")'
cd $currDir
fi

################################################
############### Final spectra
################################################

if [ $isFinalSpectra -eq 1 ]; then
sysDir=$outdirBase/$outdir/systematics
  cd $finalDir
  aliroot -l -b -q finalJetSpectra.C'("'$signalUnfoldedFile'","'$analysisDataFile'","'$PromptSimDirOut'","'$finalDirOut'","'$sysDir'",1,1,0)'
  cd $currDir
fi

################################################
############### END
################################################
