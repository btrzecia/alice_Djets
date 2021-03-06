void Directory_Name_merge(const char *dir, Int_t stage=0)
{
// Automatically generated merging macro executed in grid subjobs

   TStopwatch timer;
   timer.Start();

// Reset existing include path and add current directory first in the search
   gSystem->SetIncludePath("-I.");
// load base root libraries
   gSystem->Load("libTree");
   gSystem->Load("libGeom");
   gSystem->Load("libVMC");
   gSystem->Load("libPhysics");

   gSystem->Load("libMinuit");

// Load analysis framework libraries
   gSystem->Load("libSTEERBase");
   gSystem->Load("libESD");
   gSystem->Load("libAOD");
   gSystem->Load("libANALYSIS");
   gSystem->Load("libANALYSISalice");
   gSystem->Load("libOADB");
   gSystem->Load("libCORRFW");

// include path
   TString intPath = gInterpreter->GetIncludePath();
   TObjArray *listpaths = intPath.Tokenize(" ");
   TIter nextpath(listpaths);
   TObjString *pname;
   while ((pname=(TObjString*)nextpath())) {
      TString current = pname->GetName();
      if (current.Contains("AliRoot") || current.Contains("ALICE_ROOT")) continue;
      gSystem->AddIncludePath(current);
   }
   if (listpaths) delete listpaths;
   gSystem->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include -I$ALICE_PHYSICS/PWG/EMCAL -I$ALICE_ROOT/EMCAL -I$ALICE_ROOT/STEER/STEER -I$ALICE_ROOT/STEER/STEERBase -I$ALICE_ROOT/STEER/ESD -I$ALICE_ROOT/STEER/AOD -I$ALICE_ROOT/ANALYSIS/ -I$ALICE_ROOT/ANALYSIS/Tender -I$ALICE_ROOT/ANALYSIS/TenderSupplies -I$ALICE_ROOT/ANALYSIS/ESDfilter -I$ALICE_PHYSICS/OADB -I$ALICE_PHYSICS/PWGHF -I$ALICE_PHYSICS/PWGHF/base -I$ALICE_PHYSICS/PWGHF/vertexingHF -I$ALICE_PHYSICS/PWG/FLOW/Base -I$ALICE_PHYSICS/PWG/FLOW/Tasks  -I$ALICE_PHYSICS/PWGJE  -I$ALICE_PHYSICS/JETAN -I$ALICE_PHYSICS/PWGJE/EMCALJetTasks -I$ALICE_PHYSICS/PWGJE/FlavourJetTasks -I$ALICE_PHYSICS/PWGJE/EMCALJetTasks/UserTasks -g ");
   gROOT->ProcessLine(".include $ALICE_ROOT/include");
   printf("Include path: %s\n", gSystem->GetIncludePath());

// Add aditional AliRoot libraries

// Analysis source to be compiled at runtime (if any)

// Connect to AliEn
   if (!TGrid::Connect("alien://")) return;
// Set temporary merging directory to current one
   gSystem->Setenv("TMPDIR", gSystem->pwd());

// Set temporary compilation directory to current one
   gSystem->SetBuildDir(gSystem->pwd(), kTRUE);

   TString outputDir = dir;
   TString outputFiles = "EventStat_temp.root,AnalysisResults.root";
   TString mergeExcludes = " ";
   TObjArray *list = outputFiles.Tokenize(",");
   TIter *iter = new TIter(list);
   TObjString *str;
   TString outputFile;
   Bool_t merged = kTRUE;
   AliAnalysisManager *mgr = AliAnalysisAlien::LoadAnalysisManager("Directory_Name.root");
   if (!mgr) {
      printf("ERROR: Analysis manager could not be extracted from file ");
      return;
   }
   while((str=(TObjString*)iter->Next())) {
      outputFile = str->GetString();
      if (outputFile.Contains("*")) continue;
      Int_t index = outputFile.Index("@");
      if (index > 0) outputFile.Remove(index);
      // Skip already merged outputs
      if (!gSystem->AccessPathName(outputFile)) {
         printf("Output file <%s> found. Not merging again.\n",outputFile.Data());
         continue;
      }
      if (mergeExcludes.Contains(outputFile.Data())) continue;
      merged = AliAnalysisAlien::MergeOutput(outputFile, outputDir, 100, stage);
      if (!merged) {
         printf("ERROR: Cannot merge %s\n", outputFile.Data());
         return;
      }
   }
   // all outputs merged, validate
   ofstream out;
   out.open("outputs_valid", ios::out);
   out.close();
   // read the analysis manager from file
   if (!outputDir.Contains("Stage")) return;
   mgr->SetRunFromPath(mgr->GetRunFromAlienPath(dir));
   mgr->SetSkipTerminate(kFALSE);
   mgr->PrintStatus();
   gEnv->SetValue("XNet.Debug", "1");
   TTree *tree = NULL;
   mgr->StartAnalysis("gridterminate", tree);
}

