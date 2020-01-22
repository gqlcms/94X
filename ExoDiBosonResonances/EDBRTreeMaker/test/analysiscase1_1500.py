import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
#,
#				     SkipEvent = cms.untracked.vstring('ProductNotFound'))
filterMode = False # True                
 
######## Sequence settings ##########
corrJetsOnTheFly = True
runOnMC = True
DOHLTFILTERS = True
#useJSON = not (runOnMC)
#JSONfile = 'Cert_246908-258750_13TeV_PromptReco_Collisions15_25ns_JSON.txt'
#****************************************************************************************************#

#process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
if runOnMC:
   process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v6'#'MCRUN2_74_V9::All'
   #process.GlobalTag.globaltag = '94X_mc2017_realistic_v14'#'MCRUN2_74_V9::All'
elif not(runOnMC):
   process.GlobalTag.globaltag = '92X_dataRun2_Prompt_v4'

hltFiltersProcessName = 'RECO'
if runOnMC:
   hltFiltersProcessName = 'PAT' #'RECO'


######### read JSON file for data ##########					                                                             
'''if not(runOnMC) and useJSON:

  import FWCore.PythonUtilities.LumiList as LumiList
  import FWCore.ParameterSet.Types as CfgTypes
  process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
  myLumis = LumiList.LumiList(filename = JSONfile).getCMSSWString().split(',')
  process.source.lumisToProcess.extend(myLumis) 
'''

####### Redo Jet clustering sequence ##########

from RecoJets.Configuration.RecoPFJets_cff import ak4PFJetsCHS, ak8PFJetsCHS, ak8PFJetsCHSPruned, ak8PFJetsCHSSoftDrop, ak8PFJetsCHSPrunedMass, ak8PFJetsCHSSoftDropMass# , ak8PFJetsCSTrimmed, ak8PFJetsCSFiltered, ak8PFJetsCHSFilteredMass, ak8PFJetsCHSTrimmedMass

from CommonTools.PileupAlgos.Puppi_cff import puppi

process.puppi = puppi.clone()
process.puppi.useExistingWeights = True
process.puppi.candName = cms.InputTag('packedPFCandidates')
process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')

process.ak8PFJetsCHS = ak8PFJetsCHS.clone( src = 'puppi', jetPtMin = 100.0 )
process.ak8PFJetsCHSPruned = ak8PFJetsCHSPruned.clone( src = 'puppi', jetPtMin = 100.0 )
process.ak8PFJetsCHSPrunedMass = ak8PFJetsCHSPrunedMass.clone()
process.ak8PFJetsCHSSoftDrop = ak8PFJetsCHSSoftDrop.clone( src = 'puppi', jetPtMin = 100.0 )
process.ak8PFJetsCHSSoftDropMass = ak8PFJetsCHSSoftDropMass.clone()


process.NjettinessAK8 = cms.EDProducer("NjettinessAdder",
                   src = cms.InputTag("ak8PFJetsCHS"),
                   Njets = cms.vuint32(1, 2, 3, 4),
                   # variables for measure definition : 
                   measureDefinition = cms.uint32( 0 ), # CMS default is normalized measure
                   beta = cms.double(1.0),          # CMS default is 1
                   R0 = cms.double( 0.8 ),          # CMS default is jet cone size
                   Rcutoff = cms.double( 999.0),       # not used by default
                   # variables for axes definition :
                   axesDefinition = cms.uint32( 6 ),    # CMS default is 1-pass KT axes
                   nPass = cms.int32(0),         # not used by default
                   akAxesR0 = cms.double(-999.0)        # not used by default
                   )

process.substructureSequence = cms.Sequence()
process.substructureSequence+=process.puppi
process.substructureSequence+=process.ak8PFJetsCHS
process.substructureSequence+=process.NjettinessAK8
process.substructureSequence+=process.ak8PFJetsCHSPruned
process.substructureSequence+=process.ak8PFJetsCHSPrunedMass
process.substructureSequence+=process.ak8PFJetsCHSSoftDrop
process.substructureSequence+=process.ak8PFJetsCHSSoftDropMass

####### Redo pat jets sequence ##########
process.redoPatJets = cms.Sequence()
process.redoPrunedPatJets = cms.Sequence()
process.redoSoftDropPatJets = cms.Sequence()

from ExoDiBosonResonances.EDBRJets.redoPatJets_cff import patJetCorrFactorsAK8, patJetsAK8, selectedPatJetsAK8

# Redo pat jets from ak8PFJetsCHS
process.patJetCorrFactorsAK8 = patJetCorrFactorsAK8.clone( src = 'ak8PFJetsCHS' )
process.patJetsAK8 = patJetsAK8.clone( jetSource = 'ak8PFJetsCHS' )
process.patJetsAK8.userData.userFloats.src = [ cms.InputTag("ak8PFJetsCHSPrunedMass"), cms.InputTag("ak8PFJetsCHSSoftDropMass"), cms.InputTag("NjettinessAK8:tau1"), cms.InputTag("NjettinessAK8:tau2"), cms.InputTag("NjettinessAK8:tau3"),cms.InputTag("NjettinessAK8:tau4")]
process.patJetsAK8.jetCorrFactorsSource = cms.VInputTag( cms.InputTag("patJetCorrFactorsAK8") )
process.selectedPatJetsAK8 = selectedPatJetsAK8.clone( cut = cms.string('pt > 100') )

process.redoPatJets+=process.patJetCorrFactorsAK8
process.redoPatJets+=process.patJetsAK8
process.redoPatJets+=process.selectedPatJetsAK8

# Redo pat jets ak8PFJetsCHSPruned
process.patJetCorrFactorsAK8Pruned = patJetCorrFactorsAK8.clone( src = 'ak8PFJetsCHSPruned' )
process.patJetsAK8Pruned = patJetsAK8.clone( jetSource = 'ak8PFJetsCHSPruned' )
process.patJetsAK8Pruned.userData.userFloats.src = [ "" ]
#process.patJetsAK8Pruned.userData.userFloats =cms.PSet(src = cms.VInputTag(""))
process.patJetsAK8Pruned.jetCorrFactorsSource = cms.VInputTag( cms.InputTag("patJetCorrFactorsAK8Pruned") )
process.selectedPatJetsAK8Pruned = selectedPatJetsAK8.clone(cut = 'pt > 100', src = "patJetsAK8Pruned")

process.redoPrunedPatJets+=process.patJetCorrFactorsAK8Pruned
process.redoPrunedPatJets+=process.patJetsAK8Pruned
process.redoPrunedPatJets+=process.selectedPatJetsAK8Pruned

# Redo pat jets ak8PFJetsCHSSoftDrop
process.patJetCorrFactorsAK8Softdrop = patJetCorrFactorsAK8.clone( src = 'ak8PFJetsCHSSoftDrop' )
process.patJetsAK8Softdrop = patJetsAK8.clone( jetSource = 'ak8PFJetsCHSSoftDrop' )
process.patJetsAK8Softdrop.userData.userFloats.src = [ "" ]
#process.patJetsAK8Softdrop.userData.userFloats =cms.PSet(src = cms.VInputTag(""))
process.patJetsAK8Softdrop.jetCorrFactorsSource = cms.VInputTag( cms.InputTag("patJetCorrFactorsAK8Softdrop") )
process.selectedPatJetsAK8Softdrop = selectedPatJetsAK8.clone(cut = 'pt > 100', src = "patJetsAK8Softdrop")

process.redoSoftDropPatJets+=process.patJetCorrFactorsAK8Softdrop
process.redoSoftDropPatJets+=process.patJetsAK8Softdrop
process.redoSoftDropPatJets+=process.selectedPatJetsAK8Softdrop


option = 'RECO'

process.load("ExoDiBosonResonances.EDBRCommon.goodMuons_cff")
process.load("ExoDiBosonResonances.EDBRCommon.goodElectrons_cff")
process.load("ExoDiBosonResonances.EDBRCommon.goodJets_cff")
process.load("ExoDiBosonResonances.EDBRCommon.leptonicW_cff")
process.load("ExoDiBosonResonances.EDBRCommon.hadronicW_cff")
process.load("ExoDiBosonResonances.EDBRCommon.goodPuppi_cff")

if option == 'RECO':
    process.goodMuons.src = "slimmedMuons"
    process.goodElectrons.src = "slimmedElectrons"
    process.goodJets.src = "slimmedJetsAK8"
#    process.goodJets.src = "selectedPatJetsAK8"
    process.Wtoenu.MET  = "slimmedMETs"
    process.Wtomunu.MET = "slimmedMETs"
    process.goodPuppi.src = "selectedPatJetsAK8"

process.goodOfflinePrimaryVertex = cms.EDFilter("VertexSelector",
                                       src = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                       cut = cms.string("chi2!=0 && ndof >= 4.0 && abs(z) <= 24.0 && abs(position.Rho) <= 2.0"),
                                       filter = cms.bool(True)
                                       )
if option == 'RECO':
    process.hadronicV.cut = ' '
if option == 'GEN':
    process.hadronicV.cut = ' '
WBOSONCUT = "pt > 200.0"

process.leptonicVSelector = cms.EDFilter("CandViewSelector",
                                       src = cms.InputTag("leptonicV"),
                                       cut = cms.string( WBOSONCUT ),
                                       filter = cms.bool(True)
                                       )
process.leptonicVFilter = cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag("leptonicV"),
                                       minNumber = cms.uint32(1),
                                       filter = cms.bool(True)
                                       )
process.hadronicVFilter = cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag("hadronicV"),
                                       minNumber = cms.uint32(1),
                                       filter = cms.bool(True)
                                       )
process.graviton = cms.EDProducer("CandViewCombiner",
                                       decay = cms.string("leptonicV hadronicV"),
                                       checkCharge = cms.bool(False),
                                       cut = cms.string("mass > 180"),
                                       roles = cms.vstring('leptonicV', 'hadronicV'),
                                       )
process.gravitonFilter =  cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag("graviton"),
                                       minNumber = cms.uint32(1),
                                       filter = cms.bool(True)
                                       )


process.leptonSequence = cms.Sequence(process.muSequence +
                                      process.eleSequence +
                                      process.leptonicVSequence +
                                      process.leptonicVSelector +
                                      process.leptonicVFilter )

process.jetSequence = cms.Sequence(process.substructureSequence +
                                   process.redoPatJets + 
                                   process.redoPrunedPatJets+
                                   process.redoSoftDropPatJets+
                                   process.fatJetsSequence +
                                   process.fatPuppiSequence+
                                   process.hadronicV +
                                   process.hadronicVFilter)

process.gravitonSequence = cms.Sequence(process.graviton +
                                        process.gravitonFilter)


if filterMode == False:
    process.goodOfflinePrimaryVertex.filter = False
    process.Wtomunu.cut = ''
    process.Wtoenu.cut = ''
    process.leptonicVSelector.filter = False
    process.leptonicVSelector.cut = ''
    process.hadronicV.cut = ''
    process.graviton.cut = ''
    process.leptonicVFilter.minNumber = 0
    process.hadronicVFilter.minNumber = 0
    process.gravitonFilter.minNumber = 0

######### JEC ########
METS = "slimmedMETs"
jetsAK8 = "slimmedJetsAK8"
jetsAK8pruned = "slimmedJetsAK8"
jetsAK8softdrop = "slimmedJetsAK8"
jetsAK8puppi = "cleanPuppi"
 
if runOnMC:
   jecLevelsAK8chs = [
                                   'Summer16_23Sep2016V3_MC_L1FastJet_AK8PFchs.txt',
                                   'Summer16_23Sep2016V3_MC_L2Relative_AK8PFchs.txt',
                                   'Summer16_23Sep2016V3_MC_L3Absolute_AK8PFchs.txt'
     ]
   jecLevelsAK8chsGroomed = [
                                   'Summer16_23Sep2016V3_MC_L2Relative_AK8PFchs.txt',
                                   'Summer16_23Sep2016V3_MC_L3Absolute_AK8PFchs.txt'
     ]
   jecLevelsAK8puppi = [
                                   'Summer16_23Sep2016V3_MC_L1FastJet_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016V3_MC_L2Relative_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016V3_MC_L3Absolute_AK8PFPuppi.txt'
     ]
   jecLevelsAK8puppiGroomed = [
                                   'Summer16_23Sep2016V3_MC_L2Relative_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016V3_MC_L3Absolute_AK8PFPuppi.txt'
     ]
   BjecLevelsAK4chs = [
                                   'Summer16_23Sep2016V3_MC_L1FastJet_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016V3_MC_L2Relative_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016V3_MC_L3Absolute_AK4PFPuppi.txt'
     ]
   jecLevelsAK4chs = [
          'Summer16_23Sep2016V3_MC_L1FastJet_AK4PFchs.txt',
          'Summer16_23Sep2016V3_MC_L2Relative_AK4PFchs.txt',
          'Summer16_23Sep2016V3_MC_L3Absolute_AK4PFchs.txt'
    ]
else:
   jecLevelsAK8chs = [
                                   'Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFchs.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFchs.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFchs.txt',
				   'Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFchs.txt'
     ]
   jecLevelsAK8chsGroomed = [
                                   'Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFchs.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFchs.txt',
				   'Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFchs.txt'
     ]
   jecLevelsAK8puppi = [
                                   'Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFPuppi.txt'
     ]
   jecLevelsAK8puppiGroomed = [
                                   'Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFPuppi.txt'
     ]
   BjecLevelsAK4chs = [
                                   'Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2Relative_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK4PFPuppi.txt'

     ]
   jecLevelsAK4chs = [
                                   'Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L2Relative_AK4PFPuppi.txt',
                                   'Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK4PFPuppi.txt',
				   'Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK4PFPuppi.txt'
     ]
process.treeDumper = cms.EDAnalyzer("EDBRTreeMaker",
                                    originalNEvents = cms.int32(1),
                                    crossSectionPb = cms.double(1),
                                    targetLumiInvPb = cms.double(1.0),
                                    EDBRChannel = cms.string("VW_CHANNEL"),
                                    isGen = cms.bool(False),
				    isJEC = cms.bool(corrJetsOnTheFly),
				    RunOnMC = cms.bool(runOnMC), 
				    generator =  cms.InputTag("generator"),
                                    genSrc =  cms.InputTag("prunedGenParticles"),
                                    pileup  =   cms.InputTag("slimmedAddPileupInfo"),
                                    leptonicVSrc = cms.InputTag("leptonicV"),
                                    gravitonSrc = cms.InputTag("graviton"),
				    looseMuonSrc = cms.InputTag("looseMuons"),
                                    looseElectronSrc = cms.InputTag("looseElectrons"),
				    goodMuSrc = cms.InputTag("goodMuons"),
				    MuSrc = cms.InputTag("slimmedMuons"),
                                    EleSrc = cms.InputTag("slimmedElectrons"),
                                    t1muSrc = cms.InputTag("slimmedMuons"),
                                    metSrc = cms.InputTag("slimmedMETs"),
                                    mets = cms.InputTag(METS),
                                    #ak4jetsSrc = cms.InputTag("cleanAK4Jets"), 
                                    ak4jetsSrc = cms.InputTag("cleanPuppiAK4"), 
                                    hadronicVSrc = cms.InputTag("hadronicV"),
                                    hadronicVSrc_raw = cms.InputTag("slimmedJetsAK8"),
				    jets = cms.InputTag("slimmedJets"),
                                    fatjets = cms.InputTag(jetsAK8),
                                    prunedjets = cms.InputTag(jetsAK8pruned),
                                    softdropjets = cms.InputTag(jetsAK8softdrop),
                                    puppijets = cms.InputTag(jetsAK8puppi),
				    jecAK8chsPayloadNames = cms.vstring( jecLevelsAK8chs ),
				    jecAK8chsPayloadNamesGroomed = cms.vstring( jecLevelsAK8chsGroomed ),
				    jecAK4chsPayloadNames = cms.vstring( jecLevelsAK4chs ),
                                    BjecAK4chsPayloadNames = cms.vstring( BjecLevelsAK4chs ),
					
					jecAK8puppiPayloadNames = cms.vstring( jecLevelsAK8puppi ),
                                    jecAK8puppiPayloadNamesGroomed = cms.vstring( jecLevelsAK8puppiGroomed ),
				    jecpath = cms.string(''),
				    rho = cms.InputTag("fixedGridRhoFastjetAll"),
                                    electronIDs = cms.InputTag("heepElectronID-HEEPV50-CSA14-25ns"),
				    muons = cms.InputTag("slimmedMuons"),
				    vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                    hltToken    = cms.InputTag("TriggerResults","","HLT"),
                                    elPaths1     = cms.vstring("HLT_Ele105_CaloIdVT_GsfTrkIdT_v*"),#EXO-15-002
                                    elPaths2     = cms.vstring("HLT_Ele27_eta2p1_WP75_Gsf_v*", "HLT_Ele27_eta2p1_WPLoose_Gsf_v*"), #B2G-15-005
                                    elPaths3     = cms.vstring("HLT_Ele45_WPLoose_Gsf_v*"),
                                    elPaths4     = cms.vstring("HLT_Ele115_CaloIdVT_GsfTrkIdT_v*"),#("HLT_Ele35_WPLoose_Gsf_v*"),
                                    elPaths5     = cms.vstring("HLT_Ele40_WPTight_Gsf_v*"),#("HLT_Ele35_WPLoose_Gsf_v*"),
                                    #elPaths5     = cms.vstring("HLT_Ele25_WPTight_Gsf_v*"),
                                    elPaths6     = cms.vstring("HLT_Ele38_WPTight_Gsf_v*"),#("HLT_Ele25_eta2p1_WPLoose_Gsf_v*"),
                                    elPaths7     = cms.vstring("HLT_Ele35_WPTight_Gsf_v*"),#("HLT_Ele25_eta2p1_WPTight_Gsf_v*"),
                                    elPaths8     = cms.vstring("HLT_Ele27_WPTight_Gsf_v*"),
                                    muPaths1     = cms.vstring("HLT_Mu45_eta2p1_v*"),#EXO-15-002
                                    muPaths2     = cms.vstring("HLT_Mu50_v*"), #B2G-15-005
                                    muPaths3     = cms.vstring("HLT_TkMu50_v*"), #B2G-15-005
                                    muPaths4     = cms.vstring("HLT_Mu16_eta2p1_MET30_v*", "HLT_IsoMu16_eta2p1_MET30_v*"), #MET
                                    muPaths5     = cms.vstring("HLT_IsoMu27_v*"), #MET
                                    muPaths6     = cms.vstring("HLT_IsoMu20_v*"),
                                    muPaths7     = cms.vstring("HLT_IsoTkMu20_v*"),
                                    muPaths8     = cms.vstring("HLT_IsoMu22_v*"),
                                    muPaths9     = cms.vstring("HLT_IsoTkMu22_v*"),
                                    muPaths10     = cms.vstring("HLT_IsoMu24_v*"),
                                    muPaths11     = cms.vstring("HLT_IsoTkMu24_v*"),
                                    muPaths12     = cms.vstring("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v*"),
                                    noiseFilter = cms.InputTag('TriggerResults','', hltFiltersProcessName),
                                    noiseFilterSelection_HBHENoiseFilter = cms.string('Flag_HBHENoiseFilter'),
                                    noiseFilterSelection_HBHENoiseIsoFilter = cms.string("Flag_HBHENoiseIsoFilter"),
                                    noiseFilterSelection_GlobalTightHaloFilter = cms.string('Flag_globalTightHalo2016Filter'),
                                    noiseFilterSelection_EcalDeadCellTriggerPrimitiveFilter = cms.string('Flag_EcalDeadCellTriggerPrimitiveFilter'),
                                    noiseFilterSelection_goodVertices = cms.string('Flag_goodVertices'),
                                    noiseFilterSelection_eeBadScFilter = cms.string('Flag_eeBadScFilter'),
                                    )


if option=='GEN':
    process.treeDumper.metSrc = 'genMetTrue'
    process.treeDumper.isGen  = True
 

process.analysis = cms.Path(process.leptonSequence +
                            #process.substructureSequence+
                            #process.redoPatJets+
                            #process.redoPrunedPatJets+
                            #process.redoSoftDropPatJets+
                            process.jetSequence +
                            process.gravitonSequence +
                            process.treeDumper)

if option=='RECO':
    process.analysis.replace(process.leptonSequence, process.goodOfflinePrimaryVertex + process.leptonSequence)

process.load("ExoDiBosonResonances.EDBRCommon.data.RSGravitonToWW_kMpl01_M_1000_Tune4C_13TeV_pythia8")
process.source.inputCommands = ['keep *','drop *_isolatedTracks_*_*']
process.source.fileNames = [
#'/store/data/Run2017C/SingleElectron/MINIAOD/PromptReco-v2/000/300/500/00000/C8EAB179-4D7C-E711-8CA7-02163E0144E1.root'
#'/store/data/Run2017B/SingleMuon/MINIAOD/23Jun2017-v1/120000/78711365-0B59-E711-B9E9-0025904B0FC0.root'
#'/store/data/Run2016D/SingleMuon/MINIAOD/23Sep2016-v1/60000/7E24A014-6E9D-E611-B984-F04DA2752644.root'
#"/store/mc/RunIISummer16MiniAODv2/BulkGravToWWToWlepWhad_narrow_M-2000_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/F6EF3B47-1AB7-E611-994D-0025904C7A54.root"
#'/store/mc/RunIISummer16MiniAODv2/QCD_HT1000to1500_BGenFilter_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/2C3623F9-7FBE-E611-AC00-002590E7D7DE.root'
#'file:EXO-RunIISummer16MiniAODv2-07193.root'
#'file:/eos/cms/store/user/lewang/WWW-v1/crab_WWW-MA/171024_113209/0000/EXO-RunIISummer16MiniAODv2-07193_19.root'
#'/store/mc/RunIIFall17MiniAODv2/WkkToWRadionToWWW_M4000-R0-5_TuneCP5_13TeV-madgraph/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/70000/F2D0E7A2-C54D-E811-B022-FA163E1445BD.root'
#'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v1-3000/crab_WWW-MA-3000/180715_150422/0000/EXO-RunIISummer16MiniAODv2-07193_95.root',
#'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v1-3000/crab_WWW-MA-3000/180715_150422/0000/EXO-RunIISummer16MiniAODv2-07193_96.root',
#'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v1-3000/crab_WWW-MA-3000/180715_150422/0000/EXO-RunIISummer16MiniAODv2-07193_97.root',
#'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v1-3000/crab_WWW-MA-3000/180715_150422/0000/EXO-RunIISummer16MiniAODv2-07193_98.root',
#'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v1-3000/crab_WWW-MA-3000/180715_150422/0000/EXO-RunIISummer16MiniAODv2-07193_99.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_100.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_101.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_102.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_103.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_104.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_105.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_106.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_107.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_108.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_109.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_10.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_110.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_111.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_112.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_113.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_114.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_115.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_116.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_117.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_118.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_119.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_11.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_120.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_121.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_122.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_123.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_124.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_125.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_126.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_127.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_128.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_129.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_12.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_130.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_131.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_132.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_133.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_134.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_135.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_136.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_137.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_138.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_139.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_13.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_140.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_141.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_142.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_143.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_144.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_145.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_146.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_147.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_148.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_149.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_14.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_150.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_151.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_152.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_153.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_154.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_155.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_156.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_157.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_158.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_159.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_15.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_160.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_161.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_162.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_163.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_164.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_165.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_166.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_167.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_168.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_169.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_16.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_170.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_171.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_172.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_173.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_174.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_175.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_176.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_177.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_178.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_179.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_17.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_180.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_181.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_182.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_183.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_184.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_185.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_186.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_187.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_188.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_189.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_18.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_190.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_191.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_192.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_193.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_194.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_195.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_196.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_197.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_198.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_199.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_19.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_1.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_200.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_201.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_202.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_203.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_204.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_205.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_206.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_207.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_208.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_209.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_20.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_210.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_211.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_212.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_213.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_214.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_215.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_216.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_217.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_218.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_219.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_21.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_220.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_221.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_222.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_223.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_224.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_225.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_226.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_227.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_228.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_229.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_22.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_230.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_231.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_232.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_233.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_234.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_235.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_236.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_237.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_238.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_239.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_23.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_240.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_241.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_242.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_243.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_244.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_245.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_246.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_247.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_248.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_249.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_24.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_250.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_251.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_252.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_253.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_254.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_255.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_256.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_257.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_258.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_259.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_25.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_260.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_261.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_262.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_263.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_264.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_265.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_266.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_267.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_268.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_269.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_26.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_270.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_271.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_272.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_273.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_274.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_275.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_276.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_277.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_278.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_279.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_27.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_280.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_281.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_282.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_283.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_284.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_285.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_286.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_287.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_288.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_289.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_28.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_290.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_291.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_292.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_293.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_294.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_295.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_296.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_297.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_298.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_299.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_29.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_2.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_300.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_301.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_302.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_303.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_304.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_305.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_306.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_307.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_308.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_309.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_30.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_310.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_311.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_312.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_313.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_314.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_315.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_316.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_317.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_318.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_319.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_31.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_320.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_321.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_322.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_323.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_324.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_325.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_326.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_327.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_328.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_329.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_32.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_330.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_331.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_332.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_333.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_334.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_335.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_336.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_337.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_338.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_339.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_33.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_340.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_341.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_342.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_343.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_344.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_345.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_346.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_347.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_348.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_349.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_34.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_350.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_351.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_352.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_353.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_354.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_355.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_356.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_357.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_358.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_359.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_35.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_360.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_361.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_362.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_363.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_364.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_365.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_366.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_367.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_368.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_369.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_36.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_370.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_371.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_372.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_373.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_374.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_375.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_376.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_377.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_378.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_379.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_37.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_380.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_381.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_382.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_383.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_384.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_385.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_386.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_387.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_388.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_389.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_38.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_390.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_391.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_392.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_393.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_394.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_395.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_396.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_397.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_398.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_399.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_39.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_3.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_400.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_401.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_402.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_403.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_404.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_405.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_406.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_407.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_408.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_409.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_40.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_410.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_411.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_412.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_413.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_414.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_415.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_416.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_417.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_418.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_419.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_41.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_420.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_421.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_422.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_423.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_424.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_425.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_426.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_427.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_428.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_429.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_42.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_430.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_431.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_432.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_433.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_434.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_435.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_436.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_437.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_438.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_439.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_43.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_440.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_441.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_442.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_443.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_444.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_445.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_446.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_447.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_448.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_449.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_44.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_450.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_451.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_452.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_453.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_454.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_455.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_456.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_457.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_458.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_459.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_45.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_460.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_461.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_462.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_463.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_464.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_465.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_466.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_467.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_468.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_469.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_46.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_470.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_471.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_472.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_473.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_474.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_475.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_476.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_477.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_478.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_479.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_47.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_480.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_481.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_482.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_483.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_484.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_485.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_486.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_487.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_488.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_489.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_48.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_490.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_491.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_492.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_493.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_494.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_495.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_496.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_497.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_498.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_499.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_49.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_4.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_500.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_50.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_51.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_52.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_53.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_54.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_55.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_56.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_57.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_58.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_59.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_5.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_60.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_61.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_62.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_63.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_64.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_65.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_66.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_67.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_68.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_69.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_6.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_70.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_71.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_72.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_73.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_74.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_75.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_76.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_77.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_78.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_79.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_7.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_80.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_81.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_82.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_83.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_84.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_85.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_86.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_87.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_88.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_89.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_8.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_90.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_91.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_92.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_93.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_94.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_95.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_96.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_97.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_98.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_99.root',
'file:/eos/cms/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/xulyu/WWW/WWW-v2-1500/crab_WWW-MA-1500-5/180716_091315/0000/EXO-RunIISummer16MiniAODv2-07193_9.root',
]

process.maxEvents.input = -1
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.cerr.FwkReport.limit = 99999999

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("case1_1500.root")
                                   )
