import FWCore.ParameterSet.Config as cms
from PhysicsTools.SelectorUtils.pfJetIDSelector_cfi import pfJetIDSelector

goodJets = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                        filterParams = pfJetIDSelector.clone(),
                        src = cms.InputTag("slimmedJetsAK8")
                        )



goodAK4Jets = cms.EDFilter("PFJetIDSelectionFunctorFilter",
                        filterParams = pfJetIDSelector.clone(),
                        src = cms.InputTag("slimmedJets")
                        )

### Cleaning
# We want to make sure that the jets are not the electrons or muons done previously

import PhysicsTools.PatAlgos.cleaningLayer1.jetCleaner_cfi as jetCleaner_cfi

cleanJets = jetCleaner_cfi.cleanPatJets.clone()
cleanJets.src = "goodJets"
#cleanJets.src = "slimmedJetsAK8"
cleanJets.checkOverlaps.muons.src = "goodMuons"
cleanJets.checkOverlaps.muons.deltaR = 1.0
#cleanJets.checkOverlaps.muons.deltaR = 0.
cleanJets.checkOverlaps.muons.requireNoOverlaps = True
cleanJets.checkOverlaps.electrons.src = "goodElectrons"
cleanJets.checkOverlaps.electrons.deltaR = 1.0
#cleanJets.checkOverlaps.electrons.deltaR = 0.
cleanJets.checkOverlaps.electrons.requireNoOverlaps = True
cleanJets.checkOverlaps.photons = cms.PSet()
cleanJets.checkOverlaps.taus = cms.PSet()
cleanJets.checkOverlaps.tkIsoElectrons = cms.PSet()
cleanJets.finalCut = ""#pt > 80"# & abs(eta) < 2.4"#pt > 20 & abs(eta) < 2.4"


cleanAK4Jets = jetCleaner_cfi.cleanPatJets.clone()
cleanAK4Jets.src = "goodAK4Jets"
cleanAK4Jets.checkOverlaps.muons.src = "goodMuons"
cleanAK4Jets.checkOverlaps.muons.deltaR = 0.4
cleanAK4Jets.checkOverlaps.muons.requireNoOverlaps = True
cleanAK4Jets.checkOverlaps.electrons.src = "goodElectrons"
cleanAK4Jets.checkOverlaps.electrons.deltaR = 0.4
cleanAK4Jets.checkOverlaps.electrons.requireNoOverlaps = True
#cleanAK4Jets.checkOverlaps.jets.src = "goodJets"
#cleanAK4Jets.checkOverlaps.jets.deltaR = 0.8
#cleanAK4Jets.checkOverlaps.jets.requireNoOverlaps = True
cleanAK4Jets.checkOverlaps.photons = cms.PSet()
cleanAK4Jets.checkOverlaps.taus = cms.PSet()
cleanAK4Jets.checkOverlaps.tkIsoElectrons = cms.PSet()
cleanAK4Jets.finalCut = ""#pt > 30"# & abs(eta) < 2.4"#pt > 20 & abs(eta) < 2.4"




fatJetsSequence = cms.Sequence( goodJets + cleanJets + goodAK4Jets + cleanAK4Jets )
