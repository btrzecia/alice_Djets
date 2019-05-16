// runEMCalJetAnalysis.C
// =====================
// This macro can be used to run a jet analysis within the EMCal Jet Framework.
//
// Examples:
// -> Analyze ESDs from the pA pilot run on the AliEn grid with your task in AnaClass.cxx/.h
//     dataType = "ESD", useGrid = kTRUE, pattern = "*ESDs/pass2/*ESDs.root", addCXXs = "AnaClass.cxx",
//     addHs = "AnaClass.h", gridDir = "/alice/data/2012/LHC12g", gridMode = "full", runNumbers = "188359 188362"
//
// -> Analyze AODs (up to 96 files) locally given in files_aod.txt
//     dataType = "AOD", useGrid = kFALSE, numLocalFiles = 96
//
// MERGING ON ALIEN
// ++++++++++++++++
// If you run on the grid, you can monitor the jobs with alimonitor.cern.ch. When enough of them are in DONE state,
// you have to merge the output. This can be done automatically, if you just change the gridMode to "terminate" and
// give the EXACT name of the task whose output should be merged in uniqueName.
//
//


//LHC16k4
////Runlist HadronPID: 246994 246991 246989 246984 246982 246948 246945 246928 246851 246847 246846 246845 246844 246810 246809 246808 246807 246805 246804 246766 246765 246763 246760 246759 246758 246757 246751 246750 246495 246493 246488 246487 246434 246431 246424 246276 246275 246272 246271 
//246225 246222 246217 246185 246182 246181 246180 246178 246153 246152 246151 246148 246115 246113 246089 246087 246053 246052 246049 246048 246042 246037 246036 246012 246003 246001 245963 245954 245952 245949 245923 245833 245831 245829 245705 245702 245692 245683

#include <ctime>
#include "TGrid.h"

AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers, const Int_t nrunspermaster,
                                    const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker,
                                    Int_t workerTTL, Bool_t isMC);

//______________________________________________________________________________
void AnalysisTrainCorrJetsLocal (
                                 const char*    dataType            = "AOD",                       // set the analysis type, AOD, ESD or sESD
                                 Bool_t         useGrid             = kTRUE,                      // local or grid
                                 TString localfilename = "files_aod.txt",
                                 const char*    gridMode            = "test",                      // set the grid run mode (can be "full", "test", "offline", "submit" or "terminate")
                                 const char*    pattern             = "/AOD/*/AliAOD.root", //"/AOD149/*/AliAOD.root", "/ESDs/pass2/AOD145/*/AliAOD.root", pp: /*/AOD149/*/AliAOD.root   // file pattern (here one can specify subdirs like passX etc.) (used on grid)
                                 const char*    gridDir             = "/alice/sim/2016/LHC16k4/1", ///alice/sim/2012/LHC12a17a_fix  /alice/data/2011/LHC11h_2
                                 const char*    runNumbers          = "246994 246991 246989 246984",
                                 const Int_t nrunspermaster= 100,
                                 UInt_t         numLocalFiles       = 2,                          // number of files analyzed locally
                                 const char*    runPeriod           = "lhc15o",                    // set the run period (used on grid)
                                 const char*    uniqueName          = "Directory_Name",     // sets base string for the name of the task on the grid
                                 UInt_t         pSel                = AliVEvent::kINT7,             // used event selection for every task except for the analysis tasks
                                 Bool_t         useTender           = kFALSE,                      // trigger, if tender task should be used
                                 Bool_t         isMC                = kTRUE,                      // trigger, if MC handler should be used
                                 Bool_t         isReco                = kTRUE,
                                 Bool_t         isMap                = kTRUE,
                                 // Here you have to specify additional code files you want to use but that are not in aliroot
                                 const char*    addCXXs             = "",
                                 const char*    addHs               = "",
                                 // These two settings depend on the dataset and your quotas on the AliEN services
                                 Int_t          maxFilesPerWorker   = 1,
                                 Int_t          workerTTL           = 86000,
                                 Int_t          nfiletestmode       = 2
                                 )
{
    
    // Some pre-settings and constants
    TStopwatch watch;
    watch.Start();
    
    enum AlgoType {kKT, kANTIKT};
    enum JetType  {kFULLJETS, kCHARGEDJETS, kNEUTRALJETS};
    gSystem->SetFPEMask();
    gSystem->Setenv("ETRAIN_ROOT", ".");
    gSystem->Setenv("ETRAIN_PERIOD", runPeriod);
    // change this objects to strings
    TString usedData(dataType);
    TString additionalCXXs(addCXXs);
    TString additionalHs(addHs);
    cout << dataType << " analysis chosen" << endl;
    if (useGrid)
    {
        cout << "-- using AliEn grid.\n";
        if (usedData == "sESD")
        {
            cout << "Skimmed ESD analysis not available on the grid!" << endl;
            return;
        }
    }
    else
        cout << "-- using local analysis.\n";
    
    
    // Load necessary libraries
    LoadLibs();
    
    // Create analysis manager
    AliAnalysisManager* mgr = new AliAnalysisManager(uniqueName);
    
    // Check type of input and create handler for it
    TString localFiles("-1");
    if(usedData == "AOD")
    {
        localFiles = localfilename;
        gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C");
        AliAODInputHandler* aodH = AddAODHandler();
    }
    else if((usedData == "ESD") || (usedData == "sESD"))
    {
        if (usedData == "ESD")
            localFiles = "files_esd.txt";
        else
            localFiles = "files_sesd.txt";
        
        gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
        AliESDInputHandler* esdH = AddESDHandler();
    }
    else
    {
        cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n";
    }
    
    if(!useGrid)
        cout << "Using " << localFiles.Data() << " as input file list.\n";
    
    TGrid::Connect("alien://");
    
  
    gROOT->LoadMacro("AddTasksFlavourJet.C");
    
    //AddTasksFlavourJet(0,"alien:///alice/cern.ch/user/b/btrzecia/PbPb2015/D0Cuts/D0toKpiCuts_PbPb2015_0_20_setB.root",0.,0.557,"TPCFID",runPeriod,0,pSel,isMC,isReco,isMap,"DJet",kTRUE);
    //AddTasksFlavourJet(0,"D0toKpiCuts_PbPb2015_0_20_setB_MC.root",0.,0.557,"TPCFID",runPeriod,0,pSel,isMC,isReco,isMap,"DJet",kTRUE);
    AddTasksFlavourJet(0,"alien:///alice/cern.ch/user/a/anolivei/Cuts/D0toKpiCuts_pp_veryopen.root",0.,0.557,"TPCFID",runPeriod,0,pSel,isMC,isReco,isMap,"DJet",kTRUE);
    
    
    // Set the physics selection for all given tasks
    TObjArray *toptasks = mgr->GetTasks();
    for (Int_t i=0; i<toptasks->GetEntries(); ++i)
    {
        AliAnalysisTaskSE *task = dynamic_cast<AliAnalysisTaskSE*>(toptasks->At(i));
        if (!task)
            continue;
        if (task->InheritsFrom("AliPhysicsSelectionTask"))
            continue;
        ::Info("setPSel", "Set physics selection for %s (%s)", task->GetName(), task->ClassName());
        task->SelectCollisionCandidates(pSel);
    }
    
    if(gridMode=="full") mgr->SetUseProgressBar(1, 25);
    

    if (!mgr->InitAnalysis())
        return;
    mgr->PrintStatus();
    
    if (useGrid)
    {  // GRID CALCULATION
        
        AliAnalysisGrid *plugin = CreateAlienHandler(uniqueName, gridDir, gridMode, runNumbers, nrunspermaster, pattern, additionalCXXs, additionalHs, maxFilesPerWorker, workerTTL, isMC);
        plugin->SetNtestFiles(nfiletestmode);
        
        mgr->SetGridHandler(plugin);
        
        // start analysis
        cout << "Starting GRID Analysis...";
        if(gridMode=="test") mgr->SetDebugLevel(10);
        else mgr->SetDebugLevel(0);
        mgr->StartAnalysis("grid");
    }
    else
    {  // LOCAL CALCULATION
        
        TChain* chain = 0;
        if (usedData == "AOD")
        {
            Printf("Run Create AOD Chain");
            gROOT->LoadMacro("/data/Work/jets/testEMCalJetFramework/AODchainWithFriend/CreateAODChain.C");
            chain = CreateAODChain(localFiles.Data(), numLocalFiles,0,kTRUE,kTRUE);
            //Printf("Chain Friend has %d entries", ((TTree*)chain->GetFriend())->GetEntriesFast());
        }
        else
        {  // ESD or skimmed ESD
            gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateESDChain.C");
            chain = CreateESDChain(localFiles.Data(), numLocalFiles);
        }
        
        // start analysis
        cout << "Starting LOCAL Analysis...";
        mgr->SetDebugLevel(10);
        mgr->StartAnalysis("local", chain);
    }
    watch.Stop();
    watch.Print();
}

//______________________________________________________________________________
void LoadLibs()
{
    // Load common libraries (better too many than too few)
 
    //gSystem->Load("libpythia6");
    //gSystem->Load("libEGPythia6");
    //gSystem->Load("liblhapdf");
    //gSystem->Load("libAliPythia6");
    
    gSystem->Load("libTree");
    gSystem->Load("libGeom");
    gSystem->Load("libVMC");
    gSystem->Load("libPhysics");
    // Add aditional libraries
    gSystem->Load("libTree");
    gSystem->Load("libMinuit");
    gSystem->Load("libProof");
    gSystem->Load("libSTEERBase");
    gSystem->Load("libESD");
    gSystem->Load("libAOD");
    // Load analysis framework libraries
    gSystem->Load("libSTEERBase");
    gSystem->Load("libESD");
    gSystem->Load("libAOD");
    gSystem->Load("libANALYSIS");
    gSystem->Load("libANALYSISalice");
    gSystem->Load("libOADB");
    gSystem->Load("libCORRFW");
    gSystem->Load("libCGAL");
    gSystem->Load("libfastjet");
    gSystem->Load("libsiscone");
    gSystem->Load("libsiscone_spherical");
    gSystem->Load("libfastjetplugins");
    gSystem->Load("libfastjettools");
    gSystem->Load("libfastjetcontribfragile");
    // Add aditional AliRoot libraries
/*    
gSystem->Load("libCORRFW");
    gSystem->Load("libPWGflowBase");
    gSystem->Load("libPWGflowTasks");
    gSystem->Load("libPWGHFbase");
    gSystem->Load("libPWGHFvertexingHF");
    gSystem->Load("libCORRFW");
    gSystem->Load("libPWGflowBase");
    gSystem->Load("libPWGflowTasks");
    gSystem->Load("libPWGHFbase");
    gSystem->Load("libPWGHFvertexingHF");
  */  
       // include paths
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_PHYSICS -I$ALICE_PHYSICS/include -I$ALICE_ROOT/EMCAL -I$ALICE_PHYSICS/PWG/EMCAL -I$ALICE_PHYSICS/PWGJE -I$ALICE_PHYSICS/PWGJE/EMCALJetTasks -I$ALICE_PHYSICS/PWGJE/EMCALJetTasks/UserTasks -I$ALICE_ROOT/ANALYSIS/Tender -I$ALICE_ROOT/ANALYSIS/TenderSupplies");
    gSystem->AddIncludePath(" -I$ROOTSYS/include -I$ALICE_PHYSICS/PWGHF/ -I$ALICE_PHYSICS/PWGHF/base -I$ALICE_PHYSICS/PWGHF/vertexingHF -I$ALICE_PHYSICS/PWGJE/FlavourJetTasks");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/EMCAL -I$ALICE_PHYSICS/EMCAL -I$ALICE_PHYSICS");
    gSystem->AddIncludePath("-I$ALICE_PHYSICS/PWGDQ/dielectron -I$ALICE_PHYSICS/PWGHF/hfe");
    gSystem->AddIncludePath("-I$ALICE_PHYSICS/JETAN -I$ALICE_PHYSICS/JETAN/fastjet");}
AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers, const Int_t nrunspermaster,
                                    const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker,
                                    Int_t workerTTL, Bool_t isMC)
{
    TDatime currentTime;
    TString tmpName(uniqueName);
   
    TString tmpAdditionalLibs("");
    // tmpAdditionalLibs = Form("libPythia6.so libTree.so libGeom.so libVMC.so libPhysics.so libMinuit.so libMinuit2.so libProof.so libSTEERBase.so libESD.so libAOD.so libOADB.so libANALYSIS.so libCDB.so libRAWDatabase.so libSTEER.so libEVGEN.so libANALYSISalice.so libCORRFW.so libESDfilter.so  libTENDER.so libSTAT.so libPWGTools.so libPWGHFbase.so libPWGflowBase.so libPWGflowTasks.so libPWGHFvertexingHF.so libEMCALUtils.so libPHOSUtils.so libPWGCaloTrackCorrBase.so libEMCALraw.so libEMCALbase.so libEMCALrec.so libTRDbase.so libVZERObase.so libVZEROrec.so libTENDER.so libTENDERSupplies.so libPWGEMCAL.so libPWGGAEMCALTasks.so libPWGTools.so libPWGCFCorrelationsBase.so libPWGCFCorrelationsDPhi.so libCGAL.so libfastjet.so libsiscone.so libsiscone_spherical.so libfastjetplugins.so libfastjettools.so libfastjetcontribfragile.so libPWGJE.so libPWGmuon.so libPWGJEEMCALJetTasks.so libPWGJEFlavourJetTasks.so %s %s",additionalCode.Data(),additionalHeaders.Data());
   
    tmpAdditionalLibs = Form("%s %s",additionalCode.Data(),additionalHeaders.Data());
 
    TString macroName("");
    TString execName("");
    TString jdlName("");
    macroName = Form("%s.C", tmpName.Data());
    execName = Form("%s.sh", tmpName.Data());
    jdlName = Form("%s.jdl", tmpName.Data());
    
    AliAnalysisAlien *plugin = new AliAnalysisAlien();
    plugin->SetOverwriteMode();
    plugin->SetRunMode(gridMode);
    
    // Here you can set the (Ali)ROOT version you want to use
    plugin->SetAPIVersion("V1.1x");
    //plugin->SetROOTVersion("v5-34-30-1");
    //plugin->SetAliROOTVersion("v5-06-38");
    plugin->SetAliPhysicsVersion("vAN-20190219-1");
    plugin->SetGridDataDir(gridDir); // e.g. "/alice/sim/LHC10a6"
    plugin->SetDataPattern(pattern); //dir structure in run directory
    plugin->SetFriendChainName("./AliAOD.VertexingHF.root");
    if (!isMC)
        plugin->SetRunPrefix("000");
    
    plugin->AddRunList(runNumbers);
    plugin->SetNrunsPerMaster(nrunspermaster);
    
    plugin->SetGridWorkingDir(Form("%s",tmpName.Data()));
    plugin->SetGridOutputDir("output");
    
    plugin->SetAnalysisSource(additionalCode.Data());
    plugin->SetAdditionalLibs(tmpAdditionalLibs.Data());
    plugin->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include -I$ALICE_PHYSICS/PWG/EMCAL -I$ALICE_ROOT/EMCAL -I$ALICE_ROOT/STEER/STEER -I$ALICE_ROOT/STEER/STEERBase -I$ALICE_ROOT/STEER/ESD -I$ALICE_ROOT/STEER/AOD -I$ALICE_ROOT/ANALYSIS/ -I$ALICE_ROOT/ANALYSIS/Tender -I$ALICE_ROOT/ANALYSIS/TenderSupplies -I$ALICE_ROOT/ANALYSIS/ESDfilter -I$ALICE_PHYSICS/OADB -I$ALICE_PHYSICS/PWGHF -I$ALICE_PHYSICS/PWGHF/base -I$ALICE_PHYSICS/PWGHF/vertexingHF -I$ALICE_PHYSICS/PWG/FLOW/Base -I$ALICE_PHYSICS/PWG/FLOW/Tasks  -I$ALICE_PHYSICS/PWGJE  -I$ALICE_PHYSICS/JETAN -I$ALICE_PHYSICS/PWGJE/EMCALJetTasks -I$ALICE_PHYSICS/PWGJE/FlavourJetTasks -I$ALICE_PHYSICS/PWGJE/EMCALJetTasks/UserTasks -g");
    

    /*
    plugin->AddExternalPackage("boost::v1_53_0");
    plugin->AddExternalPackage("cgal::v4.4");
    plugin->AddExternalPackage("fastjet::v3.0.6_1.012");
    */
    plugin->SetDefaultOutputs(kTRUE);
    // merging via jdl
    plugin->SetMergeViaJDL(kTRUE);
    plugin->SetOneStageMerging(kFALSE);
    plugin->SetMaxMergeStages(2);
    
    //plugin->SetMergeExcludes("");
    plugin->SetAnalysisMacro(macroName.Data());
    plugin->SetSplitMaxInputFileNumber(maxFilesPerWorker);
    plugin->SetExecutable(execName.Data());
    plugin->SetTTL(workerTTL);
    plugin->SetInputFormat("xml-single");
    plugin->SetJDLName(jdlName.Data());
    plugin->SetPrice(1);      
    plugin->SetSplitMode("se");
    
    return plugin;
}
