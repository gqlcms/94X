import FWCore.ParameterSet.Config as cms

#eleisolationCutString = cms.string("")
#eleisolationCutString = "(pfIsolationVariables().sumChargedHadronPt+max(0.0, pfIsolationVariables().sumNeutralHadronEt+pfIsolationVariables().sumPhotonEt-0.5*pfIsolationVariables().sumPUPt))/pt < 0.10"
tightEleIdLabel = "tight"
mediumEleIdLabel = "medium"
looseEleIdLabel = "loose"
vetoEleIdLabel = "veto"

#goodElectrons = cms.EDProducer("PATElectronIdSelector",
#    src = cms.InputTag( "slimmedElectrons" ),
#    idLabel = cms.string(looseEleIdLabel),
#)


#goodElectrons = cms.EDFilter("PATElectronSelector",
#'''GoodElectrons = cms.EDProducer("PATElectronIdSelector",
#                             src = cms.InputTag("slimmedElectrons"),
#                             cut = cms.string("pt > 35 "),#&& abs(eta) < 2.5 "), 
#			     idLabel = cms.string(tightEleIdLabel),
#pt > 90 && abs(eta) < 2.5 "
#                                              " && ecalDrivenSeed()==1"
#                                              " && abs(1.0/ecalEnergy() - eSuperClusterOverP()/ecalEnergy())<0.05 "
##                                              " && abs(gsfTrack()->dxy())<0.02"
##                                              " && abs(gsfTrack()->dz())<0.1"
##                                              " && gsfTrack()->trackerExpectedHitsInner().numberOfLostHits()==0 "
#                                              " && ( abs(convDist())>0.02 || abs(convDcot())>0.02 ) " 
#                                              " && passConversionVeto()==1 "
#                                              " && ( (isEB() && sigmaIetaIeta()<0.01 && abs(deltaPhiSuperClusterTrackAtVtx())<0.03 && abs(deltaEtaSuperClusterTrackAtVtx())<0.004 && hadronicOverEm()<0.12 ) || " +\
#                                              "      (isEE() && sigmaIetaIeta()<0.03 && abs(deltaPhiSuperClusterTrackAtVtx())<0.02 && abs(deltaEtaSuperClusterTrackAtVtx())<0.005 && hadronicOverEm()<0.10 ))"
#                                              " && " + eleisolationCutString
#                                             )
#                             )
#'''

goodElectrons = cms.EDProducer("PATElectronIdSelector",
                             src = cms.InputTag("slimmedElectrons"),
                             #cut = cms.string("pt > 90 "),  # does't work!!!!#&& abs(eta) < 2.5 "), 
                             trkIsolMap=cms.InputTag("heepIDVarValueMaps","eleTrkPtIso"),
                             vid=cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV70"),
                             vertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
			     idLabel = cms.string(tightEleIdLabel),
                             rho = cms.InputTag("fixedGridRhoFastjetAll")
                             )

looseElectrons = cms.EDProducer("PATElectronIdSelector",
                             src = cms.InputTag("slimmedElectrons"),
                             #cut = cms.string("pt > 90 "),#&& abs(eta) < 2.5 "), 
                             trkIsolMap=cms.InputTag("heepIDVarValueMaps","eleTrkPtIso"),
                             vid=cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV70"),
                             vertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
			     idLabel = cms.string(looseEleIdLabel),
                             rho = cms.InputTag("fixedGridRhoFastjetAll")
                             )

eleSequence = cms.Sequence(goodElectrons + looseElectrons)
