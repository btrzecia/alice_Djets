void AddTasksFlavourJet(const Int_t iCandType = 1 /*0 = D0, 1=Dstar...*/,
   const TString sCutFile = "cutsHF/D0toKpiCutsppRecVtxNoPileupRejNoEMCAL.root",
   const Double_t dJetPtCut   = 0.,
   const Double_t dJetAreaCut = 0.,
   const char *acctype = "TPC",
   const TString sRunPeriod = "LHC10b",
   const Int_t    uBeamType = 0,
   const UInt_t uTriggerMask = AliVEvent::kAny, /*for jets; the D mesons trigger is defined in the cut object*/
   const Bool_t bIsMC = kFALSE,
   const Bool_t bIsReco = kFALSE,
   const Bool_t bIsMap = kFALSE,
   TString sText="",/*completes the name of the candidate task lists*/
   Bool_t doBkg = kTRUE
   )
{


   TString rhoName = "";
   TString rhoNameBkg = "";
   TString rhoNameMC = "";

   const Int_t    nRadius = 1;
   const Double_t aRadius[] = {  0.3  };
   const TString  sRadius[] = { "R03" };
    
    
   //=============================================================================
   
   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
   
   if (!mgr) {
      ::Error("AddTasksFlavourJet.C::AddTasksFlavourJet", "No analysis manager to connect to.");
      return;
   }
   
   TString type = mgr->GetInputEventHandler()->GetDataType();
   if (!type.Contains("ESD") && !type.Contains("AOD")) {
      ::Error("AddTasksFlavourJet.C::AddTasksFlavourJet", "Task manager to have an ESD or AOD input handler.");
      return;
   }
   
   if (!mgr->GetInputEventHandler()) {
      ::Error("AddTasksFlavourJet.C::AddTasksFlavourJet", "This task requires an input event handler");
      return;
   }
   //=============================================================================

    
   //Centrality Selection (only for heavy-ions)
  // gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
  // AliMultSelectionTask * task = AddTaskMultSelection(kFALSE);

   //D mesons -- PID
   gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
   AliAnalysisTaskSE *taskRespPID = AddTaskPIDResponse(bIsMC,kTRUE,kTRUE,"1");
    
   // EMCal framework
   // -- Physics selection task
    if(!bIsMC){
        gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/AddTaskEmcalPhysicsSelection.C");
        AliPhysicsSelectionTask *physSelTask = AddTaskEmcalPhysicsSelection(kTRUE, kTRUE, uTriggerMask, 5, 5, 10, kTRUE, -1, -1, -1, -1);
   
        if (!physSelTask) {
            cout << "no physSelTask"; 
            return; 
        }
    }
    
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
	AliPhysicsSelectionTask *physSelTask = AddTaskPhysicsSelection(kTRUE, kTRUE);
   
   
    //D meson filtering task
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGHF/jetsHF/macros/AddTaskDmesonsFilterCJ.C");
    //D-jet correlation task
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGHF/jetsHF/macros/AddTaskDFilterCorrelations.C");
    //Jet task
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskEmcalJet.C");
    //Rho task
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskRho.C");
    //Embedding task
    gROOT->LoadMacro("AddEmbeddingEventForHF.C");
    
    //This line sets the right filter depending on the period
    //AliTrackContainer::SetDefTrackCutsPeriod("lhc15o");
    
    // Setting embedding task
    AliEmbeddingEventForHFTask* embed = AddEmbeddingEventForHF("","aodTree","tracks","alien:///alice/data/2015/LHC15o","pass1/AOD194","AliAOD.root",500,kFALSE,0,20,AliVEvent::kAnyINT,"Embedding",sCutFile,iCandType);
    
    // Add track efficiency to the base sample (Pythia pp events in this case)
    embed->SetBaseTrackEff(kTRUE);
    embed->SetBaseTrackEffPath("alien:///alice/cern.ch/user/b/btrzecia/PbPb2015/TrackingEff/EffRatioFile_fine.root");
    //embed->SetBaseTrackEffPath("alien:///alice/cern.ch/user/a/anolivei/TrackingEff/EffRatioFile_fine.root");
    embed->SetBaseTrackEffHistName("EffRatio");
    
    AliTrackContainer* trackCont0 = embed->AddTrackContainer("tracks");
    trackCont0->SetClassName("AliAODTrack");
    //trackCont0->SetTrackCutsPeriod("lhc15o");
    trackCont0->SetTrackFilterType(AliEmcalTrackSelection::kCustomTrackFilter);
    trackCont0->SetAODFilterBits(1<<4);
    trackCont0->AddAODFilterBit(1<<9);
    
    
    for(Int_t i=0; i<1; i++)
    {
        
        TString TaskText = sText;
        TaskText += Form("N%d",i);
        
        //Filtering task. It will create a set of particles with the D meson instead of the daughters
        AliAnalysisTaskDmesonsFilterCJ *filter = AddTaskDmesonsFilterCJ(iCandType,sCutFile,bIsMC,bIsReco,TaskText);
        filter->SetAnalysedCandidate(i); 
		filter->SetCombineDmesons(kTRUE);
		filter->SetMultipleCandidates(kTRUE); 
		filter->SetRejectDfromB(kTRUE);
		filter->SetKeepOnlyDfromB(kFALSE);
		filter->SetBuildRMEff(kTRUE);
		filter->SetUsePythia(kFALSE);
        
        // Giving embedded tracks to the filtering task in order to include D meson and remove daughters
        AliTrackContainer* trackCont1 = filter->AddTrackContainer("EmbeddedTracks");
        trackCont1->SetClassName("AliAODTrack");
        //trackCont1->SetTrackCutsPeriod("lhc15o");
        trackCont1->SetTrackFilterType(AliEmcalTrackSelection::kCustomTrackFilter);
        trackCont1->SetAODFilterBits(1<<4);
        trackCont1->AddAODFilterBit(1<<9);
        
        AliMCParticleContainer* trackCont2 = filter->AddMCParticleContainer("mcparticles");
        trackCont2->SetClassName("AliAODMCParticle");
        
        TString candArrName = "Dcandidates";
        TString sbArrName = "DSBcandidates";
        TString DcandAndTracks = "DcandidatesAndTracks";
        TString DBkgAndTracks = "DSBcandidatesAndTracks";
        TString MCDcandAndTracks = "MCDcandidatesAndTracks";
        TString candName;
        if(iCandType==0) candName = "D0";
        if(iCandType==1) candName = "DStar";
        
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
        
        if (bIsMC) {
            candArrName += "MC";
            sbArrName  += "MC";
            DcandAndTracks += "MC";
            DBkgAndTracks += "MC";
            MCDcandAndTracks += "MC";
            if (bIsReco)
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
        
        
        // Anti-kT jets
        AliEmcalJetTask *taskFJDandTracks = AddTaskEmcalJet(DcandAndTracks,"",1,0.3,0,0.15,0.30,0.005,1,AKTJet,0.,kFALSE,kFALSE);
        taskFJDandTracks->SelectCollisionCandidates(uTriggerMask);
        
        // kT jets for calculating Rho
        AliEmcalJetTask *taskFJMCDandTracks = AddTaskEmcalJet(MCDcandAndTracks,"",1,0.3,0,0.15,0.30,0.005,1,AKTJet,0.,kFALSE,kFALSE);
        
        TString KTJet = "KTJet";
        KTJet += TaskText;

        
        rhoName = "Rho";
        rhoName += TaskText;
        AliEmcalJetTask *taskFJ2 = AddTaskEmcalJet(DcandAndTracks,"",0,0.3,0,0.15,0.30,0.005,1,KTJet,0.,kFALSE,kFALSE);
        taskFJ2->SelectCollisionCandidates(uTriggerMask);
        
        rhotask = (AliAnalysisTaskRho*) AddTaskRho(taskFJ2->GetName(), DcandAndTracks,"", rhoName, 0.3, "TPCFID", 0.01, 0, 0, 2, kTRUE);
        rhotask->SelectCollisionCandidates(uTriggerMask);
        rhotask->SetHistoBins(100,0,250);
        
        
        AliAnalysisTaskDJetCorrelations *CorrTask = AddTaskDFilterCorrelations(
                                                                                         iCandType,
                                                                                         sCutFile,
                                                                                         bIsMC,
                                                                                         bIsReco,
                                                                                         TaskText,
                                                                                         taskFJDandTracks->GetName(),
                                                                                         DcandAndTracks,
                                                                                         "",
                                                                                         rhoName,
                                                                                         "",
                                                                                         "",
                                                                                         "",
                                                                                         "",
                                                                                         0.3,
                                                                                         dJetPtCut,
                                                                                         acctype,
                                                                                         dJetAreaCut,
                                                                                         AliAnalysisTaskDJetCorrelations::kConstituent

                                                                                         );
                                                                                         
        
        CorrTask->SetJetPtFactor(4);

        CorrTask->SetBuildResponseMatrix(kFALSE);
		CorrTask->SetBuildResponseMatrixEff(kTRUE);
		CorrTask->SetUsePythia(kFALSE);
		AliMCParticleContainer *MCpartCont  = CorrTask->AddMCParticleContainer(MCDcandAndTracks);
		AliJetContainer *jetContMC = CorrTask->AddJetContainer(taskFJMCDandTracks->GetName(),"TPC",0.3);
		jetContMC->ConnectParticleContainer(MCpartCont);
		jetContMC->SetJetPtCut(0);
		jetContMC->SetPercAreaCut(dJetAreaCut);                                                                                 
        
        //CorrTask->SetMC(kTRUE);
        // Extra histrograms for cross-section
        //CorrTask->SetVzRange(-10,10);
        //CorrTask->SetIsPythia(kTRUE);
        //CorrTask->SetMakeGeneralHistograms(kTRUE);
        // Generator level tracks and jets
        
        
    }
   
   return;
}
