#!/bin/bash
rm MCFileList
for eachfile in /home/basia/Work/ALICE/Analysis/Results/PbPb2018/QA/DJetMC/output/*.root
do
  echo $eachfile >> ./MCFileList
done
