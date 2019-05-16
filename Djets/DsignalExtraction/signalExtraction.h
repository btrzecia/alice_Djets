#ifndef SIGNALEXTRACTION_H
#define SIGNALEXTRACTION_H

/**************************************************************************
 * Copyright(c) 1998-2017, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//
//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------


#include "config.h"

    double jetmin = fptbinsJetMeasA[0], jetmax = fptbinsJetMeasA[fptbinsJetMeasN];
    double jetplotmin = fptbinsJetMeasA[0], jetplotmax = fptbinsJetMeasA[fptbinsJetMeasN];

    bool isEta = 0;
    double jetEta = 0.4;

    bool savePlots = 1;
    bool bEff = 0;
    bool isptcut = 1;
    bool isdetails = 0;

    TString fReflFilename;
    TString plotsDir;
    // mass fit params

    //------- efficiency
    double *efficiency = 0x0;

    TH1F* hmass[fptbinsDN];
    TH1F *hmass_l[fptbinsDN];
    TH1F *hmass_u[fptbinsDN];
    TH1F *hmass_c[fptbinsDN];
    TF1* fullfit[fptbinsDN];
    TF1* massfit[fptbinsDN];
    TF1* bkgfit[fptbinsDN];
    TF1* bkgRfit[fptbinsDN];

    TH1F* hjetpt[fptbinsDN];
    TH1F *hjetpt_sb[fptbinsDN];
    TH1F *hjetptsub[fptbinsDN];
    TH1F *hjetptcorr[fptbinsDN];

    TH1F *hjetptspectrum;
    TH1F *hrawjetptspectrum;
    TH1F *hjetptspectrumReb;
    TH1F *hjetptspectrumRebScaled;
    TH1F *hjetptspectrumRebUnc;

    TH1F *hmean;
    TH1F *hsigma;
    TH1F *hrelErr;
    TH1F *hsign;
    TH1F *hsb;
    TH1F *hSignal;
    TH3D* hInvMassptD;
    TH1F* hReflRS;

    setHistoDetails(TH1 *h, Color_t color, Style_t Mstyle, Width_t width);
    Bool_t rawJetSpectra(TString outdir, TString prod);
    void  saveSpectraPlots(TString outdir,TString prod);
    void  saveFitParams(TString outdir,TString prod);
    Bool_t SetReflection(AliHFInvMassFitter* &fitter, Int_t iBin, Float_t fLeftFitRange, Float_t fRightFitRange,Float_t &RS);

#endif
