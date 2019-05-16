

#include "TParallelCoord.h"
#include "TParallelCoordVar.h"
#include "TROOT.h"
#include "TTree.h"
#include "TLeaf.h"
#include "THnSparse.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TFile.h"
#include "TH3.h"


TTree* toTree(THnSparse* h);
void drawsparse_draw(THnSparse* h);
void drawsparse();

void convertSparseTree(){

    TString datafile =  "~/Work/ALICE/Analysis/Results/pp5TeV/D0jet/outMC/AnalysisResults_fast_R03_D0MC_def.root";
    TFile *File = new TFile(datafile,"read");
    TDirectoryFile* dir=(TDirectoryFile*)File->Get("DmesonsForJetCorrelations");

    TList *List = dynamic_cast<TList*>(dir->Get("histosD0MBN0MCrec"));
    THnSparseF *Sparse = dynamic_cast<THnSparseF*>(List->FindObject("ResponseMatrix"));

    TFile *oFile = new TFile("~/Work/ALICE/Analysis/Results/pp5TeV/D0jet/outMC/RMTree.root","recreate");

    drawsparse_draw(Sparse);
    //return;

    TTree *Tree = nullptr;
    Tree = toTree(Sparse);

    oFile->cd();
    Tree->Write();
    oFile->Close();

    return;
}


//______________________________________________________________________________
TTree* toTree(THnSparse* h)
{
   // Creates a TTree and fills it with the coordinates of all
   // filled bins. The tree will have one branch for each dimension,
   // and one for the bin content.

   Int_t dim = h->GetNdimensions();
   TString name(h->GetName()); name += "_tree";
   TString title(h->GetTitle()); title += " tree";

   TTree* tree = new TTree(name, title);
   Double_t* x = new Double_t[dim + 1];
   memset(x, 0, sizeof(Double_t) * (dim + 1));

   TString branchname;
   for (Int_t d = 0; d < dim; ++d) {
      if (branchname.Length())
         branchname += ":";
      TAxis* axis = h->GetAxis(d);
      branchname += axis->GetName();
      branchname += "/D";
   }
   tree->Branch("coord", x, branchname);
   tree->Branch("bincontent", &x[dim], "bincontent/D");

   Int_t *bins = new Int_t[dim];
   for (Long64_t i = 0; i < h->GetNbins(); ++i) {
      x[dim] = h->GetBinContent(i, bins);
      for (Int_t d = 0; d < dim; ++d) {
         x[d] = h->GetAxis(d)->GetBinCenter(bins[d]);
      }

      tree->Fill();
   }

   delete [] bins;
   //delete [] x;
   return tree;
}


//______________________________________________________________________________
void drawsparse_draw(THnSparse* h)
{
   // Draw a THnSparse using TParallelCoord, creating a temporary TTree.

   TTree* tree = toTree(h);

   TString whatToDraw;
   TIter iLeaf(tree->GetListOfLeaves());
   const TLeaf* leaf = nullptr;
   while ((leaf = (const TLeaf*)iLeaf())) {
      if (whatToDraw.Length())
         whatToDraw += ":";
      whatToDraw += leaf->GetName();
   }
   tree->Draw(whatToDraw, "", "para");
   TParallelCoord* parallelCoord = (TParallelCoord*)gPad->GetListOfPrimitives()->FindObject("ParaCoord");

   TIter iVar(parallelCoord->GetVarList());
   TParallelCoordVar* var = nullptr;
   for (Int_t d = 0;(var = (TParallelCoordVar*) iVar()) && d < h->GetNdimensions(); ++d) {
      TAxis* axis = h->GetAxis(d);
      var->SetHistogramBinning(axis->GetNbins());
      var->SetCurrentLimits(axis->GetXmin(), axis->GetXmax());
      var->SetTitle(axis->GetTitle());
   }
   var->SetTitle("bin content");
}

//______________________________________________________________________________
void drawsparse()
{
   // create a THnSparse and draw it.

#ifdef __CINT__
   printf("For performance reasons we advise to run \".x drawsparse.C+\"\n");
#endif

   const Int_t ndims = 8;
   Int_t bins[ndims] = {10, 10, 5, 30, 10, 4, 18, 12};
   Double_t xmin[ndims] = {-5., -10., -1000., -3., 0.,   0., 0., 0.};
   Double_t xmax[ndims] = {10., 70., 3000.,   3.,   5.,  2., 2., 5.};
   THnSparse* hs = new THnSparseD("hs", "Sparse Histogram", ndims, bins, xmin, xmax);
   
   // fill it
   Double_t x[ndims];
   for (Long_t i = 0; i < 100000; ++i) {
      for (Int_t d = 0; d < ndims; ++d) {
         switch (d) {
         case 0: x[d] = gRandom->Gaus()*2 + 3.; break;
         case 1:
         case 2: 
         case 3: x[d] = (x[d-1]*x[d-1] - 1.5)/1.5 + (0.5*gRandom->Rndm()); break;
         default: x[d] = sin(gRandom->Gaus()*i/1000.) + 1.;
         }
      }
      hs->Fill(x);
   }


   TFile* f = new TFile("drawsparse.root","RECREATE");

   TCanvas* canv = new TCanvas("hDrawSparse", "Drawing a sparse hist");
   canv->Divide(2);

   // draw it
   canv->cd(1);
   drawsparse_draw(hs);

   // project it
   canv->cd(2);
   TH3D* h3proj = hs->Projection(2, 3, 6);
   h3proj->SetLineColor(kOrange);
   h3proj->SetDirectory(nullptr);
   h3proj->Draw("lego1");

   // save everything to a file
   canv->Write();
   hs->Write();
   h3proj->Write();

   delete f;
}
