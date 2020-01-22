import FWCore.ParameterSet.Config as cms


hadronicV = cms.EDFilter("PATJetSelector",
                         src = cms.InputTag("cleanJets"),
                         cut = cms.string("pt > 100 && (40.0 < mass < 150.0)")
                         )

hadronicVSequence = cms.Sequence(hadronicV)
