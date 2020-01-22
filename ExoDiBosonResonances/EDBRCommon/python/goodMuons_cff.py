import FWCore.ParameterSet.Config as cms

tightMuIdLabel = "tight"
mediumMuIdLabel = "medium"
looseMuIdLabel = "loose"
vetoMuIdLabel = "veto"


#isolationCutString = cms.string("")
#isolationCutString = "(pfIsolationR04().sumChargedHadronPt+max(0.,pfIsolationR04().sumNeutralHadronEt+pfIsolationR04().sumPhotonEt-0.5*pfIsolationR04().sumPUPt))/pt< 0.1"

#goodMuons = cms.EDFilter("PATMuonSelector",
'''GoodMuons = cms.EDProducer("PATMuonIdSelector",

                             src = cms.InputTag("slimmedMuons"),
                             idLabel = cms.string(tightMuIdLabel),
                             #cut = cms.string("pt > 30 && abs(eta) < 2.1 && trackIso/pt<0.1"
                             cut = cms.string("pt > 30 && abs(eta) < 2.1 && trackIso/pt<0.1"

#					      " && isHighPtMuon(vertex->position()) "
##						pt > 30 && abs(eta) < 2.4" 
#                                              "&& isGlobalMuon && isPFMuon "
#                                              " && globalTrack().normalizedChi2<10"
#                                              " && globalTrack().hitPattern().numberOfValidMuonHits>0"
#                                              " && numberOfMatchedStations() > 1"
##                                              " && abs(muonBestTrack()->dxy(vertex->position())) < 0.2 "   
#                                              " && dB() < 0.2 "
##                                              " && abs(muonBestTrack()->dz(vertex->position())) < 0.5 "   
#                                              " && globalTrack().hitPattern().numberOfValidPixelHits>0"
#                                              " && numberOfMatchedStations>1"
#                                              " && globalTrack().hitPattern().trackerLayersWithMeasurement>5"
#                                              " && " + isolationCutString
                                             )
                             )
'''
goodMuons = cms.EDProducer("PATMuonIdSelector",
                             src = cms.InputTag("slimmedMuons"),
                             vertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
                             idLabel = cms.string(tightMuIdLabel),
                             #DO NOT WORK #cut = cms.string("pt > 40 && abs(eta) < 2.1 && trackIso/pt<0.1")
                             )

looseMuons = cms.EDProducer("PATMuonIdSelector",
                             src = cms.InputTag("slimmedMuons"),
                             vertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
                             idLabel = cms.string(looseMuIdLabel),
                             #cut = cms.string("pt > 20 && abs(eta) < 2.4 && trackIso/pt<0.1")
                             )

muSequence = cms.Sequence(goodMuons + looseMuons)
