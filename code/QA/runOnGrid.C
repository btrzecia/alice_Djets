#if !defined (__CINT__) || defined (__CLING__)
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAODInputHandler.h"

#include "AliPhysicsSelectionTask.h"
#include "AliMultSelectionTask.h"
#include "AliAnalysisTaskPIDResponse.h"

#include "AliAnalysisTaskSEDs.h"
#include "AliAnalysisTaskSED0Mass.h"
#include "AliEmcalJetTask.h"

#include "TChain.h"

#include "AliAnalysisTaskDmesonsFilterCJ.h"
#include "AliAnalysisTaskDJetCorrelationsQA.h"
//#include "AddTasksFlavourJetMCQA.C"
#endif

//SETTINGS
//************************************

Bool_t runLocal=kFALSE;                                  // flag to run locally on AliAOD.root + AliAOD.VertexingHF.root
TString pathToLocalAODfiles="../..";               // path to find AOD files when running locally
Bool_t runGridTest=kTRUE;                               // flag to run a grid test: kTRUE (+runLocal=kFALSE). To run job on GRID: runGridTest=kFALSE, runLocal=kFALSE
TString runMode="full";                                  // sets the run grid mode: "full", "terminate"
Int_t maxFilesPerWorker = 20;
Int_t workerTTL = 7200;
Int_t nfiletestmode = 4;

TString aliPhysVersion="vAN-20190425_ROOT6-1";

Bool_t isRunOnMC= kTRUE;                //MC 1 D 0                 // set to kTRUE to run on Mone Carlo and uncomment/comment accordingly the following lines about paths on Alien
Bool_t isReco   = kTRUE;                //mc 1 dD1
//paths on Alien
// Monte Carlo
//TString gridDataDir="/alice/sim/2018/LHC18a4a2_fast/";
//TString gridDataPattern="AOD";
// Data
//TString gridDataDir="/alice/sim/2018/LHC18a4b2_fast/"; //old
TString gridDataDir="/alice/sim/2019/LHC19c5a/1/";
TString gridDataPattern="AOD";


// Alien output director
TString gridWorkingDir="outputMC_LHC19c5a_0";
TString gridOutputDir="output";

//run numbers
const Int_t nruns = 1;
//Int_t runlist[nruns] = {282367}; //old
Int_t runlist[nruns] = {296623}; // new prod

//Task configuration
TString cutFile="D0toKpiCuts_pp13TeV_pPbBase_newNorm.root";          // file containing the cuts for the different mesons
const char*    runPeriod           = "LHC18q";                    // set the run period (used on grid)
const char*    uniqueName          = "QATest";           			// sets base string for the name of the task on the grid
UInt_t         pSel                = AliVEvent::kINT7;             // used event selection for every task except for the analysis tasks
Bool_t         isMap               = kFALSE;
Bool_t         bRM                 = kFALSE;
Bool_t         bRMEff              = kTRUE;
Bool_t         bPythia             = kTRUE;
Bool_t			bPythiaMult			= kTRUE;
Bool_t         isPrompt            = kTRUE;
const Double_t aRadius[] = {  0.3  };


//************************************


void runOnGrid()
{
    // set if you want to run the analysis locally (kTRUE), or on grid (kFALSE)
    Bool_t local = runLocal;
    // if you run on grid, specify test mode (kTRUE) or full grid model (kFALSE)
    Bool_t gridTest = runGridTest;

    // since we will compile a class, tell root where to look for headers
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
#else
    gROOT->ProcessLine(".include $ROOTSYS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/include");
#endif

    // create the analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager(uniqueName);
    AliAODInputHandler *aodH = new AliAODInputHandler();
    mgr->SetInputEventHandler(aodH);


    // compile the class and load the add task macro
    // here we have to differentiate between using the just-in-time compiler
    // from root6, or the interpreter of root5
#if !defined (__CINT__) || defined (__CLING__)

    AliPhysicsSelectionTask *physseltask = reinterpret_cast<AliPhysicsSelectionTask *>(gInterpreter->ProcessLine(Form(".x %s (%d)", gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C"),isRunOnMC)));

    AliAnalysisTaskPIDResponse *pidResp = reinterpret_cast<AliAnalysisTaskPIDResponse *>(gInterpreter->ProcessLine(Form(".x %s (%d)", gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"),isRunOnMC)));
    //gROOT->LoadMacro("AliAnalysisTaskSEDmesonsFilterCJQA.cxx++g");
   // gROOT->LoadMacro("AliAnalysisTaskDJetCorrelationsQA.cxx++g");

    //currently failing when i>1 at AliAnalysisTaskSEDmesonsFilterCJQA *filter = reinterpret_cast<AliAnalysisTaskSEDmesonsFilterCJQA*>(gInterpreter->ProcessLine
    //dont even enter AddTask...error somewhere in preprocessor???

    for(Int_t i=0; i<1  ; i++){
        TString TaskText = "MB";
        TaskText += Form("N%d",i);
        //Filtering task. It will create a set of particles with the D meson instead of the daughters

    //                        gInterpreter->ProcessLine(Form(".x %s (%d)",
    //                        gSystem->ExpandPathName("AddTasksFlavourJetMCQA.C"),0);

        AliAnalysisTaskDmesonsFilterCJ *filter = reinterpret_cast<AliAnalysisTaskDmesonsFilterCJ*>(
                    gInterpreter->ProcessLine(Form(".x %s ((AliAnalysisTaskDmesonsFilterCJ::kD0toKpi),\"%s\",%d,%d,\"%s\")",
                    gSystem->ExpandPathName("$ALICE_PHYSICS/PWGHF/jetsHF/macros/AddTaskDmesonsFilterCJ.C"),
                    cutFile.Data(),
                    isRunOnMC,
                    isReco,
                    TaskText.Data())));


        filter->SetCombineDmesons(kTRUE);
        filter->SetMultipleCandidates(kTRUE); //Analyse one candidate per event
        filter->SetAnalysedCandidate(0); //Number of the candidate that will be analysed (0 = first candidate)

        if(isPrompt){
                filter->SetRejectDfromB(kTRUE);
                filter->SetKeepOnlyDfromB(kFALSE);
        }
        else{
                filter->SetRejectDfromB(kFALSE);
                filter->SetKeepOnlyDfromB(kTRUE);
        }

        filter->SetBuildRMEff(bRMEff);
        filter->SetUsePythia(bPythia);
        filter->SetUseMultPythia(bPythiaMult);
        filter->SetPythiaEvent(i);
        // set filter bits if needed

       //This is the particle container with the tracks of the event
        AliTrackContainer* trackCont1 = filter->AddTrackContainer("tracks");
        trackCont1->SetClassName("AliAODTrack");
        trackCont1->SetTrackFilterType(AliEmcalTrackSelection::kCustomTrackFilter);
        trackCont1->SetAODFilterBits((1<<4)|(1<<9));

        AliMCParticleContainer* trackContMC = filter->AddMCParticleContainer("mcparticles");
        trackContMC->SetClassName("AliAODMCParticle");

        TString candArrName = "Dcandidates";
        TString sbArrName = "DSBcandidates";
        TString DcandAndTracks = "DcandidatesAndTracks";
        TString DBkgAndTracks = "DSBcandidatesAndTracks";
        TString MCDcandAndTracks = "MCDcandidatesAndTracks";
        TString candName;
        candName = "D0";

        candArrName += candName;
        candArrName += TaskText;
        sbArrName += candName;
        sbArrName += TaskText;
        DcandAndTracks += candName;
        DcandAndTracks += TaskText;
        DBkgAndTracks += candName;
        DBkgAndTracks += TaskText;
        MCDcandAndTracks += candName;
        MCDcandAndTracks += TaskText;

        if (isRunOnMC) {
            candArrName += "MC";
            sbArrName  += "MC";
            DcandAndTracks += "MC";
            DBkgAndTracks += "MC";
            MCDcandAndTracks += "MC";
            if (isReco)
            {
                candArrName += "rec";
                sbArrName += "rec";
                DcandAndTracks += "rec";
                DBkgAndTracks += "rec";
                MCDcandAndTracks += "rec";

            }
        }

        TString AKTJet = "AKTJet";
        AKTJet += TaskText;

        //AliEmcalJetTask *taskFJDandTracks = AddTaskEmcalJet(DcandAndTracks,"",AliJetContainer::antikt_algorithm,aRadius[0],AliJetContainer::kFullJet,0.15,0.30,0.005,AliJetContainer::pt_scheme,AKTJet,0.,kFALSE,kFALSE);
        AliEmcalJetTask *taskFJDandTracks =reinterpret_cast<AliEmcalJetTask*>(gInterpreter->ProcessLine(Form(".x %s (\"%s\",\"%s\",(AliJetContainer::EJetAlgo_t)%d,%f,(AliJetContainer::EJetType_t)%d,%f,%f,%f,(AliJetContainer::ERecoScheme_t)%d,\"%s\",%f,%d,%d)",gSystem->ExpandPathName("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C"),DcandAndTracks.Data(),"",AliJetContainer::EJetAlgo_t::antikt_algorithm,aRadius[0],AliJetContainer::EJetType_t::kFullJet,0.15,0.30,0.005,AliJetContainer::ERecoScheme_t::pt_scheme,AKTJet.Data(),0.,kFALSE,kFALSE)));
        taskFJDandTracks->SelectCollisionCandidates(pSel);
        taskFJDandTracks->SetNeedEmcalGeom(kFALSE);

        //"DcandidatesAndTracksD0MBN0","",AliJetContainer::EJetAlgo_t::antikt_algorithm,0.4,AliJetContainer::kFullJet,0.15,0.30,0.005,AliJetContainer::ERecoScheme_t::pt_scheme,"AKTJetMBN0",0.,kFALSE,kFALSE

        //Generated level jets. No pT>0.15 GeV/c cut should be applied here!
    //    AliEmcalJetTask *taskFJMCDandTracks = AddTaskEmcalJet(MCDcandAndTracks,"",AliJetContainer::antikt_algorithm,aRadius[0],AliJetContainer::kFullJet,0.0,0.0,0.005,AliJetContainer::pt_scheme,AKTJet,0.,kFALSE,kFALSE);
   //data comment
            AliEmcalJetTask *taskFJMCDandTracks =reinterpret_cast<AliEmcalJetTask*>(gInterpreter->ProcessLine(Form(".x %s (\"%s\",\"%s\",(AliJetContainer::EJetAlgo_t)%d,%f,(AliJetContainer::EJetType_t)%d,%f,%f,%f,(AliJetContainer::ERecoScheme_t)%d,\"%s\",%f,%d,%d)",gSystem->ExpandPathName("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C"),MCDcandAndTracks.Data(),"",AliJetContainer::EJetAlgo_t::antikt_algorithm,aRadius[0],AliJetContainer::EJetType_t::kFullJet,0.0,0.0,0.005,AliJetContainer::ERecoScheme_t::pt_scheme,AKTJet.Data(),0.,kFALSE,kFALSE)));
        taskFJMCDandTracks->SelectCollisionCandidates(pSel);
        taskFJMCDandTracks->SetNeedEmcalGeom(kFALSE);

        AliAnalysisTaskDJetCorrelationsQA *CorrTask = reinterpret_cast<AliAnalysisTaskDJetCorrelationsQA*>(
                    gInterpreter->ProcessLine(Form(".x %s ((AliAnalysisTaskDJetCorrelationsQA::ECandidateType)%d,\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%f,%f,\"%s\",%f,(AliAnalysisTaskDJetCorrelationsQA::ECorrelationMethod)%d)",gSystem->ExpandPathName("$ALICE_PHYSICS/PWGHF/jetsHF/macros/AddTaskDJetCorrelationsQA.C"),AliAnalysisTaskDJetCorrelationsQA::kD0toKpi,
                    cutFile.Data(),
                    isRunOnMC,
                    isReco,
                    TaskText.Data(),
                    //"AKTJetMBN0_AKTFullR030_DcandidatesAndTracksD0MBN0MCrec_pT0150_pt_scheme,",
                    taskFJDandTracks->GetName(),
                    DcandAndTracks.Data(),
                    "",
                    "",
                    "",
                    "",
                    "",
                    "",
                    aRadius[0],
                    0.,
                    "TPCFID",
                    0.,
                    AliAnalysisTaskDJetCorrelationsQA::kConstituent)));

        //Flag to build the Response Matrix
        CorrTask->SetBuildResponseMatrix(bRM);
        CorrTask->SetBuildResponseMatrixEff(bRMEff);
        //if to use only Pythia tracks for MC
        CorrTask->SetUsePythia(bPythia);
        CorrTask->SetIsPbPbData(kTRUE);

        //Container with generated level particles and D meson instead of the daughters
        AliMCParticleContainer *MCpartCont  = CorrTask->AddMCParticleContainer(MCDcandAndTracks);
        AliJetContainer *jetContMC = CorrTask->AddJetContainer(taskFJMCDandTracks->GetName(),"TPCFID",aRadius[0]);
        if(jetContMC) {
            jetContMC->ConnectParticleContainer(MCpartCont);
            jetContMC->SetJetPtCut(0.0);
            jetContMC->SetPercAreaCut(0.0);
        }

    }
#else
/*
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
    AliPhysicsSelectionTask *physSelTask= AddTaskPhysicsSelection(isRunOnMC);

    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
    AliAnalysisTaskPIDResponse *pidResp = AddTaskPIDResponse(isRunOnMC);

//    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
//    AliMultSelectionTask *multSel = AddTaskMultSelection();
    //multSel->SetAlternateOADBforEstimators("LHC15o-DefaultMC-HIJING");

 
    
    gROOT->LoadMacro("AliAnalysisTaskSEDmesonsFilterCJQA.cxx++g");
    gROOT->LoadMacro("AliAnalysisTaskDJetCorrelationsQAloc.cxx++g");
    gROOT->LoadMacro("AddTasksFlavourJetMCQA.C");
    AddTasksFlavourJetMCQA(0,"alien:///alice/cern.ch/user/b/btrzecia/pp5TeV/CutsDzero/D0toKpiCuts_pp5TeV_pPbBase.root",0.,0.,"TPCFID",runPeriod,0,pSel,isRunOnMC,isReco,isMap,bRM,bRMEff,bPythia,isPrompt,"MB"); // Jet(1,..) for D*
*/
#endif


    if(!mgr->InitAnalysis()) return;
    //mgr->SetDebugLevel(2);
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);

    if(local) {

        // if you want to run locally, we need to define some input
        TChain* chainAOD = new TChain("aodTree");
        TChain *chainAODfriend = new TChain("aodTree");

        // add a few files to the chain (change this so that your local files are added)
        chainAOD->Add(Form("%s/AliAOD.root",pathToLocalAODfiles.Data()));
        chainAODfriend->Add(Form("%s/AliAOD.VertexingHF.root",pathToLocalAODfiles.Data()));

        chainAOD->AddFriend(chainAODfriend);

		mgr->SetDebugLevel(10);
        // start the analysis locally, reading the events from the tchain
        mgr->StartAnalysis("local", chainAOD);


    } else {
        // if we want to run on grid, we create and configure the plugin
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();

        // also specify the include (header) paths on grid
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");

        // make sure your source files get copied to grid
        //alienHandler->SetAdditionalLibs("AliAnalysisTaskDJetCorrelationsQA.cxx AliAnalysisTaskDJetCorrelationsQA.h AliAnalysisTaskSEDmesonsFilterCJQA.cxx AliAnalysisTaskSEDmesonsFilterCJQA.h");
        //alienHandler->SetAnalysisSource("AliAnalysisTaskDJetCorrelationsQA.cxx AliAnalysisTaskSEDmesonsFilterCJQA.cxx");

        // select the aliphysics version. all other packages
        // are LOADED AUTOMATICALLY!
        alienHandler->SetAliPhysicsVersion(aliPhysVersion.Data());

        // set the Alien API version
        alienHandler->SetAPIVersion("V1.1x");

        // select the input data
        alienHandler->SetGridDataDir(gridDataDir.Data());
        alienHandler->SetDataPattern(Form("%s/*AliAOD.root",gridDataPattern.Data()));
        alienHandler->SetFriendChainName("AliAOD.VertexingHF.root");

        // MC has no prefix, data has prefix 000
        if(!isRunOnMC)alienHandler->SetRunPrefix("000");

        for(Int_t k=0; k<nruns; k++){
            alienHandler->AddRunNumber(runlist[k]);
        }
        alienHandler->SetNrunsPerMaster(nruns);


		TString tmpName(uniqueName);
		TString macroName("");
		TString execName("");
		TString jdlName("");
		macroName = Form("%s.C", tmpName.Data());
		execName = Form("%s.sh", tmpName.Data());
		jdlName = Form("%s.jdl", tmpName.Data());
    
        // number of files per subjob
        alienHandler->SetSplitMaxInputFileNumber(maxFilesPerWorker);
        alienHandler->SetExecutable("myTask.sh");

        // specify how many seconds your job may take
        alienHandler->SetTTL(workerTTL);
        alienHandler->SetJDLName("myTask.jdl");

        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);

        // merging: run with kTRUE to merge on grid
        // after re-running the jobs in SetRunMode("terminate")
        // (see below) mode, set SetMergeViaJDL(kFALSE)
        // to collect final results
        alienHandler->SetMaxMergeStages(3); //2, 3
        alienHandler->SetMergeViaJDL(kFALSE);

        // define the output folders
        alienHandler->SetGridWorkingDir(gridWorkingDir.Data());
        alienHandler->SetGridOutputDir(gridOutputDir.Data());

        // connect the alien plugin to the manager
        mgr->SetGridHandler(alienHandler);

        if(gridTest) {

            // speficy on how many files you want to run
            alienHandler->SetNtestFiles(nfiletestmode);
            // and launch the analysis
            alienHandler->SetRunMode("test");
            //mgr->SetDebugLevel(10);
            mgr->StartAnalysis("grid");
        }
        else {
            // else launch the full grid analysis
            alienHandler->SetRunMode(runMode.Data()); //terminate
            mgr->StartAnalysis("grid");
        }
    }
}
