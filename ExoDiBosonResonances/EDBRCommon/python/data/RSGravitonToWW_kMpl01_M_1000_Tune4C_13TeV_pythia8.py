import FWCore.ParameterSet.Config as cms

maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
#       '/store/mc/Fall13dr/RSGravToZZ_kMpl01_M-1000_Tune4C_13TeV-pythia8/AODSIM/tsg_PU20bx25_POSTLS162_V2-v1/20000/D6458543-E870-E311-BFAF-002618943834.root',
#       '/store/mc/Fall13dr/RSGravToZZ_kMpl01_M-1000_Tune4C_13TeV-pythia8/AODSIM/tsg_PU20bx25_POSTLS162_V2-v1/20000/E8806E66-1A72-E311-9203-003048678B8E.root',
#       '/store/mc/Fall13dr/RSGravToZZ_kMpl01_M-1000_Tune4C_13TeV-pythia8/AODSIM/tsg_PU20bx25_POSTLS162_V2-v1/20000/F2FE64F2-0671-E311-AF38-0026189437F0.root' ] );
       '/store/mc/Phys14DR/RSGravitonToWW_kMpl01_M_1000_Tune4C_13TeV_pythia8/MINIAODSIM/BUNNIES-v3/00000/6C50167B-188B-E411-8F26-02163E00E976.root',
       '/store/mc/Phys14DR/RSGravitonToWW_kMpl01_M_1000_Tune4C_13TeV_pythia8/MINIAODSIM/BUNNIES-v3/00000/DEE8E783-B18A-E411-83C1-02163E00F463.root',
       '/store/mc/Phys14DR/RSGravitonToWW_kMpl01_M_1000_Tune4C_13TeV_pythia8/MINIAODSIM/BUNNIES-v3/30000/1A442016-A08A-E411-A1AF-02163E00E96E.root' ] );

secFiles.extend( [
               ] )

