// system include files
#include <iostream>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"  

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include <DataFormats/JetReco/interface/Jet.h>
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "EDBRChannels.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include <TFormula.h>

#define Pi 3.141593
using namespace std;
//
// class declaration
//
/*
struct sortPt
{
   bool operator()(TLorentzVector* s1, TLorentzVector* s2) const
   {
      return s1->Pt() >= s2->Pt();
   }
} mysortPt;
*/
class EDBRTreeMaker : public edm::EDAnalyzer {
public:
  explicit EDBRTreeMaker(const edm::ParameterSet&);
  ~EDBRTreeMaker();
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
  virtual void endRun(const edm::Run&, const edm::EventSetup&) override;

  virtual bool looseJetID( const pat::Jet& j);
    virtual const reco::Candidate* findLastW(const reco::Candidate *particle,int IDpdg);
    virtual const reco::Candidate* findFirstW(const reco::Candidate *particle);

     virtual bool tightJetID( const pat::Jet& j);
  virtual float dEtaInSeed( const pat::Electron* ele) ;
  virtual void initJetCorrFactors( void );
  virtual void addTypeICorr( edm::Event const & event );
  virtual double getJEC( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ );
  virtual double getJECOffset( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ );
    
  math::XYZTLorentzVector getNeutrinoP4(double& MetPt, double& MetPhi, TLorentzVector& lep, int lepType);
    

  std::vector<std::string>                    jecAK8PayloadNames_;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK8_            ;
  std::vector<std::string>                    jecAK8PayloadNamesGroomed_;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK8Groomed_            ;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK8GroomedSD_            ;

  std::vector<std::string>                    jecAK8puppiPayloadNames_;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK8puppi_            ;
  std::vector<std::string>                    jecAK8puppiPayloadNamesGroomed_;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK8puppiGroomed_            ;


  std::vector<std::string>                    jecAK4PayloadNames_;
  boost::shared_ptr<FactorizedJetCorrector>   jecAK4_            ;
  std::vector<std::string> offsetCorrLabel_;
  boost::shared_ptr<FactorizedJetCorrector> jecOffset_;

  edm::Handle< double >  rho_;
  edm::InputTag  METsRawLabel_;
  edm::Handle<pat::METCollection>  METs_;
  edm::Handle<pat::JetCollection> jets_;
  edm::Handle<reco::VertexCollection> vertices_;
  edm::EDGetTokenT<pat::MuonCollection> muons_;

  edm::Handle<pat::METCollection>  reclusteredMETs_;
  edm::Handle<edm::View<reco::PFMET> >     pfMET_ ;
  edm::EDGetTokenT<pat::JetCollection> prunedjetInputToken_;
  edm::EDGetTokenT<pat::JetCollection> softdropjetInputToken_;
  edm::EDGetTokenT<pat::JetCollection> fatjetInputToken_;
  edm::EDGetTokenT<pat::JetCollection> puppijetInputToken_;

// add 3 up
  edm::EDGetTokenT<pat::METCollection>  metInputToken_;
  edm::EDGetTokenT<pat::METCollection>  reclusteredmetInputToken_;
  std::vector<edm::EDGetTokenT<pat::METCollection>> mettokens;
  edm::Handle<pat::JetCollection> prunedjets_;
  edm::Handle<pat::JetCollection> softdropjets_;
  edm::Handle<pat::JetCollection> puppijets_;

// add 2 up
  std::vector<edm::EDGetTokenT<pat::JetCollection>> jetTokens;
  edm::EDGetTokenT<pat::METCollection> metToken_;
  edm::EDGetTokenT<pat::METCollection> reclusteredmetToken_;
  edm::EDGetTokenT<pat::JetCollection> jetToken_;
  edm::EDGetTokenT<pat::JetCollection> fatjetToken_;
  edm::EDGetTokenT<pat::JetCollection> prunedjetToken_;
  edm::EDGetTokenT<pat::JetCollection> softdropjetToken_;
  edm::EDGetTokenT<pat::JetCollection> puppijetToken_;
  edm::Handle<pat::JetCollection> fatjets_;
// add 4 up
  edm::EDGetTokenT<double> rhoToken_;
  edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
  edm::EDGetTokenT<pat::MuonCollection> muonToken_;
  std::vector<std::string> jetCorrLabel_;
  std::vector<std::string> jecAK4Labels;
  std::vector<std::string> jecAK8Labels;
  bool doCorrOnTheFly_;
// Filter
  edm::EDGetTokenT<edm::TriggerResults> 		     noiseFilterToken_;
  edm::Handle< edm::TriggerResults> 			     noiseFilterBits_;
  std::string HBHENoiseFilter_Selector_;
  std::string HBHENoiseIsoFilter_Selector_;
  std::string GlobalHaloNoiseFilter_Selector_;
  std::string ECALDeadCellNoiseFilter_Selector_;
  std::string GoodVtxNoiseFilter_Selector_;
  std::string EEBadScNoiseFilter_Selector_;

 // bool doHltFilters_;

  // ----------member data ---------------------------
  TTree* outTree_;

  double MW_;
  int nmetmatch, nmetno;
  int nevent, run, ls;
  int nVtx;
  int numCands;
  int nLooseEle, nLooseMu;//Synch
  double ptVlep, ptVhad, yVlep, yVhadJEC, yVhad, phiVlep, phiVhad, massVlep, massVhad;
  double met, metPhi, mtVlep;
  double tau1, tau2, tau3, tau21, sdrop, sdropJEC, massVhadJEC;

  double jetAK8puppi_ptJEC, jetAK8puppi_eta, jetAK8puppi_phi, jetAK8puppi_tau1,  jetAK8puppi_tau2, jetAK8puppi_tau3, jetAK8puppi_tau4,jetAK8puppi_tau21, jetAK8puppi_tau42, jetAK8puppi_sd, jetAK8puppi_sdJEC, jetAK8puppi_sdcorr;
  double jetAK8puppi_ptJEC_2, jetAK8puppi_eta_2, jetAK8puppi_phi_2, jetAK8puppi_tau1_2,  jetAK8puppi_tau2_2, jetAK8puppi_tau3_2,jetAK8puppi_tau4_2, jetAK8puppi_tau21_2,jetAK8puppi_tau42_2,  jetAK8puppi_sd_2, jetAK8puppi_sdJEC_2, jetAK8puppi_sdcorr_2;
  double vbfeta, vbfmjj,vbfeta_2, vbfmjj_2;
  int      vbftag;
  int nj1, nj2,nj1_2, nj2_2;
  double ptlep1, ptlep2;
  double etalep1, etalep2 ;
  double philep1, philep2 ;
  double triggerWeight, lumiWeight, pileupWeight;
  int channel, lep;
  double deltaRlepjet, delPhilepmet, delPhijetmet, delPhijetlep;
  double deltaRlepjet_2,  delPhijetmet_2, delPhijetlep_2;
  double candMass;
  double pt_graviton,pt_graviton1;
  double candMassJEC, ptVlepJEC, yVlepJEC, phiVlepJEC;
  double candMasspuppiJEC;
    double massww[3],massWWZ,masslvj1,masslvj2,massj1j2;
  double massVlepJEC, mtVlepJEC;

  double theWeight;
  double  nump=0;
  double  numm=0;
  double  npT, npIT;
  int     nBX;
  //Gen Level
  double gen_gra_m, gen_gra_pt, gen_gra_eta,gen_gra_phi;
  double gen_rad_m, gen_rad_pt, gen_rad_eta,gen_rad_phi;
  double gen_ele_pt, gen_ele_eta, gen_ele_phi, gen_ele_e;
  double gen_mu_pt, gen_mu_eta, gen_mu_phi, gen_mu_e;
  double genmatch_ele_pt, genmatch_ele_eta, genmatch_ele_phi, genmatch_ele_e, genmatch_ele_dr;
  double genmatch_mu_pt, genmatch_mu_eta, genmatch_mu_phi, genmatch_mu_e, genmatch_mu_dr;
  double gen_ele_pt_2, gen_ele_eta_2, gen_ele_phi_2, gen_ele_e_2;
  double gen_mu_pt_2, gen_mu_eta_2, gen_mu_phi_2, gen_mu_e_2;
  double gen_ele_pt_3, gen_ele_eta_3, gen_ele_phi_3, gen_ele_e_3;
  double gen_mu_pt_3, gen_mu_eta_3, gen_mu_phi_3, gen_mu_e_3;
  double gentop_pt, gentop_eta, gentop_phi, gentop_mass;
  double genantitop_pt, genantitop_eta, genantitop_phi, genantitop_mass;
  double ptGenVlep, etaGenVlep, phiGenVlep, massGenVlep;
  double ptGenVhad, etaGenVhad, phiGenVhad, massGenVhad;
    double ptGenVhad_2, etaGenVhad_2, phiGenVhad_2, massGenVhad_2;
    double ptGenVhad_3, etaGenVhad_3, phiGenVhad_3, massGenVhad_3;

  double ptGenV_2, etaGenV_2, phiGenV_2, massGenV_2;
  double ptGenV_3, etaGenV_3, phiGenV_3, massGenV_3;
    int status_1,status_2, status_3;
  bool IDLoose, IDTight,IDLoose_2, IDTight_2, isHighPt, isHEEP;
  double muchaiso, muneuiso, muphoiso, muPU, muisolation;
  double iso, isoCut, et, trackIso;
//  double rho,fastJetRho;
  double useless;
	double prundM[3];
	double sdropM[3];
	double prundMtestJEC[3];
	double sdropMtestJEC[3];
	double corr_AK8Groomed[3];
	double  corr_AK8GroomedSD[3];
//  JEC
  double corr_AK8, corr_AK81[3];//, prundM[3], sdropM[3];
  double corr_AK8puppi[3],corr_AK8puppiSD[3];
  double jetAK8_pt,jetAK8_mass,jetAK8_jec,jetAK8_e,jetAK8_eta,jetAK8_phi;
  double jetAK8_pt1[3], jetAK8_mass1[3], jetAK8_SF_mass1[3], jetAK8_SF_mass2[3], jetAK8_jec1[3],jetAK8_eta1[3];
  double jetAK8puppi_pt1[4], jetAK8puppi_mass1[4], jetAK8puppi_eta1[4], jetAK8puppi_jec1[4], jetAK8puppiSD_jec1[4];

  double corr;
  double METraw_et, METraw_phi, METraw_sumEt;
  double MET_et, MET_phi, MET_sumEt, MET_corrPx, MET_corrPy;
  // AK4 Jets
  int ak4jet_hf[8],ak4jet_pf[8],ak4jet_hf_2[8],ak4jet_pf_2[8];
  double ak4jet_pt[8],ak4jet_pt_uncorr[8],ak4jet_eta[8],ak4jet_phi[8],ak4jet_e[8], ak4jet_dr[8]; 
  double ak4jet_csv[8],ak4jet_icsv[8],deltaRAK4AK8[8], ak4jet_IDLoose[8], ak4jet_IDTight[8];
    double ak4jet_pt_2[8],ak4jet_pt_uncorr_2[8],ak4jet_eta_2[8],ak4jet_phi_2[8],ak4jet_e_2[8], ak4jet_dr_2[8];
    double ak4jet_csv_2[8],ak4jet_icsv_2[8],deltaRAK4AK8_2[8], ak4jet_IDLoose_2[8], ak4jet_IDTight_2[8];
    


  void setDummyValues();

  /// Parameters to steer the treeDumper
  int originalNEvents_;
  double crossSectionPb_;
  double targetLumiInvPb_;
  std::string EDBRChannel_;
  bool isGen_;
  bool isJEC_;
  bool RunOnMC_;
//  std::string hadronicVSrc_, leptonicVSrc_;
//  std::string ak4jetsSrc_;
//  std::string gravitonSrc_;//, metSrc_;
//  std::string looseMuonSrc_, looseElectronsSrc_;
//  std::string goodMuSrc_;
  std::vector<JetCorrectorParameters> vPar;
  std::map<std::string,double>  TypeICorrMap_;
  edm::InputTag mets_;


  //High Level Trigger
  HLTConfigProvider hltConfig;
  edm::EDGetTokenT<edm::TriggerResults> hltToken_;
  std::vector<std::string> elPaths1_, elPaths2_, elPaths3_, elPaths4_, elPaths5_, elPaths6_, elPaths7_, elPaths8_;
  std::vector<std::string> muPaths1_, muPaths2_, muPaths3_, muPaths4_, muPaths5_, muPaths6_, muPaths7_, muPaths8_, muPaths9_, muPaths10_, muPaths11_, muPaths12_;
  std::vector<std::string> elPaths1, elPaths2, elPaths3, elPaths4, elPaths5, elPaths6, elPaths7, elPaths8;
  std::vector<std::string> muPaths1, muPaths2, muPaths3, muPaths4, muPaths5, muPaths6, muPaths7, muPaths8, muPaths9, muPaths10, muPaths11, muPaths12;
  int  HLT_Ele1, HLT_Ele2, HLT_Ele3, HLT_Ele4, HLT_Ele5, HLT_Ele6, HLT_Ele7, HLT_Ele8;
  int  HLT_Mu1, HLT_Mu2, HLT_Mu3, HLT_Mu4, HLT_Mu5, HLT_Mu6, HLT_Mu7, HLT_Mu8, HLT_Mu9, HLT_Mu10, HLT_Mu11,  HLT_Mu12;

// filter
  bool passFilter_HBHE_                   ;
  bool passFilter_HBHEIso_                ;
  bool passFilter_GlobalHalo_             ;
  bool passFilter_ECALDeadCell_           ;
  bool passFilter_GoodVtx_                ;
  bool passFilter_EEBadSc_                ;
  bool passFilter_badMuon_                ;
  bool passFilter_badChargedHadron_       ;

  edm::EDGetTokenT<edm::View<reco::Candidate>> leptonicVSrc_;
  edm::EDGetTokenT<edm::View<pat::Jet>> hadronicVSrc_;
  edm::EDGetTokenT<edm::View<pat::Jet>> hadronicVSrc_raw_;
  edm::EDGetTokenT<edm::View<pat::Jet>> ak4jetsSrc_;
  edm::EDGetTokenT<edm::View<pat::Electron> > looseelectronToken_ ;
  edm::EDGetTokenT<edm::View<pat::Muon>> loosemuonToken_;
  edm::EDGetTokenT<edm::View<pat::Muon>> goodMuSrc_;
  edm::EDGetTokenT<edm::View<pat::Muon>> MuSrc_;
  edm::EDGetTokenT<edm::View<pat::Electron> > EleSrc_;
  edm::EDGetTokenT<edm::View<pat::Muon>> t1muSrc_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> gravitonSrc_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> metSrc_;
  edm::EDGetTokenT<GenEventInfoProduct> GenToken_;
  edm::EDGetTokenT<edm::View<reco::GenParticle>> genSrc_;
  edm::EDGetTokenT<std::vector<PileupSummaryInfo>> PUToken_;

};

//
// constructors and destructor
//
EDBRTreeMaker::EDBRTreeMaker(const edm::ParameterSet& iConfig):
  hltToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("hltToken"))),
  elPaths1_(iConfig.getParameter<std::vector<std::string>>("elPaths1")),
  elPaths2_(iConfig.getParameter<std::vector<std::string>>("elPaths2")),
  elPaths3_(iConfig.getParameter<std::vector<std::string>>("elPaths3")),
  elPaths4_(iConfig.getParameter<std::vector<std::string>>("elPaths4")),
  elPaths5_(iConfig.getParameter<std::vector<std::string>>("elPaths6")),
  elPaths6_(iConfig.getParameter<std::vector<std::string>>("elPaths5")),
  elPaths7_(iConfig.getParameter<std::vector<std::string>>("elPaths7")),
  elPaths8_(iConfig.getParameter<std::vector<std::string>>("elPaths8")),
  muPaths1_(iConfig.getParameter<std::vector<std::string>>("muPaths1")),
  muPaths2_(iConfig.getParameter<std::vector<std::string>>("muPaths2")),
  muPaths3_(iConfig.getParameter<std::vector<std::string>>("muPaths3")),
  muPaths4_(iConfig.getParameter<std::vector<std::string>>("muPaths4")),
  muPaths5_(iConfig.getParameter<std::vector<std::string>>("muPaths5")),
  muPaths6_(iConfig.getParameter<std::vector<std::string>>("muPaths6")),
  muPaths7_(iConfig.getParameter<std::vector<std::string>>("muPaths7")),
  muPaths8_(iConfig.getParameter<std::vector<std::string>>("muPaths8")),
  muPaths9_(iConfig.getParameter<std::vector<std::string>>("muPaths9")),
  muPaths10_(iConfig.getParameter<std::vector<std::string>>("muPaths10")),
  muPaths11_(iConfig.getParameter<std::vector<std::string>>("muPaths11")),
  muPaths12_(iConfig.getParameter<std::vector<std::string>>("muPaths12"))//  noiseFilterToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("noiseFilter")))
{
  originalNEvents_ = iConfig.getParameter<int>("originalNEvents");
  crossSectionPb_  = iConfig.getParameter<double>("crossSectionPb");
  targetLumiInvPb_ = iConfig.getParameter<double>("targetLumiInvPb");
  EDBRChannel_     = iConfig.getParameter<std::string>("EDBRChannel");
  isGen_           = iConfig.getParameter<bool>("isGen");
  isJEC_           = iConfig.getParameter<bool>("isJEC");
  RunOnMC_           = iConfig.getParameter<bool>("RunOnMC");
  // Sources
//  leptonicVSrc_ = iConfig.getParameter<std::string>("leptonicVSrc");
  leptonicVSrc_=consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>( "leptonicVSrc") ) ;
  looseelectronToken_    = (consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>("looseElectronSrc"))) ;
  loosemuonToken_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("looseMuonSrc")));
//  gravitonSrc_     = iConfig.getParameter<std::string>("gravitonSrc");
  goodMuSrc_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("goodMuSrc")));
  MuSrc_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("MuSrc")));
  EleSrc_    = (consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>("EleSrc")));

//  goodMuSrc_    = iConfig.getParameter<std::string>("goodMuSrc");
//  looseMuonSrc_    = iConfig.getParameter<std::string>("looseMuonSrc");
//  looseElectronsSrc_= iConfig.getParameter<std::string>("looseElectronsSrc");
  muonToken_ = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
//  ak4jetsSrc_      = iConfig.getParameter<std::string>("ak4jetsSrc");
  ak4jetsSrc_      = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>( "ak4jetsSrc") ) ;

//  hadronicVSrc_ = iConfig.getParameter<std::string>("hadronicVSrc");
  hadronicVSrc_ = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("hadronicVSrc") ) ;
    hadronicVSrc_raw_ = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("hadronicVSrc_raw") ) ;
  jetToken_ = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
  puppijetToken_ = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppijets"));
  fatjetToken_ = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("fatjets"));
  prunedjetToken_ = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("prunedjets"));
  softdropjetToken_ = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("softdropjets"));
// add 4 up
  rhoToken_  = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
  vtxToken_  = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
  GenToken_=consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>( "generator") ) ;
  genSrc_      = consumes<edm::View<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>( "genSrc") ) ;
  PUToken_=consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pileup") ) ;

  metSrc_      = consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>( "metSrc") ) ;
  gravitonSrc_      = consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>( "gravitonSrc") ) ;

//  metSrc_          = iConfig.getParameter<std::string>("metSrc");
  metToken_ = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
  t1muSrc_      = consumes<edm::View<pat::Muon>>(iConfig.getParameter<edm::InputTag>( "t1muSrc") ) ;

// filter
  noiseFilterToken_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("noiseFilter"));
  HBHENoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseFilter");
  HBHENoiseIsoFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseIsoFilter");
  GlobalHaloNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_GlobalTightHaloFilter");
  ECALDeadCellNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_EcalDeadCellTriggerPrimitiveFilter");
  GoodVtxNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_goodVertices");
  EEBadScNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_eeBadScFilter");

  std::string jecpath = iConfig.getParameter<std::string>("jecpath");
  std::string tmpString;
  std::vector<std::string> tmpVec = iConfig.getParameter<std::vector<std::string> >("jecAK8chsPayloadNames");
  for( unsigned int v = 0; v < tmpVec.size(); ++v ){
     tmpString = jecpath + tmpVec[v];
     jecAK8Labels.push_back(tmpString);
  }
  std::vector<std::string> jecAK8LabelsGroomed;
  tmpVec.clear(); tmpVec = iConfig.getParameter<std::vector<std::string> >("jecAK8chsPayloadNamesGroomed");
  for( unsigned int v = 0; v < tmpVec.size(); ++v ){
     tmpString = jecpath + tmpVec[v];
     jecAK8LabelsGroomed.push_back(tmpString);
  }

  std::vector<std::string> jecAK8Labelspuppi;
  tmpVec.clear(); tmpVec = iConfig.getParameter<std::vector<std::string> >("jecAK8puppiPayloadNames");
  for( unsigned int v = 0; v < tmpVec.size(); ++v ){
     tmpString = jecpath + tmpVec[v];
     jecAK8Labelspuppi.push_back(tmpString);
  }

  std::vector<std::string> jecAK8LabelspuppiGroomed;
  tmpVec.clear(); tmpVec = iConfig.getParameter<std::vector<std::string> >("jecAK8puppiPayloadNamesGroomed");
  for( unsigned int v = 0; v < tmpVec.size(); ++v ){
     tmpString = jecpath + tmpVec[v];
     jecAK8LabelspuppiGroomed.push_back(tmpString);
  }



  std::vector<std::string> jecAK4Labels;
  tmpVec.clear(); tmpVec = iConfig.getParameter<std::vector<std::string> >("jecAK4chsPayloadNames");
  for( unsigned int v = 0; v < tmpVec.size(); ++v ){
     tmpString = jecpath + tmpVec[v];
     jecAK4Labels.push_back(tmpString);
  }

  /*=======================================================================================*/
  MW_=80.385;
  nmetmatch = 0;
  nmetno = 0;
  mettokens.push_back( metToken_ );
  mettokens.push_back( reclusteredmetToken_ );
  jetTokens.push_back( jetToken_ );
  jetTokens.push_back( fatjetToken_         );
  jetTokens.push_back( prunedjetToken_      );
  jetTokens.push_back( softdropjetToken_    );
  jetTokens.push_back( puppijetToken_      );

// add 3 up
 

  metInputToken_ = mettokens[0]; 
  reclusteredmetInputToken_ = mettokens[1];

  jetCorrLabel_ = jecAK4Labels;
  offsetCorrLabel_.push_back(jetCorrLabel_[0]);
 
  doCorrOnTheFly_ = false;
  if( jecAK4Labels.size() != 0 && jecAK8Labels.size() != 0 ){

     jecAK4PayloadNames_ = jecAK4Labels;
     //jecAK4PayloadNames_.pop_back();

     jecAK8PayloadNames_ = jecAK8Labels;
     //jecAK8PayloadNames_.pop_back();

     jecAK8PayloadNamesGroomed_ = jecAK8LabelsGroomed;
     //jecAK8PayloadNamesGroomed_.pop_back();

     jecAK8puppiPayloadNames_ = jecAK8Labelspuppi;
     jecAK8puppiPayloadNamesGroomed_ = jecAK8LabelspuppiGroomed;


  fatjetInputToken_ = jetTokens[1];
  prunedjetInputToken_ = jetTokens[2];
  softdropjetInputToken_ = jetTokens[3];
  puppijetInputToken_ = jetTokens[4];
// add 3 up
     initJetCorrFactors();

     doCorrOnTheFly_ = true;

  }

  if(EDBRChannel_ == "VZ_CHANNEL")
    channel=VZ_CHANNEL;
  else if(EDBRChannel_ == "VW_CHANNEL")
    channel=VW_CHANNEL;
  else if(EDBRChannel_ == "VH_CHANNEL")
    channel=VH_CHANNEL;
  else {
    cms::Exception ex("InvalidConfiguration");
    ex << "Unknown channel " << EDBRChannel_  
       << ". Please check EDBRTreeMaker.cc for allowed values.";
    throw ex;
  }
  
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;

  outTree_ = fs->make<TTree>("EDBRCandidates","EDBR Candidates");

  /// Basic event quantities
  outTree_->Branch("run"             ,&run            ,"run/I");//
  outTree_->Branch("ls"              ,&ls             ,"ls/I"             );//Synch
  outTree_->Branch("nLooseEle"       ,&nLooseEle      ,"nLooseEle/I");//
  outTree_->Branch("nLooseMu"        ,&nLooseMu       ,"nLooseMu/I");//
  outTree_->Branch("event"           ,&nevent         ,"event/I"          );
  outTree_->Branch("nVtx"            ,&nVtx           ,"nVtx/I"           );
  outTree_->Branch("numCands"        ,&numCands       ,"numCands/I"       );
  outTree_->Branch("ptVlep"          ,&ptVlep         ,"ptVlep/D"         );
  outTree_->Branch("ptVhad"          ,&ptVhad         ,"ptVhad/D"         );

  outTree_->Branch("jetAK8puppi_ptJEC"          ,&jetAK8puppi_ptJEC         ,"jetAK8puppi_ptJEC/D"         );
  outTree_->Branch("jetAK8puppi_eta"          ,&jetAK8puppi_eta         ,"jetAK8puppi_eta/D"         );
  outTree_->Branch("jetAK8puppi_phi"          ,&jetAK8puppi_phi         ,"jetAK8puppi_phi/D"         );
  outTree_->Branch("jetAK8puppi_tau1"          ,&jetAK8puppi_tau1         ,"jetAK8puppi_tau1/D"         );
  outTree_->Branch("jetAK8puppi_tau2"          ,&jetAK8puppi_tau2         ,"jetAK8puppi_tau2/D"         );
  outTree_->Branch("jetAK8puppi_tau3"          ,&jetAK8puppi_tau3         ,"jetAK8puppi_tau3/D"         );
  outTree_->Branch("jetAK8puppi_tau21"          ,&jetAK8puppi_tau21         ,"jetAK8puppi_tau21/D"         );
    outTree_->Branch("jetAK8puppi_tau4"          ,&jetAK8puppi_tau4         ,"jetAK8puppi_tau4/D"         );
    outTree_->Branch("jetAK8puppi_tau42"          ,&jetAK8puppi_tau42         ,"jetAK8puppi_tau42/D"         );
  outTree_->Branch("jetAK8puppi_sd"          ,&jetAK8puppi_sd         ,"jetAK8puppi_sd/D"         );

  outTree_->Branch("jetAK8puppi_sdJEC"          ,&jetAK8puppi_sdJEC         ,"jetAK8puppi_sdJEC/D"         );
  outTree_->Branch("jetAK8puppi_sdcorr"          ,&jetAK8puppi_sdcorr         ,"jetAK8puppi_sdcorr/D"         );

  outTree_->Branch("jetAK8puppi_ptJEC_2"          ,&jetAK8puppi_ptJEC_2         ,"jetAK8puppi_ptJEC_2/D"         );
  outTree_->Branch("jetAK8puppi_eta_2"          ,&jetAK8puppi_eta_2         ,"jetAK8puppi_eta_2/D"         );
  outTree_->Branch("jetAK8puppi_phi_2"          ,&jetAK8puppi_phi_2         ,"jetAK8puppi_phi_2/D"         );
  outTree_->Branch("jetAK8puppi_tau1_2"          ,&jetAK8puppi_tau1_2         ,"jetAK8puppi_tau1_2/D"         );
  outTree_->Branch("jetAK8puppi_tau2_2"          ,&jetAK8puppi_tau2_2         ,"jetAK8puppi_tau2_2/D"         );
  outTree_->Branch("jetAK8puppi_tau3_2"          ,&jetAK8puppi_tau3_2         ,"jetAK8puppi_tau3_2/D"         );
  outTree_->Branch("jetAK8puppi_tau21_2"          ,&jetAK8puppi_tau21_2         ,"jetAK8puppi_tau21_2/D"         );
    outTree_->Branch("jetAK8puppi_tau4_2"          ,&jetAK8puppi_tau4_2         ,"jetAK8puppi_tau4_2/D"         );
    outTree_->Branch("jetAK8puppi_tau42_2"          ,&jetAK8puppi_tau42_2         ,"jetAK8puppi_tau42_2/D"         );
  outTree_->Branch("jetAK8puppi_sd_2"          ,&jetAK8puppi_sd_2         ,"jetAK8puppi_sd_2/D"         );
    
  outTree_->Branch("jetAK8puppi_sdJEC_2"          ,&jetAK8puppi_sdJEC_2         ,"jetAK8puppi_sdJEC_2/D"         );
  outTree_->Branch("jetAK8puppi_sdcorr_2"          ,&jetAK8puppi_sdcorr_2         ,"jetAK8puppi_sdcorr_2/D"         );

  outTree_->Branch("vbfeta"    ,&vbfeta   ,"vbfeta/D"   );
  outTree_->Branch("vbfmjj"    ,&vbfmjj   ,"vbfmjj/D"   );
    outTree_->Branch("vbfeta_2"    ,&vbfeta_2   ,"vbfeta_2/D"   );
    outTree_->Branch("vbfmjj_2"    ,&vbfmjj_2   ,"vbfmjj_2/D"   );

  outTree_->Branch("vbftag"    ,&vbftag   ,"vbftag/I"   );
  outTree_->Branch("nj1"    ,&nj1   ,"nj1/I"   );
  outTree_->Branch("nj2"    ,&nj2   ,"nj2/I"   );
    outTree_->Branch("nj1_2"    ,&nj1_2   ,"nj1_2/I"   );
    outTree_->Branch("nj2_2"    ,&nj2_2   ,"nj2_2/I"   );

  outTree_->Branch("yVlep"           ,&yVlep          ,"yVlep/D"          );
  outTree_->Branch("yVhad"           ,&yVhad          ,"yVhad/D"          );
  outTree_->Branch("yVhadJEC"           ,&yVhadJEC          ,"yVhadJEC/D"          );
  outTree_->Branch("phiVlep"         ,&phiVlep        ,"phiVlep/D"        );
  outTree_->Branch("phiVhad"         ,&phiVhad        ,"phiVhad/D"        );
  outTree_->Branch("massVlep"        ,&massVlep       ,"massVlep/D"       );
  outTree_->Branch("mtVlep"          ,&mtVlep         ,"mtVlep/D"         );
  outTree_->Branch("massVhad"        ,&massVhad       ,"massVhad/D"       );
  outTree_->Branch("tau1"            ,&tau1           ,"tau1/D"           );
  outTree_->Branch("tau2"            ,&tau2           ,"tau2/D"           );
  outTree_->Branch("tau3"            ,&tau3           ,"tau3/D"           );
  outTree_->Branch("tau21"           ,&tau21          ,"tau21/D"          );
  outTree_->Branch("lep"             ,&lep            ,"lep/I"            );
  outTree_->Branch("channel"         ,&channel        ,"channel/I"        );
  outTree_->Branch("candMass"        ,&candMass       ,"candMass/D"       );

 
  /// Generic kinematic quantities
  outTree_->Branch("ptlep1"          ,&ptlep1         ,"ptlep1/D"         );
  outTree_->Branch("ptlep2"          ,&ptlep2         ,"ptlep2/D"         );
  outTree_->Branch("etalep1"         ,&etalep1        ,"etalep1/D"        );
  outTree_->Branch("etalep2"         ,&etalep2        ,"etalep2/D"        );
  outTree_->Branch("philep1"         ,&philep1        ,"philep1/D"        );
  outTree_->Branch("philep2"         ,&philep2        ,"philep2/D"        );
  outTree_->Branch("met"             ,&met            ,"met/D"            );
  outTree_->Branch("metPhi"          ,&metPhi         ,"metPhi/D"         );

  /// Other quantities
  outTree_->Branch("theWeight", &theWeight, "theWeight/D");  
  outTree_->Branch("nump", &nump, "nump/D");  
  outTree_->Branch("numm", &numm, "numm/D");  
  outTree_->Branch("npT"           ,&npT         ,"npT/D"          );
  outTree_->Branch("npIT"           ,&npIT         ,"npIT/D"          );
  outTree_->Branch("nBX"           ,&nBX         ,"nBX/I"          );
  outTree_->Branch("triggerWeight"   ,&triggerWeight  ,"triggerWeight/D"  );
  outTree_->Branch("lumiWeight"      ,&lumiWeight     ,"lumiWeight/D"     );
  outTree_->Branch("pileupWeight"    ,&pileupWeight   ,"pileupWeight/D"   );
  outTree_->Branch("delPhilepmet"    ,&delPhilepmet   ,"delPhilepmet/D"   );
  outTree_->Branch("deltaRlepjet"    ,&deltaRlepjet   ,"deltaRlepjet/D"   );
  outTree_->Branch("delPhijetmet"    ,&delPhijetmet   ,"delPhijetmet/D"   );
  outTree_->Branch("delPhijetlep"    ,&delPhijetlep   ,"delPhijetlep/D"   );
  
  
  outTree_->Branch("deltaRlepjet_2"    ,&deltaRlepjet_2   ,"deltaRlepjet_2/D"   );
  outTree_->Branch("delPhijetmet_2"    ,&delPhijetmet_2   ,"delPhijetmet_2/D"   );
  outTree_->Branch("delPhijetlep_2"    ,&delPhijetlep_2   ,"delPhijetlep_2/D"   );
 
  outTree_->Branch("IDLoose", &IDLoose, "IDLoose/O");
  outTree_->Branch("IDTight", &IDTight, "IDTight/O");
  outTree_->Branch("IDLoose_2", &IDLoose_2, "IDLoose_2/O");
  outTree_->Branch("IDTight_2", &IDTight_2, "IDTight_2/O");
  outTree_->Branch("isHighPt",&isHighPt, "isHighPt/O");
  outTree_->Branch("isHEEP",&isHEEP, "isHEEP/O");
  outTree_->Branch("trackIso",&trackIso,"trackIso/D");
  outTree_->Branch("muchaiso",&muchaiso,"muchaiso/D");
  outTree_->Branch("muneuiso",&muneuiso,"muneuiso/D");
  outTree_->Branch("muphoiso",&muphoiso,"muphoiso/D");
  outTree_->Branch("muPU",&muPU,"muPU/D");
  outTree_->Branch("muisolation",&muisolation,"muisolation/D");
//after JEC varible
  outTree_->Branch("METraw_et",&METraw_et,"METraw_et/D");
  outTree_->Branch("METraw_phi",&METraw_phi,"METraw_phi/D");
  outTree_->Branch("METraw_sumEt",&METraw_sumEt,"METraw_sumEt/D");
  outTree_->Branch("MET_et",&MET_et,"MET_et/D");
  outTree_->Branch("MET_phi",&MET_phi,"MET_phi/D");
  outTree_->Branch("MET_sumEt",&MET_sumEt,"MET_sumEt/D");
//  outTree_->Branch("MET_corrPx",&MET_corrPx,"MET_corrPx/D");
//  outTree_->Branch("MET_corrPy",&MET_corrPy,"MET_corrPy/D");

  outTree_->Branch("jetAK8_pt",&jetAK8_pt,"jetAK8_pt/D");
  outTree_->Branch("jetAK8_mass",&jetAK8_mass,"jetAK8_mass/D");
  outTree_->Branch("jetAK8_jec",&jetAK8_jec,"jetAK8_jec/D");
  outTree_->Branch("jetAK8_pt1",&jetAK8_pt1,"jetAK8_pt1[3]/D");
  outTree_->Branch("jetAK8_eta1",&jetAK8_eta1,"jetAK8_eta1[3]/D");
  outTree_->Branch("jetAK8_mass1",&jetAK8_mass1,"jetAK8_mass1[3]/D");
    outTree_->Branch("jetAK8puppi_pt1",&jetAK8puppi_pt1,"jetAK8puppi_pt1[4]/D");
    outTree_->Branch("jetAK8puppi_eta1",&jetAK8puppi_eta1,"jetAK8puppi_eta1[4]/D");
    outTree_->Branch("jetAK8puppi_mass1",&jetAK8puppi_mass1,"jetAK8puppi_mass1[4]/D");
  outTree_->Branch("jetAK8_SF_mass1",&jetAK8_SF_mass1,"jetAK8_SF_mass1[3]/D");
  outTree_->Branch("jetAK8_SF_mass2",&jetAK8_SF_mass2,"jetAK8_SF_mass2[3]/D");
  outTree_->Branch("jetAK8_jec1",&jetAK8_jec1,"jetAK8_jec1[3]/D");
//  outTree_->Branch("prundM",&prundM,"prundM[3]/D");
//  outTree_->Branch("sdropM",&sdropM,"sdropM[3]/D");
  outTree_->Branch("jetAK8_eta",&jetAK8_eta,"jetAK8_eta/D");
  outTree_->Branch("jetAK8_phi",&jetAK8_phi,"jetAK8_phi/D");

  outTree_->Branch("candMassJEC",&candMassJEC,"candMassJEC/D");
  outTree_->Branch("candMasspuppiJEC",&candMasspuppiJEC,"candMasspuppiJEC/D");
    outTree_->Branch("massww",&massww,"massww[3]/D");
    outTree_->Branch("masslvj1",&masslvj1,"masslvj1/D");
    outTree_->Branch("masslvj2",&masslvj2,"masslvj2/D");
    outTree_->Branch("massj1j2",&massj1j2,"massj1j2/D");

    outTree_->Branch("massWWZ",&massWWZ,"massWWZ/D");
  outTree_->Branch("ptVlepJEC",&ptVlepJEC,"ptVlepJEC/D");
  outTree_->Branch("yVlepJEC",&yVlepJEC,"yVlepJEC/D");
  outTree_->Branch("phiVlepJEC",&phiVlepJEC,"phiVlepJEC/D");
  outTree_->Branch("massVlepJEC",&massVlepJEC,"massVlepJEC/D");
  outTree_->Branch("massVhadJEC"        ,&massVhadJEC       ,"massVhadJEC/D"       );
  outTree_->Branch("mtVlepJEC",&mtVlepJEC,"mtVlepJEC/D");

  ///HLT bits
  outTree_->Branch("HLT_Ele1"  ,&HLT_Ele1 ,"HLT_Ele1/I" );
  outTree_->Branch("HLT_Ele2"  ,&HLT_Ele2 ,"HLT_Ele2/I" );
  outTree_->Branch("HLT_Ele3"  ,&HLT_Ele3 ,"HLT_Ele3/I" );
  outTree_->Branch("HLT_Ele4"  ,&HLT_Ele4 ,"HLT_Ele4/I" );
  outTree_->Branch("HLT_Ele5"  ,&HLT_Ele5 ,"HLT_Ele5/I" );
  outTree_->Branch("HLT_Ele6"  ,&HLT_Ele6 ,"HLT_Ele6/I" );
  outTree_->Branch("HLT_Ele7"  ,&HLT_Ele7 ,"HLT_Ele7/I" );
  outTree_->Branch("HLT_Ele8"  ,&HLT_Ele8 ,"HLT_Ele8/I" );
  outTree_->Branch("HLT_Mu1"   ,&HLT_Mu1  ,"HLT_Mu1/I"  );
  outTree_->Branch("HLT_Mu2"   ,&HLT_Mu2  ,"HLT_Mu2/I"  );
  outTree_->Branch("HLT_Mu3"   ,&HLT_Mu3  ,"HLT_Mu3/I"  );
  outTree_->Branch("HLT_Mu4"   ,&HLT_Mu4  ,"HLT_Mu4/I"  );
  outTree_->Branch("HLT_Mu5"   ,&HLT_Mu5  ,"HLT_Mu5/I"  );
  outTree_->Branch("HLT_Mu6"   ,&HLT_Mu6  ,"HLT_Mu6/I"  );
  outTree_->Branch("HLT_Mu7"   ,&HLT_Mu7  ,"HLT_Mu7/I"  );
  outTree_->Branch("HLT_Mu8"   ,&HLT_Mu8  ,"HLT_Mu8/I"  );
  outTree_->Branch("HLT_Mu9"   ,&HLT_Mu9  ,"HLT_Mu9/I"  );
  outTree_->Branch("HLT_Mu10"   ,&HLT_Mu10  ,"HLT_Mu10/I"  );
  outTree_->Branch("HLT_Mu11"   ,&HLT_Mu11  ,"HLT_Mu11/I"  );
  outTree_->Branch("HLT_Mu12"   ,&HLT_Mu12  ,"HLT_Mu12/I"  );
// filter
  outTree_->Branch("passFilter_HBHE"                 ,&passFilter_HBHE_                ,"passFilter_HBHE_/O");
  outTree_->Branch("passFilter_HBHEIso"                 ,&passFilter_HBHEIso_                ,"passFilter_HBHEIso_/O");
  outTree_->Branch("passFilter_GlobalHalo"              ,&passFilter_GlobalHalo_             ,"passFilter_GlobalHalo_/O");
  outTree_->Branch("passFilter_ECALDeadCell"         ,&passFilter_ECALDeadCell_        ,"passFilter_ECALDeadCell_/O");
  outTree_->Branch("passFilter_GoodVtx"              ,&passFilter_GoodVtx_             ,"passFilter_GoodVtx_/O");
  outTree_->Branch("passFilter_EEBadSc"              ,&passFilter_EEBadSc_             ,"passFilter_EEBadSc_/O");
  outTree_->Branch("passFilter_badMuon"                 ,&passFilter_badMuon_                ,"passFilter_badMuon_/O");
  outTree_->Branch("passFilter_badChargedHadron"                 ,&passFilter_badChargedHadron_                ,"passFilter_badChargedHadron_/O");

  /// AK4 Jets Info
  outTree_->Branch("ak4jet_hf"        , ak4jet_hf       ,"ak4jet_hf[8]/I"       );
  outTree_->Branch("ak4jet_pf"        , ak4jet_pf       ,"ak4jet_pf[8]/I"       );
  outTree_->Branch("ak4jet_pt"        , ak4jet_pt       ,"ak4jet_pt[8]/D"       );
  outTree_->Branch("ak4jet_pt_uncorr"        , ak4jet_pt_uncorr       ,"ak4jet_pt_uncorr[8]/D"       );
  outTree_->Branch("ak4jet_eta"        , ak4jet_eta       ,"ak4jet_eta[8]/D"       );
  outTree_->Branch("ak4jet_phi"        , ak4jet_phi       ,"ak4jet_phi[8]/D"       );
  outTree_->Branch("ak4jet_e"        , ak4jet_e       ,"ak4jet_e[8]/D"       );
  outTree_->Branch("ak4jet_dr"        , ak4jet_dr       ,"ak4jet_dr[8]/D"       );
  outTree_->Branch("ak4jet_csv"        , ak4jet_csv       ,"ak4jet_csv[8]/D"       );
  outTree_->Branch("ak4jet_icsv"        , ak4jet_icsv       ,"ak4jet_icsv[8]/D"       );
  outTree_->Branch("deltaRAK4AK8"        , deltaRAK4AK8       ,"deltaRAK4AK8[8]/D"       );
  outTree_->Branch("ak4jet_IDLoose"        , ak4jet_IDLoose       ,"ak4jet_IDLoose[8]/D"       );
  outTree_->Branch("ak4jet_IDTight"        , ak4jet_IDTight       ,"ak4jet_IDTight[8]/D"       );
    outTree_->Branch("ak4jet_hf_2"        , ak4jet_hf_2       ,"ak4jet_hf_2[8]/I"       );
    outTree_->Branch("ak4jet_pf_2"        , ak4jet_pf_2       ,"ak4jet_pf[8]_2/I"       );
    outTree_->Branch("ak4jet_pt_2"        , ak4jet_pt_2       ,"ak4jet_pt[8]_2/D"       );
    outTree_->Branch("ak4jet_pt_uncorr_2"        , ak4jet_pt_uncorr_2       ,"ak4jet_pt_uncorr_2[8]/D"       );
    outTree_->Branch("ak4jet_eta_2"        , ak4jet_eta_2       ,"ak4jet_eta_2[8]/D"       );
    outTree_->Branch("ak4jet_phi_2"        , ak4jet_phi_2       ,"ak4jet_phi_2[8]/D"       );
    outTree_->Branch("ak4jet_e_2"        , ak4jet_e_2       ,"ak4jet_e_2[8]/D"       );
    outTree_->Branch("ak4jet_dr_2"        , ak4jet_dr_2       ,"ak4jet_dr_2[8]/D"       );
    outTree_->Branch("ak4jet_csv_2"        , ak4jet_csv_2       ,"ak4jet_csv_2[8]/D"       );
    outTree_->Branch("ak4jet_icsv_2"        , ak4jet_icsv_2       ,"ak4jet_icsv_2[8]/D"       );
    outTree_->Branch("deltaRAK4AK8_2"        , deltaRAK4AK8_2       ,"deltaRAK4AK8_2[8]/D"       );
    outTree_->Branch("ak4jet_IDLoose_2"        , ak4jet_IDLoose_2       ,"ak4jet_IDLoose_2[8]/D"       );
    outTree_->Branch("ak4jet_IDTight_2"        , ak4jet_IDTight_2       ,"ak4jet_IDTight_2[8]/D"       );


  /// Gen Level quantities
  outTree_->Branch("gen_gra_m"        ,&gen_gra_m       ,"gen_gra_m/D"       );
  outTree_->Branch("gen_gra_pt"        ,&gen_gra_pt       ,"gen_gra_pt/D"       );
    outTree_->Branch("gen_gra_phi"        ,&gen_gra_phi       ,"gen_gra_phi/D"       );

  outTree_->Branch("gen_gra_eta"        ,&gen_gra_eta       ,"gen_gra_eta/D"       );
  outTree_->Branch("gen_rad_m"        ,&gen_rad_m       ,"gen_rad_m/D"       );
  outTree_->Branch("gen_rad_pt"        ,&gen_rad_pt       ,"gen_rad_pt/D"       );
    outTree_->Branch("gen_rad_phi"        ,&gen_rad_phi       ,"gen_rad_phi/D"       );

  outTree_->Branch("gen_rad_eta"        ,&gen_rad_eta       ,"gen_rad_eta/D"       );
  outTree_->Branch("gen_ele_pt"        ,&gen_ele_pt       ,"gen_ele_pt/D"       );
  outTree_->Branch("gen_ele_eta"        ,&gen_ele_eta       ,"gen_ele_eta/D"       );
  outTree_->Branch("gen_ele_phi"        ,&gen_ele_phi       ,"gen_ele_phi/D"       );
  outTree_->Branch("gen_ele_e"        ,&gen_ele_e       ,"gen_ele_e/D"       );
  outTree_->Branch("gen_mu_pt"        ,&gen_mu_pt       ,"gen_mu_pt/D"       );
  outTree_->Branch("gen_mu_eta"        ,&gen_mu_eta       ,"gen_mu_eta/D"       );
  outTree_->Branch("gen_mu_phi"        ,&gen_mu_phi       ,"gen_mu_phi/D"       );
  outTree_->Branch("gen_mu_e"        ,&gen_mu_e       ,"gen_mu_e/D"       );
  outTree_->Branch("gen_ele_pt_2"        ,&gen_ele_pt_2       ,"gen_ele_pt_2/D"       );
  outTree_->Branch("gen_ele_eta_2"        ,&gen_ele_eta_2       ,"gen_ele_eta_2/D"       );
  outTree_->Branch("gen_ele_phi_2"        ,&gen_ele_phi_2       ,"gen_ele_phi_2/D"       );
  outTree_->Branch("gen_ele_e_2"        ,&gen_ele_e_2       ,"gen_ele_e_2/D"       );
  outTree_->Branch("gen_mu_pt_2"        ,&gen_mu_pt_2       ,"gen_mu_pt_2/D"       );
  outTree_->Branch("gen_mu_eta_2"        ,&gen_mu_eta_2       ,"gen_mu_eta_2/D"       );
  outTree_->Branch("gen_mu_phi_2"        ,&gen_mu_phi_2       ,"gen_mu_phi_2/D"       );
  outTree_->Branch("gen_mu_e_2"        ,&gen_mu_e_2       ,"gen_mu_e_2/D"       );
  outTree_->Branch("gen_ele_pt_3"        ,&gen_ele_pt_3       ,"gen_ele_pt_3/D"       );
  outTree_->Branch("gen_ele_eta_3"        ,&gen_ele_eta_3       ,"gen_ele_eta_3/D"       );
  outTree_->Branch("gen_ele_phi_3"        ,&gen_ele_phi_3       ,"gen_ele_phi_3/D"       );
  outTree_->Branch("gen_ele_e_3"        ,&gen_ele_e_3       ,"gen_ele_e_3/D"       );
  outTree_->Branch("gen_mu_pt_3"        ,&gen_mu_pt_3       ,"gen_mu_pt_3/D"       );
  outTree_->Branch("gen_mu_eta_3"        ,&gen_mu_eta_3       ,"gen_mu_eta_3/D"       );
  outTree_->Branch("gen_mu_phi_3"        ,&gen_mu_phi_3       ,"gen_mu_phi_3/D"       );
  outTree_->Branch("gen_mu_e_3"        ,&gen_mu_e_3       ,"gen_mu_e_3/D"       );
  outTree_->Branch("genmatch_ele_pt"        ,&genmatch_ele_pt       ,"genmatch_ele_pt/D"       );
  outTree_->Branch("genmatch_ele_eta"        ,&genmatch_ele_eta       ,"genmatch_ele_eta/D"       );
  outTree_->Branch("genmatch_ele_phi"        ,&genmatch_ele_phi       ,"genmatch_ele_phi/D"       );
  outTree_->Branch("genmatch_ele_e"        ,&genmatch_ele_e       ,"genmatch_ele_e/D"       );
  outTree_->Branch("genmatch_ele_dr"        ,&genmatch_ele_dr       ,"genmatch_ele_dr/D"       );
  outTree_->Branch("genmatch_mu_pt"        ,&genmatch_mu_pt       ,"genmatch_mu_pt/D"       );
  outTree_->Branch("genmatch_mu_eta"        ,&genmatch_mu_eta       ,"genmatch_mu_eta/D"       );
  outTree_->Branch("genmatch_mu_phi"        ,&genmatch_mu_phi       ,"genmatch_mu_phi/D"       );
  outTree_->Branch("genmatch_mu_e"        ,&genmatch_mu_e       ,"genmatch_mu_e/D"       );
  outTree_->Branch("genmatch_mu_dr"        ,&genmatch_mu_dr       ,"genmatch_mu_dr/D"       );
  outTree_->Branch("gentop_pt"        ,&gentop_pt       ,"gentop_pt/D"       );
  outTree_->Branch("gentop_eta"        ,&gentop_eta       ,"gentop_eta/D"       );
  outTree_->Branch("gentop_phi"        ,&gentop_phi       ,"gentop_phi/D"       );
  outTree_->Branch("gentop_mass"        ,&gentop_mass       ,"gentop_mass/D"       );
  outTree_->Branch("genantitop_pt"        ,&genantitop_pt       ,"genantitop_pt/D"       );
  outTree_->Branch("genantitop_eta"        ,&genantitop_eta       ,"genantitop_eta/D"       );
  outTree_->Branch("genantitop_phi"        ,&genantitop_phi       ,"genantitop_phi/D"       );
  outTree_->Branch("genantitop_mass"        ,&genantitop_mass       ,"genantitop_mass/D"       );
  outTree_->Branch("ptGenVlep"        ,&ptGenVlep       ,"ptGenVlep/D"       );
  outTree_->Branch("etaGenVlep"        ,&etaGenVlep       ,"etaGenVlep/D"       );
  outTree_->Branch("phiGenVlep"        ,&phiGenVlep       ,"phiGenVlep/D"       );
  outTree_->Branch("massGenVlep"        ,&massGenVlep       ,"massGenVlep/D"       );
  outTree_->Branch("ptGenVhad"        ,&ptGenVhad       ,"ptGenVhad/D"       );
  outTree_->Branch("etaGenVhad"        ,&etaGenVhad       ,"etaGenVhad/D"       );
  outTree_->Branch("phiGenVhad"        ,&phiGenVhad       ,"phiGenVhad/D"       );
  outTree_->Branch("massGenVhad"        ,&massGenVhad       ,"massGenVhad/D"       );
    outTree_->Branch("ptGenVhad_2"        ,&ptGenVhad_2       ,"ptGenVhad_2/D"       );
    outTree_->Branch("etaGenVhad_2"        ,&etaGenVhad_2       ,"etaGenVhad_2/D"       );
    outTree_->Branch("phiGenVhad_2"        ,&phiGenVhad_2       ,"phiGenVhad_2/D"       );
    outTree_->Branch("massGenVhad_2"        ,&massGenVhad_2       ,"massGenVhad_2/D"       );
    outTree_->Branch("ptGenVhad_3"        ,&ptGenVhad_3       ,"ptGenVhad_3/D"       );
    outTree_->Branch("etaGenVhad_3"        ,&etaGenVhad_3       ,"etaGenVhad_3/D"       );
    outTree_->Branch("phiGenVhad_3"        ,&phiGenVhad_3       ,"phiGenVhad_3/D"       );
    outTree_->Branch("massGenVhad_3"        ,&massGenVhad_3       ,"massGenVhad_3/D"       );

  outTree_->Branch("ptGenV_2"        ,&ptGenV_2       ,"ptGenV_2/D"       );
  outTree_->Branch("etaGenV_2"        ,&etaGenV_2       ,"etaGenV_2/D"       );
  outTree_->Branch("phiGenV_2"        ,&phiGenV_2       ,"phiGenV_2/D"       );
  outTree_->Branch("massGenV_2"        ,&massGenV_2       ,"massGenV_2/D"       );
  outTree_->Branch("ptGenV_3"        ,&ptGenV_3       ,"ptGenV_3/D"       );
  outTree_->Branch("etaGenV_3"        ,&etaGenV_3       ,"etaGenV_3/D"       );
  outTree_->Branch("phiGenV_3"        ,&phiGenV_3       ,"phiGenV_3/D"       );
  outTree_->Branch("massGenV_3"        ,&massGenV_3       ,"massGenV_3/D"       );
    outTree_->Branch("status_1"           ,&status_1         ,"status_1/I"          );
  outTree_->Branch("status_2"           ,&status_2         ,"status_2/I"          );
  outTree_->Branch("status_3"           ,&status_3         ,"status_3/I"          );

  //outTree_->Branch("");

}

const reco::Candidate*  EDBRTreeMaker::findLastW(const reco::Candidate *particle,int IDpdg){
    
    int iw=0;
    int pidw=0;
    const reco::Candidate* pw=particle;
    //cout<<"check 1 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<endl;
    for(int ii=0;particle->daughter(ii)!=NULL;ii++){
        
        if(abs(particle->daughter(ii)->pdgId())>pidw) {
            iw=ii;
            pidw=abs(particle->daughter(ii)->pdgId());
            //cout<<"check 2 "<<iw<<"    "<<pidw<<"   "<<endl;
        }
    }
    
    if( abs(pidw) == IDpdg ){
        pw = particle->daughter(iw);
        //cout<<"check 5 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<endl;

        return (findLastW(pw,IDpdg));
        
        
    }
    //cout<<"check 3 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<pw->daughter(0)->pdgId()<<"    "<<endl;
    return pw;
    }

const reco::Candidate*  EDBRTreeMaker::findFirstW(const reco::Candidate *particle){
    
    if (particle->mother(0)!=NULL){
        
    if(abs(particle->mother(0)->pdgId()) == 24 )
        return (findFirstW(particle->mother(0)));

    }
    //cout<<"check 3 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<pw->daughter(0)->pdgId()<<"    "<<endl;
    return particle;
}


EDBRTreeMaker::~EDBRTreeMaker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


bool
EDBRTreeMaker::looseJetID( const pat::Jet& j ) {
// refer to https://twiki.cern.ch/twiki/bin/view/CMS/JetID#Recommendations_for_13_TeV_data
	double NHF = j.neutralHadronEnergyFraction();
	double NEMF = j.neutralEmEnergyFraction();
	double CHF = j.chargedHadronEnergyFraction();
	//double MUF = j.muonEnergyFraction();
	double CEMF = j.chargedEmEnergyFraction();
	int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
	int NumNeutralParticle =j.neutralMultiplicity();
	int CHM = j.chargedMultiplicity(); 
	double eta = j.eta();

//	return (( (NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=3.0  ) || (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0) );

        return ((  (NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=2.7 ) || (NHF<0.98 && NEMF>0.01 && NumNeutralParticle>2 && abs(eta)>2.7 && abs(eta)<=3.0 ) || (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0) ) ;

}

bool
EDBRTreeMaker::tightJetID( const pat::Jet& j ) {
// refer to https://twiki.cern.ch/twiki/bin/view/CMS/JetID#Recommendations_for_13_TeV_data
        double NHF = j.neutralHadronEnergyFraction();
        double NEMF = j.neutralEmEnergyFraction();
        double CHF = j.chargedHadronEnergyFraction();
        //double MUF = j.muonEnergyFraction();
        double CEMF = j.chargedEmEnergyFraction();
        int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
        int NumNeutralParticle =j.neutralMultiplicity();
        int CHM = j.chargedMultiplicity();
        double eta = j.eta();
	
	return ( (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=3.0 ) || (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 )  ;

}

float
EDBRTreeMaker::dEtaInSeed( const pat::Electron*  ele ){
  return ele->superCluster().isNonnull() && ele->superCluster()->seed().isNonnull() ?
    ele->deltaEtaSuperClusterTrackAtVtx() - ele->superCluster()->eta() + ele->superCluster()->seed()->eta() : std::numeric_limits<float>::max();
}

void EDBRTreeMaker::initJetCorrFactors( void ){
  std::vector<JetCorrectorParameters> vPar;
  for ( std::vector<std::string>::const_iterator payloadBegin = jecAK8PayloadNames_.begin(), payloadEnd = jecAK8PayloadNames_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }

  // Make the FactorizedJetCorrector
  jecAK8_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );

  vPar.clear();
  for ( std::vector<std::string>::const_iterator payloadBegin = jecAK8PayloadNamesGroomed_.begin(), payloadEnd = jecAK8PayloadNamesGroomed_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }

  // Make the FactorizedJetCorrector
  jecAK8Groomed_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );
  
  vPar.clear();
  for ( std::vector<std::string>::const_iterator payloadBegin = jecAK8PayloadNamesGroomed_.begin(), payloadEnd = jecAK8PayloadNamesGroomed_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }

  jecAK8GroomedSD_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );



  vPar.clear();
  for ( std::vector<std::string>::const_iterator payloadBegin = jecAK8puppiPayloadNames_.begin(), payloadEnd = jecAK8puppiPayloadNames_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }
 jecAK8puppi_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );

  vPar.clear();
  for ( std::vector<std::string>::const_iterator payloadBegin = jecAK8puppiPayloadNamesGroomed_.begin(), payloadEnd = jecAK8puppiPayloadNamesGroomed_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }
 jecAK8puppiGroomed_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );




  vPar.clear();
  for ( std::vector<std::string>::const_iterator payloadBegin = jecAK4PayloadNames_.begin(), payloadEnd = jecAK4PayloadNames_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }

  // Make the FactorizedJetCorrector
  jecAK4_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );

  vPar.clear();
  for ( std::vector<std::string>::const_iterator payloadBegin = offsetCorrLabel_.begin(), payloadEnd = offsetCorrLabel_.end(), ipayload = payloadBegin; ipayload != payloadEnd; ++ipayload ) {
     JetCorrectorParameters pars(*ipayload);
     vPar.push_back(pars);
  }

  jecOffset_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );

}


double EDBRTreeMaker::getJEC( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ ){

   double jetCorrFactor = 1.;
   if ( fabs(rawJetP4.eta()) < jetCorrEtaMax ){
      jecAK4_->setJetEta( rawJetP4.eta() );
      jecAK4_->setJetPt ( rawJetP4.pt() );
      jecAK4_->setJetE  ( rawJetP4.energy() );
      jecAK4_->setJetPhi( rawJetP4.phi()    );
      jecAK4_->setJetA  ( jet.jetArea() );
      jecAK4_->setRho   ( *(rho_.product()) );
      jecAK4_->setNPV   ( nVtx );
      jetCorrFactor = jecAK4_->getCorrection();
   }

   reco::Candidate::LorentzVector corrJetP4 = rawJetP4;
   corrJetP4 *= jetCorrFactor;

   return jetCorrFactor;

}

double EDBRTreeMaker::getJECOffset( reco::Candidate::LorentzVector& rawJetP4, const pat::Jet& jet, double& jetCorrEtaMax, std::vector<std::string> jecPayloadNames_ ){

   double jetCorrFactor = 1.;
   if ( fabs(rawJetP4.eta()) < jetCorrEtaMax ){
      jecOffset_->setJetEta( rawJetP4.eta()     );
      jecOffset_->setJetPt ( rawJetP4.pt()      );
      jecOffset_->setJetE  ( rawJetP4.energy()  );
      jecOffset_->setJetPhi( rawJetP4.phi()     );
      jecOffset_->setJetA  ( jet.jetArea()      );
      jecOffset_->setRho   ( *(rho_.product())  );
      jecOffset_->setNPV   ( nVtx  );
      jetCorrFactor = jecOffset_->getCorrection();
   }

   reco::Candidate::LorentzVector corrJetP4 = rawJetP4;
   corrJetP4 *= jetCorrFactor;

   return jetCorrFactor;

}

//-------------------------------------------------------------------------------------------------------------------------------------//
//
// member functions
//
void EDBRTreeMaker::addTypeICorr( edm::Event const & event ){
   TypeICorrMap_.clear();

   event.getByToken(jetToken_      , jets_    );
   event.getByToken(rhoToken_      , rho_     );
//   edm::Handle<double> rho_;
//   event.getByLabel("fixedGridRhoFastjetAll",rho_);
//   edm::Handle<reco::VertexCollection> vertices_;
//   event.getByLabel("offlineSlimmedPrimaryVertices", vertices_);
//   event.getByToken(vtxToken_, vertices_);
   edm::Handle<reco::VertexCollection> vertices_;
   event.getByToken(vtxToken_, vertices_);

//   event.getByToken(muonToken_     , muons_   );
   edm::Handle<edm::View<pat::Muon>> muons_;
//   event.getByLabel("slimmedMuons",muons_);
   event.getByToken(t1muSrc_,muons_);

   bool skipEM_                    = true;
   double skipEMfractionThreshold_ = 0.9;
   bool skipMuons_                 = true;

   std::string skipMuonSelection_string = "isGlobalMuon | isStandAloneMuon";
   StringCutObjectSelector<reco::Candidate>* skipMuonSelection_ = new StringCutObjectSelector<reco::Candidate>(skipMuonSelection_string,true);

   double jetCorrEtaMax_           = 9.9;
   double type1JetPtThreshold_     = 15.0; //10.0;

   double corrEx    = 0;
   double corrEy    = 0;
   double corrSumEt = 0;

   for (const pat::Jet &jet : *jets_) {

     double emEnergyFraction = jet.chargedEmEnergyFraction() + jet.neutralEmEnergyFraction();
     if ( skipEM_ && emEnergyFraction > skipEMfractionThreshold_ ) continue;

     reco::Candidate::LorentzVector rawJetP4 = jet.correctedP4(0);
     double corr = getJEC(rawJetP4, jet, jetCorrEtaMax_, jetCorrLabel_);

     if ( skipMuons_ ) {
       const std::vector<reco::CandidatePtr> & cands = jet.daughterPtrVector();
       for ( std::vector<reco::CandidatePtr>::const_iterator cand = cands.begin();
             cand != cands.end(); ++cand ) {
     	 const reco::PFCandidate *pfcand = dynamic_cast<const reco::PFCandidate *>(cand->get());
     	 const reco::Candidate *mu = (pfcand != 0 ? ( pfcand->muonRef().isNonnull() ? pfcand->muonRef().get() : 0) : cand->get());
         if ( mu != 0 && (*skipMuonSelection_)(*mu) ) {
           reco::Candidate::LorentzVector muonP4 = (*cand)->p4();
           rawJetP4 -= muonP4;
         }
       }
         }

     reco::Candidate::LorentzVector corrJetP4 = corr*rawJetP4;

     if ( corrJetP4.pt() > type1JetPtThreshold_ ) {
                 reco::Candidate::LorentzVector tmpP4 = jet.correctedP4(0);
                 corr = getJECOffset(tmpP4, jet, jetCorrEtaMax_, offsetCorrLabel_);
                 reco::Candidate::LorentzVector rawJetP4offsetCorr = corr*rawJetP4;

                 corrEx    -= (corrJetP4.px() - rawJetP4offsetCorr.px());
                 corrEy    -= (corrJetP4.py() - rawJetP4offsetCorr.py());
                 corrSumEt += (corrJetP4.Et() - rawJetP4offsetCorr.Et());
         }
 }
 TypeICorrMap_["corrEx"]    = corrEx;
 TypeICorrMap_["corrEy"]    = corrEy;
 TypeICorrMap_["corrSumEt"] = corrSumEt;
}

//-------------------------------------------------------------------------------------------------------------------------------------//
math::XYZTLorentzVector
EDBRTreeMaker::getNeutrinoP4(double& MetPt, double& MetPhi, TLorentzVector& lep, int lepType){
    double leppt = lep.Pt();
    double lepphi = lep.Phi();
    double lepeta = lep.Eta();
    double lepenergy = lep.Energy();
    
    double metpt = MetPt;
    double metphi = MetPhi;
    
    double  px = metpt*cos(metphi);
    double  py = metpt*sin(metphi);
    double  pz = 0;
    double  pxl= leppt*cos(lepphi);
    double  pyl= leppt*sin(lepphi);
    double  pzl= leppt*sinh(lepeta);
    double  El = lepenergy;
    double  a = pow(MW_,2) + pow(px+pxl,2) + pow(py+pyl,2) - px*px - py*py - El*El + pzl*pzl;
    double  b = 2.*pzl;
    double  A = b*b -4.*El*El;
    double  B = 2.*a*b;
    double  C = a*a-4.*(px*px+py*py)*El*El;
    
    ///////////////////////////pz for fnal
    double M_mu =  0;
    
    //if(lepType==1)M_mu=0.105658367;//mu
    //if(lepType==0)M_mu=0.00051099891;//electron
    
    int type=2; // use the small abs real root
    
    a = MW_*MW_ - M_mu*M_mu + 2.0*pxl*px + 2.0*pyl*py;
    A = 4.0*(El*El - pzl*pzl);
    B = -4.0*a*pzl;
    C = 4.0*El*El*(px*px + py*py) - a*a;
    
    
    double tmproot = B*B - 4.0*A*C;
    
    if (tmproot<0) {
        //std::cout << "Complex root detected, taking real part..." << std::endl;
        pz = - B/(2*A); // take real part of complex roots
    }
    else {
        
        double tmpsol1 = (-B + sqrt(tmproot))/(2.0*A);
        double tmpsol2 = (-B - sqrt(tmproot))/(2.0*A);
        
        //std::cout << " Neutrino Solutions: " << tmpsol1 << ", " << tmpsol2 << std::endl;
        
        if (type == 0 ) {
            // two real roots, pick the one closest to pz of muon
            if (TMath::Abs(tmpsol2-pzl) < TMath::Abs(tmpsol1-pzl)) { pz = tmpsol2; }
            else { pz = tmpsol1; }
            // if pz is > 300 pick the most central root
            if ( abs(pz) > 300. ) {
                if (TMath::Abs(tmpsol1)<TMath::Abs(tmpsol2) ) { pz = tmpsol1; }
                else { pz = tmpsol2; }
            }
        }
        if (type == 1 ) {
            // two real roots, pick the one closest to pz of muon
            if (TMath::Abs(tmpsol2-pzl) < TMath::Abs(tmpsol1-pzl)) { pz = tmpsol2; }
            else {pz = tmpsol1; }
        }
        if (type == 2 ) {
            // pick the most central root.
            if (TMath::Abs(tmpsol1)<TMath::Abs(tmpsol2) ) { pz = tmpsol1; }
            else { pz = tmpsol2; }
        }
        /*if (type == 3 ) {
         // pick the largest value of the cosine
         TVector3 p3w, p3mu;
         p3w.SetXYZ(pxl+px, pyl+py, pzl+ tmpsol1);
         p3mu.SetXYZ(pxl, pyl, pzl );
         
         double sinthcm1 = 2.*(p3mu.Perp(p3w))/MW_;
         p3w.SetXYZ(pxl+px, pyl+py, pzl+ tmpsol2);
         double sinthcm2 = 2.*(p3mu.Perp(p3w))/MW_;
         
         double costhcm1 = sqrt(1. - sinthcm1*sinthcm1);
         double costhcm2 = sqrt(1. - sinthcm2*sinthcm2);
         
         if ( costhcm1 > costhcm2 ) { pz = tmpsol1; otherSol_ = tmpsol2; }
         else { pz = tmpsol2;otherSol_ = tmpsol1; }
         
         }*///end of type3
        
    }//endl of if real root
    
    //dont correct pt neutrino
    math::XYZTLorentzVector outP4(px,py,pz,sqrt(px*px+py*py+pz*pz));
    return outP4;
    
}//end neutrinoP4



// ------------ method called for each event  ------------
void
EDBRTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   setDummyValues(); //Initalize variables with dummy values

   nevent = iEvent.eventAuxiliary().event();
   run    = iEvent.eventAuxiliary().run();
   ls     = iEvent.eventAuxiliary().luminosityBlock();

//   std::cout<< "num of run:" << run << "  lumi:" << ls << "  n event:" << nevent << std::endl;
   Handle<TriggerResults> trigRes;
   iEvent.getByToken(hltToken_, trigRes);

   int xtemp1=0;
   for (size_t i=0; i<elPaths1.size();i++) {
      xtemp1 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths1[i]));
      if(HLT_Ele1<xtemp1) HLT_Ele1=xtemp1;
   }
   int xtemp2=0;
   for (size_t i=0; i<elPaths2.size();i++) {
      xtemp2 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths2[i]));
      if(HLT_Ele2<xtemp2) HLT_Ele2=xtemp2;
   }
   int xtemp3=0;
   for (size_t i=0; i<elPaths3.size();i++) {
      xtemp3 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths3[i]));
      if(HLT_Ele3<xtemp3) HLT_Ele3=xtemp3;
   }
   int xtemp4=0;
   for (size_t i=0; i<elPaths4.size();i++) {
      xtemp4 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths4[i]));
      if(HLT_Ele4<xtemp4) HLT_Ele4=xtemp4;
   }
   int xtemp5=0;
   for (size_t i=0; i<elPaths5.size();i++) {
      xtemp5 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths5[i]));
      if(HLT_Ele5<xtemp5) HLT_Ele5=xtemp5;
   }
   int xtemp6=0;
   for (size_t i=0; i<elPaths6.size();i++) {
      xtemp6 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths6[i]));
      if(HLT_Ele6<xtemp6) HLT_Ele6=xtemp6;
   }
   int xtemp7=0;
   for (size_t i=0; i<elPaths7.size();i++) {
      xtemp7 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths7[i]));
      if(HLT_Ele7<xtemp7) HLT_Ele7=xtemp7;
   }
   int xtemp8=0;
   for (size_t i=0; i<elPaths8.size();i++) {
      xtemp8 = (int)trigRes->accept(hltConfig.triggerIndex(elPaths8[i]));
      if(HLT_Ele8<xtemp8) HLT_Ele8=xtemp8;
   }

   int mtemp1=0;
   for (size_t i=0; i<muPaths1.size();i++) {
      mtemp1 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths1[i]));
      if(HLT_Mu1<mtemp1) HLT_Mu1=mtemp1;
   }
   int mtemp2=0;
   for (size_t i=0; i<muPaths2.size();i++) {
      mtemp2 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths2[i]));
      if(HLT_Mu2<mtemp2) HLT_Mu2=mtemp2;
   }
   int mtemp3=0;
   for (size_t i=0; i<muPaths3.size();i++) {
      mtemp3 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths3[i]));
      if(HLT_Mu3<mtemp3) HLT_Mu3=mtemp3;
   }
   int mtemp4=0;
   for (size_t i=0; i<muPaths4.size();i++) {
      mtemp4 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths4[i]));
      if(HLT_Mu4<mtemp4) HLT_Mu4=mtemp4;
   }
   int mtemp5=0;
   for (size_t i=0; i<muPaths5.size();i++) {
      mtemp5 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths5[i]));
      if(HLT_Mu5<mtemp5) HLT_Mu5=mtemp5;
   }
   int mtemp6=0;
   for (size_t i=0; i<muPaths6.size();i++) {
      mtemp6 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths6[i]));
      if(HLT_Mu6<mtemp6) HLT_Mu6=mtemp6;
   }
   int mtemp7=0;
   for (size_t i=0; i<muPaths7.size();i++) {
      mtemp7 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths7[i]));
      if(HLT_Mu7<mtemp7) HLT_Mu7=mtemp7;
   }
   int mtemp8=0;
   for (size_t i=0; i<muPaths8.size();i++) {
      mtemp8 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths8[i]));
      if(HLT_Mu8<mtemp8) HLT_Mu8=mtemp8;
   }
   int mtemp9=0;
   for (size_t i=0; i<muPaths9.size();i++) {
      mtemp9 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths9[i]));
      if(HLT_Mu9<mtemp9) HLT_Mu9=mtemp9;
   }
   int mtemp10=0;
   for (size_t i=0; i<muPaths10.size();i++) {
      mtemp10 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths10[i]));
      if(HLT_Mu10<mtemp10) HLT_Mu10=mtemp10;
   }
   int mtemp11=0;
   for (size_t i=0; i<muPaths11.size();i++) {
      mtemp11 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths11[i]));
      if(HLT_Mu11<mtemp11) HLT_Mu11=mtemp11;
   }
   int mtemp12=0;
   for (size_t i=0; i<muPaths12.size();i++) {
      mtemp12 = (int)trigRes->accept(hltConfig.triggerIndex(muPaths12[i]));
      if(HLT_Mu12<mtemp12) HLT_Mu12=mtemp12;
   }




   edm::Handle<edm::View<pat::Jet> > hadronicVs;
//   iEvent.getByLabel(hadronicVSrc_.c_str(), hadronicVs);
   iEvent.getByToken(hadronicVSrc_, hadronicVs);
   
   edm::Handle<edm::View<pat::Jet> > hadronicVs_raw;
//   iEvent.getByLabel("slimmedJetsAK8", hadronicVs_raw);
   iEvent.getByToken(hadronicVSrc_raw_, hadronicVs_raw);
   edm::Handle<edm::View<reco::Candidate> > leptonicVs;
//   iEvent.getByLabel(leptonicVSrc_.c_str(), leptonicVs);
   iEvent.getByToken(leptonicVSrc_, leptonicVs);

   edm::Handle<double> rho;
//   iEvent.getByLabel("fixedGridRhoFastjetAll",rho);

   iEvent.getByToken(rhoToken_      , rho     );
   double fastJetRho = *(rho.product());
   useless = fastJetRho;

   edm::Handle<edm::View<pat::Jet> > ak4jets;
//   iEvent.getByLabel(ak4jetsSrc_.c_str(), ak4jets);
   iEvent.getByToken(ak4jetsSrc_, ak4jets);
 
   edm::Handle<edm::View<reco::Candidate> > gravitons;
//   iEvent.getByLabel(gravitonSrc_.c_str(), gravitons);
   iEvent.getByToken(gravitonSrc_, gravitons);
   edm::Handle<edm::View<reco::Candidate> > metHandle;
//   iEvent.getByLabel(metSrc_.c_str(), metHandle);
   iEvent.getByToken(metSrc_, metHandle);
  
   edm::Handle<edm::View<pat::Muon>> loosemus;
//   iEvent.getByLabel(looseMuonSrc_.c_str(), loosemus);
   iEvent.getByToken(loosemuonToken_,loosemus);

   edm::Handle<edm::View<pat::Muon>> goodmus;
//   iEvent.getByLabel(goodMuSrc_.c_str(), goodmus);
   iEvent.getByToken(goodMuSrc_, goodmus);

   edm::Handle<edm::View<pat::Electron>> looseels;
//   iEvent.getByLabel(looseElectronsSrc_.c_str(), looseels);
   iEvent.getByToken(looseelectronToken_, looseels);

   edm::Handle<edm::View<reco::GenParticle> > genParticles;//define genParticle
//   iEvent.getByLabel(InputTag("prunedGenParticles"), genParticles);
   iEvent.getByToken(genSrc_, genParticles);

   edm::Handle<edm::View<pat::Muon>> mus;
//   iEvent.getByLabel("slimmedMuons",mus);
   iEvent.getByToken(MuSrc_, mus);
   edm::Handle<edm::View<pat::Electron>> eles;
//   iEvent.getByLabel("slimmedElectrons",eles);
   iEvent.getByToken(EleSrc_, eles);
   if (RunOnMC_){
//   edm::Handle<LHEEventProduct> wgtsource;
//   iEvent.getByLabel("externalLHEProducer", wgtsource);
   //iEvent.getByLabel("source", wgtsource);

   edm::Handle<GenEventInfoProduct> genEvtInfo;
//   iEvent.getByLabel( "generator", genEvtInfo );
   iEvent.getByToken(GenToken_,genEvtInfo);

//   const std::vector<double>& evtWeights = genEvtInfo->weights();
   theWeight = genEvtInfo->weight();
   if(theWeight>0) nump = nump+1;
   if(theWeight<0) numm = numm+1;

   edm::Handle<std::vector<PileupSummaryInfo>>  PupInfo;
//   iEvent.getByLabel(edm::InputTag("slimmedAddPileupInfo"), PupInfo);
   iEvent.getByToken(PUToken_, PupInfo);
   std::vector<PileupSummaryInfo>::const_iterator PVI;
   for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
      nBX = PVI->getBunchCrossing();
         if(nBX == 0) { // "0" is the in-time crossing, negative values are the early crossings, positive are late
            npT = PVI->getTrueNumInteractions();
            npIT = PVI->getPU_NumInteractions();
         }
   }
   }
//   cout << "npT" << npT << " nBX" << nBX << endl;

//filter
    iEvent.getByToken(noiseFilterToken_, noiseFilterBits_);
    const edm::TriggerNames &names = iEvent.triggerNames(*noiseFilterBits_);
    for (unsigned int i = 0, n = noiseFilterBits_->size(); i < n; ++i) {
      if (names.triggerName(i) == HBHENoiseFilter_Selector_)
        passFilter_HBHE_ = noiseFilterBits_->accept(i); // TO BE USED
      if (names.triggerName(i) == HBHENoiseIsoFilter_Selector_)
        passFilter_HBHEIso_ = noiseFilterBits_->accept(i); // TO BE USED
      if (names.triggerName(i) == GlobalHaloNoiseFilter_Selector_)
        passFilter_GlobalHalo_ = noiseFilterBits_->accept(i); // TO BE USED
      if (names.triggerName(i) == ECALDeadCellNoiseFilter_Selector_)
        passFilter_ECALDeadCell_ = noiseFilterBits_->accept(i); // under scrutiny
      if (names.triggerName(i) == GoodVtxNoiseFilter_Selector_)
        passFilter_GoodVtx_ = noiseFilterBits_->accept(i); // TO BE USED
      if (names.triggerName(i) == EEBadScNoiseFilter_Selector_)
        passFilter_EEBadSc_ = noiseFilterBits_->accept(i); // under scrutiny  
    }

 
   numCands = gravitons->size();
 
    
 
// ************************* Gen Level Information******************//
   if(RunOnMC_)
   {//MC Info
       Int_t havegra=0;
	  for(size_t ik=0; ik<genParticles->size();ik++)
	{// loop on gen
              const reco::Candidate* ptop = &(*genParticles)[ik];
                if(ptop->pdgId()== 6) {
                   gentop_pt = ptop->pt();
                   gentop_eta = ptop->eta();
                   gentop_phi = ptop->phi();
                   gentop_mass = ptop->mass(); }
                if(ptop->pdgId()== -6) {
                   genantitop_pt = ptop->pt();
                   genantitop_eta = ptop->eta();
                   genantitop_phi = ptop->phi();
                   genantitop_mass = ptop->mass();}
        //if((abs((*genParticles)[ik].pdgId())!=24)&&(abs((*genParticles)[ik].pdgId())!=9000024)&&(abs((*genParticles)[ik].pdgId())!=9000025))
            //cout<<"(*genParticles)[ik]->pdgId() "<<(*genParticles)[ik].pdgId()<<endl;
        if( abs((*genParticles)[ik].pdgId())==9000024|| abs((*genParticles)[ik].pdgId())==6) havegra=1;
        
		if( abs((*genParticles)[ik].pdgId())==9000024 || abs((*genParticles)[ik].pdgId())==6 )
		{//if Wkk
            const reco::Candidate* pwkk0 = &(*genParticles)[ik];
             const reco::Candidate* pwkk=findLastW(pwkk0,9000024);
		   gen_gra_eta=pwkk->eta();
            gen_gra_m=pwkk->mass();
            gen_gra_pt=pwkk->pt();
            gen_gra_phi=pwkk->phi();

                for(int i=0;pwkk->daughter(i)!=NULL;i++)
                   {//loop on Wkk daughter
                       
                    if(abs(pwkk->daughter(i)->pdgId())==24)
                       {//if w
                        
                         const reco::Candidate* pw0 = pwkk->daughter(i);
                           
                           const reco::Candidate* pw= findLastW(pw0,24);                           //cout<<"check 4  "<<pw->daughter(0)->pdgId()<<endl;
                         if(pw->daughter(0)!=NULL)
                         {//loop on w daughter
                            const reco::Candidate* pl = pw->daughter(0);
                             

                            if( (abs(pl->pdgId())==11) || (abs(pl->pdgId())==13)|| (abs(pl->pdgId())==15)||(abs(pl->pdgId())==12) || (abs(pl->pdgId())==14)|| (abs(pl->pdgId())==16))
                            {//beign of lep-w
                               ptGenVlep = pw->pt();
                               etaGenVlep = pw->eta();
                               phiGenVlep = pw->phi();
                               massGenVlep = pw->mass();
                                status_1=0;
                            for(int ii=0;pw->daughter(ii)!=NULL;ii++){
                                const reco::Candidate* pl = pw->daughter(ii);
                               if(abs(pl->pdgId())==11)
                               {
                                 gen_ele_pt=pl->pt();
                                 gen_ele_eta=pl->eta();
                                 gen_ele_phi=pl->phi();
                                 gen_ele_e=pl->energy();
                                   status_1=1;
                               }
                               if(abs(pl->pdgId())==13)
                               {
                                 gen_mu_pt=pl->pt();
                                 gen_mu_eta=pl->eta();
                                 gen_mu_phi=pl->phi();
                                 gen_mu_e=pl->energy();
                                   status_1=2;
                               }
                                if(abs(pl->pdgId())==15)
                                    {
                                        status_1=3;
                                    }
                                }
                             }//end of if lep-w
        		     if(abs(pl->pdgId())<6) 
                             {
			         ptGenVhad = pw->pt();
			         etaGenVhad = pw->eta();
			         phiGenVhad = pw->phi();
			         massGenVhad = pw->mass();
                                 status_1=4;
                             }
        		     if(abs(pl->pdgId())==24) 
                             {
                                 status_1=5;
                             }
                     //if(status_1<0) cout<<"pw->daughter(0)  "<<pl->pdgId()<<endl;
			   }//end of loop on w daughter
                       }//end of if w				 
                   }//end of loop on Wkk daughter
 		}//end of if Wkk

        //if(status_1<0) cout<<"(*genParticles)[ik].pdgId()"<<(*genParticles)[ik].pdgId()<<endl;
        if( havegra==0&&abs((*genParticles)[ik].pdgId())==24 )
        {//if W
            const reco::Candidate* pw0 = &(*genParticles)[ik];
            const reco::Candidate* pw=findFirstW(pw0);
            if(pw->mother(0)->pdgId()!=9000025){
                const reco::Candidate* pw1= findLastW(pw0,24);
                if(pw1->daughter(0)!=NULL)
                {//loop on w daughter
                    const reco::Candidate* pl = pw1->daughter(0);

                if( (abs(pl->pdgId())==11) || (abs(pl->pdgId())==13)|| (abs(pl->pdgId())==15)||(abs(pl->pdgId())==12) || (abs(pl->pdgId())==14)|| (abs(pl->pdgId())==16))
                {//beign of lep-w
                    ptGenVlep = pw1->pt();
                    etaGenVlep = pw1->eta();
                    phiGenVlep = pw1->phi();
                    massGenVlep = pw1->mass();
                    status_1=0;
                    for(int ii=0;pw1->daughter(ii)!=NULL;ii++){
                        const reco::Candidate* pl = pw1->daughter(ii);
                        if(abs(pl->pdgId())==11)
                        {
                            gen_ele_pt=pl->pt();
                            gen_ele_eta=pl->eta();
                            gen_ele_phi=pl->phi();
                            gen_ele_e=pl->energy();
                            status_1=1;
                        }
                        if(abs(pl->pdgId())==13)
                        {
                            gen_mu_pt=pl->pt();
                            gen_mu_eta=pl->eta();
                            gen_mu_phi=pl->phi();
                            gen_mu_e=pl->energy();
                            status_1=2;
                        }
                        if(abs(pl->pdgId())==15)
                        {
                            status_1=3;
                        }
                    }
                }//end of if lep-w
                if(abs(pl->pdgId())<6)
                {
                    ptGenVhad = pw1->pt();
                    etaGenVhad = pw1->eta();
                    phiGenVhad = pw1->phi();
                    massGenVhad = pw1->mass();
                    status_1=4;
                }
                if(abs(pl->pdgId())==24)
                {
                    status_1=5;
                }
                }
            }
        }
        
		if( abs((*genParticles)[ik].pdgId())==9000025 )
		{//if Radion
                   gen_rad_m=(*genParticles)[ik].mass();
		   gen_rad_pt=(*genParticles)[ik].pt();
            gen_rad_phi=(*genParticles)[ik].phi();

	           gen_rad_eta=(*genParticles)[ik].eta();
                   for(int i=0;(*genParticles)[ik].daughter(i)!=NULL;i++)
                   {//loop on Radion daughter

                      if(((*genParticles)[ik].daughter(i)->pdgId())==24)
                       {//if w-
                         const reco::Candidate* pw0 = (*genParticles)[ik].daughter(i);
                           const reco::Candidate* pw= findLastW(pw0,24);
                         if(pw->daughter(0)!=NULL)
                         {//loop on w daughter
                            const reco::Candidate* pl = pw->daughter(0);
                            if( (abs(pl->pdgId())==11) || (abs(pl->pdgId())==13)||(abs(pl->pdgId())==15)||(abs(pl->pdgId())==12) || (abs(pl->pdgId())==14)|| (abs(pl->pdgId())==16))
                            {//beign of lep-w
                               ptGenV_2 = pw->pt();
                               etaGenV_2 = pw->eta();
                               phiGenV_2 = pw->phi();
                               massGenV_2 = pw->mass();
                               status_2=0;
                            for(int ii=0;pw->daughter(ii)!=NULL;ii++){
                               const reco::Candidate* pl = pw->daughter(ii);
                               if(abs(pl->pdgId())==11)
                               {
                                 gen_ele_pt_2=pl->pt();
                                 gen_ele_eta_2=pl->eta();
                                 gen_ele_phi_2=pl->phi();
                                 gen_ele_e_2=pl->energy();
                                 status_2=1;  
                               }
                               if(abs(pl->pdgId())==13)
                               {
                                 gen_mu_pt_2=pl->pt();
                                 gen_mu_eta_2=pl->eta();
                                 gen_mu_phi_2=pl->phi();
                                 gen_mu_e_2=pl->energy();
                                 status_2=2;  
                               }
				if(abs(pl->pdgId())==15)
                               {
                                 status_2=3;
                               }
                            }
                             }//end of if lep-w
        		     if(abs(pl->pdgId())<6) 
                             {
                                 ptGenVhad_2 = pw->pt();
                                 etaGenVhad_2 = pw->eta();
                                 phiGenVhad_2 = pw->phi();
                                 massGenVhad_2 = pw->mass();

                                 status_2=4;  
                             }
        		     if(abs(pl->pdgId())==24) 
                             {
                                 status_2=5;  
                             }
			   }//end of loop on w daughter
                       }//end of if w-	
                      if(((*genParticles)[ik].daughter(i)->pdgId())==-24)
                       {//if w+
                         const reco::Candidate* pw0 = (*genParticles)[ik].daughter(i);
                           const reco::Candidate* pw= findLastW(pw0,24);
			//cout<<((*genParticles)[ik].daughter(i)->pdgId())<<endl;
                         if(pw->daughter(0)!=NULL)
                         {//loop on w daughter
                            const reco::Candidate* pl = pw->daughter(0);
			//cout<<(pl->pdgId())<<endl;
                            if( (abs(pl->pdgId())==11) || (abs(pl->pdgId())==13)|| (abs(pl->pdgId())==15)||(abs(pl->pdgId())==12) || (abs(pl->pdgId())==14)|| (abs(pl->pdgId())==16))
                            {//beign of lep-w
                               ptGenV_3 = pw->pt();
                               etaGenV_3 = pw->eta();
                               phiGenV_3 = pw->phi();
                               massGenV_3 = pw->mass();
                               status_3=0;
                            for(int ii=0;pw->daughter(ii)!=NULL;ii++){
                              const reco::Candidate* pl = pw->daughter(ii);
                               if(abs(pl->pdgId())==11)
                               {
                                 gen_ele_pt_3=pl->pt();
                                 gen_ele_eta_3=pl->eta();
                                 gen_ele_phi_3=pl->phi();
                                 gen_ele_e_3=pl->energy();
                                 status_3=1;  
                               }
                               if(abs(pl->pdgId())==13)
                               {
                                 gen_mu_pt_3=pl->pt();
                                 gen_mu_eta_3=pl->eta();
                                 gen_mu_phi_3=pl->phi();
                                 gen_mu_e_3=pl->energy();
                                 status_3=2;  
                               }
                               if(abs(pl->pdgId())==15)
                               {
                                 status_3=3;  
                               }
                            }
                             }//end of if lep-w
        		     if(abs(pl->pdgId())<6) 
                             {
                                 ptGenVhad_3 = pw->pt();
                                 etaGenVhad_3 = pw->eta();
                                 phiGenVhad_3 = pw->phi();
                                 massGenVhad_3 = pw->mass();

                                 status_3=4;  
                             }
        		     if(abs(pl->pdgId())==24) 
                             {
                                 status_3=5;  
                             }
			   }//end of loop on w daughter
                       }//end of if w+	
			 
                   }//end of loop on Radion daughter
 		}//end of if Radion

        }//end of loop on gen

        if(gen_mu_pt>0. && mus->size()>0 ) 
        {
            double drmumatch=10000.;  size_t mk=0;
            for(size_t ik=0; ik<mus->size();ik++)   
            { 
              double drtemp=deltaR(gen_mu_eta,gen_mu_phi,(*mus)[ik].eta(),(*mus)[ik].phi());    
              if (drtemp<drmumatch) {drmumatch=drtemp; mk=ik;}
            } 
            genmatch_mu_pt=(*mus)[mk].pt();
            genmatch_mu_eta=(*mus)[mk].eta();
            genmatch_mu_phi=(*mus)[mk].phi();
            genmatch_mu_e=(*mus)[mk].energy();
            genmatch_mu_dr=drmumatch;
         }
        if(gen_ele_pt>0. && eles->size()>0) 
        {
            double drelematch=10000.;  size_t mk=0;
            for(size_t ik=0; ik<eles->size();ik++)    
            {
              double drtemp=deltaR(gen_ele_eta,gen_ele_phi,(*eles)[ik].eta(),(*eles)[ik].phi());
              if (drtemp<drelematch) {drelematch=drtemp; mk=ik;}
            }
            genmatch_ele_pt=(*eles)[mk].pt();
            genmatch_ele_eta=(*eles)[mk].eta();
            genmatch_ele_phi=(*eles)[mk].phi();
            genmatch_ele_e=(*eles)[mk].energy();
            genmatch_ele_dr=drelematch;
         }
   }//end of MC Info
// *************************End of Gen Level Information******************//


//   if(numCands != 0 ) {
//      const reco::Candidate& graviton  = gravitons->at(0);
    if((hadronicVs->size()!= 0 )  && (leptonicVs->size()!= 0) ){

       const reco::Candidate& leptonicV = leptonicVs->at(0);
       const reco::Candidate& hadronicV = hadronicVs->at(0);
       //       const reco::Candidate& leptonicV = (*graviton.daughter("leptonicV"));
       const reco::Candidate& metCand = metHandle->at(0);
       const reco::Candidate& lepton = (*leptonicV.daughter(0));
       nLooseMu = loosemus->size();
       nLooseEle = looseels->size();

       edm::Handle<reco::VertexCollection> vertices;
       iEvent.getByToken(vtxToken_, vertices);
//       edm::Handle<reco::VertexCollection> vertices;
//       iEvent.getByLabel("offlineSlimmedPrimaryVertices", vertices);
//       iEvent.getByToken(vtxToken_, vertices);
       if (vertices->empty()) return; // skip the event if no PV found
       nVtx = vertices->size();
       reco::VertexCollection::const_iterator firstGoodVertex = vertices->end();
       for (reco::VertexCollection::const_iterator vtx = vertices->begin(); vtx != vertices->end(); ++vtx) {
               // Replace isFake() for miniAOD because it requires tracks and miniAOD vertices don't have tracks:
               // Vertex.h: bool isFake() const {return (chi2_==0 && ndof_==0 && tracks_.empty());}
               if (  /// !vtx->isFake() &&
                     !(vtx->chi2()==0 && vtx->ndof()==0) 
	             &&  vtx->ndof()>=4. && vtx->position().Rho()<=2.0
	             && fabs(vtx->position().Z())<=24.0) {
                     firstGoodVertex = vtx;
                     break;
                    }           
       }
       if ( firstGoodVertex==vertices->end() ) return; // skip event if there are no good PVs



          // ***************************************************************** //
           // ************************* MET ********************** //
              iEvent.getByToken(metInputToken_ , METs_ );
                addTypeICorr(iEvent);
                for (const pat::MET &met : *METs_) {
                        //const float rawPt  = met.shiftedPt(pat::MET::NoShift, pat::MET::Raw);
                        //const float rawPhi = met.shiftedPhi(pat::MET::NoShift, pat::MET::Raw);
                        //const float rawSumEt = met.shiftedSumEt(pat::MET::NoShift, pat::MET::Raw);
			const float rawPt	 = met.uncorPt();//met.shiftedPt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
		        const float rawPhi   = met.uncorPhi();//met.shiftedPhi(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
		        const float rawSumEt = met.uncorSumEt();
		        //const float rawPt	 = met.shiftedPt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
		        //const float rawPhi   = met.shiftedPhi(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
		        //const float rawSumEt = met.shiftedSumEt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
                        TVector2 rawMET_;
                        rawMET_.SetMagPhi (rawPt, rawPhi );
                        Double_t rawPx = rawMET_.Px();
                        Double_t rawPy = rawMET_.Py();
                        Double_t rawEt = std::hypot(rawPx,rawPy);
            		METraw_et = rawEt;
        	   	METraw_phi = rawPhi;
        	    	METraw_sumEt = rawSumEt;
                        double pxcorr = rawPx+TypeICorrMap_["corrEx"];
                        double pycorr = rawPy+TypeICorrMap_["corrEy"];
                        double et     = std::hypot(pxcorr,pycorr);
                        double sumEtcorr = rawSumEt+TypeICorrMap_["corrSumEt"];
                        TLorentzVector corrmet; corrmet.SetPxPyPzE(pxcorr,pycorr,0.,et);
            		useless = sumEtcorr;
            		useless = rawEt;
            		MET_et = et;
            		MET_phi = corrmet.Phi();
            		MET_sumEt = sumEtcorr;
            		MET_corrPx = TypeICorrMap_["corrEx"];
            		MET_corrPy = TypeICorrMap_["corrEy"]; 
                }
           // ***************************************************************** //  
 
       /// For the time being, set these to 1
       triggerWeight=1.0;
       pileupWeight=1.0;

       double targetEvents = targetLumiInvPb_*crossSectionPb_;
       lumiWeight = targetEvents/originalNEvents_;

       ptlep1       = leptonicV.daughter(0)->pt();
       ptlep2       = leptonicV.daughter(1)->pt();
       etalep1      = leptonicV.daughter(0)->eta();
       etalep2      = leptonicV.daughter(1)->eta();
       philep1      = leptonicV.daughter(0)->phi();
       philep2      = leptonicV.daughter(1)->phi();
       lep          = std::max(abs(leptonicV.daughter(0)->pdgId()), abs(leptonicV.daughter(1)->pdgId()));
       double energylep1     = leptonicV.daughter(0)->energy();
       met          = metCand.pt();
       metPhi       = metCand.phi();
       //candMass     = graviton.mass();
       ptVlep       = leptonicV.pt();
       yVlep        = leptonicV.eta();
       phiVlep      = leptonicV.phi();
       massVlep     = leptonicV.mass();
       mtVlep       = leptonicV.mt();
       TLorentzVector g_graviton, g_vhad, g_vlep;
       g_vlep.SetPtEtaPhiM(leptonicV.pt(),leptonicV.eta(),leptonicV.phi(),leptonicV.mass());
       g_vhad.SetPtEtaPhiM(hadronicV.pt(),hadronicV.eta(),hadronicV.phi(),hadronicV.mass());
       g_graviton = g_vlep + g_vhad ;
       candMass = g_graviton.Mag();        

////////////////////////lep ID  ////////////////////////////////////
        if( leptonicV.daughter(0)->isMuon()||leptonicV.daughter(1)->isMuon()){

                       const pat::Muon *mu1 = abs(leptonicV.daughter(0)->pdgId())==13 ?
                                                  (pat::Muon*)leptonicV.daughter(0):
                                                  (pat::Muon*)leptonicV.daughter(1);
		isHighPt = mu1->isHighPtMuon(vertices->at(0));
		trackIso = mu1->trackIso();
                muchaiso=mu1->pfIsolationR04().sumChargedHadronPt;
                muneuiso=mu1->pfIsolationR04().sumNeutralHadronEt;
                muphoiso=mu1->pfIsolationR04().sumPhotonEt;
                muPU=mu1->pfIsolationR04().sumPUPt;
                muisolation = (muchaiso+ std::max(0.0,muneuiso+muphoiso-0.5*muPU))/mu1->pt();

}
	if( leptonicV.daughter(0)->isElectron()||leptonicV.daughter(1)->isElectron() ) {
                       const pat::Electron *el1 = leptonicV.daughter(0)->isElectron() ?
                                                  (pat::Electron*)leptonicV.daughter(0):
                                                  (pat::Electron*)leptonicV.daughter(1);
		double etaSC1         = el1->superCluster()->eta();
		double d01            = (-1)*el1->gsfTrack()->dxy(firstGoodVertex->position());  
                isHEEP = false;
                et = el1->energy()!=0. ? el1->et()/el1->energy()*el1->caloEnergy() : 0.;
                if( et > 35. ) {
                     if( fabs(etaSC1) < 1.4442 ){
                        iso = el1->dr03EcalRecHitSumEt() + el1->dr03HcalDepth1TowerSumEt();
                        isoCut = 2 + 0.03*et + 0.28*fastJetRho;
                        if( el1->ecalDriven() == 1 && dEtaInSeed( el1 ) < 0.004 && el1->deltaPhiSuperClusterTrackAtVtx() < 0.06 &&
                         el1->hadronicOverEm() < (1./el1->superCluster()->energy()+0.05) &&
                         (el1->full5x5_e2x5Max()/el1->full5x5_e5x5() > 0.94 || el1->full5x5_e1x5()/el1->full5x5_e5x5() > 0.83) &&
                         el1->dr03TkSumPt() < 5. && el1->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&//numberOfHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
                         iso < isoCut && fabs(d01) < 0.02 ) isHEEP = true;
                     }
                     if( fabs(etaSC1) > 1.566 && fabs(etaSC1) < 2.5 ){
                        iso = el1->dr03EcalRecHitSumEt() + el1->dr03HcalDepth1TowerSumEt();
                        if( et <= 50 )
                                isoCut = 2.5 + 0.28*fastJetRho;
                        else
                                isoCut = 2.5+0.03*(et-50.) + 0.28*fastJetRho;
                        if( el1->ecalDriven() == 1 && dEtaInSeed( el1 ) < 0.006 && el1->deltaPhiSuperClusterTrackAtVtx() < 0.06 &&
                         el1->hadronicOverEm() < (5./el1->superCluster()->energy()+0.05) && el1->full5x5_sigmaIetaIeta() < 0.03 &&
                         el1->dr03TkSumPt() < 5. && el1->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&//numberOfHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
                         iso < isoCut && fabs(d01) < 0.05 ) isHEEP = true;
                     }
	}
	}

/////////////////////////Leptonic Part///////////////////////////////
                TLorentzVector  glepton, gleptonicV;
	        glepton.SetPtEtaPhiE(ptlep1, etalep1, philep1, energylep1);
	        math::XYZTLorentzVector neutrinoP4 = getNeutrinoP4(MET_et, MET_phi, glepton, 1);
	        reco::CandidateBaseRef METBaseRef = metHandle->refAt(0);
	        reco::ShallowCloneCandidate neutrino(METBaseRef, 0 , neutrinoP4);
	        reco::CompositeCandidate WLeptonic;
                WLeptonic.addDaughter(lepton);
                WLeptonic.addDaughter(neutrino); 
                AddFourMomenta addP4;
                addP4.set(WLeptonic);
                gleptonicV.SetPtEtaPhiM(WLeptonic.pt(),WLeptonic.eta(),WLeptonic.phi(),WLeptonic.mass());
                ptVlepJEC       = WLeptonic.pt();
                yVlepJEC        = WLeptonic.eta();
                phiVlepJEC      = WLeptonic.phi();
                massVlepJEC     = WLeptonic.mass();
                mtVlepJEC       =   sqrt(2*ptlep1*MET_et*(1.0-cos(philep1-MET_phi))); //WLeptonic.mt();
                delPhilepmet = deltaPhi(philep1, MET_phi);
        //cout<<"mtVlepJEC"<<mtVlepJEC<<endl;
////////////////////////JEC for AK8/////////////////////////////////

	reco::Candidate::LorentzVector uncorrPrunedJet;

	        bool doPuppi  = iEvent.getByToken(puppijetInputToken_, puppijets_ );


        if( doPuppi ){//1

         for(size_t ij=0; ij<puppijets_->size();ij++){
           corr_AK8puppi[ij] = 1;
           corr_AK8puppiSD[ij] = 1;
           const pat::Jet& hadronicVa = puppijets_->at(ij);
           reco::Candidate::LorentzVector uncorrJet;
           if(not isJEC_) doCorrOnTheFly_ = false;
           if( doCorrOnTheFly_ ){
              uncorrJet = hadronicVa.correctedP4(0);
              jecAK8puppi_->setJetEta( uncorrJet.eta()          );
              jecAK8puppi_->setJetPt ( uncorrJet.pt()           );
              jecAK8puppi_->setJetE  ( uncorrJet.energy()       );
              jecAK8puppi_->setRho   (fastJetRho);
              jecAK8puppi_->setNPV   (nVtx);
              jecAK8puppi_->setJetA  (hadronicVa.jetArea());
              corr_AK8puppi[ij] = jecAK8puppi_->getCorrection();
              jecAK8puppiGroomed_->setJetEta( uncorrJet.eta()          );
              jecAK8puppiGroomed_->setJetPt ( uncorrJet.pt()           );
              jecAK8puppiGroomed_->setJetE  ( uncorrJet.energy()       );
              jecAK8puppiGroomed_->setRho   (fastJetRho);
              jecAK8puppiGroomed_->setNPV   (nVtx);
              jecAK8puppiGroomed_->setJetA  (hadronicVa.jetArea());
              corr_AK8puppiSD[ij] = jecAK8puppiGroomed_->getCorrection();
           }
           else{uncorrJet = hadronicVa.p4();}

           if(ij<4){
              jetAK8puppi_pt1[ij] = corr_AK8puppi[ij]*uncorrJet.pt();
              jetAK8puppi_mass1[ij] = corr_AK8puppi[ij]*uncorrJet.mass();
              jetAK8puppi_eta1[ij] = uncorrJet.eta();
              jetAK8puppi_jec1[ij] = corr_AK8puppi[ij];
              jetAK8puppiSD_jec1[ij] = corr_AK8puppiSD[ij];
           }
         }

         int usenumber3 = -1; double pt_larger=0;
         int numvhad = puppijets_->size();
         for( int inum = 0; inum< numvhad; inum++){
           const pat::Jet& Vpuppi = puppijets_->at(inum);
           if(looseJetID(Vpuppi)<1) continue;     
           if(jetAK8puppi_pt1[inum] > pt_larger && fabs(jetAK8puppi_eta1[inum])<2.4 && inum<4) {pt_larger = jetAK8puppi_pt1[inum]; usenumber3 = inum; continue;}
        }
            //cout<<"usenumber3"<<usenumber3<<endl;
       if (usenumber3>-1) {//2
        const pat::Jet& hadronicVpuppi = puppijets_->at(usenumber3);
                jetAK8puppi_ptJEC       = jetAK8puppi_pt1[usenumber3]; // unpruned corrected jet pt
                jetAK8puppi_eta     = jetAK8puppi_eta1[usenumber3]; // unpruned (w/o jec) jet eta
                jetAK8puppi_phi      = hadronicVpuppi.phi(); // unpruned (w/o jec) jet phi
                jetAK8puppi_tau1         = hadronicVpuppi.userFloat("NjettinessAK8:tau1");
                jetAK8puppi_tau2         = hadronicVpuppi.userFloat("NjettinessAK8:tau2");
                jetAK8puppi_tau3         = hadronicVpuppi.userFloat("NjettinessAK8:tau3");
                jetAK8puppi_tau21        = jetAK8puppi_tau2/jetAK8puppi_tau1;
           jetAK8puppi_tau4         = hadronicVpuppi.userFloat("NjettinessAK8:tau4");
           jetAK8puppi_tau42        = jetAK8puppi_tau4/jetAK8puppi_tau2;
                jetAK8puppi_sd       =  hadronicVpuppi.userFloat("ak8PFJetsCHSSoftDropMass"); // uncorrected pruned mass
                jetAK8puppi_sdJEC  =corr_AK8puppiSD[usenumber3]*jetAK8puppi_sd;
                //sdrop        = hadronicVpuppi.userFloat("ak8PFJetsPuppiSoftDropMass");
                Double_t gencorrect=1.0;
                Double_t recocorrect_0eta1p3=1.0;
                Double_t recocorrect_1p3eta2p5=1.0;
                gencorrect=1.006-1.062*pow(jetAK8puppi_ptJEC*0.08,-1.2);
                recocorrect_0eta1p3=1.093-1.501e-04*jetAK8puppi_ptJEC+3.449e-07*pow(jetAK8puppi_ptJEC,2)-2.681e-10*pow(jetAK8puppi_ptJEC,3)+8.674e-14*pow(jetAK8puppi_ptJEC,4)-1.001e-17*pow(jetAK8puppi_ptJEC,5);
                recocorrect_1p3eta2p5=1.272-5.72e-04*jetAK8puppi_ptJEC+8.37e-07*pow(jetAK8puppi_ptJEC,2)-5.204e-10*pow(jetAK8puppi_ptJEC,3)+1.454e-13*pow(jetAK8puppi_ptJEC,4)-1.504e-17*pow(jetAK8puppi_ptJEC,5);
                if (fabs(jetAK8puppi_eta)<=1.3){jetAK8puppi_sdcorr=jetAK8puppi_sd*gencorrect*recocorrect_0eta1p3;}
                else if (fabs(jetAK8puppi_eta)<2.5 && fabs(jetAK8puppi_eta)>1.3){jetAK8puppi_sdcorr=jetAK8puppi_sd*gencorrect*recocorrect_1p3eta2p5;}
           
           int usenumber2 = -1; double pt_larger2=0;
           int numvhad = puppijets_->size();
           for( int inum = 0; inum< numvhad; inum++){
               const pat::Jet& Vpuppi = puppijets_->at(inum);
               if(looseJetID(Vpuppi)<1) continue;
               if(jetAK8puppi_pt1[inum] > pt_larger2 && fabs(jetAK8puppi_eta1[inum])<2.4 && inum != usenumber3 && inum<4) {pt_larger2 = jetAK8puppi_pt1[inum]; usenumber2 = inum; continue;}
           }
           
           
           //cout<<"usenumber2"<<usenumber2<<endl;
           if(usenumber2>-1)  {
               const pat::Jet& hadronicVpuppi_2 = puppijets_->at(usenumber2);
               jetAK8puppi_ptJEC_2       = jetAK8puppi_pt1[usenumber2]; // unpruned corrected jet pt
               jetAK8puppi_eta_2     = jetAK8puppi_eta1[usenumber2]; // unpruned (w/o jec) jet eta
               jetAK8puppi_phi_2      = hadronicVpuppi_2.phi(); // unpruned (w/o jec) jet phi
               jetAK8puppi_tau1_2         = hadronicVpuppi_2.userFloat("NjettinessAK8:tau1");
               jetAK8puppi_tau2_2         = hadronicVpuppi_2.userFloat("NjettinessAK8:tau2");
               jetAK8puppi_tau3_2         = hadronicVpuppi_2.userFloat("NjettinessAK8:tau3");
               jetAK8puppi_tau21_2        = jetAK8puppi_tau2_2/jetAK8puppi_tau1_2;
               jetAK8puppi_tau4_2         = hadronicVpuppi_2.userFloat("NjettinessAK8:tau4");
               jetAK8puppi_tau42_2        = jetAK8puppi_tau4_2/jetAK8puppi_tau2_2;
               
               jetAK8puppi_sd_2       =  hadronicVpuppi_2.userFloat("ak8PFJetsCHSSoftDropMass"); // uncorrected pruned mass
               jetAK8puppi_sdJEC_2  =corr_AK8puppiSD[usenumber2]*jetAK8puppi_sd_2;
               Double_t gencorrect=1.0;
               Double_t recocorrect_0eta1p3=1.0;
               Double_t recocorrect_1p3eta2p5=1.0;
               gencorrect=1.006-1.062*pow(jetAK8puppi_ptJEC_2*0.08,-1.2);
               recocorrect_0eta1p3=1.093-1.501e-04*jetAK8puppi_ptJEC_2+3.449e-07*pow(jetAK8puppi_ptJEC_2,2)-2.681e-10*pow(jetAK8puppi_ptJEC_2,3)+8.674e-14*pow(jetAK8puppi_ptJEC_2,4)-1.001e-17*pow(jetAK8puppi_ptJEC_2,5);
               recocorrect_1p3eta2p5=1.272-5.72e-04*jetAK8puppi_ptJEC_2+8.37e-07*pow(jetAK8puppi_ptJEC_2,2)-5.204e-10*pow(jetAK8puppi_ptJEC_2,3)+1.454e-13*pow(jetAK8puppi_ptJEC_2,4)-1.504e-17*pow(jetAK8puppi_ptJEC_2,5);
               if (fabs(jetAK8puppi_eta_2)<=1.3){jetAK8puppi_sdcorr_2=jetAK8puppi_sd_2*gencorrect*recocorrect_0eta1p3;}
               else if (fabs(jetAK8puppi_eta_2)<2.5 && fabs(jetAK8puppi_eta_2)>1.3){jetAK8puppi_sdcorr_2=jetAK8puppi_sd_2*gencorrect*recocorrect_1p3eta2p5;}
               IDLoose_2 = looseJetID(hadronicVpuppi_2);
               IDTight_2 = tightJetID(hadronicVpuppi_2);
               //sdrop_2        = hadronicVpuppi_2.userFloat("ak8PFJetsPuppiSoftDropMass");
           }
           


	int nak4 = 0,nak4_2 = 0;
        double tj1=-10.0, tj2=-10.0,tj1_2=-10.0, tj2_2=-10.0;
 
        for (size_t ik=0; ik<ak4jets->size();ik++)
         {//3
            double corr = 1;
            reco::Candidate::LorentzVector uncorrJet;
             if( doCorrOnTheFly_ ){
	    uncorrJet = (*ak4jets)[ik].correctedP4(0);
            jecAK4_->setJetEta( uncorrJet.eta() );
            jecAK4_->setJetPt ( uncorrJet.pt() );
            jecAK4_->setJetE ( uncorrJet.energy() );
            jecAK4_->setRho ( fastJetRho );
            jecAK4_->setNPV ( vertices->size() );
            jecAK4_->setJetA ( (*ak4jets)[ik].jetArea() );
            corr = jecAK4_->getCorrection();
	    } else {uncorrJet = (*ak4jets)[ik].p4();}
	    
            //double dtemp=deltaR(ak4jet_eta[nak4],ak4jet_phi[nak4],hadronicVpuppi.eta(),hadronicVpuppi.phi());
            double dtemp=deltaR(ak4jet_eta[nak4],ak4jet_phi[nak4],jetAK8puppi_eta,jetAK8puppi_phi);
             double dtemp_2=deltaR(ak4jet_eta_2[nak4_2],ak4jet_phi_2[nak4_2],jetAK8puppi_eta_2,jetAK8puppi_phi_2);
//cout<<hadronicVpuppi.eta()<<"   "<<hadronicVpuppi.phi()<<endl;
//cout<<nak4<<"    "<<ak4jet_eta[nak4]<<"   "<<ak4jet_phi[nak4]<<endl;

	    if( (corr*uncorrJet.pt())>20 && (fabs((*ak4jets)[ik].eta()) < 5.0) && looseJetID((*ak4jets)[ik])>0 && dtemp>0.8 && nak4<8){
  //              cout<<dtemp<<endl;
		ak4jet_hf[nak4]=(*ak4jets)[ik].hadronFlavour();
                ak4jet_pf[nak4]=(*ak4jets)[ik].partonFlavour();
                ak4jet_pt[nak4] =  corr*uncorrJet.pt();
                ak4jet_pt_uncorr[nak4] =  uncorrJet.pt();  
                ak4jet_eta[nak4] = (*ak4jets)[ik].eta();
                ak4jet_phi[nak4] = (*ak4jets)[ik].phi();
                ak4jet_e[nak4] =   corr*uncorrJet.energy();
                ak4jet_csv[nak4] = (*ak4jets)[ik].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
                ak4jet_icsv[nak4] = (*ak4jets)[ik].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");   
		deltaRAK4AK8[nak4] = dtemp;
		ak4jet_IDLoose[nak4] = looseJetID((*ak4jets)[ik]);
		ak4jet_IDTight[nak4] = tightJetID((*ak4jets)[ik]);
                if(ak4jet_pt[nak4]>tj1 ) {
                          if(tj1>tj2) {tj2=tj1; nj2=nj1;}
                          tj1=ak4jet_pt[nak4]; nj1=nak4; 
                } 
                else if(ak4jet_pt[nak4]>tj2){
                          tj2=ak4jet_pt[nak4]; nj2=nak4;}
               
		nak4 = nak4 + 1;
            }
            
             if( (corr*uncorrJet.pt())>20 && (fabs((*ak4jets)[ik].eta()) < 5.0) && looseJetID((*ak4jets)[ik])>0 && dtemp_2>0.8 && nak4_2<8){
                 ak4jet_hf_2[nak4_2]=(*ak4jets)[ik].hadronFlavour();
                 ak4jet_pf_2[nak4_2]=(*ak4jets)[ik].partonFlavour();
                 ak4jet_pt_2[nak4_2] =  corr*uncorrJet.pt();
                 ak4jet_pt_uncorr_2[nak4_2] =  uncorrJet.pt();
                 ak4jet_eta_2[nak4_2] = (*ak4jets)[ik].eta();
                 ak4jet_phi_2[nak4_2] = (*ak4jets)[ik].phi();
                 ak4jet_e_2[nak4_2] =   corr*uncorrJet.energy();
                 ak4jet_csv_2[nak4_2] = (*ak4jets)[ik].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
                 ak4jet_icsv_2[nak4_2] = (*ak4jets)[ik].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
                 deltaRAK4AK8_2[nak4_2] = dtemp_2;
                 ak4jet_IDLoose_2[nak4_2] = looseJetID((*ak4jets)[ik]);
                 ak4jet_IDTight_2[nak4_2] = tightJetID((*ak4jets)[ik]);
                 if(ak4jet_pt_2[nak4_2]>tj1_2 ) {
                     if(tj1_2>tj2_2) {tj2_2=tj1_2; nj2_2=nj1_2;}
                     tj1_2=ak4jet_pt_2[nak4_2]; nj1_2=nak4_2;
                 }
                 else if(ak4jet_pt_2[nak4_2]>tj2_2){
                     tj2_2=ak4jet_pt_2[nak4_2]; nj2_2=nak4_2;}
                 
                 nak4_2 = nak4_2 + 1;
             }

          }//3
         
        


               if(nj1>-1 && nj2>-1 && ak4jet_pt[nj1]>30. && ak4jet_pt[nj2]>30.) {
                  vbfeta=fabs(ak4jet_eta[nj1]-ak4jet_eta[nj2]);
                  TLorentzVector vbfj1, vbfj2;
	          vbfj1.SetPtEtaPhiE(ak4jet_pt[nj1], ak4jet_eta[nj1], ak4jet_phi[nj1], ak4jet_e[nj1]);
	          vbfj2.SetPtEtaPhiE(ak4jet_pt[nj2], ak4jet_eta[nj2], ak4jet_phi[nj2], ak4jet_e[nj2]);
                  vbfmjj=(vbfj1+vbfj2).Mag();
               }
           if(nj1_2>-1 && nj2_2>-1 && ak4jet_pt[nj1_2]>30. && ak4jet_pt[nj2_2]>30.) {
               vbfeta_2=fabs(ak4jet_eta_2[nj1_2]-ak4jet_eta_2[nj2_2]);
               TLorentzVector vbfj1_2, vbfj2_2;
               vbfj1_2.SetPtEtaPhiE(ak4jet_pt_2[nj1_2], ak4jet_eta_2[nj1_2], ak4jet_phi_2[nj1_2], ak4jet_e_2[nj1_2]);
               vbfj2_2.SetPtEtaPhiE(ak4jet_pt_2[nj2_2], ak4jet_eta_2[nj2_2], ak4jet_phi_2[nj2_2], ak4jet_e_2[nj2_2]);
               vbfmjj_2=(vbfj1_2+vbfj2_2).Mag();
           }

              if(vbfeta>4.0 && vbfmjj>400&&vbfeta_2>4.0 && vbfmjj_2>400) {vbftag=1;}


                deltaRlepjet = deltaR(etalep1,philep1,jetAK8puppi_eta,jetAK8puppi_phi);
                deltaRlepjet_2 = deltaR(etalep1,philep1,jetAK8puppi_eta_2,jetAK8puppi_phi_2);
                TLorentzVector ghadronicVpuppi, gravitonpuppiJEC,ghadronicVpuppi_2, gravitonpuppiJEC_2;
                ghadronicVpuppi.SetPtEtaPhiM(jetAK8puppi_ptJEC, jetAK8puppi_eta, jetAK8puppi_phi, jetAK8puppi_sdJEC);
                ghadronicVpuppi_2.SetPtEtaPhiM(jetAK8puppi_ptJEC_2, jetAK8puppi_eta_2, jetAK8puppi_phi_2, jetAK8puppi_sdJEC_2);
	            gravitonpuppiJEC = gleptonicV + ghadronicVpuppi+ ghadronicVpuppi_2;
                candMasspuppiJEC     = gravitonpuppiJEC.Mag();
                delPhijetmet = deltaPhi(jetAK8puppi_phi, MET_phi);
                delPhijetlep = deltaPhi(jetAK8puppi_phi, phiVlepJEC);
                IDLoose = looseJetID(hadronicVpuppi);
                IDTight = tightJetID(hadronicVpuppi);
                delPhijetmet_2 = deltaPhi(jetAK8puppi_phi_2, MET_phi);
                delPhijetlep_2 = deltaPhi(jetAK8puppi_phi_2, phiVlepJEC);
           
           TLorentzVector lvw[3];
           lvw[0] = gleptonicV;
           lvw[1] = ghadronicVpuppi;
           lvw[2] = ghadronicVpuppi_2;
           Double_t Wpt[3];
           Wpt[0]=ptVlepJEC;
           Wpt[1]=jetAK8puppi_ptJEC;
           Wpt[2]=jetAK8puppi_ptJEC_2;
           Int_t *indexx=new Int_t[3];
           TMath::Sort(3,Wpt,indexx,1);
           //cout<<Wpt[indexx[0]]<<"   "<<Wpt[indexx[1]]<<"   "<<Wpt[indexx[2]]<<"   "<<endl;
           massww[0] = (lvw[indexx[0]]+lvw[indexx[1]]).Mag();
           massww[1] = (lvw[indexx[0]]+lvw[indexx[2]]).Mag();
           massww[2] = (lvw[indexx[1]]+lvw[indexx[2]]).Mag();

           masslvj1 = (lvw[0]+lvw[1]).Mag();
           masslvj2 = (lvw[0]+lvw[2]).Mag();
           massj1j2 = (lvw[1]+lvw[2]).Mag();


           
         }//2
        }//1
       outTree_->Fill();

//cout<< "test end2" <<endl;
   }
   else {
       outTree_->Fill();
   }
//cout<< "test end3" <<endl;
}
//-------------------------------------------------------------------------------------------------------------------------------------//


void EDBRTreeMaker::setDummyValues() {
     npT=-1.;
     npIT=-1.;
     nBX=-1;
     nLooseEle      =-99;
     nLooseMu       =-99;

     nVtx           = -99;
     triggerWeight  = -99;
     pileupWeight   = -99;
     lumiWeight     = -99;
     candMass       = -99;
     ptVlep         = -99;
     ptVhad         = -99;

     jetAK8puppi_ptJEC         = -99;
     jetAK8puppi_eta         = -99;
     jetAK8puppi_phi         = -99;
     jetAK8puppi_tau1         = -99;
     jetAK8puppi_tau2         = -99;
     jetAK8puppi_tau3         = -99;
     jetAK8puppi_tau21         = -99;
    jetAK8puppi_tau4         = -99;
    jetAK8puppi_tau42         = -99;

     jetAK8puppi_sd         = -99;
     jetAK8puppi_sdJEC         = -99; 
     jetAK8puppi_sdcorr         = -99;

    
    jetAK8puppi_ptJEC_2         = -99;
    jetAK8puppi_eta_2         = -99;
    jetAK8puppi_phi_2         = -99;
    jetAK8puppi_tau1_2         = -99;
    jetAK8puppi_tau2_2         = -99;
    jetAK8puppi_tau3_2         = -99;
    jetAK8puppi_tau21_2         = -99;
    jetAK8puppi_tau4_2         = -99;
    jetAK8puppi_tau42_2         = -99;
    
    jetAK8puppi_sd_2         = -99;
    jetAK8puppi_sdJEC_2         = -99;
    jetAK8puppi_sdcorr_2         = -99;
    
     vbfeta=-10.;
     vbfmjj=-10.;
    vbfeta_2=-10.;
    vbfmjj_2=-10.;

     vbftag=0;
     nj1=-1;
     nj2=-1;
    nj1_2=-1;
    nj2_2=-1;

     yVlep          = -99;
     yVhad          = -99;
     yVhadJEC          = -99;
     phiVlep        = -99;
     phiVhad        = -99;
     massVlep       = -99;
     massVhad       = -99;
     massVhadJEC       = -99;
     mtVlep         = -99;
     tau1           = -99;
     tau2           = -99;
     tau3           = -99;
     tau21          = -99;
     sdrop          = -99;
     sdropJEC          = -99;
     ptlep1         = -99;
     ptlep2         = -99;
     etalep1        = -99;
     etalep2        = -99;
     philep1        = -99;
     philep2        = -99;
     met            = -99;
     metPhi         = -99;
     deltaRlepjet   = -99;
     delPhilepmet   = -99;
     delPhijetmet =  -99;
     delPhijetlep =  -99;
    
     deltaRlepjet_2   = -99;
    
     delPhijetmet_2 =  -99;
     delPhijetlep_2 =  -99;
    
     lep            = -99;
     gen_gra_m      = -99;
     gen_gra_pt     = -99;
    gen_gra_phi     = -99;

     gen_gra_eta     = -99;
     gen_rad_m      = -99;
     gen_rad_pt     = -99;
    gen_rad_phi     = -99;

     gen_rad_eta     = -99;
     gen_ele_pt     = -99;
     gen_ele_eta    = -99;
     gen_ele_phi    = -99;
     gen_ele_e      = -99;
     gen_mu_pt     = -99;
     gen_mu_eta    = -99;
     gen_mu_phi    = -99;
     gen_mu_e      = -99;
     genmatch_ele_pt     = -99;
     genmatch_ele_eta    = -99;
     genmatch_ele_phi    = -99;
     genmatch_ele_e      = -99;
     genmatch_ele_dr     =  99;
     genmatch_mu_pt     = -99;
     genmatch_mu_eta    = -99;
     genmatch_mu_phi    = -99;
     genmatch_mu_e      = -99;
     genmatch_mu_dr      = -99;
     gen_ele_pt_2     = -99;
     gen_ele_eta_2    = -99;
     gen_ele_phi_2    = -99;
     gen_ele_e_2      = -99;
     gen_mu_pt_2     = -99;
     gen_mu_eta_2    = -99;
     gen_mu_phi_2    = -99;
     gen_mu_e_2      = -99;
     gen_ele_pt_3     = -99;
     gen_ele_eta_3    = -99;
     gen_ele_phi_3    = -99;
     gen_ele_e_3      = -99;
     gen_mu_pt_3     = -99;
     gen_mu_eta_3    = -99;
     gen_mu_phi_3    = -99;
     gen_mu_e_3      = -99;
     gentop_pt  = -99;
     gentop_eta  = -99;
     gentop_phi  = -99;
     gentop_mass  = -99;
     genantitop_pt  = -99;
     genantitop_eta  = -99;
     genantitop_phi  = -99;
     genantitop_mass  = -99;
     ptGenVlep      = -99;
     etaGenVlep      = -99;
     phiGenVlep      = -99;
     massGenVlep      = -99;
     ptGenV_2      = -99;
     etaGenV_2      = -99;
     phiGenV_2      = -99;
     massGenV_2      = -99;
     ptGenV_3      = -99;
     etaGenV_3      = -99;
     phiGenV_3      = -99;
     massGenV_3      = -99;
     ptGenVhad      = -99;
     etaGenVhad      = -99;
     phiGenVhad      = -99;
     massGenVhad      = -99;
    ptGenVhad_2      = -99;
    etaGenVhad_2      = -99;
    phiGenVhad_2      = -99;
    massGenVhad_2      = -99;
    ptGenVhad_3      = -99;
    etaGenVhad_3      = -99;
    phiGenVhad_3      = -99;
    massGenVhad_3      = -99;
  
     status_1       =  -1;
     status_2       =  -1;
     status_3       =  -1;


        for(Int_t ii=0;ii<8;ii++){
        ak4jet_hf[ii] = -99;
        ak4jet_pf[ii] = -99;
        ak4jet_pt[ii] = -99;
        ak4jet_pt_uncorr[ii] = -99;
        ak4jet_eta[ii] = -99;
        ak4jet_phi[ii] = -99;
        ak4jet_e[ii] = -99;
        ak4jet_dr[ii] = -99;
        ak4jet_csv[ii] = -99;
        ak4jet_icsv[ii] = -99;
        deltaRAK4AK8[ii] = -99;
        ak4jet_IDLoose[ii] = -99;
        ak4jet_IDTight[ii] = -99;
            
            ak4jet_hf_2[ii] = -99;
            ak4jet_pf_2[ii] = -99;
            ak4jet_pt_2[ii] = -99;
            ak4jet_pt_uncorr_2[ii] = -99;
            ak4jet_eta_2[ii] = -99;
            ak4jet_phi_2[ii] = -99;
            ak4jet_e_2[ii] = -99;
            ak4jet_dr_2[ii] = -99;
            ak4jet_csv_2[ii] = -99;
            ak4jet_icsv_2[ii] = -99;
            deltaRAK4AK8_2[ii] = -99;
            ak4jet_IDLoose_2[ii] = -99;
            ak4jet_IDTight_2[ii] = -99;

        
    }
    for(int i=0;i<4;i++){
        jetAK8puppi_mass1[i] = -99;
        jetAK8puppi_pt1[i] = -99;
        jetAK8puppi_eta1[i] = -99;

    }
 
     IDLoose = false;
     IDTight = false;
     IDLoose_2 = false;
     IDTight_2 = false;
     isHighPt = false;
     isHEEP = false;
//     rho = -99;
     iso = -99;
     isoCut = -99;
     et = -99;
     trackIso = -99;
     muchaiso=-99.;
     muneuiso=-99.;
     muphoiso=-99.;
     muPU=-99.;
     muisolation=-99.;
//JEC
     jetAK8_mass = -99;
     jetAK8_pt = -99;
     jetAK8_jec = -99;
     jetAK8_mass1[0] = -99;
     jetAK8_mass1[1] = -99;
     jetAK8_mass1[2] = -99;
     jetAK8_SF_mass1[0] = -99;
     jetAK8_SF_mass1[1] = -99;
     jetAK8_SF_mass1[2] = -99;
     jetAK8_SF_mass2[0] = -99;
     jetAK8_SF_mass2[1] = -99;
     jetAK8_SF_mass2[2] = -99;
     jetAK8_pt1[0] = -99;
     jetAK8_pt1[1] = -99;
     jetAK8_pt1[2] = -99;
     jetAK8_jec1[0] = -99;
     jetAK8_jec1[1] = -99;
     jetAK8_jec1[2] = -99;
  /*   prundM[0] = -99;
     prundM[1] = -99;
     prundM[2] = -99;
     sdropM[0] = -99;
     sdropM[1] = -99;
     sdropM[2] = -99;
*/     corr_AK81[0] = -99;
     corr_AK81[1] = -99;
     corr_AK81[2] = -99;
     jetAK8_eta = -99;
     jetAK8_eta1[0] = -99;
     jetAK8_eta1[1] = -99;
     jetAK8_eta1[2] = -99;
     jetAK8_phi = -99;

     METraw_et = -99;
     METraw_phi = -99;
     METraw_sumEt = -99;
     MET_et = -99;
     MET_phi = -99;
     MET_sumEt = -99;
     MET_corrPx = -99;
     MET_corrPy = -99;

     candMassJEC     =  -99;
     candMasspuppiJEC     =  -99;
     ptVlepJEC       =  -99;
     yVlepJEC        =  -99;
     phiVlepJEC      =  -99;
     massVlepJEC     =  -99;
     mtVlepJEC       =  -99;
    massww[0] = -99;
    massww[1] = -99;
    massww[2] = -99;
    masslvj1 = -99;
    masslvj2 = -99;
    massj1j2 = -99;

    massWWZ = -99;

     HLT_Ele1=-99;
     HLT_Ele2=-99;
     HLT_Ele3=-99;
     HLT_Ele4=-99;
     HLT_Ele5=-99;
     HLT_Ele6=-99;
     HLT_Ele7=-99;
     HLT_Ele8=-99;
     HLT_Mu1=-99;
     HLT_Mu2=-99;
     HLT_Mu3=-99;
     HLT_Mu4=-99;
     HLT_Mu5=-99;
     HLT_Mu6=-99;
     HLT_Mu7=-99;
     HLT_Mu8=-99;
     HLT_Mu9=-99;
     HLT_Mu10=-99;
     HLT_Mu11=-99;
     HLT_Mu12=-99;

     theWeight = -99;
     //nump = 0;
     //numm = 0;
     passFilter_HBHE_                  = false;
     passFilter_HBHEIso_               = false;
     passFilter_GlobalHalo_            = false;
     passFilter_ECALDeadCell_          = false;
     passFilter_GoodVtx_               = false;
     passFilter_EEBadSc_               = false;
     passFilter_badMuon_               = false;
     passFilter_badChargedHadron_      = false;

}

// ------------ method called once each job just before starting event loop  ------------
void 
EDBRTreeMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void EDBRTreeMaker::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{


  elPaths1.clear();
  elPaths2.clear();
  elPaths3.clear();
  elPaths4.clear();
  elPaths5.clear();
  elPaths6.clear();
  elPaths7.clear();
  elPaths8.clear();
  muPaths1.clear();
  muPaths2.clear();
  muPaths3.clear();
  muPaths4.clear();
  muPaths5.clear();
  muPaths6.clear();
  muPaths7.clear();
  muPaths8.clear();
  muPaths9.clear();
  muPaths10.clear();
  muPaths11.clear();
  muPaths12.clear();

  std::cout<<"-----begin-----"<<std::endl;
   bool changed;
   if ( !hltConfig.init(iRun, iSetup, "HLT", changed) ) {
        edm::LogError("HltAnalysis") << "Initialization of HLTConfigProvider failed!!";
       return;
      }
   for (size_t i = 0; i < elPaths1_.size(); i++) {
         std::vector<std::string> foundPaths1 = hltConfig.matched( hltConfig.triggerNames(), elPaths1_[i] );
         while ( !foundPaths1.empty() ){
               elPaths1.push_back( foundPaths1.back() );
               foundPaths1.pop_back(); }
                                                }
   for (size_t i = 0; i < muPaths1_.size(); i++) {
         std::vector<std::string> foundPaths1 = hltConfig.matched( hltConfig.triggerNames(), muPaths1_[i] );
         while ( !foundPaths1.empty() ){
               muPaths1.push_back( foundPaths1.back() );
               foundPaths1.pop_back();
                                      }
                                                }
   std::cout<<"\n************** HLT-1 Information **************\n";
   for (size_t i=0; i < elPaths1.size(); i++) std::cout << "\n Electron paths-1:    " << i<<"  "<<elPaths1[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths1.size(); i++) std::cout << "\n Muon paths-1:   " << i<<"  "<<muPaths1[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths2_.size(); i++) {
         std::vector<std::string> foundPaths2 = hltConfig.matched( hltConfig.triggerNames(), elPaths2_[i] );
         while ( !foundPaths2.empty() ){
               elPaths2.push_back( foundPaths2.back() );
               foundPaths2.pop_back();
                                      }
                                                }
   for (size_t i = 0; i < muPaths2_.size(); i++) {
         std::vector<std::string> foundPaths2 = hltConfig.matched( hltConfig.triggerNames(), muPaths2_[i] );
         while ( !foundPaths2.empty() ){
               muPaths2.push_back( foundPaths2.back() );
               foundPaths2.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-2 Information **************\n";
   for (size_t i=0; i < elPaths2.size(); i++) std::cout << "\n Electron paths-2:    " << i<<"  "<<elPaths2[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths2.size(); i++) std::cout << "\n Muon paths-2:   " << i<<"  "<<muPaths2[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths3_.size(); i++) {
         std::vector<std::string> foundPaths3 = hltConfig.matched( hltConfig.triggerNames(), elPaths3_[i] );
         while ( !foundPaths3.empty() ){
               elPaths3.push_back( foundPaths3.back() );
               foundPaths3.pop_back();
                                      }
                                                }

   for (size_t i = 0; i < muPaths3_.size(); i++) {
         std::vector<std::string> foundPaths3 = hltConfig.matched( hltConfig.triggerNames(), muPaths3_[i] );
         while ( !foundPaths3.empty() ){
               muPaths3.push_back( foundPaths3.back() );
               foundPaths3.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-3 Information **************\n";
   for (size_t i=0; i < elPaths3.size(); i++) std::cout << "\n Electron paths-3:    " << i<<"  "<<elPaths3[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths3.size(); i++) std::cout << "\n Muon paths-3:   " << i<<"  "<<muPaths3[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths4_.size(); i++) {
         std::vector<std::string> foundPaths4 = hltConfig.matched( hltConfig.triggerNames(), elPaths4_[i] );
         while ( !foundPaths4.empty() ){
               elPaths4.push_back( foundPaths4.back() );
               foundPaths4.pop_back();
                                      }
                                                }

   for (size_t i = 0; i < muPaths4_.size(); i++) {
         std::vector<std::string> foundPaths4 = hltConfig.matched( hltConfig.triggerNames(), muPaths4_[i] );
         while ( !foundPaths4.empty() ){
               muPaths4.push_back( foundPaths4.back() );
               foundPaths4.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-4 Information **************\n";
   for (size_t i=0; i < elPaths4.size(); i++) std::cout << "\n Electron paths-4:    " << i<<"  "<<elPaths4[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths4.size(); i++) std::cout << "\n Muon paths-4:   " << i<<"  "<<muPaths4[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths5_.size(); i++) {
         std::vector<std::string> foundPaths5 = hltConfig.matched( hltConfig.triggerNames(), elPaths5_[i] );
         while ( !foundPaths5.empty() ){
               elPaths5.push_back( foundPaths5.back() );
               foundPaths5.pop_back();
                                      }
                                                }

   for (size_t i = 0; i < muPaths5_.size(); i++) {
         std::vector<std::string> foundPaths5 = hltConfig.matched( hltConfig.triggerNames(), muPaths5_[i] );
         while ( !foundPaths5.empty() ){
               muPaths5.push_back( foundPaths5.back() );
               foundPaths5.pop_back();
                                      }
                                                }
   std::cout<<"\n************** HLT-5 Information **************\n";
   for (size_t i=0; i < elPaths5.size(); i++) std::cout << "\n Electron paths-5:    " << i<<"  "<<elPaths5[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths5.size(); i++) std::cout << "\n Muon paths-5:   " << i<<"  "<<muPaths5[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths6_.size(); i++) {
         std::vector<std::string> foundPaths6 = hltConfig.matched( hltConfig.triggerNames(), elPaths6_[i] );
         while ( !foundPaths6.empty() ){
               elPaths6.push_back( foundPaths6.back() );
               foundPaths6.pop_back();
                                      }
                                                }

   for (size_t i = 0; i < muPaths6_.size(); i++) {
         std::vector<std::string> foundPaths6 = hltConfig.matched( hltConfig.triggerNames(), muPaths6_[i] );
         while ( !foundPaths6.empty() ){
               muPaths6.push_back( foundPaths6.back() );
               foundPaths6.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-6 Information **************\n";
   for (size_t i=0; i < elPaths6.size(); i++) std::cout << "\n Electron paths-6:    " << i<<"  "<<elPaths6[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths6.size(); i++) std::cout << "\n Muon paths-6:   " << i<<"  "<<muPaths6[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths7_.size(); i++) {
         std::vector<std::string> foundPaths7 = hltConfig.matched( hltConfig.triggerNames(), elPaths7_[i] );
         while ( !foundPaths7.empty() ){
               elPaths7.push_back( foundPaths7.back() );
               foundPaths7.pop_back();
                                      }
                                                }

   for (size_t i = 0; i < muPaths7_.size(); i++) {
         std::vector<std::string> foundPaths7 = hltConfig.matched( hltConfig.triggerNames(), muPaths7_[i] );
         while ( !foundPaths7.empty() ){
               muPaths7.push_back( foundPaths7.back() );
               foundPaths7.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-7 Information **************\n";
   for (size_t i=0; i < elPaths7.size(); i++) std::cout << "\n Electron paths-7:    " << i<<"  "<<elPaths7[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths7.size(); i++) std::cout << "\n Muon paths-7:   " << i<<"  "<<muPaths7[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < elPaths8_.size(); i++) {
         std::vector<std::string> foundPaths8 = hltConfig.matched( hltConfig.triggerNames(), elPaths8_[i] );
         while ( !foundPaths8.empty() ){
               elPaths8.push_back( foundPaths8.back() );
               foundPaths8.pop_back();
                                      }
                                                }

   for (size_t i = 0; i < muPaths8_.size(); i++) {
         std::vector<std::string> foundPaths8 = hltConfig.matched( hltConfig.triggerNames(), muPaths8_[i] );
         while ( !foundPaths8.empty() ){
               muPaths8.push_back( foundPaths8.back() );
               foundPaths8.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-8 Information **************\n";
   for (size_t i=0; i < elPaths8.size(); i++) std::cout << "\n Electron paths-8:    " << i<<"  "<<elPaths8[i].c_str() <<"\t"<< std::endl;
   for (size_t i=0; i < muPaths8.size(); i++) std::cout << "\n Muon paths-8:   " << i<<"  "<<muPaths8[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < muPaths9_.size(); i++) {
         std::vector<std::string> foundPaths9 = hltConfig.matched( hltConfig.triggerNames(), muPaths9_[i] );
         while ( !foundPaths9.empty() ){
               muPaths9.push_back( foundPaths9.back() );
               foundPaths9.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-9 Information **************\n";
   for (size_t i=0; i < muPaths9.size(); i++) std::cout << "\n Muon paths-9:   " << i<<"  "<<muPaths9[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < muPaths10_.size(); i++) {
         std::vector<std::string> foundPaths10 = hltConfig.matched( hltConfig.triggerNames(), muPaths10_[i] );
         while ( !foundPaths10.empty() ){
               muPaths10.push_back( foundPaths10.back() );
               foundPaths10.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-10 Information **************\n";
   for (size_t i=0; i < muPaths10.size(); i++) std::cout << "\n Muon paths-10:   " << i<<"  "<<muPaths10[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < muPaths11_.size(); i++) {
         std::vector<std::string> foundPaths11 = hltConfig.matched( hltConfig.triggerNames(), muPaths11_[i] );
         while ( !foundPaths11.empty() ){
               muPaths11.push_back( foundPaths11.back() );
               foundPaths11.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-11 Information **************\n";
   for (size_t i=0; i < muPaths11.size(); i++) std::cout << "\n Muon paths-11:   " << i<<"  "<<muPaths11[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

   for (size_t i = 0; i < muPaths12_.size(); i++) {
         std::vector<std::string> foundPaths12 = hltConfig.matched( hltConfig.triggerNames(), muPaths12_[i] );
         while ( !foundPaths12.empty() ){
               muPaths12.push_back( foundPaths12.back() );
               foundPaths12.pop_back();
                                      }
                                                }

   std::cout<<"\n************** HLT-12 Information **************\n";
   for (size_t i=0; i < muPaths12.size(); i++) std::cout << "\n Muon paths-12:   " << i<<"  "<<muPaths12[i].c_str() <<"\t"<< std::endl;
   std::cout<<"\n*********************************************\n\n";

}

void EDBRTreeMaker::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
std::cout << "EDBRTreeMaker endJob()... endRun" << std::endl;
}


void
EDBRTreeMaker::endJob() {
  std::cout << "EDBRTreeMaker endJob()..." << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(EDBRTreeMaker);
