#! /bin/bash

#--------------------------#
#-- Author: Auro Mohanty --#
#-- Utrecht University	 --#
#-- auro.mohanty@cern.ch --#
#--------------------------#

#-- script for D-jets analysis
#-- (development stage)

#- Instructions.
#---------------
# run this by executing 'bash zrun.csh' on your terminal in AliPhysics environment.

bin_zjet=1 # change the bin number you want to use here.
		# 0 means no cut on jet pt
		# 1 through 5 mean all 5 bins of jet pt
conffile_z1=configDzero_ppz1.h
conffile_z2=configDzero_ppz2.h
cat $conffile_z1 > configDzero_ppz.h
echo "int zjetbin = ${bin_zjet};" >> configDzero_ppz.h
if [ $bin_zjet -eq 0 ]; then
 echo "double        fptbinsDA[] = {1,2,3,4,5,6,7,8,10,12,16,24,36 };" >> configDzero_ppz.h
elif [ $bin_zjet -eq 1 ]; then
 #echo "double        fptbinsDA[] = {1,2,3,4,5,6,7};" >> configDzero_ppz.h
 echo "double        fptbinsDA[] = {2,3,4,5,6,7};" >> configDzero_ppz.h
elif [ $bin_zjet -eq 2 ]; then
 #echo "double        fptbinsDA[] = {1,2,3,4,5,6,7,8,10};" >> configDzero_ppz.h
 echo "double        fptbinsDA[] = {3,4,5,6,7,8,10};" >> configDzero_ppz.h
elif [ $bin_zjet -eq 3 ]; then
 #echo "double        fptbinsDA[] = {1,2,3,4,5,6,7,8,10,12,16};" >> configDzero_ppz.h
 echo "double        fptbinsDA[] = {5,6,7,8,10,12,16};" >> configDzero_ppz.h
elif [ $bin_zjet -eq 4 ]; then
 #echo "double        fptbinsDA[] = {1,2,3,4,5,6,7,8,10,12,16,24,36};" >> configDzero_ppz.h
 echo "double        fptbinsDA[] = {10,12,16,24,36};" >> configDzero_ppz.h
elif [ $bin_zjet -eq 5 ]; then
 echo "double        fptbinsDA[] = {1,36 };" >> configDzero_ppz.h
fi

cat $conffile_z2 >> configDzero_ppz.h
root -l -b signalExtraction_SBz.C
#root -l  signalExtraction_SBz.C
