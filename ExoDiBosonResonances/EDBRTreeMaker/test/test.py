import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )
#process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True),allowUnscheduled=cms.untracked.bool(True))
#,
#				     SkipEvent = cms.untracked.vstring('ProductNotFound'))
filterMode = False # True                
 
######## Sequence settings ##########
corrJetsOnTheFly = True
runOnMC  = True
runOnSig = False
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
   process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v4'

hltFiltersProcessName = 'RECO'
if runOnMC:
   hltFiltersProcessName = 'PAT' #'RECO'

#if DOHLTFILTERS and not(runOnMC):
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
process.HBHENoiseFilterResultProducer.IgnoreTS4TS5ifJetInLowBVRegion=cms.bool(False)
process.HBHENoiseFilterResultProducer.defaultDecision = cms.string("HBHENoiseFilterResultRun2Loose")

process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
                                                    inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResult'),
                                                    reverseDecision = cms.bool(False)
                                                    )
process.ApplyBaselineHBHEIsoNoiseFilter = cms.EDFilter('BooleanFlagFilter',
                                                       inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHEIsoNoiseFilterResult'),
                                                       reverseDecision = cms.bool(False)
                                                       )

######### read JSON file for data ##########					                                                             
'''if not(runOnMC) and useJSON:

  import FWCore.PythonUtilities.LumiList as LumiList
  import FWCore.ParameterSet.Types as CfgTypes
  process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
  myLumis = LumiList.LumiList(filename = JSONfile).getCMSSWString().split(',')
  process.source.lumisToProcess.extend(myLumis) 
'''

# ---------------------------------------------------------
# DeepAK8: set up TransientTrackBuilder
process.load('Configuration.StandardSequences.MagneticField_cff')
process.TransientTrackBuilderESProducer = cms.ESProducer("TransientTrackBuilderESProducer",
    ComponentName=cms.string('TransientTrackBuilder')
)
# ---------------------------------------------------------

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

from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
## PATify soft drop subjets
addJetCollection(
    process,
    labelName = 'AK8SoftDropSubjets',
    jetSource = cms.InputTag('ak8PFJetsCHSSoftDrop','SubJets'),
    algo = 'ak',  # needed for subjet flavor clustering
    rParam = 0.8, # needed for subjet flavor clustering
    getJetMCFlavour = False,
    pvSource = cms.InputTag( 'offlineSlimmedPrimaryVertices' ),
    genJetCollection = cms.InputTag('slimmedGenJets'),
    genParticles = cms.InputTag( 'prunedGenParticles' ),
    btagDiscriminators = ['None'],
    jetCorrections = ('AK4PFPuppi', ['L2Relative', 'L3Absolute'], 'None'),
#    explicitJTA = True,  # needed for subjet b tagging
#    svClustering = True, # needed for subjet b tagging
#    fatJets=cms.InputTag('ak8PFJetsCHS'),             # needed for subjet flavor clustering
#    groomedFatJets=cms.InputTag('ak8PFJetsCHSSoftDrop') # needed for subjet flavor clustering
)
#'''
#from RecoBTag.DeepFlavour.DeepFlavourJetTagsProducer_cfi import *
# this loads all available b-taggers
#process.load("RecoBTag.Configuration.RecoBTag_cff")
#process.load("RecoBTag.DeepFlavour.DeepFlavourJetTagsProducer_cfi")
#process.load("RecoBTag.DeepFlavour.deepFlavour_cff")
#'''
from RecoBTag.Configuration.RecoBTag_EventContent_cff import *
from RecoBTag.Configuration.RecoBTag_cff import *
from RecoBTag.DeepFlavour.DeepFlavourJetTagsProducer_cfi import deepFlavourJetTags
from RecoBTag.DeepFlavour.deepFlavour_cff import *
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

updateJetCollection(
   process,
   labelName = 'DeepFlavour',
   jetSource = cms.InputTag('cleanPuppiAK4'),
   pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
   svSource = cms.InputTag('slimmedSecondaryVertices'),
   jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
   btagDiscriminators =      ['deepFlavourJetTags:probb',     'deepFlavourJetTags:probbb','deepFlavourJetTags:probc','deepFlavourJetTags:probudsg','deepFlavourJetTags:probcc'],
   postfix='NewDFTraining'
)

#process.selectedUpdatedPatJetsDeepFlavourNewDFTraining.userData.userFloats.src =[]
#'''
'''
process.patjets = cms.EDAnalyzer('EDBRTreeMaker',
   PatJets = cms.InputTag("selectedUpdatedPatJets"),
   PTMin = cms.double(-1),
   BTag = cms.string("deepFlavourJetTags:probb"),
)
'''
process.selectedPatJetsAK8SoftDropPacked = cms.EDProducer("BoostedJetMerger",
    jetSrc = cms.InputTag("selectedPatJetsAK8Softdrop"),
    subjetSrc = cms.InputTag("selectedPatJetsAK8SoftDropSubjets")
)


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
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff']
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

process.leptonSequence = cms.Sequence(process.muSequence +
                                      process.egmGsfElectronIDSequence*process.eleSequence +
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

process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
process.load("RecoMET.METFilters.BadChargedCandidateFilter_cfi")
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.metfilterSequence = cms.Sequence(process.BadPFMuonFilter+process.BadChargedCandidateFilter)

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
                                    lhe =  cms.InputTag("externalLHEProducer"),
                                    isGen = cms.bool(False),
                                    isJEC = cms.bool(corrJetsOnTheFly),
                                    RunOnMC  = cms.bool(runOnMC),
                                    RunOnSig = cms.bool(runOnSig),
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
                                    ##ak4jetsSrc = cms.InputTag("cleanAK4Jets"), 
                                    #ak4jetsSrc = cms.InputTag("cleanPuppiAK4"), 
                                    ak4jetsSrc = cms.InputTag("selectedUpdatedPatJetsDeepFlavourNewDFTraining"), 
                                    hadronicVSrc = cms.InputTag("hadronicV"),
                                    hadronicVSrc_raw = cms.InputTag("slimmedJetsAK8"),
                                    hadronicVSoftDropSrc = cms.InputTag("selectedPatJetsAK8SoftDropPacked"),
                                    jets = cms.InputTag("slimmedJets"),
                                    fatjets = cms.InputTag(jetsAK8),
                                    ak8JetSrc = cms.InputTag(jetsAK8),
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
                                    elPaths1     = cms.vstring("HLT_PFMETNoMu110_PFMHTNoMu110_IDTight*"),#EXO-15-002
                                    elPaths2     = cms.vstring("HLT_Ele27_eta2p1_WP75_Gsf_v*", "HLT_Ele27_eta2p1_WPLoose_Gsf_v*"), #B2G-15-005
                                    elPaths3     = cms.vstring("HLT_Ele45_WPLoose_Gsf_v*"),
                                    elPaths4     = cms.vstring("HLT_Ele115_CaloIdVT_GsfTrkIdT_v*"),#("HLT_Ele35_WPLoose_Gsf_v*"),
                                    elPaths5     = cms.vstring("HLT_Ele40_WPTight_Gsf_v*"),#("HLT_Ele35_WPLoose_Gsf_v*"),
                                    #elPaths5     = cms.vstring("HLT_Ele25_WPTight_Gsf_v*"),
                                    elPaths6     = cms.vstring("HLT_Ele38_WPTight_Gsf_v*"),#("HLT_Ele25_eta2p1_WPLoose_Gsf_v*"),
                                    elPaths7     = cms.vstring("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v*"),#("HLT_Ele25_eta2p1_WPTight_Gsf_v*"),
                                    elPaths8     = cms.vstring("HLT_Ele27_WPTight_Gsf_v*"),
                                    muPaths1     = cms.vstring("HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v*"),#EXO-15-002
                                    muPaths2     = cms.vstring("HLT_Mu50_v*"), #B2G-15-005
                                    muPaths3     = cms.vstring("HLT_TkMu50_v*"), #B2G-15-005
                                    muPaths4     = cms.vstring("HLT_Mu16_eta2p1_MET30_v*", "HLT_IsoMu16_eta2p1_MET30_v*"), #MET
                                    muPaths5     = cms.vstring("HLT_PFHT800_v*"), #MET
                                    muPaths6     = cms.vstring("HLT_PFHT900_v*"),
                                    muPaths7     = cms.vstring("HLT_PFJet450_v*"),
                                    muPaths8     = cms.vstring("HLT_PFJet500_v*"),
                                    muPaths9     = cms.vstring("HLT_AK8PFJet450_v*"),
                                    muPaths10     = cms.vstring("HLT_AK8PFJet500_v*"),
                                    muPaths11     = cms.vstring("HLT_AK8PFJet360_TrimMass30_v*"),#e1,MET110,e7,m1,m5-m11,HLT_hadronic
                                    muPaths12     = cms.vstring("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v*"),
                                    noiseFilter = cms.InputTag('TriggerResults','', hltFiltersProcessName),
                                    noiseFilterSelection_HBHENoiseFilter = cms.string('Flag_HBHENoiseFilter'),
                                    noiseFilterSelection_HBHENoiseIsoFilter = cms.string("Flag_HBHENoiseIsoFilter"),
                                    noiseFilterSelection_GlobalTightHaloFilter = cms.string('Flag_globalTightHalo2016Filter'),
                                    noiseFilterSelection_EcalDeadCellTriggerPrimitiveFilter = cms.string('Flag_EcalDeadCellTriggerPrimitiveFilter'),
                                    noiseFilterSelection_goodVertices = cms.string('Flag_goodVertices'),
                                    noiseFilterSelection_eeBadScFilter = cms.string('Flag_eeBadScFilter'),
                                    noiseFilterSelection_badMuon = cms.InputTag('BadPFMuonFilter'),
                                    noiseFilterSelection_badChargedHadron = cms.InputTag('BadChargedCandidateFilter'),
                                    )


if option=='GEN':
    process.treeDumper.metSrc = 'genMetTrue'
    process.treeDumper.isGen  = True
 

process.analysis = cms.Path(process.leptonSequence +
                            #process.substructureSequence+
                            #process.redoPatJets+
                            #process.redoPrunedPatJets+
                            #process.redoSoftDropPatJets+
                            process.HBHENoiseFilterResultProducer+
                            process.ApplyBaselineHBHENoiseFilter+
                            process.ApplyBaselineHBHEIsoNoiseFilter+
                            process.jetSequence +
			    #process.patjets +
                            process.metfilterSequence +
                            process.gravitonSequence +
                            process.treeDumper)

if option=='RECO':
    process.analysis.replace(process.leptonSequence, process.goodOfflinePrimaryVertex + process.leptonSequence)

process.load("ExoDiBosonResonances.EDBRCommon.data.RSGravitonToWW_kMpl01_M_1000_Tune4C_13TeV_pythia8")
#process.source.inputCommands = ['keep *','drop *_isolatedTracks_*_*']
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
#'file:/eos/cms/store/user/xulyu/EXO-RunIISummer16MiniAODv2-07193_1.root',
#'/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/FC517460-77BE-E611-940D-90B11C2801E1.root'
#'/store/mc/RunIISummer16MiniAODv2/WkkToWRadionToWWW_M1500-R0-9-TuneCUEP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/D20E05D7-5FC6-E811-AA1E-0CC47A5FBE21.root'
#'/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/FC517460-77BE-E611-940D-90B11C2801E1.root',
#'/store/mc/RunIISummer16MiniAODv2/ttWJets_13TeV_madgraphMLM/MINIAODSIM/80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/90000/F4B12F5A-D2C9-E611-892C-0025905A6136.root',
#'/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/FED04F50-A3BE-E611-A893-0025900E3502.root',
#'/store/mc/RunIISummer16MiniAODv2/ttZJets_13TeV_madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/FEA79E79-70FC-E611-A9F6-02163E019B52.root',
'/store/mc/RunIISummer16MiniAODv2/ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/80000/FA34DAFF-6FC1-E611-A7AF-0CC47A7C35F4.root',
]

#process.maxEvents.input = 1
process.maxEvents.input = 500
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.MessageLogger.cerr.FwkReport.limit = 99999999

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("RStreeEDBR_pickup.root")
                                   )
