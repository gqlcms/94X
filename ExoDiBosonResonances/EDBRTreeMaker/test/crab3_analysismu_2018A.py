from WMCore.Configuration import Configuration
name = 'WWW'
steam_dir = 'xulyu'

config = Configuration()
config.section_("General")
config.General.requestName   = 'SingleMuon_2018A_new'
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName  = 'Analysis'
config.JobType.inputFiles = ['Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFPuppi.txt']
#config.JobType.inputFiles = ['PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt','PHYS14_25_V2_All_L2Relative_AK4PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt','PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt','PHYS14_25_V2_All_L2Relative_AK8PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt']
# Name of the CMSSW configuration file
#config.JobType.psetName    = 'bkg_ana.py'
config.JobType.psetName    = 'analysis.py'
#config.JobType.allowUndistributedCMSSW = True
config.JobType.allowUndistributedCMSSW = True

config.section_("Data")
#config.Data.inputDataset = '/WJetsToLNu_13TeV-madgraph-pythia8-tauola/Phys14DR-PU20bx25_PHYS14_25_V1-v1/MINIAODSIM'
config.Data.inputDataset = '/SingleMuon/Run2018A-PromptReco-v1/MINIAOD'
config.Data.inputDBS = 'global'
#config.Data.inputDBS = 'phys03'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob =50
config.Data.totalUnits = -1
config.Data.lumiMask = 'Cert_314472-317080_13TeV_PromptReco_Collisions18_JSON.txt'

config.Data.publication = False
config.Data.outLFNDirBase = '/store/group/dpg_trigger/comm_trigger/TriggerStudiesGroup/STEAM/' + steam_dir + '/' + name + '/'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'SingleMuon_2018A_new'

config.section_("Site")
# Where the output files will be transmitted to
config.Site.storageSite = 'T2_CH_CERN'
