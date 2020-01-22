from WMCore.Configuration import Configuration
config = Configuration()
config.section_("General")
config.General.requestName   = 'singleMuon-17B-v1_0'
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName  = 'Analysis'
#config.JobType.inputFiles = ['Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK4PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK4PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK4PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK4PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFPuppi.txt']
#config.JobType.inputFiles = ['Summer16_23Sep2016HV3_DATA_L1FastJet_AK4PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L2Relative_AK4PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L3Absolute_AK4PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L2L3Residual_AK4PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L1FastJet_AK8PFchs.txt','Summer16_23Sep2016HV3_DATA_L2Relative_AK8PFchs.txt','Summer16_23Sep2016HV3_DATA_L3Absolute_AK8PFchs.txt','Summer16_23Sep2016HV3_DATA_L2L3Residual_AK8PFchs.txt','Summer16_23Sep2016HV3_DATA_L1FastJet_AK8PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L2Relative_AK8PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L3Absolute_AK8PFPuppi.txt','Summer16_23Sep2016HV3_DATA_L2L3Residual_AK8PFPuppi.txt']
config.JobType.inputFiles = ['Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK4PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFchs.txt','Summer16_23Sep2016BCDV3_DATA_L1FastJet_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2Relative_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L3Absolute_AK8PFPuppi.txt','Summer16_23Sep2016BCDV3_DATA_L2L3Residual_AK8PFPuppi.txt']
# Name of the CMSSW configuration file
#config.JobType.psetName    = 'bkg_ana.py'
config.JobType.psetName    = 'analysis.py'
#config.JobType.allowUndistributedCMSSW = True
config.JobType.allowUndistributedCMSSW = True

config.section_("Data")
config.Data.inputDataset = '/SingleMuon/Run2017B-PromptReco-v1/MINIAOD'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 50
#https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/DCSOnly/json_DCSONLY.txt
config.Data.lumiMask = 'Cert_294927-297723_13TeV_PromptReco_Collisions17_JSON.txt'
config.Data.runRange = ''#'250843-250932' # '193093-194075'
#config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = 'singleMuon-17B-v1_0'

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.storageSite = 'T3_US_FNALLPC'


