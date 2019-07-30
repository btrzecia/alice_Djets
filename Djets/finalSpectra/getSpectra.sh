#!/bin/bash

root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/JetPtSpectrum_final_PRELIMINARY.root",1,0,fHistJetPt,5,50,2,36)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/JetPtSpectrum_final_PRELIMINARY.root",0,0,fHistJetPt,5,50,2,36)'

root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final1.root",1,0,fHistZ,5,7,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final2.root",1,0,fHistZ,7,10,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_01/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final3.root",1,0,fHistZ,10,15,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_01/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final4.root",1,0,fHistZ,15,50,3,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final1.root",1,1,fHistZ,5,7,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final2.root",1,1,fHistZ,7,10,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_01/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final3.root",1,1,fHistZ,10,15,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_01/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final4.root",1,1,fHistZ,15,50,3,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final1.root",0,0,fHistZ,5,7,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_021/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final2.root",0,0,fHistZ,7,10,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_01/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final3.root",0,0,fHistZ,10,15,2,36,0.4)'
root -l -b -q dataWSimulation.C'(1,"~/Work/ALICE/Analysis/Results/pp13tev/Z_01/Default_AnalysisResults_Run2.root/unfolding_Bayes_5/finalSpectra/JetPtSpectrum_final4.root",0,0,fHistZ,15,50,3,36,0.4)'
