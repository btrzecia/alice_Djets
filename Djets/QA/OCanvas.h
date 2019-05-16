#ifndef OCANVAS_H
#define OCANVAS_H
#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include "TFile.h"
#include "TCollection.h"
#include "TKey.h"
#include "TObject.h"
#include "TClass.h"
#include "TTree.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLeaf.h"
#include "AliEmcalList.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TSystem.h"
#include "THStack.h"
#include "TLegend.h"
#include "TRatioPlot.h"
#include "TVirtualHistPainter.h"
#include "TStyle.h"

class OCanvas {
public:
    OCanvas(){}
    ~OCanvas(){}
    void OCreate(TString p, Int_t x, Int_t y);
    bool OGetCanvas(TString p){return (TCanvas_.find(p)!=TCanvas_.end())?  true: false;}
    void OPlace(TString p, TH2 *h, Int_t pad, TString opt = "");
    void OPlace(TString p, TH1* h, Int_t pad, TString opt = "",TString stat ="");
    void OPlaceRatio(TString p, TH1* h1, TH1* h2, Int_t pad, TString opt = "");
    void OPlace(TString p, TLegend *l, Int_t pad, TString opt = "");
    void OForceStack(TString p, Int_t pad, Double_t x_min, Double_t x_max);
    void OFlush(TString folder,TString sufix);
    void ODraw(TString p);
    void OLegend(TString p, TH1* h,TString name);
    TLegend* OGetLegend(TString p);
private:
    std::map<TString,TCanvas*> TCanvas_;
    std::map<TString,TH1*> TH1dummy_;
    std::map<TString,THStack*> THStack_;
    std::map<TString,TLegend*> TLegend_;
    std::map<TString,TRatioPlot*> TRatioPlot_;
};
#endif

