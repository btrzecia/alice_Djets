#ifndef OLOADER_H
#define OLOADER_H
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
#include "AliNormalizationCounter.h"


class OLoader {
public:
    OLoader(const TString &fileName, bool fusion, bool debug = false);
    ~OLoader(){}
    void OLoad(TObject *obj, TString p);
    void OLoadTList(TObject *obj, TString p);
  //  void OLoadTTree(TObject *obj, TString p);
    void OLoadTHX(TObject *obj, TString p);
    void OLoadTDirectory(TObject *obj, TString p);
    void OLoadSparse(TObject *obj, TString p);
    void OLoadNorm(TObject *obj);
    void OLoadPrint();
    //TString OGetName(){return className_;}
    TH1* OGetHisto1(TString p);
    TH2* OGetHisto2(TString p);
    TH3* OGetHisto3(TString p);
    THnSparse* OGetSparse(TString p);
    void OProjectSparse(TString p, Int_t axis);
    void OCutSparse(TString p, Int_t axis, Double_t min, Double_t max);
    void OSetX(TString p, Double_t x_min, Double_t x_max);
    void OSetXf(TString p, Double_t x_min, Double_t x_max);
    void OSetY(TString p, Double_t y_min, Double_t y_max);
    void OSetNameSpace(TString p){nameSpace_ = p;}
    void OSetDataName(TString p){dataName_ = p;}
    TString OGetDataName(){return dataName_;}
    Double_t OGetNevents(){return Nevents_;}
    void OSetColor(TString p, Int_t c, Float_t m);
    void ODo1DprojectionX(TString p, Double_t *ydiv, UInt_t nbins);
    void ODo1DprojectionY(TString p, Double_t *xdiv, UInt_t nbins);
    void OSetProp(TH1* h, TString scale, Int_t rebin, Double_t weight=0);
   // void OPrepare(TString name,TString type, TString bins, TString leaf1);// 1D histogram
   // void OPrepare(TString name,TString type, TString bins, TString leaf1, TString leaf2); // 2D histogram
   // void OPrepare(TString name,TString type, TString bins, TString leaf1, TString leaf2, TString leaf3); // 3D histogram
    //void OConstruct();
    //void OFill();
    //void OSetGlobalCut(TString variablename, Double_t low, Double_t high);
    //void OUseTreeNamespace(TString pathname){treeNamespace_ = pathname;}
private:
    TFile *TFile_;
    TString nameSpace_= "";
    TString dataName_= "";
    //TString
    bool errorTerminate_ = false;
    bool debug_ = false;
    bool fusion_ = false;
    Double_t Nevents_ = 0;
    //TTree *m_tree;
    std::map<TString,TTree*> TTree_;
    std::map<TString,TLeaf*> TLeaf_;
    std::map<TString,TH1F*> TH1F_;
    std::map<TString,TH1D*> TH1D_;
    std::map<TString,TH1I*> TH1I_;
    std::map<TString,TH2F*> TH2F_;
    std::map<TString,TH2D*> TH2D_;
    std::map<TString,TH2I*> TH2I_;
    std::map<TString,TH3F*> TH3F_;
    std::map<TString,TH3D*> TH3D_;
    std::map<TString,TH3I*> TH3I_;
    std::map<TString,THnSparseF*> THnSparseF_;
    //std::map<TString,std::vector<PrepareCont>> PrepareContainer_;
    //std::vector<GlobalCut> GlobalCut_;
    //std::map<TString,auto> m_TTree_var;
    //ClassDef(OLoader, 1);

};
#endif

