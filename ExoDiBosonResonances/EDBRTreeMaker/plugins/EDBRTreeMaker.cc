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
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
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

// DeepAK8
#include "NNKit/FatJetNN/interface/FatJetNN.h"
#include "NNKit/FatJetNN/interface/FatJetNNHelper.h"

#include "EDBRChannels.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include <TFormula.h>

#define Pi 3.141593
using namespace std;
using namespace deepntuples; // DeepAK8
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
    virtual const reco::Candidate* findLasttau(const reco::Candidate *particle,int IDpdg);
    virtual const reco::Candidate* findFirstW(const reco::Candidate *particle,int IDpdg);
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
    edm::EDGetTokenT<bool>  badMuon_Selector_;
    edm::EDGetTokenT<bool>  badChargedHadron_Selector_;

    // bool doHltFilters_;
    // DeepAK8
    deepntuples::FatJetNN* fatjetNN_ = nullptr;
    deepntuples::FatJetNN* decorrNN_ = nullptr;
  
    // ----------member data ---------------------------
    TTree* outTree_;
    TTree* outTreew_;

    double MW_;
    int nmetmatch, nmetno;
    int nevent, run, ls;
    int nVtx;
    int numCands;
    int nLooseEle, nLooseMu;//Synch
    double ptVlep, yVlep, phiVlep, massVlep;
    double met, metPhi, mtVlep;
    double jetAK8puppi_ptJEC, jetAK8puppi_eta, jetAK8puppi_phi, jetAK8puppi_tau1,  jetAK8puppi_tau2, jetAK8puppi_tau3, jetAK8puppi_tau4,jetAK8puppi_tau21, jetAK8puppi_tau42, jetAK8puppi_sd, jetAK8puppi_sdJEC, jetAK8puppi_sdcorr;
    double jetAK8puppi_ptJEC_2, jetAK8puppi_eta_2, jetAK8puppi_phi_2, jetAK8puppi_tau1_2,  jetAK8puppi_tau2_2, jetAK8puppi_tau3_2,jetAK8puppi_tau4_2, jetAK8puppi_tau21_2,jetAK8puppi_tau42_2,  jetAK8puppi_sd_2, jetAK8puppi_sdJEC_2, jetAK8puppi_sdcorr_2;
    double jetAK8puppi_ptJEC_3, jetAK8puppi_eta_3, jetAK8puppi_phi_3, jetAK8puppi_tau1_3,  jetAK8puppi_tau2_3, jetAK8puppi_tau3_3,jetAK8puppi_tau4_3, jetAK8puppi_tau21_3,jetAK8puppi_tau42_3,  jetAK8puppi_sd_3, jetAK8puppi_sdJEC_3, jetAK8puppi_sdcorr_3;
    double jetAK8puppi_dnnTop, jetAK8puppi_dnnW,jetAK8puppi_dnnH4q,jetAK8puppi_dnnTop_2, jetAK8puppi_dnnW_2,jetAK8puppi_dnnH4q_2,jetAK8puppi_dnnTop_3, jetAK8puppi_dnnW_3,jetAK8puppi_dnnH4q_3; //DeepAK8
    double jetAK8puppi_dnnqcd,jetAK8puppi_dnntop,jetAK8puppi_dnnw,jetAK8puppi_dnnz,jetAK8puppi_dnnzbb,jetAK8puppi_dnnhbb,jetAK8puppi_dnnh4q,jetAK8puppi_dnnqcd_2,jetAK8puppi_dnntop_2,jetAK8puppi_dnnw_2,jetAK8puppi_dnnz_2,jetAK8puppi_dnnzbb_2,jetAK8puppi_dnnhbb_2,jetAK8puppi_dnnh4q_2,jetAK8puppi_dnnqcd_3,jetAK8puppi_dnntop_3,jetAK8puppi_dnnw_3,jetAK8puppi_dnnz_3,jetAK8puppi_dnnzbb_3,jetAK8puppi_dnnhbb_3,jetAK8puppi_dnnh4q_3;
    double jetAK8puppi_dnnZ,jetAK8puppi_dnnZbb,jetAK8puppi_dnnHbb,jetAK8puppi_dnnZ_2,jetAK8puppi_dnnZbb_2,jetAK8puppi_dnnHbb_2,jetAK8puppi_dnnZ_3,jetAK8puppi_dnnZbb_3,jetAK8puppi_dnnHbb_3;
    double jetAK8puppi_dnnDecorrTop, jetAK8puppi_dnnDecorrW,jetAK8puppi_dnnDecorrH4q,jetAK8puppi_dnnDecorrTop_2, jetAK8puppi_dnnDecorrW_2, jetAK8puppi_dnnDecorrH4q_2,jetAK8puppi_dnnDecorrTop_3, jetAK8puppi_dnnDecorrW_3, jetAK8puppi_dnnDecorrH4q_3; //Decorrelated DeepAK8
    double jetAK8puppi_dnnDecorrZ,jetAK8puppi_dnnDecorrZbb,jetAK8puppi_dnnDecorrHbb,jetAK8puppi_dnnDecorrZ_2,jetAK8puppi_dnnDecorrZbb_2,jetAK8puppi_dnnDecorrHbb_2,jetAK8puppi_dnnDecorrZ_3,jetAK8puppi_dnnDecorrZbb_3,jetAK8puppi_dnnDecorrHbb_3;
    double jetAK8puppi_dnnDecorrbb,jetAK8puppi_dnnDecorrcc,jetAK8puppi_dnnDecorrbbnog,jetAK8puppi_dnnDecorrccnog,jetAK8puppi_dnnDecorrbb_2,jetAK8puppi_dnnDecorrcc_2,jetAK8puppi_dnnDecorrbbnog_2,jetAK8puppi_dnnDecorrccnog_2,jetAK8puppi_dnnDecorrbb_3,jetAK8puppi_dnnDecorrcc_3,jetAK8puppi_dnnDecorrbbnog_3,jetAK8puppi_dnnDecorrccnog_3;
    double jetAK8puppi_dnnDecorrqcd,jetAK8puppi_dnnDecorrtop,jetAK8puppi_dnnDecorrw,jetAK8puppi_dnnDecorrz,jetAK8puppi_dnnDecorrzbb,jetAK8puppi_dnnDecorrhbb,jetAK8puppi_dnnDecorrh4q,jetAK8puppi_dnnDecorrqcd_2,jetAK8puppi_dnnDecorrtop_2,jetAK8puppi_dnnDecorrw_2,jetAK8puppi_dnnDecorrz_2,jetAK8puppi_dnnDecorrzbb_2,jetAK8puppi_dnnDecorrhbb_2,jetAK8puppi_dnnDecorrh4q_2,jetAK8puppi_dnnDecorrqcd_3,jetAK8puppi_dnnDecorrtop_3,jetAK8puppi_dnnDecorrw_3,jetAK8puppi_dnnDecorrz_3,jetAK8puppi_dnnDecorrzbb_3,jetAK8puppi_dnnDecorrhbb_3,jetAK8puppi_dnnDecorrh4q_3;
    
    double ptgenwl[5],etagenwl[5],phigenwl[5],massgenwl[5],taggenwl[5],taggenwmother[5];
    double genw_q1_pt[5],genw_q1_eta[5],genw_q1_phi[5],genw_q1_e[5],genw_q1_pdg[5];
    double genw_q2_pt[5],genw_q2_eta[5],genw_q2_phi[5],genw_q2_e[5],genw_q2_pdg[5];
    double ptgenzl[5],etagenzl[5],phigenzl[5],massgenzl[5],taggenzl[5];
    double ptgengl[10],etagengl[10],phigengl[10],egengl[10];
    double ptgenwf[5],etagenwf[5],phigenwf[5],massgenwf[5];
    double ptgenzf[5],etagenzf[5],phigenzf[5],massgenzf[5];
    double ptgengf[10],etagengf[10],phigengf[10],egengf[10];
    double gent_b_pt,gent_b_phi,gent_b_eta,gent_b_mass;
    double genantit_b_pt,genantit_b_phi,genantit_b_eta,genantit_b_mass;
    double gent_w_pt,gent_w_phi,gent_w_eta,gent_w_mass;
    double genantit_w_pt,genantit_w_phi,genantit_w_eta,genantit_w_mass;
    double gent_w_q1_pt,gent_w_q1_phi,gent_w_q1_eta,gent_w_q1_e,gent_w_q1_pdg;
    double genantit_w_q1_pt,genantit_w_q1_phi,genantit_w_q1_eta,genantit_w_q1_e,genantit_w_q1_pdg;
    double gent_w_q2_pt,gent_w_q2_phi,gent_w_q2_eta,gent_w_q2_e,gent_w_q2_pdg;
    double genantit_w_q2_pt,genantit_w_q2_phi,genantit_w_q2_eta,genantit_w_q2_e,genantit_w_q2_pdg;
    double ptgenq1l[5],etagenq1l[5],phigenq1l[5],egenq1l[5];
    double ptgenq1f[5],etagenq1f[5],phigenq1f[5],egenq1f[5];
    double ptgenq2l[5],etagenq2l[5],phigenq2l[5],egenq2l[5];
    double ptgenq2f[5],etagenq2f[5],phigenq2f[5],egenq2f[5];
    double ptgenq3l[5],etagenq3l[5],phigenq3l[5],egenq3l[5];
    double ptgenq3f[5],etagenq3f[5],phigenq3f[5],egenq3f[5];
    double ptgenq4l[5],etagenq4l[5],phigenq4l[5],egenq4l[5];
    double ptgenq4f[5],etagenq4f[5],phigenq4f[5],egenq4f[5];
    double ptgenq5l[5],etagenq5l[5],phigenq5l[5],egenq5l[5];
    double ptgenq5f[5],etagenq5f[5],phigenq5f[5],egenq5f[5];
    double mothergenq1f[5],mothergenq2f[5],mothergenq3f[5],mothergenq4f[5],mothergenq5f[5];
    
    double gent_w_tag,genantit_w_tag,mothergengf[10],mmothergengf[10],mmothergenq1f[5],mmothergenq2f[5],mmothergenq3f[5],mmothergenq4f[5],mmothergenq5f[5];
    
    double vbfeta, vbfmjj;
    int      vbftag;
    int nj1, nj2;
    int numq,numq_2,numq_3;
    double ptlep1, ptlep2;
    double etalep1, etalep2 ;
    double philep1, philep2 ;
    double triggerWeight, lumiWeight, pileupWeight;
    int channel, lep;
    double deltaRlepjet, delPhilepmet, delPhijetmet, delPhijetlep;
    double deltaRlepjet_2,  delPhijetmet_2, delPhijetlep_2;
    double candMass;
    double pt_graviton,pt_graviton1;
    double ptVlepJEC, yVlepJEC, phiVlepJEC;
    double candMasspuppiJEC,m_jlv;
    double massww[3],masslvj1,masslvj2,massj1j2;
    double massVlepJEC, mtVlepJEC;

    double theWeight;
    double  nump=0;
    double  numm=0;
    double pweight[212];
    double  npT, npIT;
    int     nBX;
    //Gen Level
    double gen_gra_m, gen_gra_pt, gen_gra_eta,gen_gra_phi;
    double gen_rad_m, gen_rad_pt, gen_rad_eta,gen_rad_phi;
    double gen_ele_pt, gen_ele_eta, gen_ele_phi, gen_ele_e;
    double gen_tau_pt, gen_tau_eta, gen_tau_phi, gen_tau_e;
    double gen_tau_pt_2, gen_tau_eta_2, gen_tau_phi_2, gen_tau_e_2;
    double gen_tau_pt_3, gen_tau_eta_3, gen_tau_phi_3, gen_tau_e_3;

    double pttau[4],etatau[4],phitau[4],etau[4],pdgidtau[4];
    double pttau_2[4],etatau_2[4],phitau_2[4],etau_2[4],pdgidtau_2[4];
    double pttau_3[4],etatau_3[4],phitau_3[4],etau_3[4],pdgidtau_3[4];
   
    double ptq[3],etaq[3],phiq[3],eq[3],pdgidq[3];
    double ptq_2[3],etaq_2[3],phiq_2[3],eq_2[3],pdgidq_2[3];
    double ptq_3[3],etaq_3[3],phiq_3[3],eq_3[3],pdgidq_3[3];

    double gen_nele_pt, gen_nele_eta, gen_nele_phi, gen_nele_e;
    double gen_nele_pt_2, gen_nele_eta_2, gen_nele_phi_2, gen_nele_e_2;
    double gen_nmu_pt, gen_nmu_eta, gen_nmu_phi, gen_nmu_e;
    double gen_nmu_pt_2, gen_nmu_eta_2, gen_nmu_phi_2, gen_nmu_e_2;
    double gen_nele_pt_3, gen_nele_eta_3, gen_nele_phi_3, gen_nele_e_3;
    double gen_nmu_pt_3, gen_nmu_eta_3, gen_nmu_phi_3, gen_nmu_e_3;
    double gen_ntau_pt, gen_ntau_eta, gen_ntau_phi, gen_ntau_e;
    double gen_ntau_pt_2, gen_ntau_eta_2, gen_ntau_phi_2, gen_ntau_e_2;
    double gen_ntau_pt_3, gen_ntau_eta_3, gen_ntau_phi_3, gen_ntau_e_3;

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
    
    bool IDLoose, IDTight,IDLoose_2, IDTight_2,IDLoose_3, IDTight_3, isHighPt, isHEEP;
    double muchaiso, muneuiso, muphoiso, muPU, muisolation;
    double iso, isoCut, et, trackIso;
    //  double rho,fastJetRho;
    double useless;
    //  JEC
    double corr_AK8puppi[4],corr_AK8puppiSD[4];
    double jetAK8puppi_pt1[4], jetAK8puppi_mass1[4], jetAK8puppi_eta1[4], jetAK8puppi_jec1[4], jetAK8puppiSD_jec1[4];

    double corr;
    double METraw_et, METraw_phi, METraw_sumEt;
    double MET_et, MET_phi, MET_sumEt, MET_corrPx, MET_corrPy;
    // AK4 Jets
    int ak4jet_hf[8],ak4jet_pf[8],ak4jet_hf_2[8],ak4jet_pf_2[8];
    double ak4jet_pt[8],ak4jet_pt_uncorr[8],ak4jet_eta[8],ak4jet_phi[8],ak4jet_e[8], ak4jet_dr[8];
    double ak4jet_csv[8],ak4jet_icsv[8], ak4jet_IDLoose[8], ak4jet_IDTight[8],ak4jet_deepcsvudsg[8],ak4jet_deepcsvb[8],ak4jet_deepcsvc[8],ak4jet_deepcsvbb[8],ak4jet_deepcsvcc[8];
    TLorentzVector ak8sj11,ak8sj12,ak8sj13,ak8sj14,ak8sj15,puppi_softdropj1;
    TLorentzVector ak8sj21,ak8sj22,ak8sj23,ak8sj24,ak8sj25,puppi_softdropj2;

    void setDummyValues();
    
    //// L1 prefiring
    edm::EDGetTokenT< double > prefweight_token;
    edm::EDGetTokenT< double > prefweightup_token;
    edm::EDGetTokenT< double > prefweightdown_token;
    
    /// Parameters to steer the treeDumper
    int originalNEvents_;
    double crossSectionPb_;
    double targetLumiInvPb_;
    std::string EDBRChannel_;
    bool isGen_;
    bool isJEC_;
    bool RunOnSig_,RunOnMC_;
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

    //L1 prefiring
    double L1prefiring,L1prefiringup,L1prefiringdown;
    
    int num_correctpuppijets, num_totalpuppijets;


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
    edm::EDGetTokenT<pat::JetCollection> hadronicVSoftDropSrc_;
    edm::EDGetTokenT<edm::View<pat::Jet>> ak4jetsSrc_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetsAK8Label_;
    edm::EDGetTokenT<LHEEventProduct> LheToken_;
    edm::EDGetTokenT<LHERunInfoProduct> LhestrToken_;

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
    muPaths12_(iConfig.getParameter<std::vector<std::string>>("muPaths12"))
    //  noiseFilterToken_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("noiseFilter")))
{
    LheToken_=consumes<LHEEventProduct> (iConfig.getParameter<edm::InputTag>( "lhe") ) ;
    LhestrToken_=consumes<LHERunInfoProduct,edm::InRun> (iConfig.getParameter<edm::InputTag>( "lhe") ) ;
    originalNEvents_ = iConfig.getParameter<int>("originalNEvents");
    crossSectionPb_  = iConfig.getParameter<double>("crossSectionPb");
    targetLumiInvPb_ = iConfig.getParameter<double>("targetLumiInvPb");
    EDBRChannel_     = iConfig.getParameter<std::string>("EDBRChannel");
    isGen_           = iConfig.getParameter<bool>("isGen");
    isJEC_           = iConfig.getParameter<bool>("isJEC");
    RunOnSig_        = iConfig.getParameter<bool>("RunOnSig");
    RunOnMC_        = iConfig.getParameter<bool>("RunOnMC");
    // Sources
    //  leptonicVSrc_ = iConfig.getParameter<std::string>("leptonicVSrc");
    leptonicVSrc_=consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>( "leptonicVSrc") ) ;
    looseelectronToken_    = (consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>("looseElectronSrc"))) ;
    loosemuonToken_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("looseMuonSrc")));
    //  gravitonSrc_     = iConfig.getParameter<std::string>("gravitonSrc");
    goodMuSrc_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("goodMuSrc")));
    MuSrc_    = (consumes<edm::View<pat::Muon> > (iConfig.getParameter<edm::InputTag>("MuSrc")));
    EleSrc_    = (consumes<edm::View<pat::Electron> > (iConfig.getParameter<edm::InputTag>("EleSrc")));
    jetsAK8Label_      = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>( "ak8JetSrc") ) ;

    //  goodMuSrc_    = iConfig.getParameter<std::string>("goodMuSrc");
    //  looseMuonSrc_    = iConfig.getParameter<std::string>("looseMuonSrc");
    //  looseElectronsSrc_= iConfig.getParameter<std::string>("looseElectronsSrc");
    muonToken_ = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
    //  ak4jetsSrc_      = iConfig.getParameter<std::string>("ak4jetsSrc");
    ak4jetsSrc_      = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>( "ak4jetsSrc") ) ;

    //  hadronicVSrc_ = iConfig.getParameter<std::string>("hadronicVSrc");
    hadronicVSrc_ = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("hadronicVSrc") ) ;
    hadronicVSrc_raw_ = consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("hadronicVSrc_raw") ) ;
    hadronicVSoftDropSrc_ = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("hadronicVSoftDropSrc") ) ;
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

    //  L1 prefiring
    prefweight_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProb"));
    prefweightup_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProbUp"));
    prefweightdown_token = consumes< double >(edm::InputTag("prefiringweight:NonPrefiringProbDown"));
    
    // filter
    noiseFilterToken_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("noiseFilter"));
    HBHENoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseFilter");
    HBHENoiseIsoFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_HBHENoiseIsoFilter");
    GlobalHaloNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_GlobalTightHaloFilter");
    ECALDeadCellNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_EcalDeadCellTriggerPrimitiveFilter");
    GoodVtxNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_goodVertices");
    EEBadScNoiseFilter_Selector_ =  iConfig.getParameter<std::string> ("noiseFilterSelection_eeBadScFilter");
    badMuon_Selector_ =  consumes<bool>(iConfig.getParameter<edm::InputTag> ("noiseFilterSelection_badMuon"));
    badChargedHadron_Selector_ =  consumes<bool>(iConfig.getParameter<edm::InputTag> ("noiseFilterSelection_badChargedHadron"));

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
        ex << "Unknown channel " << EDBRChannel_<< ". Please check EDBRTreeMaker.cc for allowed values.";
    throw ex;
    }
  
    // DeepAK8
    // initialize the FatJetNN class in the constructor
    auto cc = consumesCollector();
    fatjetNN_ = new deepntuples::FatJetNN(iConfig, cc, 0.8); // jetR=0.8
    // load json for input variable transformation
    fatjetNN_->load_json(edm::FileInPath("NNKit/data/ak8/full/preprocessing.json").fullPath());
    // load DNN model and parameter files
    fatjetNN_->load_model(edm::FileInPath("NNKit/data/ak8/full/resnet-symbol.json").fullPath(),
      edm::FileInPath("NNKit/data/ak8/full/resnet.params").fullPath());
    // -----------

    // Decorrelated DeepAK8
    decorrNN_ = new FatJetNN(iConfig, cc, 0.8);
    decorrNN_->load_json(edm::FileInPath("NNKit/data/ak8/decorrelated/preprocessing.json").fullPath());
    decorrNN_->load_model(edm::FileInPath("NNKit/data/ak8/decorrelated/resnet-symbol.json").fullPath(),
      edm::FileInPath("NNKit/data/ak8/decorrelated/resnet.params").fullPath());
    // -----------

    //now do what ever initialization is needed
    edm::Service<TFileService> fs;

    outTree_ = fs->make<TTree>("EDBRCandidates","EDBR Candidates");
    outTreew_ = fs->make<TTree>("EDBRCandidatesw","EDBR Candidates");
    outTreew_->Branch("num_correctpuppijets,"           ,&num_correctpuppijets         ,"num_correctpuppijets/I"          );
    outTreew_->Branch("num_totalpuppijets"           ,&num_totalpuppijets         ,"num_totalpuppijets/I"          );
    outTree_->Branch("L1prefiring"           ,&L1prefiring         ,"L1prefiring/D"          );
    outTree_->Branch("L1prefiringup"           ,&L1prefiringup         ,"L1prefiringup/D"          );
    outTree_->Branch("L1prefiringdown"           ,&L1prefiringdown         ,"L1prefiringdown/D"          );
    /// Basic event quantities
    if (RunOnMC_){
        outTree_->Branch("pweight"           ,pweight         ,"pweight[212]/D"          );
        outTree_->Branch("ptgenwl"           ,ptgenwl         ,"ptgenwl[5]/D"          );
        outTree_->Branch("etagenwl"           ,etagenwl         ,"etagenwl[5]/D"          );
        outTree_->Branch("phigenwl"           ,phigenwl       ,"phigenwl[5]/D"          );
        outTree_->Branch("massgenwl"           ,massgenwl         ,"massgenwl[5]/D"          );
        outTree_->Branch("taggenwl"           ,taggenwl         ,"taggenwl[5]/D"          );
        outTree_->Branch("taggenwmother"           ,taggenwmother         ,"taggenwmother[5]/D"          );
        outTree_->Branch("genw_q1_pt"           ,genw_q1_pt         ,"genw_q1_pt[5]/D"          );
        outTree_->Branch("genw_q1_phi"           ,genw_q1_phi         ,"genw_q1_phi[5]/D"          );
        outTree_->Branch("genw_q1_eta"           ,genw_q1_eta         ,"genw_q1_eta[5]/D"          );
        outTree_->Branch("genw_q1_e"           ,genw_q1_e         ,"genw_q1_e[5]/D"          );
        outTree_->Branch("genw_q1_pdg"           ,genw_q1_pdg         ,"genw_q1_pdg[5]/D"          );
        outTree_->Branch("genw_q2_pt"           ,genw_q2_pt         ,"genw_q2_pt[5]/D"          );
        outTree_->Branch("genw_q2_phi"           ,genw_q2_phi         ,"genw_q2_phi[5]/D"          );
        outTree_->Branch("genw_q2_eta"           ,genw_q2_eta         ,"genw_q2_eta[5]/D"          );
        outTree_->Branch("genw_q2_e"           ,genw_q2_e         ,"genw_q2_e[5]/D"          );
        outTree_->Branch("genw_q2_pdg"           ,genw_q2_pdg         ,"genw_q2_pdg[5]/D"          );

        outTree_->Branch("ptgenzl"           ,ptgenzl         ,"ptgenzl[5]/D"          );
        outTree_->Branch("etagenzl"           ,etagenzl         ,"etagenzl[5]/D"          );
        outTree_->Branch("phigenzl"           ,phigenzl       ,"phigenzl[5]/D"          );
        outTree_->Branch("massgenzl"           ,massgenzl         ,"massgenzl[5]/D"          );
        outTree_->Branch("taggenzl"           ,taggenzl         ,"taggenzl[5]/D"          );
        outTree_->Branch("ptgengl"           ,ptgengl         ,"ptgengl[10]/D"          );
        outTree_->Branch("etagengl"           ,etagengl         ,"etagengl[10]/D"          );
        outTree_->Branch("phigengl"           ,phigengl       ,"phigengl[10]/D"          );
        outTree_->Branch("egengl"           ,egengl         ,"egengl[10]/D"          );
        outTree_->Branch("ptgenwf"           ,ptgenwf         ,"ptgenwf[5]/D"          );
        outTree_->Branch("etagenwf"           ,etagenwf         ,"etagenwf[5]/D"          );
        outTree_->Branch("phigenwf"           ,phigenwf       ,"phigenwf[5]/D"          );
        outTree_->Branch("massgenwf"           ,massgenwf         ,"massgenwf[5]/D"          );
        outTree_->Branch("ptgenzf"           ,ptgenzf         ,"ptgenzf[5]/D"          );
        outTree_->Branch("etagenzf"           ,etagenzf         ,"etagenzf[5]/D"          );
        outTree_->Branch("phigenzf"           ,phigenzf       ,"phigenzf[5]/D"          );
        outTree_->Branch("massgenzf"           ,massgenzf         ,"massgenzf[5]/D"          );
        outTree_->Branch("ptgengf"           ,ptgengf         ,"ptgengf[10]/D"          );
        outTree_->Branch("etagengf"           ,etagengf         ,"etagengf[10]/D"          );
        outTree_->Branch("phigengf"           ,phigengf       ,"phigengf[10]/D"          );
        outTree_->Branch("egengf"           ,egengf         ,"egengf[10]/D"          );
        
        outTree_->Branch("gent_b_pt"           ,&gent_b_pt         ,"gent_b_pt/D"          );
        outTree_->Branch("gent_b_eta"           ,&gent_b_eta         ,"gent_b_eta/D"          );
        outTree_->Branch("gent_b_phi"           ,&gent_b_phi         ,"gent_b_phi/D"          );
        outTree_->Branch("gent_b_mass"           ,&gent_b_mass         ,"gent_b_mass/D"          );
        outTree_->Branch("genantit_b_pt"           ,&genantit_b_pt         ,"genantit_b_pt/D"          );
        outTree_->Branch("genantit_b_eta"           ,&genantit_b_eta         ,"genantit_b_eta/D"          );
        outTree_->Branch("genantit_b_phi"           ,&genantit_b_phi         ,"genantit_b_phi/D"          );
        outTree_->Branch("genantit_b_mass"           ,&genantit_b_mass         ,"genantit_b_mass/D"          );
        outTree_->Branch("gent_w_pt"           ,&gent_w_pt         ,"gent_w_pt/D"          );
        outTree_->Branch("gent_w_eta"           ,&gent_w_eta         ,"gent_w_eta/D"          );
        outTree_->Branch("gent_w_phi"           ,&gent_w_phi         ,"gent_w_phi/D"          );
        outTree_->Branch("gent_w_mass"           ,&gent_w_mass         ,"gent_w_mass/D"          );
        outTree_->Branch("genantit_w_pt"           ,&genantit_w_pt         ,"genantit_w_pt/D"          );
        outTree_->Branch("genantit_w_eta"           ,&genantit_w_eta         ,"genantit_w_eta/D"          );
        outTree_->Branch("genantit_w_phi"           ,&genantit_w_phi         ,"genantit_w_phi/D"          );
        outTree_->Branch("genantit_w_mass"           ,&genantit_w_mass         ,"genantit_w_mass/D"          );
        outTree_->Branch("gent_w_tag"           ,&gent_w_tag         ,"gent_w_tag/D"          );
        outTree_->Branch("gent_w_q1_pt"           ,&gent_w_q1_pt         ,"gent_w_q1_pt/D"          );
        outTree_->Branch("gent_w_q1_eta"           ,&gent_w_q1_eta         ,"gent_w_q1_eta/D"          );
        outTree_->Branch("gent_w_q1_phi"           ,&gent_w_q1_phi         ,"gent_w_q1_phi/D"          );
        outTree_->Branch("gent_w_q1_e"           ,&gent_w_q1_e         ,"gent_w_q1_e/D"          );
        outTree_->Branch("gent_w_q1_pdg"           ,&gent_w_q1_pdg         ,"gent_w_q1_pdg/D"          );
        outTree_->Branch("gent_w_q2_pt"           ,&gent_w_q2_pt         ,"gent_w_q2_pt/D"          );
        outTree_->Branch("gent_w_q2_eta"           ,&gent_w_q2_eta         ,"gent_w_q2_eta/D"          );
        outTree_->Branch("gent_w_q2_phi"           ,&gent_w_q2_phi         ,"gent_w_q2_phi/D"          );
        outTree_->Branch("gent_w_q2_e"           ,&gent_w_q2_e         ,"gent_w_q2_e/D"          );
        outTree_->Branch("gent_w_q2_pdg"           ,&gent_w_q2_pdg         ,"gent_w_q2_pdg/D"          );
        outTree_->Branch("genantit_w_tag"           ,&genantit_w_tag         ,"genantit_w_tag/D"          );
        outTree_->Branch("genantit_w_q1_pt"           ,&genantit_w_q1_pt         ,"genantit_w_q1_pt/D"          );
        outTree_->Branch("genantit_w_q1_eta"           ,&genantit_w_q1_eta         ,"genantit_w_q1_eta/D"          );
        outTree_->Branch("genantit_w_q1_phi"           ,&genantit_w_q1_phi         ,"genantit_w_q1_phi/D"          );
        outTree_->Branch("genantit_w_q1_e"           ,&genantit_w_q1_e         ,"genantit_w_q1_e/D"          );
        outTree_->Branch("genantit_w_q1_pdg"           ,&genantit_w_q1_pdg         ,"genantit_w_q1_pdg/D"          );
        outTree_->Branch("genantit_w_q2_pt"           ,&genantit_w_q2_pt         ,"genantit_w_q2_pt/D"          );
        outTree_->Branch("genantit_w_q2_eta"           ,&genantit_w_q2_eta         ,"genantit_w_q2_eta/D"          );
        outTree_->Branch("genantit_w_q2_phi"           ,&genantit_w_q2_phi         ,"genantit_w_q2_phi/D"          );
        outTree_->Branch("genantit_w_q2_e"           ,&genantit_w_q2_e         ,"gent_w_q2_e/D"          );
        outTree_->Branch("genantit_w_q2_pdg"           ,&genantit_w_q2_pdg         ,"genantit_w_q2_pdg/D"          );

        outTree_->Branch("ptgenq1l"           ,ptgenq1l         ,"ptgenq1l[5]/D"          );
        outTree_->Branch("etagenq1l"           ,etagenq1l         ,"etagenq1l[5]/D"          );
        outTree_->Branch("phigenq1l"           ,phigenq1l       ,"phigenq1l[5]/D"          );
        outTree_->Branch("egenq1l"           ,egenq1l         ,"egenq1l[5]/D"          );
        outTree_->Branch("ptgenq1f"           ,ptgenq1f         ,"ptgenq1f[5]/D"          );
        outTree_->Branch("etagenq1f"           ,etagenq1f         ,"etagenq1f[5]/D"          );
        outTree_->Branch("phigenq1f"           ,phigenq1f       ,"phigenq1f[5]/D"          );
        outTree_->Branch("egenq1f"           ,egenq1f         ,"egenq1f[5]/D"          );
        outTree_->Branch("ptgenq2l"           ,ptgenq2l         ,"ptgenq2l[5]/D"          );
        outTree_->Branch("etagenq2l"           ,etagenq2l         ,"etagenq2l[5]/D"          );
        outTree_->Branch("phigenq2l"           ,phigenq2l       ,"phigenq2l[5]/D"          );
        outTree_->Branch("egenq2l"           ,egenq2l         ,"egenq2l[5]/D"          );
        outTree_->Branch("ptgenq2f"           ,ptgenq2f         ,"ptgenq2f[5]/D"          );
        outTree_->Branch("etagenq2f"           ,etagenq2f         ,"etagenq2f[5]/D"          );
        outTree_->Branch("phigenq2f"           ,phigenq2f       ,"phigenq2f[5]/D"          );
        outTree_->Branch("egenq2f"           ,egenq2f         ,"egenq2f[5]/D"          );
        outTree_->Branch("ptgenq3l"           ,ptgenq3l         ,"ptgenq3l[5]/D"          );
        outTree_->Branch("etagenq3l"           ,etagenq3l         ,"etagenq3l[5]/D"          );
        outTree_->Branch("phigenq3l"           ,phigenq3l       ,"phigenq3l[5]/D"          );
        outTree_->Branch("egenq3l"           ,egenq3l         ,"egenq3l[5]/D"          );
        outTree_->Branch("ptgenq3f"           ,ptgenq3f         ,"ptgenq3f[5]/D"          );
        outTree_->Branch("etagenq3f"           ,etagenq3f         ,"etagenq3f[5]/D"          );
        outTree_->Branch("phigenq3f"           ,phigenq3f       ,"phigenq3f[5]/D"          );
        outTree_->Branch("egenq3f"           ,egenq3f         ,"egenq3f[5]/D"          );
        outTree_->Branch("ptgenq4l"           ,ptgenq4l         ,"ptgenq4l[5]/D"          );
        outTree_->Branch("etagenq4l"           ,etagenq4l         ,"etagenq4l[5]/D"          );
        outTree_->Branch("phigenq4l"           ,phigenq4l       ,"phigenq4l[5]/D"          );
        outTree_->Branch("egenq4l"           ,egenq4l         ,"egenq4l[5]/D"          );
        outTree_->Branch("ptgenq4f"           ,ptgenq4f         ,"ptgenq4f[5]/D"          );
        outTree_->Branch("etagenq4f"           ,etagenq4f         ,"etagenq4f[5]/D"          );
        outTree_->Branch("phigenq4f"           ,phigenq4f       ,"phigenq4f[5]/D"          );
        outTree_->Branch("egenq4f"           ,egenq4f         ,"egenq4f[5]/D"          );
        outTree_->Branch("ptgenq5l"           ,ptgenq5l         ,"ptgenq5l[5]/D"          );
        outTree_->Branch("etagenq5l"           ,etagenq5l         ,"etagenq5l[5]/D"          );
        outTree_->Branch("phigenq5l"           ,phigenq5l       ,"phigenq5l[5]/D"          );
        outTree_->Branch("egenq5l"           ,egenq5l         ,"egenq5l[5]/D"          );
        outTree_->Branch("ptgenq5f"           ,ptgenq5f         ,"ptgenq5f[5]/D"          );
        outTree_->Branch("etagenq5f"           ,etagenq5f         ,"etagenq5f[5]/D"          );
        outTree_->Branch("phigenq5f"           ,phigenq5f       ,"phigenq5f[5]/D"          );
        outTree_->Branch("egenq5f"           ,egenq5f         ,"egenq5f[5]/D"          );
        outTree_->Branch("mothergenq1f"           ,mothergenq1f         ,"mothergenq1f[5]/D"          );
        outTree_->Branch("mothergenq2f"           ,mothergenq2f         ,"mothergenq2f[5]/D"          );
        outTree_->Branch("mothergenq3f"           ,mothergenq3f         ,"mothergenq3f[5]/D"          );
        outTree_->Branch("mothergenq4f"           ,mothergenq4f         ,"mothergenq4f[5]/D"          );
        outTree_->Branch("mothergenq5f"           ,mothergenq5f         ,"mothergenq5f[5]/D"          );
        
        outTree_->Branch("mothergengf"           ,mothergengf         ,"mothergengf[10]/D"          );
        outTree_->Branch("mmothergengf"           ,mmothergengf         ,"mmothergengf[10]/D"          );

        outTree_->Branch("mmothergenq1f"           ,mmothergenq1f         ,"mmothergenq1f[5]/D"          );
        outTree_->Branch("mmothergenq2f"           ,mmothergenq2f         ,"mmothergenq2f[5]/D"          );
        outTree_->Branch("mmothergenq3f"           ,mmothergenq3f         ,"mmothergenq3f[5]/D"          );
        outTree_->Branch("mmothergenq4f"           ,mmothergenq4f         ,"mmothergenq4f[5]/D"          );
        outTree_->Branch("mmothergenq5f"           ,mmothergenq5f         ,"mmothergenq5f[5]/D"          );

    }
    outTree_->Branch("run"             ,&run            ,"run/I");//
    outTree_->Branch("ls"              ,&ls             ,"ls/I"             );//Synch
    outTree_->Branch("nLooseEle"       ,&nLooseEle      ,"nLooseEle/I");//
    outTree_->Branch("nLooseMu"        ,&nLooseMu       ,"nLooseMu/I");//
    outTree_->Branch("event"           ,&nevent         ,"event/I"          );
    outTree_->Branch("nVtx"            ,&nVtx           ,"nVtx/I"           );
    outTree_->Branch("numCands"        ,&numCands       ,"numCands/I"       );
    outTree_->Branch("ptVlep"          ,&ptVlep         ,"ptVlep/D"         );

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
    // DeepAK8
    outTree_->Branch("jetAK8puppi_dnnTop"         ,&jetAK8puppi_dnnTop       ,"jetAK8puppi_dnnTop/D"         );
    outTree_->Branch("jetAK8puppi_dnnW"           ,&jetAK8puppi_dnnW         ,"jetAK8puppi_dnnW/D"           );
    outTree_->Branch("jetAK8puppi_dnnH4q"         ,&jetAK8puppi_dnnH4q       ,"jetAK8puppi_dnnH4q/D"         );
    outTree_->Branch("jetAK8puppi_dnnTop_2"         ,&jetAK8puppi_dnnTop_2       ,"jetAK8puppi_dnnTop_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnW_2"           ,&jetAK8puppi_dnnW_2         ,"jetAK8puppi_dnnW_2/D"           );
    outTree_->Branch("jetAK8puppi_dnnH4q_2"         ,&jetAK8puppi_dnnH4q_2       ,"jetAK8puppi_dnnH4q_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnTop_3"         ,&jetAK8puppi_dnnTop_3       ,"jetAK8puppi_dnnTop_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnW_3"           ,&jetAK8puppi_dnnW_3         ,"jetAK8puppi_dnnW_3/D"           );
    outTree_->Branch("jetAK8puppi_dnnH4q_3"         ,&jetAK8puppi_dnnH4q_3       ,"jetAK8puppi_dnnH4q_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnZ"         ,&jetAK8puppi_dnnZ       ,"jetAK8puppi_dnnZ/D"         );
    outTree_->Branch("jetAK8puppi_dnnZbb"         ,&jetAK8puppi_dnnZbb       ,"jetAK8puppi_dnnZbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnHbb"         ,&jetAK8puppi_dnnHbb       ,"jetAK8puppi_dnnHbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnZ_2"         ,&jetAK8puppi_dnnZ_2       ,"jetAK8puppi_dnnZ_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnZbb_2"         ,&jetAK8puppi_dnnZbb_2       ,"jetAK8puppi_dnnZbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnHbb_2"         ,&jetAK8puppi_dnnHbb_2       ,"jetAK8puppi_dnnHbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnZ_3"         ,&jetAK8puppi_dnnZ_3       ,"jetAK8puppi_dnnZ_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnZbb_3"         ,&jetAK8puppi_dnnZbb_3       ,"jetAK8puppi_dnnZbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnHbb_3"         ,&jetAK8puppi_dnnHbb_3       ,"jetAK8puppi_dnnHbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnqcd"         ,&jetAK8puppi_dnnqcd       ,"jetAK8puppi_dnnqcd/D"         );
    outTree_->Branch("jetAK8puppi_dnntop"         ,&jetAK8puppi_dnntop       ,"jetAK8puppi_dnntop/D"         );
    outTree_->Branch("jetAK8puppi_dnnw"         ,&jetAK8puppi_dnnw       ,"jetAK8puppi_dnnw/D"         );
    outTree_->Branch("jetAK8puppi_dnnz"         ,&jetAK8puppi_dnnz       ,"jetAK8puppi_dnnz/D"         );
    outTree_->Branch("jetAK8puppi_dnnzbb"         ,&jetAK8puppi_dnnzbb       ,"jetAK8puppi_dnnzbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnhbb"         ,&jetAK8puppi_dnnhbb       ,"jetAK8puppi_dnnhbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnh4q"         ,&jetAK8puppi_dnnh4q       ,"jetAK8puppi_dnnh4q/D"         );
    outTree_->Branch("jetAK8puppi_dnnqcd_2"         ,&jetAK8puppi_dnnqcd_2       ,"jetAK8puppi_dnnqcd_2/D"         );
    outTree_->Branch("jetAK8puppi_dnntop_2"         ,&jetAK8puppi_dnntop_2       ,"jetAK8puppi_dnntop_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnw_2"         ,&jetAK8puppi_dnnw_2       ,"jetAK8puppi_dnnw_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnz_2"         ,&jetAK8puppi_dnnz_2       ,"jetAK8puppi_dnnz_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnzbb_2"         ,&jetAK8puppi_dnnzbb_2       ,"jetAK8puppi_dnnzbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnhbb_2"         ,&jetAK8puppi_dnnhbb_2       ,"jetAK8puppi_dnnhbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnh4q_2"         ,&jetAK8puppi_dnnh4q_2       ,"jetAK8puppi_dnnh4q_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnqcd_3"         ,&jetAK8puppi_dnnqcd_3       ,"jetAK8puppi_dnnqcd_3/D"         );
    outTree_->Branch("jetAK8puppi_dnntop_3"         ,&jetAK8puppi_dnntop_3       ,"jetAK8puppi_dnntop_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnw_3"         ,&jetAK8puppi_dnnw_3       ,"jetAK8puppi_dnnw_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnz_3"         ,&jetAK8puppi_dnnz_3       ,"jetAK8puppi_dnnz_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnzbb_3"         ,&jetAK8puppi_dnnzbb_3       ,"jetAK8puppi_dnnzbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnhbb_3"         ,&jetAK8puppi_dnnhbb_3       ,"jetAK8puppi_dnnhbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnh4q_3"         ,&jetAK8puppi_dnnh4q_3       ,"jetAK8puppi_dnnh4q_3/D"         );

    //Decorrelated DeepAK8
    outTree_->Branch("jetAK8puppi_dnnDecorrTop"         ,&jetAK8puppi_dnnDecorrTop       ,"jetAK8puppi_dnnDecorrTop/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrW"           ,&jetAK8puppi_dnnDecorrW         ,"jetAK8puppi_dnnDecorrW/D"           );
    outTree_->Branch("jetAK8puppi_dnnDecorrH4q"         ,&jetAK8puppi_dnnDecorrH4q       ,"jetAK8puppi_dnnDecorrH4q/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrTop_2"         ,&jetAK8puppi_dnnDecorrTop_2       ,"jetAK8puppi_dnnDecorrTop_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrW_2"           ,&jetAK8puppi_dnnDecorrW_2         ,"jetAK8puppi_dnnDecorrW_2/D"           );
    outTree_->Branch("jetAK8puppi_dnnDecorrH4q_2"         ,&jetAK8puppi_dnnDecorrH4q_2       ,"jetAK8puppi_dnnDecorrH4q_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrTop_3"         ,&jetAK8puppi_dnnDecorrTop_3       ,"jetAK8puppi_dnnDecorrTop_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrW_3"           ,&jetAK8puppi_dnnDecorrW_3         ,"jetAK8puppi_dnnDecorrW_3/D"           );
    outTree_->Branch("jetAK8puppi_dnnDecorrH4q_3"         ,&jetAK8puppi_dnnDecorrH4q_3       ,"jetAK8puppi_dnnDecorrH4q_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrZ"         ,&jetAK8puppi_dnnDecorrZ       ,"jetAK8puppi_dnnDecorrZ/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrZbb"         ,&jetAK8puppi_dnnDecorrZbb       ,"jetAK8puppi_dnnDecorrZbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrHbb"         ,&jetAK8puppi_dnnDecorrHbb       ,"jetAK8puppi_dnnDecorrHbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrZ_2"         ,&jetAK8puppi_dnnDecorrZ_2       ,"jetAK8puppi_dnnDecorrZ_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrZbb_2"         ,&jetAK8puppi_dnnDecorrZbb_2       ,"jetAK8puppi_dnnDecorrZbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrHbb_2"         ,&jetAK8puppi_dnnDecorrHbb_2       ,"jetAK8puppi_dnnDecorrHbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrZ_3"         ,&jetAK8puppi_dnnDecorrZ_3       ,"jetAK8puppi_dnnDecorrZ_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrZbb_3"         ,&jetAK8puppi_dnnDecorrZbb_3       ,"jetAK8puppi_dnnDecorrZbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrHbb_3"         ,&jetAK8puppi_dnnDecorrHbb_3       ,"jetAK8puppi_dnnDecorrHbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrbb"         ,&jetAK8puppi_dnnDecorrbb       ,"jetAK8puppi_dnnDecorrbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrcc"         ,&jetAK8puppi_dnnDecorrcc       ,"jetAK8puppi_dnnDecorrcc/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrbbnog"         ,&jetAK8puppi_dnnDecorrbbnog       ,"jetAK8puppi_dnnDecorrbbnog/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrccnog"         ,&jetAK8puppi_dnnDecorrccnog       ,"jetAK8puppi_dnnDecorrccnog/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrbb_2"         ,&jetAK8puppi_dnnDecorrbb_2       ,"jetAK8puppi_dnnDecorrbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrcc_2"         ,&jetAK8puppi_dnnDecorrcc_2       ,"jetAK8puppi_dnnDecorrcc_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrbbnog_2"         ,&jetAK8puppi_dnnDecorrbbnog_2       ,"jetAK8puppi_dnnDecorrbbnog_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrccnog_2"         ,&jetAK8puppi_dnnDecorrccnog_2       ,"jetAK8puppi_dnnDecorrccnog_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrbb_3"         ,&jetAK8puppi_dnnDecorrbb_3       ,"jetAK8puppi_dnnDecorrbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrcc_3"         ,&jetAK8puppi_dnnDecorrcc_3       ,"jetAK8puppi_dnnDecorrcc_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrbbnog_3"         ,&jetAK8puppi_dnnDecorrbbnog_3       ,"jetAK8puppi_dnnDecorrbbnog_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrccnog_3"         ,&jetAK8puppi_dnnDecorrccnog_3       ,"jetAK8puppi_dnnDecorrccnog_3/D"         );

    outTree_->Branch("jetAK8puppi_dnnDecorrqcd"         ,&jetAK8puppi_dnnDecorrqcd       ,"jetAK8puppi_dnnDecorrqcd/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrtop"         ,&jetAK8puppi_dnnDecorrtop       ,"jetAK8puppi_dnnDecorrtop/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrw"         ,&jetAK8puppi_dnnDecorrw       ,"jetAK8puppi_dnnDecorrw/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrz"         ,&jetAK8puppi_dnnDecorrz       ,"jetAK8puppi_dnnDecorrz/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrzbb"         ,&jetAK8puppi_dnnDecorrzbb       ,"jetAK8puppi_dnnDecorrzbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrhbb"         ,&jetAK8puppi_dnnDecorrhbb       ,"jetAK8puppi_dnnDecorrhbb/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrh4q"         ,&jetAK8puppi_dnnDecorrh4q       ,"jetAK8puppi_dnnDecorrh4q/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrqcd_2"         ,&jetAK8puppi_dnnDecorrqcd_2       ,"jetAK8puppi_dnnDecorrqcd_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrtop_2"         ,&jetAK8puppi_dnnDecorrtop_2       ,"jetAK8puppi_dnnDecorrtop_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrw_2"         ,&jetAK8puppi_dnnDecorrw_2       ,"jetAK8puppi_dnnDecorrw_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrz_2"         ,&jetAK8puppi_dnnDecorrz_2       ,"jetAK8puppi_dnnDecorrz_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrzbb_2"         ,&jetAK8puppi_dnnDecorrzbb_2       ,"jetAK8puppi_dnnDecorrzbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrhbb_2"         ,&jetAK8puppi_dnnDecorrhbb_2       ,"jetAK8puppi_dnnDecorrhbb_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrh4q_2"         ,&jetAK8puppi_dnnDecorrh4q_2       ,"jetAK8puppi_dnnDecorrh4q_2/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrqcd_3"         ,&jetAK8puppi_dnnDecorrqcd_3       ,"jetAK8puppi_dnnDecorrqcd_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrtop_3"         ,&jetAK8puppi_dnnDecorrtop_3       ,"jetAK8puppi_dnnDecorrtop_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrw_3"         ,&jetAK8puppi_dnnDecorrw_3       ,"jetAK8puppi_dnnDecorrw_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrz_3"         ,&jetAK8puppi_dnnDecorrz_3       ,"jetAK8puppi_dnnDecorrz_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrzbb_3"         ,&jetAK8puppi_dnnDecorrzbb_3       ,"jetAK8puppi_dnnDecorrzbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrhbb_3"         ,&jetAK8puppi_dnnDecorrhbb_3       ,"jetAK8puppi_dnnDecorrhbb_3/D"         );
    outTree_->Branch("jetAK8puppi_dnnDecorrh4q_3"         ,&jetAK8puppi_dnnDecorrh4q_3       ,"jetAK8puppi_dnnDecorrh4q_3/D"         );

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

    outTree_->Branch("jetAK8puppi_ptJEC_3"          ,&jetAK8puppi_ptJEC_3         ,"jetAK8puppi_ptJEC_3/D"         );
    outTree_->Branch("jetAK8puppi_eta_3"          ,&jetAK8puppi_eta_3         ,"jetAK8puppi_eta_3/D"         );
    outTree_->Branch("jetAK8puppi_phi_3"          ,&jetAK8puppi_phi_3         ,"jetAK8puppi_phi_3/D"         );
    outTree_->Branch("jetAK8puppi_tau1_3"          ,&jetAK8puppi_tau1_3         ,"jetAK8puppi_tau1_3/D"         );
    outTree_->Branch("jetAK8puppi_tau2_3"          ,&jetAK8puppi_tau2_3         ,"jetAK8puppi_tau2_3/D"         );
    outTree_->Branch("jetAK8puppi_tau3_3"          ,&jetAK8puppi_tau3_3         ,"jetAK8puppi_tau3_3/D"         );
    outTree_->Branch("jetAK8puppi_tau21_3"          ,&jetAK8puppi_tau21_3         ,"jetAK8puppi_tau21_3/D"         );
    outTree_->Branch("jetAK8puppi_tau4_3"          ,&jetAK8puppi_tau4_3         ,"jetAK8puppi_tau4_3/D"         );
    outTree_->Branch("jetAK8puppi_tau42_3"          ,&jetAK8puppi_tau42_3         ,"jetAK8puppi_tau42_3/D"         );
    outTree_->Branch("jetAK8puppi_sd_3"          ,&jetAK8puppi_sd_3         ,"jetAK8puppi_sd_3/D"         );
    outTree_->Branch("jetAK8puppi_sdJEC_3"          ,&jetAK8puppi_sdJEC_3         ,"jetAK8puppi_sdJEC_3/D"         );
    outTree_->Branch("jetAK8puppi_sdcorr_3"          ,&jetAK8puppi_sdcorr_3         ,"jetAK8puppi_sdcorr_3/D"         );

    outTree_->Branch("vbfeta"    ,&vbfeta   ,"vbfeta/D"   );
    outTree_->Branch("vbfmjj"    ,&vbfmjj   ,"vbfmjj/D"   );

    outTree_->Branch("vbftag"    ,&vbftag   ,"vbftag/I"   );
    outTree_->Branch("nj1"    ,&nj1   ,"nj1/I"   );
    outTree_->Branch("nj2"    ,&nj2   ,"nj2/I"   );
    outTree_->Branch("ak8sj11"    ,&ak8sj11 ) ;
    outTree_->Branch("ak8sj12"    ,&ak8sj12 ) ;
    outTree_->Branch("ak8sj13"    ,&ak8sj13 ) ;
    outTree_->Branch("ak8sj14"    ,&ak8sj14 ) ;
    outTree_->Branch("ak8sj15"    ,&ak8sj15  );
    outTree_->Branch("ak8sj21"    ,&ak8sj21 ) ;
    outTree_->Branch("ak8sj22"    ,&ak8sj22 ) ;
    outTree_->Branch("ak8sj23"    ,&ak8sj23 ) ;
    outTree_->Branch("ak8sj24"    ,&ak8sj24 ) ;
    outTree_->Branch("ak8sj25"    ,&ak8sj25  );
    outTree_->Branch("puppi_softdropj1"    ,&puppi_softdropj1 ) ;
    outTree_->Branch("puppi_softdropj2"    ,&puppi_softdropj2 ) ;

    outTree_->Branch("yVlep"           ,&yVlep          ,"yVlep/D"          );
    outTree_->Branch("phiVlep"         ,&phiVlep        ,"phiVlep/D"        );
    outTree_->Branch("massVlep"        ,&massVlep       ,"massVlep/D"       );
    outTree_->Branch("mtVlep"          ,&mtVlep         ,"mtVlep/D"         );
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
    outTreew_->Branch("theWeight", &theWeight, "theWeight/D");
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
    outTree_->Branch("IDLoose_3", &IDLoose_3, "IDLoose_3/O");
    outTree_->Branch("IDTight_3", &IDTight_3, "IDTight_3/O");
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

    outTree_->Branch("jetAK8puppi_pt1",&jetAK8puppi_pt1,"jetAK8puppi_pt1[4]/D");
    outTree_->Branch("jetAK8puppi_eta1",&jetAK8puppi_eta1,"jetAK8puppi_eta1[4]/D");
    outTree_->Branch("jetAK8puppi_mass1",&jetAK8puppi_mass1,"jetAK8puppi_mass1[4]/D");
    outTree_->Branch("candMasspuppiJEC",&candMasspuppiJEC,"candMasspuppiJEC/D");
    outTree_->Branch("m_jlv",&m_jlv,"m_jlv/D");

    outTree_->Branch("massww",&massww,"massww[3]/D");
    outTree_->Branch("masslvj1",&masslvj1,"masslvj1/D");
    outTree_->Branch("masslvj2",&masslvj2,"masslvj2/D");
    outTree_->Branch("massj1j2",&massj1j2,"massj1j2/D");

    outTree_->Branch("ptVlepJEC",&ptVlepJEC,"ptVlepJEC/D");
    outTree_->Branch("yVlepJEC",&yVlepJEC,"yVlepJEC/D");
    outTree_->Branch("phiVlepJEC",&phiVlepJEC,"phiVlepJEC/D");
    outTree_->Branch("massVlepJEC",&massVlepJEC,"massVlepJEC/D");
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
    outTree_->Branch("ak4jet_deepcsvudsg"        , ak4jet_deepcsvudsg       ,"ak4jet_deepcsvudsg[8]/D"       );
    outTree_->Branch("ak4jet_deepcsvb"        , ak4jet_deepcsvb       ,"ak4jet_deepcsvb[8]/D"       );
    outTree_->Branch("ak4jet_deepcsvc"        , ak4jet_deepcsvc       ,"ak4jet_deepcsvc[8]/D"       );
    outTree_->Branch("ak4jet_deepcsvbb"        , ak4jet_deepcsvbb       ,"ak4jet_deepcsvbb[8]/D"       );
    outTree_->Branch("ak4jet_deepcsvcc"        , ak4jet_deepcsvcc       ,"ak4jet_deepcsvcc[8]/D"       );
    outTree_->Branch("ak4jet_IDLoose"        , ak4jet_IDLoose       ,"ak4jet_IDLoose[8]/D"       );
    outTree_->Branch("ak4jet_IDTight"        , ak4jet_IDTight       ,"ak4jet_IDTight[8]/D"       );
    
    /// Gen Level quantities
    if(RunOnMC_){
    outTree_->Branch("pttau"        , pttau       ,"pttau[4]/D"       );
    outTree_->Branch("etatau"        , etatau       ,"etatau[4]/D"       );
    outTree_->Branch("phitau"        , phitau       ,"phitau[4]/D"       );
    outTree_->Branch("etau"        , etau       ,"etau[4]/D"       );
    outTree_->Branch("pdgidtau"        , pdgidtau       ,"pdgidtau[4]/D"       );
    
    outTree_->Branch("pttau_2"        , pttau_2       ,"pttau_2[4]/D"       );
    outTree_->Branch("etatau_2"        , etatau_2       ,"etatau_2[4]/D"       );
    outTree_->Branch("phitau_2"        , phitau_2       ,"phitau_2[4]/D"       );
    outTree_->Branch("etau_2"        , etau_2       ,"etau_2[4]/D"       );
    outTree_->Branch("pdgidtau_2"        , pdgidtau_2       ,"pdgidtau_2[4]/D"       );
    
    outTree_->Branch("pttau_3"        , pttau_3       ,"pttau_3[4]/D"       );
    outTree_->Branch("etatau_3"        , etatau_3       ,"etatau_3[4]/D"       );
    outTree_->Branch("phitau_3"        , phitau_3       ,"phitau_3[4]/D"       );
    outTree_->Branch("etau_3"        , etau_3       ,"etau_3[4]/D"       );
    outTree_->Branch("pdgidtau_3"        , pdgidtau_3       ,"pdgidtau_3[4]/D"       );
    
    outTree_->Branch("ptq"        , ptq       ,"ptq[3]/D"       );
    outTree_->Branch("etaq"        , etaq       ,"etaq[3]/D"       );
    outTree_->Branch("phiq"        , phiq       ,"phiq[3]/D"       );
    outTree_->Branch("eq"        , eq       ,"eq[3]/D"       );
    outTree_->Branch("pdgidq"        , pdgidq       ,"pdgidq[3]/D"       );
    
    outTree_->Branch("ptq_2"        , ptq_2       ,"ptq_2[3]/D"       );
    outTree_->Branch("etaq_2"        , etaq_2       ,"etaq_2[3]/D"       );
    outTree_->Branch("phiq_2"        , phiq_2       ,"phiq_2[3]/D"       );
    outTree_->Branch("eq_2"        , eq_2       ,"eq_2[3]/D"       );
    outTree_->Branch("pdgidq_2"        , pdgidq_2       ,"pdgidq_2[3]/D"       );
    
    outTree_->Branch("ptq_3"        , ptq_3       ,"ptq_3[3]/D"       );
    outTree_->Branch("etaq_3"        , etaq_3       ,"etaq_3[3]/D"       );
    outTree_->Branch("phiq_3"        , phiq_3       ,"phiq_3[3]/D"       );
    outTree_->Branch("eq_3"        , eq_3       ,"eq_3[4]/D"       );
    outTree_->Branch("pdgidq_3"        , pdgidq_3       ,"pdgidq_3[3]/D"       );
    
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

    outTree_->Branch("gen_nele_pt"        ,&gen_nele_pt       ,"gen_nele_pt/D"       );
    outTree_->Branch("gen_nele_eta"        ,&gen_nele_eta       ,"gen_nele_eta/D"       );
    outTree_->Branch("gen_nele_phi"        ,&gen_nele_phi       ,"gen_nele_phi/D"       );
    outTree_->Branch("gen_nele_e"        ,&gen_nele_e       ,"gen_nele_e/D"       );
    outTree_->Branch("gen_nmu_pt"        ,&gen_nmu_pt       ,"gen_nmu_pt/D"       );
    outTree_->Branch("gen_nmu_eta"        ,&gen_nmu_eta       ,"gen_nmu_eta/D"       );
    outTree_->Branch("gen_nmu_phi"        ,&gen_nmu_phi       ,"gen_nmu_phi/D"       );
    outTree_->Branch("gen_nmu_e"        ,&gen_nmu_e       ,"gen_nmu_e/D"       );
    outTree_->Branch("gen_nele_pt_2"        ,&gen_nele_pt_2       ,"gen_nele_pt_2/D"       );
    outTree_->Branch("gen_nele_eta_2"        ,&gen_nele_eta_2       ,"gen_nele_eta_2/D"       );
    outTree_->Branch("gen_nele_phi_2"        ,&gen_nele_phi_2       ,"gen_nele_phi_2/D"       );
    outTree_->Branch("gen_nele_e_2"        ,&gen_nele_e_2       ,"gen_nele_e_2/D"       );
    outTree_->Branch("gen_nmu_pt_2"        ,&gen_nmu_pt_2       ,"gen_nmu_pt_2/D"       );
    outTree_->Branch("gen_nmu_eta_2"        ,&gen_nmu_eta_2       ,"gen_nmu_eta_2/D"       );
    outTree_->Branch("gen_nmu_phi_2"        ,&gen_nmu_phi_2       ,"gen_nmu_phi_2/D"       );
    outTree_->Branch("gen_nmu_e_2"        ,&gen_nmu_e_2       ,"gen_nmu_e_2/D"       );
    outTree_->Branch("gen_nele_pt_3"        ,&gen_nele_pt_3       ,"gen_nele_pt_3/D"       );
    outTree_->Branch("gen_nele_eta_3"        ,&gen_nele_eta_3       ,"gen_nele_eta_3/D"       );
    outTree_->Branch("gen_nele_phi_3"        ,&gen_nele_phi_3       ,"gen_nele_phi_3/D"       );
    outTree_->Branch("gen_nele_e_3"        ,&gen_nele_e_3       ,"gen_nele_e_3/D"       );
    outTree_->Branch("gen_nmu_pt_3"        ,&gen_nmu_pt_3       ,"gen_nmu_pt_3/D"       );
    outTree_->Branch("gen_nmu_eta_3"        ,&gen_nmu_eta_3       ,"gen_nmu_eta_3/D"       );
    outTree_->Branch("gen_nmu_phi_3"        ,&gen_nmu_phi_3       ,"gen_nmu_phi_3/D"       );
    outTree_->Branch("gen_nmu_e_3"        ,&gen_nmu_e_3       ,"gen_nmu_e_3/D"       );

    outTree_->Branch("gen_tau_pt"        ,&gen_tau_pt       ,"gen_tau_pt/D"       );
    outTree_->Branch("gen_tau_eta"        ,&gen_tau_eta       ,"gen_tau_eta/D"       );
    outTree_->Branch("gen_tau_phi"        ,&gen_tau_phi       ,"gen_tau_phi/D"       );
    outTree_->Branch("gen_tau_e"        ,&gen_tau_e       ,"gen_tau_e/D"       );
    outTree_->Branch("gen_tau_pt_2"        ,&gen_tau_pt_2       ,"gen_tau_pt_2/D"       );
    outTree_->Branch("gen_tau_eta_2"        ,&gen_tau_eta_2       ,"gen_tau_eta_2/D"       );
    outTree_->Branch("gen_tau_phi_2"        ,&gen_tau_phi_2       ,"gen_tau_phi_2/D"       );
    outTree_->Branch("gen_tau_e_2"        ,&gen_tau_e_2       ,"gen_tau_e_2/D"       );
    outTree_->Branch("gen_tau_pt_3"        ,&gen_tau_pt_3       ,"gen_tau_pt_3/D"       );
    outTree_->Branch("gen_tau_eta_3"        ,&gen_tau_eta_3       ,"gen_tau_eta_3/D"       );
    outTree_->Branch("gen_tau_phi_3"        ,&gen_tau_phi_3       ,"gen_tau_phi_3/D"       );
    outTree_->Branch("gen_tau_e_3"        ,&gen_tau_e_3       ,"gen_tau_e_3/D"       );

    outTree_->Branch("gen_ntau_pt"        ,&gen_ntau_pt       ,"gen_ntau_pt/D"       );
    outTree_->Branch("gen_ntau_eta"        ,&gen_ntau_eta       ,"gen_ntau_eta/D"       );
    outTree_->Branch("gen_ntau_phi"        ,&gen_ntau_phi       ,"gen_ntau_phi/D"       );
    outTree_->Branch("gen_ntau_e"        ,&gen_ntau_e       ,"gen_ntau_e/D"       );
    outTree_->Branch("gen_ntau_pt_2"        ,&gen_ntau_pt_2       ,"gen_ntau_pt_2/D"       );
    outTree_->Branch("gen_ntau_eta_2"        ,&gen_ntau_eta_2       ,"gen_ntau_eta_2/D"       );
    outTree_->Branch("gen_ntau_phi_2"        ,&gen_ntau_phi_2       ,"gen_ntau_phi_2/D"       );
    outTree_->Branch("gen_ntau_e_2"        ,&gen_ntau_e_2       ,"gen_ntau_e_2/D"       );
    outTree_->Branch("gen_ntau_pt_3"        ,&gen_ntau_pt_3       ,"gen_ntau_pt_3/D"       );
    outTree_->Branch("gen_ntau_eta_3"        ,&gen_ntau_eta_3       ,"gen_ntau_eta_3/D"       );
    outTree_->Branch("gen_ntau_phi_3"        ,&gen_ntau_phi_3       ,"gen_ntau_phi_3/D"       );
    outTree_->Branch("gen_ntau_e_3"        ,&gen_ntau_e_3       ,"gen_ntau_e_3/D"       );
    
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
    }
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

const reco::Candidate*  EDBRTreeMaker::findLasttau(const reco::Candidate *particle,int IDpdg){
    int iw=0;
    int pidw=0;
    const reco::Candidate* pw=particle;
    //cout<<"check 1 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<endl;
    for(int ii=0;particle->daughter(ii)!=NULL;ii++){
        if(abs(particle->daughter(ii)->pdgId())== IDpdg) {
            iw=ii;
            pidw=abs(particle->daughter(ii)->pdgId());
            //cout<<"check 2 "<<iw<<"    "<<pidw<<"   "<<endl;
        }
    }
    if( abs(pidw) == IDpdg ){
        pw = particle->daughter(iw);
        //cout<<"check 5 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<endl;
        
        return (findLasttau(pw,IDpdg));
    }
    //cout<<"check 3 "<<pw->pdgId()<<"    "<<pw->status()<<"   "<<pw->daughter(0)->pdgId()<<"    "<<endl;
    return pw;
}


const reco::Candidate*  EDBRTreeMaker::findFirstW(const reco::Candidate *particle,int IDpdg){
    if (particle->mother(0)!=NULL){
        if(abs(particle->mother(0)->pdgId()) == IDpdg )
        return (findFirstW(particle->mother(0),IDpdg));
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
    return ele->superCluster().isNonnull() && ele->superCluster()->seed().isNonnull() ? ele->deltaEtaSuperClusterTrackAtVtx() - ele->superCluster()->eta() + ele->superCluster()->seed()->eta() : std::numeric_limits<float>::max();
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
    //edm::Handle<double> rho_;
    //event.getByLabel("fixedGridRhoFastjetAll",rho_);
    //edm::Handle<reco::VertexCollection> vertices_;
    //event.getByLabel("offlineSlimmedPrimaryVertices", vertices_);
    //event.getByToken(vtxToken_, vertices_);
    edm::Handle<reco::VertexCollection> vertices_;
    event.getByToken(vtxToken_, vertices_);

    //event.getByToken(muonToken_     , muons_   );
    edm::Handle<edm::View<pat::Muon>> muons_;
    //event.getByLabel("slimmedMuons",muons_);
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
            for ( std::vector<reco::CandidatePtr>::const_iterator cand = cands.begin();cand != cands.end(); ++cand ) {
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
    // DeepAK8
    fatjetNN_->readEvent(iEvent, iSetup);
   
    // Decorrelated DeepAK8
    decorrNN_->readEvent(iEvent, iSetup);
   
    using namespace edm;
    setDummyValues(); //Initalize variables with dummy values

    nevent = iEvent.eventAuxiliary().event();
    run    = iEvent.eventAuxiliary().run();
    ls     = iEvent.eventAuxiliary().luminosityBlock();

    //std::cout<< "num of run:" << run << "  lumi:" << ls << "  n event:" << nevent << std::endl;
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
    //iEvent.getByLabel(hadronicVSrc_.c_str(), hadronicVs);
    iEvent.getByToken(hadronicVSrc_, hadronicVs);
   
    edm::Handle<edm::View<pat::Jet> > hadronicVs_raw;
    //iEvent.getByLabel("slimmedJetsAK8", hadronicVs_raw);
    iEvent.getByToken(hadronicVSrc_raw_, hadronicVs_raw);

    edm::Handle<pat::JetCollection> hadronicVSoftDrop;
    iEvent.getByToken(hadronicVSoftDropSrc_, hadronicVSoftDrop);

    edm::Handle<edm::View<reco::Candidate> > leptonicVs;
    //iEvent.getByLabel(leptonicVSrc_.c_str(), leptonicVs);
    iEvent.getByToken(leptonicVSrc_, leptonicVs);

    edm::Handle<double> rho;
    //iEvent.getByLabel("fixedGridRhoFastjetAll",rho);

    iEvent.getByToken(rhoToken_      , rho     );
    double fastJetRho = *(rho.product());
    useless = fastJetRho;

    edm::Handle<edm::View<pat::Jet> > ak4jets;
    //iEvent.getByLabel(ak4jetsSrc_.c_str(), ak4jets);
    iEvent.getByToken(ak4jetsSrc_, ak4jets);
    
    edm::Handle<edm::View<reco::Candidate> > gravitons;
    //iEvent.getByLabel(gravitonSrc_.c_str(), gravitons);
    iEvent.getByToken(gravitonSrc_, gravitons);
    edm::Handle<edm::View<reco::Candidate> > metHandle;
    //iEvent.getByLabel(metSrc_.c_str(), metHandle);
    iEvent.getByToken(metSrc_, metHandle);
  
    edm::Handle<edm::View<pat::Muon>> loosemus;
    //iEvent.getByLabel(looseMuonSrc_.c_str(), loosemus);
    iEvent.getByToken(loosemuonToken_,loosemus);

    edm::Handle<edm::View<pat::Muon>> goodmus;
    //iEvent.getByLabel(goodMuSrc_.c_str(), goodmus);
    iEvent.getByToken(goodMuSrc_, goodmus);

    edm::Handle<edm::View<pat::Electron>> looseels;
    //iEvent.getByLabel(looseElectronsSrc_.c_str(), looseels);
    iEvent.getByToken(looseelectronToken_, looseels);

    edm::Handle<edm::View<reco::GenParticle> > genParticles;//define genParticle
    //iEvent.getByLabel(InputTag("prunedGenParticles"), genParticles);
    iEvent.getByToken(genSrc_, genParticles);

    edm::Handle<edm::View<pat::Muon>> mus;
    //iEvent.getByLabel("slimmedMuons",mus);
    iEvent.getByToken(MuSrc_, mus);
    edm::Handle<edm::View<pat::Electron>> eles;
    //iEvent.getByLabel("slimmedElectrons",eles);
    iEvent.getByToken(EleSrc_, eles);
    if (RunOnSig_||RunOnMC_){
        //  L1 prefiring
        edm::Handle< double > theprefweight;
        iEvent.getByToken(prefweight_token, theprefweight ) ;
        L1prefiring =(*theprefweight);
        
        edm::Handle< double > theprefweightup;
        iEvent.getByToken(prefweightup_token, theprefweightup ) ;
        L1prefiringup =(*theprefweightup);
        
        edm::Handle< double > theprefweightdown;
        iEvent.getByToken(prefweightdown_token, theprefweightdown ) ;
        L1prefiringdown =(*theprefweightdown);
        
        edm::Handle<LHEEventProduct> wgtsource;
        iEvent.getByToken(LheToken_, wgtsource);
        //std::cout<<"weight number "<<wgtsource->weights().size()<<std::endl;
        for ( int i=0; i<211; ++i) {
            pweight[i]= wgtsource->weights()[i].wgt/wgtsource->originalXWGTUP();
            //cout<<wgtsource->weights()[i].id<<"    "<<pweight[i]<<endl;
        }
        if (RunOnSig_)
            pweight[211]= wgtsource->weights()[777].wgt/wgtsource->originalXWGTUP();
        if (RunOnMC_)
            pweight[211]= wgtsource->weights()[212].wgt/wgtsource->originalXWGTUP();
	/*
        for ( int i=9; i<110; ++i) {
            pweight[i]= wgtsource->weights()[i+101].wgt/wgtsource->originalXWGTUP();
            //cout<<wgtsource->weights()[i].id<<"    "<<pweight[i]<<endl;
        }*/

        /*
        edm::Handle<LHERunInfoProduct> run;
        typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
        
        iEvent.getByToken(LhestrToken_,run);
        LHERunInfoProduct myLHERunInfoProduct = *(run.product());
        
        for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
            std::cout << iter->tag() << std::endl;
            std::vector<std::string> lines = iter->lines();
            for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
                std::cout << lines.at(iLine);
            }
        }
         */
        //iEvent.getByLabel("externalLHEProducer", wgtsource);
        //iEvent.getByLabel("source", wgtsource);

        edm::Handle<GenEventInfoProduct> genEvtInfo;
        //iEvent.getByLabel( "generator", genEvtInfo );
        iEvent.getByToken(GenToken_,genEvtInfo);

        //const std::vector<double>& evtWeights = genEvtInfo->weights();
        theWeight = genEvtInfo->weight();
        if(theWeight>0) nump = nump+1;
        if(theWeight<0) numm = numm+1;
//cout<<theWeight<<endl;
        edm::Handle<std::vector<PileupSummaryInfo>>  PupInfo;
        //iEvent.getByLabel(edm::InputTag("slimmedAddPileupInfo"), PupInfo);
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
    //cout << "npT" << npT << " nBX" << nBX << endl;

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

    edm::Handle<bool> badMuonResultHandle;
    edm::Handle<bool> badChargedHadronResultHandle;
    iEvent.getByToken(badMuon_Selector_, badMuonResultHandle);
    iEvent.getByToken(badChargedHadron_Selector_, badChargedHadronResultHandle);
    passFilter_badMuon_ = *badMuonResultHandle;
    passFilter_badChargedHadron_ = *badChargedHadronResultHandle;
 
    numCands = gravitons->size();
 
    // ************************* Gen Level Information******************//
    if(RunOnMC_)
    {//MC Info
        Int_t havegra=0;
        for(size_t ik=0; ik<genParticles->size();ik++)
        {// loop on gen
            const reco::Candidate* ptop0 = &(*genParticles)[ik];
            const reco::Candidate* ptop=findLasttau(ptop0,6);
                if(ptop0->pdgId()== 6 && gentop_pt==-99) {
                    gentop_pt = ptop->pt();
                    gentop_eta = ptop->eta();
                    gentop_phi = ptop->phi();
                    gentop_mass = ptop->mass();
                    for(int i=0;ptop->daughter(i)!=NULL;i++){
                        //if(abs(ptop->daughter(0)->pdgId())!=24&&abs(ptop->daughter(1)->pdgId())!=5) cout<<"no bW  "<<i<<"   "<<ptop->daughter(i)->pdgId()<<"   "<<ptop->daughter(i)->status()<<endl;
                        if(abs(ptop->daughter(i)->pdgId())==24){
                            gent_w_pt=ptop->daughter(i)->pt();
                            gent_w_eta=ptop->daughter(i)->eta();
                            gent_w_phi=ptop->daughter(i)->phi();
                            gent_w_mass=ptop->daughter(i)->mass();
                            const reco::Candidate* ptw0 = ptop->daughter(i);
                            const reco::Candidate* ptw= findLastW(ptw0,24);
                            if(ptw->daughter(0)!=NULL)
                            {
                                //if(abs(ptw->daughter(0)->pdgId())>=5) cout<<"no W-qq   "<<ptw->daughter(0)->pdgId()<<"   "<<ptw->daughter(1)->pdgId()<<endl;
                                if( abs(ptw->daughter(0)->pdgId())<=6 ){
                                    gent_w_tag=4;
                                    gent_w_q1_pt=ptw->daughter(0)->pt();
                                    gent_w_q1_eta=ptw->daughter(0)->eta();
                                    gent_w_q1_phi=ptw->daughter(0)->phi();
                                    gent_w_q1_e=ptw->daughter(0)->energy();
                                    gent_w_q1_pdg=ptw->daughter(0)->pdgId();
                                    gent_w_q2_pt=ptw->daughter(1)->pt();
                                    gent_w_q2_eta=ptw->daughter(1)->eta();
                                    gent_w_q2_phi=ptw->daughter(1)->phi();
                                    gent_w_q2_e=ptw->daughter(1)->energy();
                                    gent_w_q2_pdg=ptw->daughter(1)->pdgId();
                                }
                                if( abs(ptw->daughter(0)->pdgId())==11 ||abs(ptw->daughter(0)->pdgId())==12 ) gent_w_tag=1;
                                if( abs(ptw->daughter(0)->pdgId())==12 ||abs(ptw->daughter(0)->pdgId())==13 ) gent_w_tag=2;
                                if( abs(ptw->daughter(0)->pdgId())==14 ||abs(ptw->daughter(0)->pdgId())==15 ) gent_w_tag=3;
                                if( abs(ptw->daughter(0)->pdgId())==11 ||abs(ptw->daughter(0)->pdgId())==12||abs(ptw->daughter(0)->pdgId())==13 ||abs(ptw->daughter(0)->pdgId())==14||abs(ptw->daughter(0)->pdgId())==15 ||abs(ptw->daughter(0)->pdgId())==16)
                                {
                                    gent_w_q1_pt=ptw->daughter(0)->pt();
                                    gent_w_q1_eta=ptw->daughter(0)->eta();
                                    gent_w_q1_phi=ptw->daughter(0)->phi();
                                    gent_w_q1_e=ptw->daughter(0)->energy();
                                    gent_w_q1_pdg=ptw->daughter(0)->pdgId();
                                    gent_w_q2_pt=ptw->daughter(1)->pt();
                                    gent_w_q2_eta=ptw->daughter(1)->eta();
                                    gent_w_q2_phi=ptw->daughter(1)->phi();
                                    gent_w_q2_e=ptw->daughter(1)->energy();
                                    gent_w_q2_pdg=ptw->daughter(1)->pdgId();
                                }
                        }
                        }
                        if(abs(ptop->daughter(i)->pdgId())==5){
                            gent_b_pt=ptop->daughter(i)->pt();
                            gent_b_eta=ptop->daughter(i)->eta();
                            gent_b_phi=ptop->daughter(i)->phi();
                            gent_b_mass=ptop->daughter(i)->mass();
                        }
                }
                }
                if(ptop0->pdgId()== -6 && genantitop_pt==-99) {
                    genantitop_pt = ptop->pt();
                    genantitop_eta = ptop->eta();
                    genantitop_phi = ptop->phi();
                    genantitop_mass = ptop->mass();
                    for(int i=0;ptop->daughter(i)!=NULL;i++){
                        //cout<<i<<"   "<<ptop->daughter(i)->pdgId()<<"   "<<ptop->daughter(i)->status()<<endl;
                        if(abs(ptop->daughter(i)->pdgId())==24){
                            genantit_w_pt=ptop->daughter(i)->pt();
                            genantit_w_eta=ptop->daughter(i)->eta();
                            genantit_w_phi=ptop->daughter(i)->phi();
                            genantit_w_mass=ptop->daughter(i)->mass();
                            const reco::Candidate* ptw0 = ptop->daughter(i);
                            const reco::Candidate* ptw= findLastW(ptw0,24);
                            if(ptw->daughter(0)!=NULL)
                            {
                                if( abs(ptw->daughter(0)->pdgId())<=6 ){
                                    genantit_w_tag=4;
                                    genantit_w_q1_pt=ptw->daughter(0)->pt();
                                    genantit_w_q1_eta=ptw->daughter(0)->eta();
                                    genantit_w_q1_phi=ptw->daughter(0)->phi();
                                    genantit_w_q1_e=ptw->daughter(0)->energy();
                                    genantit_w_q1_pdg=ptw->daughter(0)->pdgId();
                                    genantit_w_q2_pt=ptw->daughter(1)->pt();
                                    genantit_w_q2_eta=ptw->daughter(1)->eta();
                                    genantit_w_q2_phi=ptw->daughter(1)->phi();
                                    genantit_w_q2_e=ptw->daughter(1)->energy();
                                    genantit_w_q2_pdg=ptw->daughter(1)->pdgId();
                                }
                                if( abs(ptw->daughter(0)->pdgId())==11 ||abs(ptw->daughter(0)->pdgId())==12 ) genantit_w_tag=1;
                                if( abs(ptw->daughter(0)->pdgId())==12 ||abs(ptw->daughter(0)->pdgId())==13 ) genantit_w_tag=2;
                                if( abs(ptw->daughter(0)->pdgId())==14 ||abs(ptw->daughter(0)->pdgId())==15 ) genantit_w_tag=3;
                                if( abs(ptw->daughter(0)->pdgId())==11 ||abs(ptw->daughter(0)->pdgId())==12||abs(ptw->daughter(0)->pdgId())==13 ||abs(ptw->daughter(0)->pdgId())==14||abs(ptw->daughter(0)->pdgId())==15 ||abs(ptw->daughter(0)->pdgId())==16)
                                {
                                    genantit_w_q1_pt=ptw->daughter(0)->pt();
                                    genantit_w_q1_eta=ptw->daughter(0)->eta();
                                    genantit_w_q1_phi=ptw->daughter(0)->phi();
                                    genantit_w_q1_e=ptw->daughter(0)->energy();
                                    genantit_w_q1_pdg=ptw->daughter(0)->pdgId();
                                    genantit_w_q2_pt=ptw->daughter(1)->pt();
                                    genantit_w_q2_eta=ptw->daughter(1)->eta();
                                    genantit_w_q2_phi=ptw->daughter(1)->phi();
                                    genantit_w_q2_e=ptw->daughter(1)->energy();
                                    genantit_w_q2_pdg=ptw->daughter(1)->pdgId();
                                }
                            }
                        }
                        if(abs(ptop->daughter(i)->pdgId())==5){
                            genantit_b_pt=ptop->daughter(i)->pt();
                            genantit_b_eta=ptop->daughter(i)->eta();
                            genantit_b_phi=ptop->daughter(i)->phi();
                            genantit_b_mass=ptop->daughter(i)->mass();
                        }
                    }
                }
                
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
                       
                        if(abs(pwkk->daughter(i)->pdgId())==24){//if w
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
                                            gen_tau_pt=pl->pt();
                                            gen_tau_eta=pl->eta();
                                            gen_tau_phi=pl->phi();
                                            gen_tau_e=pl->energy();
                                            const reco::Candidate* pl0= findLasttau(pl,15);
                                            for(int kk=0;pl0->daughter(kk)!=NULL&&kk<4;kk++)
                                            {
                                                pttau[kk]=pl0->daughter(kk)->pt();
                                                etatau[kk]=pl0->daughter(kk)->eta();
                                                phitau[kk]=pl0->daughter(kk)->pt();
                                                etau[kk]=pl0->daughter(kk)->energy();

                                            }
                                            status_1=3;
                                        }
                                        if(abs(pl->pdgId())==12)
                                        {
                                            gen_nele_pt=pl->pt();
                                            gen_nele_eta=pl->eta();
                                            gen_nele_phi=pl->phi();
                                            gen_nele_e=pl->energy();
                                        }
                                        if(abs(pl->pdgId())==14)
                                        {
                                            gen_nmu_pt=pl->pt();
                                            gen_nmu_eta=pl->eta();
                                            gen_nmu_phi=pl->phi();
                                            gen_nmu_e=pl->energy();
                                        }
                                        if(abs(pl->pdgId())==16)
                                        {
                                            gen_ntau_pt=pl->pt();
                                            gen_ntau_eta=pl->eta();
                                            gen_ntau_phi=pl->phi();
                                            gen_ntau_e=pl->energy();
                                        }

                                    }
                                }//end of if lep-w
                                numq=0;
                                for(int ii=0;pw->daughter(ii)!=NULL;ii++){
                                    const reco::Candidate* pl = pw->daughter(ii);
                                 
                                    if(abs(pl->pdgId())<6)
                                    {
                                        if(numq<3){
                                            ptq[numq]=pl->pt();
                                            etaq[numq]=pl->eta();
                                            phiq[numq]=pl->phi();
                                            eq[numq]=pl->energy();
                                            pdgidq[numq]=pl->pdgId();
                                            //cout<<numq<<"  "<<pdgidq[numq]<<endl;
                                            numq++;}
                                    }
                                }

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
                const reco::Candidate* pw=findFirstW(pw0,24);
                if(pw->mother(0)->pdgId()!=9000025){
                const reco::Candidate* pw1= findLastW(pw0,24);
                if(pw1->daughter(0)!=NULL){//loop on w daughter
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
                                gen_tau_pt=pl->pt();
                                gen_tau_eta=pl->eta();
                                gen_tau_phi=pl->phi();
                                gen_tau_e=pl->energy();
                                const reco::Candidate* pl0= findLasttau(pl,15);
                                for(int kk=0;pl0->daughter(kk)!=NULL&&kk<4;kk++)
                                {
                                    pttau[kk]=pl0->daughter(kk)->pt();
                                    etatau[kk]=pl0->daughter(kk)->eta();
                                    phitau[kk]=pl0->daughter(kk)->pt();
                                    etau[kk]=pl0->daughter(kk)->energy();
                                    pdgidtau[kk]=pl0->daughter(kk)->pdgId();

                                }
                                status_1=3;
                            }
                            if(abs(pl->pdgId())==12)
                            {
                                gen_nele_pt=pl->pt();
                                gen_nele_eta=pl->eta();
                                gen_nele_phi=pl->phi();
                                gen_nele_e=pl->energy();
                            }
                            if(abs(pl->pdgId())==14)
                            {
                                gen_nmu_pt=pl->pt();
                                gen_nmu_eta=pl->eta();
                                gen_nmu_phi=pl->phi();
                                gen_nmu_e=pl->energy();
                            }
                            if(abs(pl->pdgId())==16)
                            {
                                gen_ntau_pt=pl->pt();
                                gen_ntau_eta=pl->eta();
                                gen_ntau_phi=pl->phi();
                                gen_ntau_e=pl->energy();
                            }
                        }
                    
                    }//end of if lep-w
                        numq=0;
                        for(int ii=0;pw1->daughter(ii)!=NULL;ii++){
                            const reco::Candidate* pl = pw1->daughter(ii);
                            if(abs(pl->pdgId())<6)
                            {
                                if(numq<3){
                                    ptq[numq]=pl->pt();
                                    etaq[numq]=pl->eta();
                                    phiq[numq]=pl->phi();
                                    eq[numq]=pl->energy();
                                    pdgidq[numq]=pl->pdgId();
                                    numq++;}
                            }
                        }
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
        
		if( abs((*genParticles)[ik].pdgId())==9000025 ){//if Radion
            gen_rad_m=(*genParticles)[ik].mass();
            gen_rad_pt=(*genParticles)[ik].pt();
            gen_rad_phi=(*genParticles)[ik].phi();

            gen_rad_eta=(*genParticles)[ik].eta();
            for(int i=0;(*genParticles)[ik].daughter(i)!=NULL;i++){//loop on Radion daughter

                if(((*genParticles)[ik].daughter(i)->pdgId())==24){//if w-
                    const reco::Candidate* pw0 = (*genParticles)[ik].daughter(i);
                    const reco::Candidate* pw= findLastW(pw0,24);
                    if(pw->daughter(0)!=NULL){//loop on w daughter
                        const reco::Candidate* pl = pw->daughter(0);
                        if( (abs(pl->pdgId())==11) || (abs(pl->pdgId())==13)||(abs(pl->pdgId())==15)||(abs(pl->pdgId())==12) || (abs(pl->pdgId())==14)|| (abs(pl->pdgId())==16)){//beign of lep-w
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
                                    gen_tau_pt_2=pl->pt();
                                    gen_tau_eta_2=pl->eta();
                                    gen_tau_phi_2=pl->phi();
                                    gen_tau_e_2=pl->energy();
                                    const reco::Candidate* pl0= findLasttau(pl,15);
                                    for(int kk=0;pl0->daughter(kk)!=NULL&&kk<4;kk++)
                                    {
                                        pttau_2[kk]=pl0->daughter(kk)->pt();
                                        etatau_2[kk]=pl0->daughter(kk)->eta();
                                        phitau_2[kk]=pl0->daughter(kk)->pt();
                                        etau_2[kk]=pl0->daughter(kk)->energy();
                                        pdgidtau_2[kk]=pl0->daughter(kk)->pdgId();

                                    }

                                    status_2=3;
                                }
                                if(abs(pl->pdgId())==12)
                                {
                                    gen_nele_pt_2=pl->pt();
                                    gen_nele_eta_2=pl->eta();
                                    gen_nele_phi_2=pl->phi();
                                    gen_nele_e_2=pl->energy();
                                }
                                if(abs(pl->pdgId())==14)
                                {
                                    gen_nmu_pt_2=pl->pt();
                                    gen_nmu_eta_2=pl->eta();
                                    gen_nmu_phi_2=pl->phi();
                                    gen_nmu_e_2=pl->energy();
                                }
                                if(abs(pl->pdgId())==16)
                                {
                                    gen_ntau_pt_2=pl->pt();
                                    gen_ntau_eta_2=pl->eta();
                                    gen_ntau_phi_2=pl->phi();
                                    gen_ntau_e_2=pl->energy();
                                }

                            }
                        }//end of if lep-w
                             
                        numq_2=0;
                        for(int ii=0;pw->daughter(ii)!=NULL;ii++){
                            const reco::Candidate* pl = pw->daughter(ii);
                            if(abs(pl->pdgId())<6)
                            {
                                if(numq_2<3){
                                    ptq_2[numq_2]=pl->pt();
                                    etaq_2[numq_2]=pl->eta();
                                    phiq_2[numq_2]=pl->phi();
                                    eq_2[numq_2]=pl->energy();
                                    pdgidq_2[numq_2]=pl->pdgId();
                                    numq_2++;}
                            }
                        }

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
                if(((*genParticles)[ik].daughter(i)->pdgId())==-24){//if w+
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
                                    gen_tau_pt_3=pl->pt();
                                    gen_tau_eta_3=pl->eta();
                                    gen_tau_phi_3=pl->phi();
                                    gen_tau_e_3=pl->energy();
                                    const reco::Candidate* pl0= findLasttau(pl,15);
                                    for(int kk=0;pl0->daughter(kk)!=NULL&&kk<4;kk++)
                                    {
                                        pttau_3[kk]=pl0->daughter(kk)->pt();
                                        etatau_3[kk]=pl0->daughter(kk)->eta();
                                        phitau_3[kk]=pl0->daughter(kk)->pt();
                                        etau_3[kk]=pl0->daughter(kk)->energy();
                                        pdgidtau_3[kk]=pl0->daughter(kk)->pdgId();
                                    }
                                    status_3=3;
                                }
                                if(abs(pl->pdgId())==12)
                                {
                                    gen_nele_pt_3=pl->pt();
                                    gen_nele_eta_3=pl->eta();
                                    gen_nele_phi_3=pl->phi();
                                    gen_nele_e_3=pl->energy();
                                }
                                if(abs(pl->pdgId())==14)
                                {
                                    gen_nmu_pt_3=pl->pt();
                                    gen_nmu_eta_3=pl->eta();
                                    gen_nmu_phi_3=pl->phi();
                                    gen_nmu_e_3=pl->energy();
                                }
                                if(abs(pl->pdgId())==16)
                                {
                                    gen_ntau_pt_3=pl->pt();
                                    gen_ntau_eta_3=pl->eta();
                                    gen_ntau_phi_3=pl->phi();
                                    gen_ntau_e_3=pl->energy();
                                }

                            }
                        }//end of if lep-w
                             
                        numq_3=0;
                        for(int ii=0;pw->daughter(ii)!=NULL;ii++){
                            const reco::Candidate* pl = pw->daughter(ii);
                            if(abs(pl->pdgId())<6)
                            {
                                if(numq_3<3){
                                    ptq_3[numq_3]=pl->pt();
                                    etaq_3[numq_3]=pl->eta();
                                    phiq_3[numq_3]=pl->phi();
                                    eq_3[numq_3]=pl->energy();
                                    pdgidq_3[numq_3]=pl->pdgId();
                                    numq_3++;}
                            }
                        }

                        if(abs(pl->pdgId())<6){
                            ptGenVhad_3 = pw->pt();
                            etaGenVhad_3 = pw->eta();
                            phiGenVhad_3 = pw->phi();
                            massGenVhad_3 = pw->mass();
                            status_3=4;
 
                        }
                        if(abs(pl->pdgId())==24){
                            status_3=5;
                        }
                    }//end of loop on w daughter
                }//end of if w+
			 
            }//end of loop on Radion daughter
 		}//end of if Radion

    }//end of loop on gen

    if(gen_mu_pt>0. && mus->size()>0 ){
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
        
    //w and top info
        for( auto p=genParticles->begin(); p!= genParticles->end(); ++p)
        {}//std::cout<<p->pdgId()<<" "<<p->status()<<std::endl;}
        
        int igenw=0;
        int sizew=5;
        for(size_t ik=0; ik<genParticles->size();ik++)
        {
            if(abs((*genParticles)[ik].pdgId())==24)
                {
                    const reco::Candidate* pwtmp1 = &(*genParticles)[ik];
                    const reco::Candidate* pwtmp=findLastW(pwtmp1,24);
                    //const reco::Candidate* pwtmp2=findLasttau(pwtmp1,24);
                    //cout<<pwtmp->pt()<<"   "<<pwtmp2->pt()<<endl;
                    int woverlap=0;
                    for (int ia=0;ia<igenw;ia++){
                        if(pwtmp->pt()==ptgenwl[ia]) woverlap=1;
                    }
                    if(pwtmp->pt()>50&&igenw<sizew&&woverlap==0){
                    ptgenwl[igenw] = pwtmp->pt();
                    etagenwl[igenw] = pwtmp->eta();
                    phigenwl[igenw] = pwtmp->phi();
                    massgenwl[igenw] = pwtmp->mass();
                    const reco::Candidate* pwtmp2=findFirstW(pwtmp1,24);
                    ptgenwf[igenw] = pwtmp2->pt();
                    etagenwf[igenw] = pwtmp2->eta();
                    phigenwf[igenw] = pwtmp2->phi();
                    massgenwf[igenw] = pwtmp2->mass();
                        taggenwmother[igenw]=pwtmp2->mother(0)->pdgId();
                        /*cout<<ptgenwl[igenw]<<"   "<<ptgenwf[igenw]<<phigenwl[igenw]<<"   "<<phigenwf[igenw]<<"   "<<etagenwl[igenw]<<"   "<<etagenwf[igenw]<<" h0  "<<endl;
                        if(ptgenwl[igenw]!=ptgenwf[igenw] && pwtmp2->daughter(0)!=NULL) {cout<<phigenwl[igenw]<<"   "<<phigenwf[igenw]<<"   "<<etagenwl[igenw]<<"   "<<etagenwf[igenw]<<"  "<<pwtmp2->daughter(0)->pdgId()<<" h1  "<<endl;}
                        if(ptgenwl[igenw]!=ptgenwf[igenw] && pwtmp2->daughter(0)!=NULL&& pwtmp2->daughter(1)!=NULL) {cout<<phigenwl[igenw]<<"   "<<phigenwf[igenw]<<"   "<<etagenwl[igenw]<<"   "<<etagenwf[igenw]<<pwtmp2->daughter(0)->pdgId()<<"   ";
                            cout<<pwtmp2->daughter(1)->pdgId()<<endl;}*/
                    //for(int i=0;pw->daughter(i)!=NULL;i++)//loop on w daughter
                    if(pwtmp->daughter(0)!=NULL)//loop on w daughter
                    {
                        const reco::Candidate* pltmp = pwtmp->daughter(0);
                        //std::cout<< "pl pdgId" << pl->pdgId() << std::endl;
                         if( (abs(pltmp->pdgId())==11) || (abs(pltmp->pdgId())==12) ){
                                taggenwl[igenw]=1;                    }//end of w daugter loop
                        if( (abs(pltmp->pdgId())==13) || (abs(pltmp->pdgId())==14) ){
                            taggenwl[igenw]=2;                    }//end of w daugter loop
                        if( (abs(pltmp->pdgId())==15) || (abs(pltmp->pdgId())==16) ){
                            taggenwl[igenw]=3;                    }//end of w daugter loop
                        if(abs(pltmp->pdgId())<6 ) {
                            taggenwl[igenw]=4;
                            genw_q1_pt[igenw]=pwtmp->daughter(0)->pt();
                            genw_q1_eta[igenw]=pwtmp->daughter(0)->eta();
                            genw_q1_phi[igenw]=pwtmp->daughter(0)->phi();
                            genw_q1_e[igenw]=pwtmp->daughter(0)->energy();
                            genw_q1_pdg[igenw]=pwtmp->daughter(0)->pdgId();
                            genw_q2_pt[igenw]=pwtmp->daughter(1)->pt();
                            genw_q2_eta[igenw]=pwtmp->daughter(1)->eta();
                            genw_q2_phi[igenw]=pwtmp->daughter(1)->phi();
                            genw_q2_e[igenw]=pwtmp->daughter(1)->energy();
                            genw_q2_pdg[igenw]=pwtmp->daughter(1)->pdgId();
                            //cout<<"w_q  "<<igenw<<"  "<<pwtmp->daughter(0)->pt()<<"   "<<pwtmp->daughter(1)->pt()<<endl;
                            }
                        if( (abs(pltmp->pdgId())==11) || (abs(pltmp->pdgId())==12) ||(abs(pltmp->pdgId())==13) || (abs(pltmp->pdgId())==14) ||(abs(pltmp->pdgId())==15) || (abs(pltmp->pdgId())==16)){
                            genw_q1_pt[igenw]=pwtmp->daughter(0)->pt();
                            genw_q1_eta[igenw]=pwtmp->daughter(0)->eta();
                            genw_q1_phi[igenw]=pwtmp->daughter(0)->phi();
                            genw_q1_e[igenw]=pwtmp->daughter(0)->energy();
                            genw_q1_pdg[igenw]=pwtmp->daughter(0)->pdgId();
                            genw_q2_pt[igenw]=pwtmp->daughter(1)->pt();
                            genw_q2_eta[igenw]=pwtmp->daughter(1)->eta();
                            genw_q2_phi[igenw]=pwtmp->daughter(1)->phi();
                            genw_q2_e[igenw]=pwtmp->daughter(1)->energy();
                            genw_q2_pdg[igenw]=pwtmp->daughter(1)->pdgId();

                        }
                    }
                    igenw+=1;
                    }
                }//end of if w

        }

        int igenz=0;
        for(size_t ik=0; ik<genParticles->size();ik++)
        {
            if(abs((*genParticles)[ik].pdgId())==23)
            {
                const reco::Candidate* pztmp1 = &(*genParticles)[ik];
                const reco::Candidate* pztmp=findLasttau(pztmp1,23);
                //const reco::Candidate* pztmp2=findLasttau(pztmp1,24);
                //cout<<pztmp->pt()<<"   "<<pztmp2->pt()<<endl;
                int zoverlap=0;
                for (int ia=0;ia<igenz;ia++){
                    if(pztmp->pt()==ptgenzl[ia]) zoverlap=1;}
                if(pztmp->pt()>50&&igenz<sizew&&zoverlap==0){
                    ptgenzl[igenz] = pztmp->pt();
                    etagenzl[igenz] = pztmp->eta();
                    phigenzl[igenz] = pztmp->phi();
                    massgenzl[igenz] = pztmp->mass();
                    const reco::Candidate* pztmp2=findFirstW(pztmp1,23);
                    ptgenzf[igenz] = pztmp2->pt();
                    etagenzf[igenz] = pztmp2->eta();
                    phigenzf[igenz] = pztmp2->phi();
                    massgenzf[igenz] = pztmp2->mass();
                    //for(int i=0;pz->daughter(i)!=NULL;i++)//loop on w daughter
                    if(pztmp->daughter(0)!=NULL)//loop on w daughter
                    {
                        const reco::Candidate* pltmp = pztmp->daughter(0);
                        //std::cout<< "pl pdgId" << pl->pdgId() << std::endl;
                        if( (abs(pltmp->pdgId())==11) || (abs(pltmp->pdgId())==12) ){
                            taggenzl[igenz]=1;                    }//end of w daugter loop
                        if( (abs(pltmp->pdgId())==13) || (abs(pltmp->pdgId())==14) ){
                            taggenzl[igenz]=2;                    }//end of w daugter loop
                        if( (abs(pltmp->pdgId())==15) || (abs(pltmp->pdgId())==16) ){
                            taggenzl[igenz]=3;                    }//end of w daugter loop
                        if(abs(pltmp->pdgId())<6 ) {
                            taggenzl[igenz]=4;}
                    }
                    igenz+=1;
                }
            }//end of if w
        }

        int igeng=0;
        int sizeg=10;
        for(size_t ik=0; ik<genParticles->size();ik++)
        {
            //std::cout<<(*genParticles)[ik].pdgId()<<" "<<(*genParticles)[ik].status()<<std::endl;
            
            //		if( (*genParticles)[ik].pdgId()==5100039 ) // && (*genParticles)[ik].status()==3)//graviton
            //		{
            if(abs((*genParticles)[ik].pdgId())==21)
            {
                const reco::Candidate* pgtmp1 = &(*genParticles)[ik];
                const reco::Candidate* pgtmp=findLasttau(pgtmp1,21);
                int goverlap=0;
                for (int ia=0;ia<igeng;ia++){
                    if(pgtmp->pt()==ptgengl[ia]) goverlap=1;}
                if(pgtmp->pt()>50&&igeng<sizeg&&goverlap==0){
                    ptgengl[igeng] = pgtmp->pt();
                    etagengl[igeng] = pgtmp->eta();
                    phigengl[igeng] = pgtmp->phi();
                    egengl[igeng] = pgtmp->energy();
                    const reco::Candidate* pgtmp2=findFirstW(pgtmp,21);
                    ptgengf[igeng] = pgtmp2->pt();
                    etagengf[igeng] = pgtmp2->eta();
                    phigengf[igeng] = pgtmp2->phi();
                    egengf[igeng] = pgtmp2->energy();
                    mothergengf[igeng] = pgtmp2->mother(0)->pdgId();
                    const reco::Candidate* pgtmp3=findLasttau(pgtmp2->mother(0),pgtmp2->mother(0)->pdgId());
                    if (pgtmp3->mother(0)!=NULL) mmothergengf[igeng] = pgtmp3->mother(0)->pdgId();
                    //cout<<pgtmp2->pdgId()<<"    "<<pgtmp2->mother(0)->pdgId()<<"  "<<mmothergengf[igeng]<<endl;
                    igeng+=1;
                }
            }//end of if w
            
        }
     
        int igenq1=0;
        int sizeq1=5;
        for(size_t ik=0; ik<genParticles->size();ik++)
        {
                if(abs((*genParticles)[ik].pdgId())==1)
                {
                    const reco::Candidate* pgtmp1 = &(*genParticles)[ik];
                    const reco::Candidate* pgtmp=findLasttau(pgtmp1,1);
                    int goverlap=0;
                    for (int ia=0;ia<igenq1;ia++){
                        if(pgtmp->pt()==ptgenq1l[ia]) goverlap=1;}
                    const reco::Candidate* pgtmp2=findFirstW(pgtmp,1);
                    if(pgtmp->pt()>50&&igenq1<sizeq1&&goverlap==0&&abs(pgtmp2->mother(0)->pdgId())!=24){
                        ptgenq1l[igenq1] = pgtmp->pt();
                        etagenq1l[igenq1] = pgtmp->eta();
                        phigenq1l[igenq1] = pgtmp->phi();
                        egenq1l[igenq1] = pgtmp->energy();
                        ptgenq1f[igenq1] = pgtmp2->pt();
                        etagenq1f[igenq1] = pgtmp2->eta();
                        phigenq1f[igenq1] = pgtmp2->phi();
                        egenq1f[igenq1] = pgtmp2->energy();
                        mothergenq1f[igenq1] = pgtmp2->mother(0)->pdgId();
                        const reco::Candidate* pgtmp3=findLasttau(pgtmp2->mother(0),pgtmp2->mother(0)->pdgId());
                        if (pgtmp3->mother(0)!=NULL) mmothergenq1f[igenq1] = pgtmp3->mother(0)->pdgId();
                        //cout<<pgtmp2->pdgId()<<"    "<<pgtmp2->mother(0)->pdgId()<<"  "<<mmothergenq1f[igenq1]<<endl;
                        igenq1+=1;
                        //cout<<"q1   "<<igenq1<<"   "<<pgtmp2->pdgId()<<"   "<<pgtmp->mother(0)->pdgId()<<"   "<<pgtmp->pt()<<"   "<<pgtmp2->pt()<<"   "<<genantit_w_q1_pt<<"  "<<genantit_w_q2_pt<<"   "<<gent_w_q1_pt<<"  "<<gent_w_q2_pt<<endl;
                    }
            }
        }

            int igenq2=0;
            int sizeq2=5;
            for(size_t ik=0; ik<genParticles->size();ik++)
            {
                if(abs((*genParticles)[ik].pdgId())==2)
                {
                    const reco::Candidate* pgtmp1 = &(*genParticles)[ik];
                    const reco::Candidate* pgtmp=findLasttau(pgtmp1,2);
                    int goverlap=0;
                    for (int ia=0;ia<igenq2;ia++){
                        if(pgtmp->pt()==ptgenq2l[ia]) goverlap=1;}
                    const reco::Candidate* pgtmp2=findFirstW(pgtmp1,2);
                    if(pgtmp->pt()>50&&igenq2<sizeq2&&goverlap==0&&abs(pgtmp2->mother(0)->pdgId())!=24){
                        ptgenq2l[igenq2] = pgtmp->pt();
                        etagenq2l[igenq2] = pgtmp->eta();
                        phigenq2l[igenq2] = pgtmp->phi();
                        egenq2l[igenq2] = pgtmp->energy();
                        ptgenq2f[igenq2] = pgtmp2->pt();
                        etagenq2f[igenq2] = pgtmp2->eta();
                        phigenq2f[igenq2] = pgtmp2->phi();
                        egenq2f[igenq2] = pgtmp2->energy();
                        mothergenq2f[igenq2] = pgtmp2->mother(0)->pdgId();
                        const reco::Candidate* pgtmp3=findLasttau(pgtmp2->mother(0),pgtmp2->mother(0)->pdgId());
                        if (pgtmp3->mother(0)!=NULL) mmothergenq2f[igenq2] = pgtmp3->mother(0)->pdgId();
                        //cout<<pgtmp2->pdgId()<<"    "<<pgtmp2->mother(0)->pdgId()<<"  "<<mmothergenq2f[igenq2]<<endl;
                        igenq2+=1;
                    }
                }
            }

            int igenq3=0;
            int sizeq3=5;
            for(size_t ik=0; ik<genParticles->size();ik++)
            {
                if(abs((*genParticles)[ik].pdgId())==3)
                {
                    const reco::Candidate* pgtmp1 = &(*genParticles)[ik];
                    const reco::Candidate* pgtmp=findLasttau(pgtmp1,3);
                    int goverlap=0;
                    for (int ia=0;ia<igenq3;ia++){
                        if(pgtmp->pt()==ptgenq3l[ia]) goverlap=1;}
                    const reco::Candidate* pgtmp2=findFirstW(pgtmp1,3);
                    if(pgtmp->pt()>50&&igenq3<sizeq3&&goverlap==0&&abs(pgtmp2->mother(0)->pdgId())!=24){
                        ptgenq3l[igenq3] = pgtmp->pt();
                        etagenq3l[igenq3] = pgtmp->eta();
                        phigenq3l[igenq3] = pgtmp->phi();
                        egenq3l[igenq3] = pgtmp->energy();
                        ptgenq3f[igenq3] = pgtmp2->pt();
                        etagenq3f[igenq3] = pgtmp2->eta();
                        phigenq3f[igenq3] = pgtmp2->phi();
                        egenq3f[igenq3] = pgtmp2->energy();
                        mothergenq3f[igenq3] = pgtmp2->mother(0)->pdgId();
                        const reco::Candidate* pgtmp3=findLasttau(pgtmp2->mother(0),pgtmp2->mother(0)->pdgId());
                        if (pgtmp3->mother(0)!=NULL) mmothergenq3f[igenq3] = pgtmp3->mother(0)->pdgId();
                        //cout<<pgtmp2->pdgId()<<"    "<<pgtmp2->mother(0)->pdgId()<<"  "<<mmothergenq3f[igenq3]<<endl;
                        igenq3+=1;
                    }
                }
            }

            int igenq4=0;
            int sizeq4=5;
            for(size_t ik=0; ik<genParticles->size();ik++)
            {
                if(abs((*genParticles)[ik].pdgId())==4)
                {
                    const reco::Candidate* pgtmp1 = &(*genParticles)[ik];
                    const reco::Candidate* pgtmp=findLasttau(pgtmp1,4);
                    int goverlap=0;
                    for (int ia=0;ia<igenq4;ia++){
                        if(pgtmp->pt()==ptgenq4l[ia]) goverlap=1;}
                    //if(pgtmp->pt()==gent_w_q1_pt||pgtmp->pt()==gent_w_q2_pt||pgtmp->pt()==genantit_w_q1_pt||pgtmp->pt()==genantit_w_q2_pt)
                    //    cout<<" q4 overlap with tw"<<endl;
                    //if(pgtmp->pt()==gent_b_pt||pgtmp->pt()==genantit_b_pt)                         cout<<" q4 overlap with tb"<<endl;
                    //if(pgtmp->pt()>50&&igenq4<sizeq4&&goverlap==0&&pgtmp->pt()!=gent_w_q1_pt&&pgtmp->pt()!=gent_w_q2_pt&&pgtmp->pt()!=genantit_w_q1_pt&&pgtmp->pt()!=genantit_w_q2_pt){
                    const reco::Candidate* pgtmp2=findFirstW(pgtmp1,4);
                    if(pgtmp->pt()>50&&igenq4<sizeq4&&goverlap==0&&abs(pgtmp2->mother(0)->pdgId())!=24){
                        ptgenq4l[igenq4] = pgtmp->pt();
                        etagenq4l[igenq4] = pgtmp->eta();
                        phigenq4l[igenq4] = pgtmp->phi();
                        egenq4l[igenq4] = pgtmp->energy();
                        ptgenq4f[igenq4] = pgtmp2->pt();
                        etagenq4f[igenq4] = pgtmp2->eta();
                        phigenq4f[igenq4] = pgtmp2->phi();
                        egenq4f[igenq4] = pgtmp2->energy();
                        mothergenq4f[igenq4] = pgtmp2->mother(0)->pdgId();
                        const reco::Candidate* pgtmp3=findLasttau(pgtmp2->mother(0),pgtmp2->mother(0)->pdgId());
                        if (pgtmp3->mother(0)!=NULL) mmothergenq4f[igenq4] = pgtmp3->mother(0)->pdgId();
                        //cout<<pgtmp2->pdgId()<<"    "<<pgtmp2->mother(0)->pdgId()<<"  "<<mmothergenq4f[igenq4]<<endl;
                        igenq4+=1;
                    }
                }
            }

            int igenq5=0;
            int sizeq5=5;
            for(size_t ik=0; ik<genParticles->size();ik++)
            {
                if(abs((*genParticles)[ik].pdgId())==5)
                {
                    const reco::Candidate* pgtmp1 = &(*genParticles)[ik];
                    //const reco::Candidate* pgtmp=findLastW(pgtmp1,5);
                    const reco::Candidate* pgtmp=findLasttau(pgtmp1,5);
                    int goverlap=0;
                    for (int ia=0;ia<igenq5;ia++){
                        if(pgtmp->pt()==ptgenq5l[ia]) goverlap=1;}
                    const reco::Candidate* pgtmp2=findFirstW(pgtmp1,5);
                    //if(pgtmp->pt()==gent_w_q1_pt||pgtmp->pt()==gent_w_q2_pt||pgtmp->pt()==genantit_w_q1_pt||pgtmp->pt()==genantit_w_q2_pt)
                      //  cout<<" q5 overlap with tw"<<endl;
                    //if(pgtmp->pt()==gent_b_pt||pgtmp->pt()==genantit_b_pt)                         cout<<" q5 overlap with tb"<<endl;
                    if(pgtmp->pt()>50&&igenq5<sizeq5&&goverlap==0&&abs(pgtmp2->mother(0)->pdgId())!=24&&abs(pgtmp2->mother(0)->pdgId())!=6){
                        ptgenq5l[igenq5] = pgtmp->pt();
                        etagenq5l[igenq5] = pgtmp->eta();
                        phigenq5l[igenq5] = pgtmp->phi();
                        egenq5l[igenq5] = pgtmp->energy();
                        ptgenq5f[igenq5] = pgtmp2->pt();
                        etagenq5f[igenq5] = pgtmp2->eta();
                        phigenq5f[igenq5] = pgtmp2->phi();
                        egenq5f[igenq5] = pgtmp2->energy();
                        mothergenq5f[igenq5] = pgtmp2->mother(0)->pdgId();
                        const reco::Candidate* pgtmp3=findLasttau(pgtmp2->mother(0),pgtmp2->mother(0)->pdgId());
                        if (pgtmp3->mother(0)!=NULL) mmothergenq5f[igenq5] = pgtmp3->mother(0)->pdgId();
                        //cout<<pgtmp2->pdgId()<<"    "<<pgtmp2->mother(0)->pdgId()<<"  "<<mmothergenq5f[igenq5]<<endl;
                        igenq5+=1;
                        //cout<<"q5   "<<igenq5<<"   "<<pgtmp2->pdgId()<<"   "<<pgtmp->mother(0)->pdgId()<<"   "<<pgtmp->pt()<<"   "<<pgtmp2->pt()<<"   "<<gent_b_pt<<"   "<<genantit_b_pt<<endl;
                    }
                }
            }
        //cout<<"nng   "<<gentop_pt<<"  "<<genantitop_pt<<"  "<<igenw<<"  "<<igenq1<<"  "<<igenq2<<"  "<<igenq3<<"  "<<igenq4<<"  "<<igenq5<<"  "<<igeng<<"  "<<endl;

    }//end of MC Info
    // *************************End of Gen Level Information******************//


    //if(numCands != 0 ) {
    //const reco::Candidate& graviton  = gravitons->at(0);
    cout<<"hadronicVs->size()!"<<hadronicVs->size()<<"leptonicVs->size()"<<leptonicVs->size()<<endl;
    if((hadronicVs->size()!= 0 )  && (leptonicVs->size()!= 0) ){

       const reco::Candidate& leptonicV = leptonicVs->at(0);
       const reco::Candidate& hadronicV = hadronicVs->at(0);
       // const reco::Candidate& leptonicV = (*graviton.daughter("leptonicV"));
       const reco::Candidate& metCand = metHandle->at(0);
       const reco::Candidate& lepton = (*leptonicV.daughter(0));
       nLooseMu = loosemus->size();
       nLooseEle = looseels->size();

       edm::Handle<reco::VertexCollection> vertices;
       iEvent.getByToken(vtxToken_, vertices);
        // edm::Handle<reco::VertexCollection> vertices;
        // iEvent.getByLabel("offlineSlimmedPrimaryVertices", vertices);
        // iEvent.getByToken(vtxToken_, vertices);
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
            Double_t rawPt	 = met.corPt();//met.shiftedPt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
            Double_t rawPhi   = met.corPhi();//met.shiftedPhi(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
            Double_t rawSumEt = met.corSumEt();
            //const float rawPt	 = met.shiftedPt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
            //const float rawPhi   = met.shiftedPhi(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
            //const float rawSumEt = met.shiftedSumEt(pat::MET::METUncertainty::NoShift, pat::MET::METUncertaintyLevel::Raw);
            TVector2 rawMET_;
            rawMET_.SetMagPhi (rawPt, rawPhi );
            Double_t rawPx = rawMET_.Px();
            Double_t rawPy = rawMET_.Py();
            Double_t rawEt = std::hypot(rawPx,rawPy);
            MET_et = rawEt;
            MET_phi = rawPhi;
            MET_sumEt = rawSumEt;
            //cout<<MET_et<<" default MET  "<<MET_phi<<endl;
            /*double pxcorr = rawPx+TypeICorrMap_["corrEx"];
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
            MET_corrPy = TypeICorrMap_["corrEy"];*/
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
        metPhi         = metCand.phi();
        //cout<<met<<" met candidate "<<metPhi<<endl;
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
                        el1->dr03TkSumPt() < 5. && el1->gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&//numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
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
                            el1->dr03TkSumPt() < 5. && el1->gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&//numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
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
        //cout<<ptVlep<<" lep W "<<ptVlepJEC<<"   "<<yVlep<<" lep W "<<yVlepJEC<<"   "<<phiVlep<<" lep W "<<phiVlepJEC<<"   "<<massVlep<<" lep W "<<massVlepJEC<<"   "<<endl;
        delPhilepmet = deltaPhi(philep1, MET_phi);
        //cout<<"mtVlepJEC"<<mtVlepJEC<<endl;
        ////////////////////////JEC for AK8/////////////////////////////////

        reco::Candidate::LorentzVector uncorrPrunedJet;

        
        bool doPuppi  = false;
        doPuppi = iEvent.getByToken(puppijetInputToken_, puppijets_ );
        cout<<"puppijets_"<<puppijets_->size()<<endl;
        cout<<"doPuppi"<<doPuppi<<endl;
        
        if( doPuppi ){//1
            num_totalpuppijets = puppijets_->size();
            num_correctpuppijets = 0;
            for(size_t i=0; i<puppijets_->size()&&i<4;i++){
            if(puppijets_->at(i).isPFJet()!=0){
            num_correctpuppijets = num_correctpuppijets +1;
            }
            }




            for(size_t ij=0; ij<puppijets_->size()&&ij<4;ij++){
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
            cout<<"puppijets_->size()"<<puppijets_->size()<<endl;
            int numvhad = puppijets_->size();
            for( int inum = 0; inum< numvhad; inum++){
                cout<<"inum1"<<inum<<endl;
                const pat::Jet& Vpuppi = puppijets_->at(inum);
                cout<<"inum2"<<inum<<endl;
                cout<<"whether PF"<<Vpuppi.isPFJet()<<endl;
                if(Vpuppi.isPFJet()==0) continue;
                
                cout<<"looseJetID(Vpuppi)----"<<looseJetID(Vpuppi)<<endl;
                cout<<"jetAK8puppi_eta1[inum]"<<jetAK8puppi_eta1[inum]<<"jetAK8puppi_pt1[inum]"<<jetAK8puppi_pt1[inum]<<endl;
                if(looseJetID(Vpuppi)<1) continue;
                if(jetAK8puppi_pt1[inum] > pt_larger && fabs(jetAK8puppi_eta1[inum])<2.4 && inum<4) {pt_larger = jetAK8puppi_pt1[inum]; usenumber3 = inum; continue;}
            }
            cout<<"usenumber3"<<usenumber3<<endl;
            if (usenumber3>-1) {//2
                cout<<"begin1"<<endl;
                const pat::Jet& hadronicVpuppi = puppijets_->at(usenumber3);
                // DeepAK8
                cout<<"1"<<endl;
                JetHelper jet_helper(&hadronicVpuppi);
               // jet_helper.setSubjets(*hadronicVSoftDrop, 0.8); // jetR=0.8
                cout<<"11"<<endl;
                const auto& nnpreds = fatjetNN_->predict(jet_helper);
                FatJetNNHelper nn(nnpreds);
                cout<<"12"<<endl;
                jetAK8puppi_dnnTop       = nn.get_binarized_score_top();
                jetAK8puppi_dnnW         = nn.get_binarized_score_w();
                cout<<"13"<<endl;
                jetAK8puppi_dnnH4q       = nn.get_binarized_score_h4q();
                jetAK8puppi_dnnZ         = nn.get_binarized_score_z();
                jetAK8puppi_dnnZbb       = nn.get_binarized_score_zbb();
                jetAK8puppi_dnnHbb       = nn.get_binarized_score_hbb();
                jetAK8puppi_dnnqcd       = nn.get_raw_score_qcd();
                jetAK8puppi_dnntop       = nn.get_raw_score_top();
                jetAK8puppi_dnnw         = nn.get_raw_score_w();
                jetAK8puppi_dnnz         = nn.get_raw_score_z();
                jetAK8puppi_dnnzbb         = nn.get_raw_score_zbb();
                jetAK8puppi_dnnhbb         = nn.get_raw_score_hbb();
                jetAK8puppi_dnnh4q         = nn.get_raw_score_h4q();
                // Decorrelated DeepAK8
                const auto& mdpreds = decorrNN_->predict(jet_helper);
                FatJetNNHelper md(mdpreds);
                jetAK8puppi_dnnDecorrTop       = md.get_binarized_score_top();
                jetAK8puppi_dnnDecorrW         = md.get_binarized_score_w();
                jetAK8puppi_dnnDecorrH4q       = md.get_binarized_score_h4q();
                jetAK8puppi_dnnDecorrZ         = md.get_binarized_score_z();
                jetAK8puppi_dnnDecorrZbb       = md.get_binarized_score_zbb();
                jetAK8puppi_dnnDecorrHbb       = md.get_binarized_score_hbb();
                jetAK8puppi_dnnDecorrbb        = md.get_flavor_score_bb();
                jetAK8puppi_dnnDecorrcc        = md.get_flavor_score_cc();
                jetAK8puppi_dnnDecorrbbnog        = md.get_flavor_score_bb_no_gluon();
                jetAK8puppi_dnnDecorrccnog        = md.get_flavor_score_cc_no_gluon();
                jetAK8puppi_dnnDecorrqcd       = md.get_raw_score_qcd();
                jetAK8puppi_dnnDecorrtop       = md.get_raw_score_top();
                jetAK8puppi_dnnDecorrw         = md.get_raw_score_w();
                jetAK8puppi_dnnDecorrz         = md.get_raw_score_z();
                jetAK8puppi_dnnDecorrzbb         = md.get_raw_score_zbb();
                jetAK8puppi_dnnDecorrhbb         = md.get_raw_score_hbb();
                jetAK8puppi_dnnDecorrh4q         = md.get_raw_score_h4q();

                 // ------
                jetAK8puppi_ptJEC       = jetAK8puppi_pt1[usenumber3]; // unpruned corrected jet pt
                jetAK8puppi_ptJEC       = jetAK8puppi_pt1[usenumber3]; // unpruned corrected jet pt
                jetAK8puppi_eta     = jetAK8puppi_eta1[usenumber3]; // unpruned (w/o jec) jet eta
                jetAK8puppi_phi      = hadronicVpuppi.phi(); // unpruned (w/o jec) jet phi
                jetAK8puppi_tau1         = hadronicVpuppi.userFloat("NjettinessAK8Puppi:tau1");
                jetAK8puppi_tau2         = hadronicVpuppi.userFloat("NjettinessAK8Puppi:tau2");
                jetAK8puppi_tau3         = hadronicVpuppi.userFloat("NjettinessAK8Puppi:tau3");
                jetAK8puppi_tau21        = jetAK8puppi_tau2/jetAK8puppi_tau1;
                jetAK8puppi_tau4         = hadronicVpuppi.userFloat("NjettinessAK8Puppi:tau4");
                jetAK8puppi_tau42        = jetAK8puppi_tau4/jetAK8puppi_tau2;
                jetAK8puppi_sd       =  hadronicVpuppi.userFloat("ak8PFJetsCHSValueMap:ak8PFJetsCHSSoftDropMass"); // uncorrected pruned mass
                jetAK8puppi_sdJEC  =corr_AK8puppiSD[usenumber3]*jetAK8puppi_sd;
                Double_t gencorrect=1.0;
                Double_t recocorrect_0eta1p3=1.0;
                Double_t recocorrect_1p3eta2p5=1.0;
                gencorrect=1.006-1.062*pow(jetAK8puppi_ptJEC*0.08,-1.2);
                recocorrect_0eta1p3=1.093-1.501e-04*jetAK8puppi_ptJEC+3.449e-07*pow(jetAK8puppi_ptJEC,2)-2.681e-10*pow(jetAK8puppi_ptJEC,3)+8.674e-14*pow(jetAK8puppi_ptJEC,4)-1.001e-17*pow(jetAK8puppi_ptJEC,5);
                recocorrect_1p3eta2p5=1.272-5.72e-04*jetAK8puppi_ptJEC+8.37e-07*pow(jetAK8puppi_ptJEC,2)-5.204e-10*pow(jetAK8puppi_ptJEC,3)+1.454e-13*pow(jetAK8puppi_ptJEC,4)-1.504e-17*pow(jetAK8puppi_ptJEC,5);
                if (fabs(jetAK8puppi_eta)<=1.3){jetAK8puppi_sdcorr=jetAK8puppi_sd*gencorrect*recocorrect_0eta1p3;}
                else if (fabs(jetAK8puppi_eta)<2.5 && fabs(jetAK8puppi_eta)>1.3){jetAK8puppi_sdcorr=jetAK8puppi_sd*gencorrect*recocorrect_1p3eta2p5;}
                IDLoose = looseJetID(hadronicVpuppi);
                IDTight = tightJetID(hadronicVpuppi);
            }
            
            int usenumber2 = -1; double pt_larger2=0;
            for( int inum = 0; inum< numvhad; inum++){
                const pat::Jet& Vpuppi = puppijets_->at(inum);
                if(Vpuppi.isPFJet()==0) continue;
                if(looseJetID(Vpuppi)<1) continue;
                if(jetAK8puppi_pt1[inum] > pt_larger2 && fabs(jetAK8puppi_eta1[inum])<2.4 && inum != usenumber3 && inum<4) {pt_larger2 = jetAK8puppi_pt1[inum]; usenumber2 = inum; continue;}
            }
            cout<<"usenumber2"<<usenumber2<<endl;
            if(usenumber2>-1)  {
                const pat::Jet& hadronicVpuppi_2 = puppijets_->at(usenumber2);
                // DeepAK8
                JetHelper jet_helper_2(&hadronicVpuppi_2);
              //  jet_helper_2.setSubjets(*hadronicVSoftDrop, 0.8); // jetR=0.8
                cout<<"2"<<endl;  
                const auto& nnpreds_2 = fatjetNN_->predict(jet_helper_2);
                cout<<"21"<<endl;
                FatJetNNHelper nn_2(nnpreds_2);
                cout<<"22"<<endl;
                jetAK8puppi_dnnTop_2       = nn_2.get_binarized_score_top();
                jetAK8puppi_dnnW_2         = nn_2.get_binarized_score_w();
                cout<<"23"<<endl;
                jetAK8puppi_dnnH4q_2       = nn_2.get_binarized_score_h4q();
                jetAK8puppi_dnnZ_2         = nn_2.get_binarized_score_z();
                jetAK8puppi_dnnZbb_2       = nn_2.get_binarized_score_zbb();
                jetAK8puppi_dnnHbb_2       = nn_2.get_binarized_score_hbb();
                jetAK8puppi_dnnqcd_2       = nn_2.get_raw_score_qcd();
                jetAK8puppi_dnntop_2       = nn_2.get_raw_score_top();
                jetAK8puppi_dnnw_2         = nn_2.get_raw_score_w();
                jetAK8puppi_dnnz_2         = nn_2.get_raw_score_z();
                jetAK8puppi_dnnzbb_2         = nn_2.get_raw_score_zbb();
                jetAK8puppi_dnnhbb_2         = nn_2.get_raw_score_hbb();
                jetAK8puppi_dnnh4q_2         = nn_2.get_raw_score_h4q();
                // Decorrelated DeepAK8
                const auto& mdpreds_2 = decorrNN_->predict(jet_helper_2);
                FatJetNNHelper md_2(mdpreds_2);
                jetAK8puppi_dnnDecorrTop_2       = md_2.get_binarized_score_top();
                jetAK8puppi_dnnDecorrW_2         = md_2.get_binarized_score_w();
                jetAK8puppi_dnnDecorrH4q_2       = md_2.get_binarized_score_h4q();
                jetAK8puppi_dnnDecorrZ_2         = md_2.get_binarized_score_z();
                jetAK8puppi_dnnDecorrZbb_2       = md_2.get_binarized_score_zbb();
                jetAK8puppi_dnnDecorrHbb_2       = md_2.get_binarized_score_hbb();
                jetAK8puppi_dnnDecorrbb_2        = md_2.get_flavor_score_bb();
                jetAK8puppi_dnnDecorrcc_2        = md_2.get_flavor_score_cc();
                jetAK8puppi_dnnDecorrbbnog_2        = md_2.get_flavor_score_bb_no_gluon();
                jetAK8puppi_dnnDecorrccnog_2        = md_2.get_flavor_score_cc_no_gluon();
                jetAK8puppi_dnnDecorrqcd_2       = md_2.get_raw_score_qcd();
                jetAK8puppi_dnnDecorrtop_2       = md_2.get_raw_score_top();
                jetAK8puppi_dnnDecorrw_2         = md_2.get_raw_score_w();
                jetAK8puppi_dnnDecorrz_2         = md_2.get_raw_score_z();
                jetAK8puppi_dnnDecorrzbb_2         = md_2.get_raw_score_zbb();
                jetAK8puppi_dnnDecorrhbb_2         = md_2.get_raw_score_hbb();
                jetAK8puppi_dnnDecorrh4q_2         = md_2.get_raw_score_h4q();

                 // ------
                jetAK8puppi_ptJEC_2       = jetAK8puppi_pt1[usenumber2]; // unpruned corrected jet pt
                jetAK8puppi_eta_2     = jetAK8puppi_eta1[usenumber2]; // unpruned (w/o jec) jet eta
                jetAK8puppi_phi_2      = hadronicVpuppi_2.phi(); // unpruned (w/o jec) jet phi
                jetAK8puppi_tau1_2         = hadronicVpuppi_2.userFloat("NjettinessAK8Puppi:tau1");
                jetAK8puppi_tau2_2         = hadronicVpuppi_2.userFloat("NjettinessAK8Puppi:tau2");
                jetAK8puppi_tau3_2         = hadronicVpuppi_2.userFloat("NjettinessAK8Puppi:tau3");
                jetAK8puppi_tau21_2        = jetAK8puppi_tau2_2/jetAK8puppi_tau1_2;
                jetAK8puppi_tau4_2         = hadronicVpuppi_2.userFloat("NjettinessAK8Puppi:tau4");
                jetAK8puppi_tau42_2        = jetAK8puppi_tau4_2/jetAK8puppi_tau2_2;
                jetAK8puppi_sd_2       =  hadronicVpuppi_2.userFloat("ak8PFJetsCHSValueMap:ak8PFJetsCHSSoftDropMass"); // uncorrected pruned mass
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
            }

            int usenumber1 = -1; double pt_larger1=0;
            for( int inum = 0; inum< numvhad; inum++){
                const pat::Jet& Vpuppi = puppijets_->at(inum);
                if(Vpuppi.isPFJet()==0) continue;
                if(looseJetID(Vpuppi)<1) continue;
                if(jetAK8puppi_pt1[inum] > pt_larger1 && fabs(jetAK8puppi_eta1[inum])<2.4 && inum != usenumber3 && inum != usenumber2 && inum<4) {pt_larger1 = jetAK8puppi_pt1[inum]; usenumber1 = inum; continue;}
            }
            cout<<"usenumber1"<<usenumber1<<endl;
            if(usenumber1>-1)  {
                const pat::Jet& hadronicVpuppi_3 = puppijets_->at(usenumber1);
                // DeepAK8
                JetHelper jet_helper_3(&hadronicVpuppi_3);
                   
                cout<<"3"<<endl;
       //         jet_helper_3.setSubjets(*hadronicVSoftDrop, 0.8); // jetR=0.8
                const auto& nnpreds_3 = fatjetNN_->predict(jet_helper_3);
                FatJetNNHelper nn_3(nnpreds_3);
                jetAK8puppi_dnnTop_3       = nn_3.get_binarized_score_top();
                jetAK8puppi_dnnW_3         = nn_3.get_binarized_score_w();
                jetAK8puppi_dnnH4q_3       = nn_3.get_binarized_score_h4q();
                jetAK8puppi_dnnZ_3         = nn_3.get_binarized_score_z();
                jetAK8puppi_dnnZbb_3       = nn_3.get_binarized_score_zbb();
                jetAK8puppi_dnnHbb_3       = nn_3.get_binarized_score_hbb();
                jetAK8puppi_dnnqcd_3       = nn_3.get_raw_score_qcd();
                jetAK8puppi_dnntop_3       = nn_3.get_raw_score_top();
                jetAK8puppi_dnnw_3         = nn_3.get_raw_score_w();
                jetAK8puppi_dnnz_3         = nn_3.get_raw_score_z();
                jetAK8puppi_dnnzbb_3         = nn_3.get_raw_score_zbb();
                jetAK8puppi_dnnhbb_3         = nn_3.get_raw_score_hbb();
                jetAK8puppi_dnnh4q_3         = nn_3.get_raw_score_h4q();
                // Decorrelated DeepAK8
                const auto& mdpreds_3 = decorrNN_->predict(jet_helper_3);
                FatJetNNHelper md_3(mdpreds_3);
                jetAK8puppi_dnnDecorrTop_3       = md_3.get_binarized_score_top();
                jetAK8puppi_dnnDecorrW_3         = md_3.get_binarized_score_w();
                jetAK8puppi_dnnDecorrH4q_3       = md_3.get_binarized_score_h4q();
                jetAK8puppi_dnnDecorrZ_3         = md_3.get_binarized_score_z();
                jetAK8puppi_dnnDecorrZbb_3       = md_3.get_binarized_score_zbb();
                jetAK8puppi_dnnDecorrHbb_3       = md_3.get_binarized_score_hbb();
                jetAK8puppi_dnnDecorrbb_3        = md_3.get_flavor_score_bb();
                jetAK8puppi_dnnDecorrcc_3        = md_3.get_flavor_score_cc();
                jetAK8puppi_dnnDecorrbbnog_3        = md_3.get_flavor_score_bb_no_gluon();
                jetAK8puppi_dnnDecorrccnog_3        = md_3.get_flavor_score_cc_no_gluon();
                jetAK8puppi_dnnDecorrqcd_3       = md_3.get_raw_score_qcd();
                jetAK8puppi_dnnDecorrtop_3       = md_3.get_raw_score_top();
                jetAK8puppi_dnnDecorrw_3         = md_3.get_raw_score_w();
                jetAK8puppi_dnnDecorrz_3         = md_3.get_raw_score_z();
                jetAK8puppi_dnnDecorrzbb_3         = md_3.get_raw_score_zbb();
                jetAK8puppi_dnnDecorrhbb_3         = md_3.get_raw_score_hbb();
                jetAK8puppi_dnnDecorrh4q_3         = md_3.get_raw_score_h4q();

                 // ------
                jetAK8puppi_ptJEC_3       = jetAK8puppi_pt1[usenumber1]; // unpruned corrected jet pt
                jetAK8puppi_eta_3     = jetAK8puppi_eta1[usenumber1]; // unpruned (w/o jec) jet eta
                jetAK8puppi_phi_3      = hadronicVpuppi_3.phi(); // unpruned (w/o jec) jet phi
                jetAK8puppi_tau1_3         = hadronicVpuppi_3.userFloat("NjettinessAK8Puppi:tau1");
                jetAK8puppi_tau2_3         = hadronicVpuppi_3.userFloat("NjettinessAK8Puppi:tau2");
                jetAK8puppi_tau3_3         = hadronicVpuppi_3.userFloat("NjettinessAK8Puppi:tau3");
                jetAK8puppi_tau21_3        = jetAK8puppi_tau2_3/jetAK8puppi_tau1_3;
                jetAK8puppi_tau4_3         = hadronicVpuppi_3.userFloat("NjettinessAK8Puppi:tau4");
                jetAK8puppi_tau42_3        = jetAK8puppi_tau4_3/jetAK8puppi_tau2_3;
                jetAK8puppi_sd_3       =  hadronicVpuppi_3.userFloat("ak8PFJetsCHSValueMap:ak8PFJetsCHSSoftDropMass"); // uncorrected pruned mass
                jetAK8puppi_sdJEC_3  =corr_AK8puppiSD[usenumber1]*jetAK8puppi_sd_3;
                Double_t gencorrect=1.0;
                Double_t recocorrect_0eta1p3=1.0;
                Double_t recocorrect_1p3eta2p5=1.0;
                gencorrect=1.006-1.062*pow(jetAK8puppi_ptJEC_3*0.08,-1.2);
                recocorrect_0eta1p3=1.093-1.501e-04*jetAK8puppi_ptJEC_3+3.449e-07*pow(jetAK8puppi_ptJEC_3,2)-2.681e-10*pow(jetAK8puppi_ptJEC_3,3)+8.674e-14*pow(jetAK8puppi_ptJEC_3,4)-1.001e-17*pow(jetAK8puppi_ptJEC_3,5);
                recocorrect_1p3eta2p5=1.272-5.72e-04*jetAK8puppi_ptJEC_3+8.37e-07*pow(jetAK8puppi_ptJEC_3,2)-5.204e-10*pow(jetAK8puppi_ptJEC_3,3)+1.454e-13*pow(jetAK8puppi_ptJEC_3,4)-1.504e-17*pow(jetAK8puppi_ptJEC_3,5);
                if (fabs(jetAK8puppi_eta_3)<=1.3){jetAK8puppi_sdcorr_3=jetAK8puppi_sd_3*gencorrect*recocorrect_0eta1p3;}
                else if (fabs(jetAK8puppi_eta_3)<2.5 && fabs(jetAK8puppi_eta_3)>1.3){jetAK8puppi_sdcorr_3=jetAK8puppi_sd_3*gencorrect*recocorrect_1p3eta2p5;}
                IDLoose_3 = looseJetID(hadronicVpuppi_3);
                IDTight_3 = tightJetID(hadronicVpuppi_3);
            }

            int nak4 = 0;
            double tj1=-10.0, tj2=-10.0;
 
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
    
                //if( (corr*uncorrJet.pt())>20 && (fabs((*ak4jets)[ik].eta()) < 5.0) && looseJetID((*ak4jets)[ik])>0 && dtemp>0.8 && nak4<8){
                if( (corr*uncorrJet.pt())>20 && (fabs((*ak4jets)[ik].eta()) < 5.0) && looseJetID((*ak4jets)[ik])>0 && nak4<8){
                    ak4jet_hf[nak4]=(*ak4jets)[ik].hadronFlavour();
                    ak4jet_pf[nak4]=(*ak4jets)[ik].partonFlavour();
                    ak4jet_pt[nak4] =  corr*uncorrJet.pt();
                    ak4jet_pt_uncorr[nak4] =  uncorrJet.pt();
                    ak4jet_eta[nak4] = (*ak4jets)[ik].eta();
                    ak4jet_phi[nak4] = (*ak4jets)[ik].phi();
                    ak4jet_e[nak4] =   corr*uncorrJet.energy();
                    ak4jet_csv[nak4] = (*ak4jets)[ik].bDiscriminator("pfCombinedSecondaryVertexV2BJetTags");
                    ak4jet_icsv[nak4] = (*ak4jets)[ik].bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
                    if(!RunOnSig_){
                        ak4jet_deepcsvudsg[nak4] = (*ak4jets)[ik].bDiscriminator("deepFlavourJetTags:probudsg");
                        ak4jet_deepcsvb[nak4] = (*ak4jets)[ik].bDiscriminator("deepFlavourJetTags:probb");
                        ak4jet_deepcsvc[nak4] = (*ak4jets)[ik].bDiscriminator("deepFlavourJetTags:probc");
                        ak4jet_deepcsvbb[nak4] = (*ak4jets)[ik].bDiscriminator("deepFlavourJetTags:probbb");
                        ak4jet_deepcsvcc[nak4] = (*ak4jets)[ik].bDiscriminator("deepFlavourJetTags:probcc");}
                    if(RunOnSig_){
                        ak4jet_deepcsvudsg[nak4] = (*ak4jets)[ik].bDiscriminator("pfDeepCSVJetTags:probudsg");
                        ak4jet_deepcsvb[nak4] = (*ak4jets)[ik].bDiscriminator("pfDeepCSVJetTags:probb");
                        ak4jet_deepcsvc[nak4] = (*ak4jets)[ik].bDiscriminator("pfDeepCSVJetTags:probc");
                        ak4jet_deepcsvbb[nak4] = (*ak4jets)[ik].bDiscriminator("pfDeepCSVJetTags:probbb");
                        ak4jet_deepcsvcc[nak4] = (*ak4jets)[ik].bDiscriminator("pfDeepCSVJetTags:probcc");}
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
            
            }//3
         

            if(nj1>-1 && nj2>-1 && ak4jet_pt[nj1]>30. && ak4jet_pt[nj2]>30.) {
                vbfeta=fabs(ak4jet_eta[nj1]-ak4jet_eta[nj2]);
                TLorentzVector vbfj1, vbfj2;
                vbfj1.SetPtEtaPhiE(ak4jet_pt[nj1], ak4jet_eta[nj1], ak4jet_phi[nj1], ak4jet_e[nj1]);
                vbfj2.SetPtEtaPhiE(ak4jet_pt[nj2], ak4jet_eta[nj2], ak4jet_phi[nj2], ak4jet_e[nj2]);
                vbfmjj=(vbfj1+vbfj2).Mag();
            }

            if(vbfeta>4.0 && vbfmjj>400) {vbftag=1;}
            
            deltaRlepjet = deltaR(etalep1,philep1,jetAK8puppi_eta,jetAK8puppi_phi);
            deltaRlepjet_2 = deltaR(etalep1,philep1,jetAK8puppi_eta_2,jetAK8puppi_phi_2);
            TLorentzVector ghadronicVpuppi, gravitonpuppiJEC,ghadronicVpuppi_2, gravitonpuppiJEC_2;
            ghadronicVpuppi.SetPtEtaPhiM(jetAK8puppi_ptJEC, jetAK8puppi_eta, jetAK8puppi_phi, jetAK8puppi_sdJEC);
            ghadronicVpuppi_2.SetPtEtaPhiM(jetAK8puppi_ptJEC_2, jetAK8puppi_eta_2, jetAK8puppi_phi_2, jetAK8puppi_sdJEC_2);
            gravitonpuppiJEC = gleptonicV + ghadronicVpuppi+ ghadronicVpuppi_2;
            candMasspuppiJEC     = gravitonpuppiJEC.Mag();
            m_jlv     = (gleptonicV + ghadronicVpuppi).Mag();
            delPhijetmet = deltaPhi(jetAK8puppi_phi, MET_phi);
            delPhijetlep = deltaPhi(jetAK8puppi_phi, phiVlepJEC);
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
           
            edm::Handle<edm::View<pat::Jet> > jetsAK8;
            //jetsAK8Label_=puppijetInputToken_;
            iEvent.getByToken(jetsAK8Label_, jetsAK8);
        
            edm::View<pat::Jet>::const_iterator beginAK8 = jetsAK8->begin();
            edm::View<pat::Jet>::const_iterator endAK8 = jetsAK8->end();
            edm::View<pat::Jet>::const_iterator ijetAK8 = beginAK8;
    
            edm::View<pat::Jet>::const_iterator ijetAK8_j1,ijetAK8_j2;
            double drak8jetmatch1=10000.,drak8jetmatch2=10000.,tmpdrak8jet1=10000.,tmpdrak8jet2=10000.;
            // Loop over the "hard" jets
            for(ijetAK8 = beginAK8; ijetAK8 != endAK8; ++ijetAK8 ) {
                if(ijetAK8->pt()>0){
                    tmpdrak8jet1=deltaR(ijetAK8->eta(),ijetAK8->phi(),jetAK8puppi_eta,jetAK8puppi_phi);
                    //cout<<tmpdrak8jet1<<endl;
                    tmpdrak8jet2=deltaR(ijetAK8->eta(),ijetAK8->phi(),jetAK8puppi_eta_2,jetAK8puppi_phi_2);
                    if (tmpdrak8jet1<drak8jetmatch1) {drak8jetmatch1=tmpdrak8jet1; ijetAK8_j1=ijetAK8;}
                    if (tmpdrak8jet2<drak8jetmatch2) {drak8jetmatch2=tmpdrak8jet2; ijetAK8_j2=ijetAK8;}
                }
            }
            //if(jetAK8puppi_ptJEC>0) cout<<ijetAK8_j1->pt()<<"   "<<jetAK8puppi_ptJEC<<"  "<<drak8jetmatch1<<endl;
            //if(jetAK8puppi_ptJEC>0) cout<<ijetAK8_j1->eta()<<"   "<<ijetAK8_j1->phi()<<"   "<<jetAK8puppi_eta<<"   "<<jetAK8puppi_phi<<"   "<<endl;
            //if(jetAK8puppi_ptJEC_2>0) cout<<ijetAK8_j2->pt()<<"   "<<jetAK8puppi_ptJEC_2<<"  "<<drak8jetmatch2<<endl;
            //if(jetAK8puppi_ptJEC_2>0) cout<<ijetAK8_j2->eta()<<"   "<<ijetAK8_j2->phi()<<"   "<<jetAK8puppi_eta_2<<"   "<<jetAK8puppi_phi_2<<"   "<<endl;
            TLorentzVector aa,bb;
            puppi_softdropj1.SetPtEtaPhiM(0,0,0,0);
            puppi_softdropj2.SetPtEtaPhiM(0,0,0,0);

            auto const & sdSubjetsPuppi = ijetAK8_j1->subjets("SoftDropPuppi");
            //cout<<(ijetAK8_j1->subjets("SoftDropPuppi")).size()<<endl;
            //cout<<ijetAK8_j1->numberOfDaughters()<<endl;
            Int_t nsj=0;
            //cout<<sdSubjetsPuppi.at(1)->pt()<<endl;
            for ( auto const & puppiSDSJ : sdSubjetsPuppi ) {
                if(jetAK8puppi_ptJEC>0&&tmpdrak8jet1<1.0){
                    aa.SetPtEtaPhiM(puppiSDSJ->pt(),puppiSDSJ->eta(),puppiSDSJ->phi(),puppiSDSJ->mass());
                    //cout<<puppiSDSJ->jetArea()/3.14*3.14<<endl;
                    if (nsj==0)
                        ak8sj11.SetPtEtaPhiM(puppiSDSJ->pt(),puppiSDSJ->eta(),puppiSDSJ->phi(),puppiSDSJ->mass());
                    if (nsj==1)
                        ak8sj12.SetPtEtaPhiM(puppiSDSJ->pt(),puppiSDSJ->eta(),puppiSDSJ->phi(),puppiSDSJ->mass());
                    if (nsj==2)
                        ak8sj13.SetPtEtaPhiM(puppiSDSJ->pt(),puppiSDSJ->eta(),puppiSDSJ->phi(),puppiSDSJ->mass());
                    if (nsj==3)
                        ak8sj14.SetPtEtaPhiM(puppiSDSJ->pt(),puppiSDSJ->eta(),puppiSDSJ->phi(),puppiSDSJ->mass());
                    if (nsj==4)
                        ak8sj15.SetPtEtaPhiM(puppiSDSJ->pt(),puppiSDSJ->eta(),puppiSDSJ->phi(),puppiSDSJ->mass());
                    nsj++;
                    puppi_softdropj1+=aa;
                }
                //if(jetAK8puppi_ptJEC>0) cout<<nsj<<"   "<<puppiSDSJ->correctedP4(0).phi()<<"   "<<puppiSDSJ->phi()<<endl;
                //cout<<puppi_softdrop_subjet.Pt()<<"   "<<puppiSDSJ->pt()<<endl;
            }
            auto const & sdSubjetsPuppi_2 = ijetAK8_j2->subjets("SoftDropPuppi");
            Int_t nsj2=0;
            for ( auto const & puppiSDSJ_2 : sdSubjetsPuppi_2 ) {
                if(jetAK8puppi_ptJEC_2>0&&tmpdrak8jet2<1.0){
                    bb.SetPtEtaPhiM(puppiSDSJ_2->pt(),puppiSDSJ_2->eta(),puppiSDSJ_2->phi(),puppiSDSJ_2->mass());
                    if (nsj2==0)
                        ak8sj21.SetPtEtaPhiM(puppiSDSJ_2->pt(),puppiSDSJ_2->eta(),puppiSDSJ_2->phi(),puppiSDSJ_2->mass());
                    if (nsj2==1)
                        ak8sj22.SetPtEtaPhiM(puppiSDSJ_2->pt(),puppiSDSJ_2->eta(),puppiSDSJ_2->phi(),puppiSDSJ_2->mass());
                    if (nsj2==2)
                        ak8sj23.SetPtEtaPhiM(puppiSDSJ_2->pt(),puppiSDSJ_2->eta(),puppiSDSJ_2->phi(),puppiSDSJ_2->mass());
                    if (nsj2==3)
                        ak8sj24.SetPtEtaPhiM(puppiSDSJ_2->pt(),puppiSDSJ_2->eta(),puppiSDSJ_2->phi(),puppiSDSJ_2->mass());
                    if (nsj2==4)
                        ak8sj25.SetPtEtaPhiM(puppiSDSJ_2->pt(),puppiSDSJ_2->eta(),puppiSDSJ_2->phi(),puppiSDSJ_2->mass());
                    nsj2++;
                    puppi_softdropj2+=bb;
                }
            }
        }//1
//        if((nLooseEle==1||nLooseMu==1)&&jetAK8puppi_ptJEC>200&&IDLoose==1&& fabs(jetAK8puppi_eta)<2.4 && IDLoose>0&&((jetAK8puppi_ptJEC_2>100)?(fabs(jetAK8puppi_eta_2)<2.4 && IDLoose_2>0):1)&&((jetAK8puppi_ptJEC_3>100)?(fabs(jetAK8puppi_eta_3)<2.4 && IDLoose_3>0):1)) outTree_->Fill();
          outTree_->Fill();
    outTreew_->Fill();
	}
    
    else {
        outTreew_->Fill();
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

    L1prefiring = -99;
    num_correctpuppijets = -99;
    num_totalpuppijets = -99;
    L1prefiringup = -99;
    L1prefiringdown = -99;
    
    jetAK8puppi_ptJEC         = -99;
    jetAK8puppi_eta         = -99;
    jetAK8puppi_phi         = -99;
    jetAK8puppi_tau1         = -99;
    jetAK8puppi_tau2         = -99;
    jetAK8puppi_tau3         = -99;
    jetAK8puppi_tau21         = -99;
    jetAK8puppi_tau4         = -99;
    jetAK8puppi_tau42         = -99;
    // DeepAK8
    jetAK8puppi_dnnTop        = -99;
    jetAK8puppi_dnnW          = -99;
    jetAK8puppi_dnnH4q        = -99;
    jetAK8puppi_dnnTop_2        = -99;
    jetAK8puppi_dnnW_2          = -99;
    jetAK8puppi_dnnH4q_2        = -99;
    jetAK8puppi_dnnTop_3        = -99;
    jetAK8puppi_dnnW_3          = -99;
    jetAK8puppi_dnnH4q_3        = -99;
    jetAK8puppi_dnnZ        = -99;
    jetAK8puppi_dnnZbb        = -99;
    jetAK8puppi_dnnHbb        = -99;
    jetAK8puppi_dnnZ_2        = -99;
    jetAK8puppi_dnnZbb_2        = -99;
    jetAK8puppi_dnnHbb_2        = -99;
    jetAK8puppi_dnnZ_3        = -99;
    jetAK8puppi_dnnZbb_3        = -99;
    jetAK8puppi_dnnHbb_3        = -99;
    
    jetAK8puppi_dnnqcd        = -99;
    jetAK8puppi_dnntop        = -99;
    jetAK8puppi_dnnw          = -99;
    jetAK8puppi_dnnz          = -99;
    jetAK8puppi_dnnzbb        = -99;
    jetAK8puppi_dnnhbb        = -99;
    jetAK8puppi_dnnh4q        = -99;
    jetAK8puppi_dnnqcd_2        = -99;
    jetAK8puppi_dnntop_2        = -99;
    jetAK8puppi_dnnw_2          = -99;
    jetAK8puppi_dnnz_2          = -99;
    jetAK8puppi_dnnzbb_2        = -99;
    jetAK8puppi_dnnhbb_2        = -99;
    jetAK8puppi_dnnh4q_2        = -99;
    jetAK8puppi_dnnqcd_3        = -99;
    jetAK8puppi_dnntop_3        = -99;
    jetAK8puppi_dnnw_3          = -99;
    jetAK8puppi_dnnz_3          = -99;
    jetAK8puppi_dnnzbb_3        = -99;
    jetAK8puppi_dnnhbb_3        = -99;
    jetAK8puppi_dnnh4q_3        = -99;

    // Decorrelated DeepAK8
    jetAK8puppi_dnnDecorrTop        = -99;
    jetAK8puppi_dnnDecorrW          = -99;
    jetAK8puppi_dnnDecorrH4q        = -99;
    jetAK8puppi_dnnDecorrTop_2        = -99;
    jetAK8puppi_dnnDecorrW_2          = -99;
    jetAK8puppi_dnnDecorrH4q_2        = -99;
    jetAK8puppi_dnnDecorrTop_3        = -99;
    jetAK8puppi_dnnDecorrW_3          = -99;
    jetAK8puppi_dnnDecorrH4q_3        = -99;
    jetAK8puppi_dnnDecorrZ        = -99;
    jetAK8puppi_dnnDecorrZbb        = -99;
    jetAK8puppi_dnnDecorrHbb        = -99;
    jetAK8puppi_dnnDecorrZ_2        = -99;
    jetAK8puppi_dnnDecorrZbb_2        = -99;
    jetAK8puppi_dnnDecorrHbb_2        = -99;
    jetAK8puppi_dnnDecorrZ_3        = -99;
    jetAK8puppi_dnnDecorrZbb_3        = -99;
    jetAK8puppi_dnnDecorrHbb_3        = -99;
    jetAK8puppi_dnnDecorrbb        = -99;
    jetAK8puppi_dnnDecorrcc        = -99;
    jetAK8puppi_dnnDecorrbbnog        = -99;
    jetAK8puppi_dnnDecorrccnog        = -99;
    jetAK8puppi_dnnDecorrbb_2        = -99;
    jetAK8puppi_dnnDecorrcc_2        = -99;
    jetAK8puppi_dnnDecorrbbnog_2        = -99;
    jetAK8puppi_dnnDecorrccnog_2        = -99;
    jetAK8puppi_dnnDecorrbb_3        = -99;
    jetAK8puppi_dnnDecorrcc_3        = -99;
    jetAK8puppi_dnnDecorrbbnog_3        = -99;
    jetAK8puppi_dnnDecorrccnog_3        = -99;
    
    jetAK8puppi_dnnDecorrqcd        = -99;
    jetAK8puppi_dnnDecorrtop        = -99;
    jetAK8puppi_dnnDecorrw          = -99;
    jetAK8puppi_dnnDecorrz          = -99;
    jetAK8puppi_dnnDecorrzbb        = -99;
    jetAK8puppi_dnnDecorrhbb        = -99;
    jetAK8puppi_dnnDecorrh4q        = -99;
    jetAK8puppi_dnnDecorrqcd_2        = -99;
    jetAK8puppi_dnnDecorrtop_2        = -99;
    jetAK8puppi_dnnDecorrw_2          = -99;
    jetAK8puppi_dnnDecorrz_2          = -99;
    jetAK8puppi_dnnDecorrzbb_2        = -99;
    jetAK8puppi_dnnDecorrhbb_2        = -99;
    jetAK8puppi_dnnDecorrh4q_2        = -99;
    jetAK8puppi_dnnDecorrqcd_3        = -99;
    jetAK8puppi_dnnDecorrtop_3        = -99;
    jetAK8puppi_dnnDecorrw_3          = -99;
    jetAK8puppi_dnnDecorrz_3          = -99;
    jetAK8puppi_dnnDecorrzbb_3        = -99;
    jetAK8puppi_dnnDecorrhbb_3        = -99;
    jetAK8puppi_dnnDecorrh4q_3        = -99;


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
    
    jetAK8puppi_ptJEC_3         = -99;
    jetAK8puppi_eta_3         = -99;
    jetAK8puppi_phi_3         = -99;
    jetAK8puppi_tau1_3         = -99;
    jetAK8puppi_tau2_3         = -99;
    jetAK8puppi_tau3_3         = -99;
    jetAK8puppi_tau21_3         = -99;
    jetAK8puppi_tau4_3         = -99;
    jetAK8puppi_tau42_3         = -99;
    jetAK8puppi_sd_3         = -99;
    jetAK8puppi_sdJEC_3         = -99;
    jetAK8puppi_sdcorr_3         = -99;

    vbfeta=-10.;
    vbfmjj=-10.;
    vbftag=0;
    nj1=-1;
    nj2=-1;

    yVlep          = -99;
    phiVlep        = -99;
    massVlep       = -99;
    mtVlep         = -99;
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
    
    gen_tau_pt     = -99;
    gen_tau_eta    = -99;
    gen_tau_phi    = -99;
    gen_tau_e      = -99;
    gen_tau_pt_2     = -99;
    gen_tau_eta_2    = -99;
    gen_tau_phi_2    = -99;
    gen_tau_e_2      = -99;
    gen_tau_pt_3     = -99;
    gen_tau_eta_3    = -99;
    gen_tau_phi_3    = -99;
    gen_tau_e_3      = -99;

    gen_nele_pt     = -99;
    gen_nele_eta    = -99;
    gen_nele_phi    = -99;
    gen_nele_e      = -99;
    gen_nmu_pt     = -99;
    gen_nmu_eta    = -99;
    gen_nmu_phi    = -99;
    gen_nmu_e      = -99;
    gen_nele_pt_2     = -99;
    gen_nele_eta_2    = -99;
    gen_nele_phi_2    = -99;
    gen_nele_e_2      = -99;
    gen_nmu_pt_2     = -99;
    gen_nmu_eta_2    = -99;
    gen_nmu_phi_2    = -99;
    gen_nmu_e_2      = -99;
    gen_nele_pt_3     = -99;
    gen_nele_eta_3    = -99;
    gen_nele_phi_3    = -99;
    gen_nele_e_3      = -99;
    gen_nmu_pt_3     = -99;
    gen_nmu_eta_3    = -99;
    gen_nmu_phi_3    = -99;
    gen_nmu_e_3      = -99;
    
    gen_ntau_pt     = -99;
    gen_ntau_eta    = -99;
    gen_ntau_phi    = -99;
    gen_ntau_e      = -99;
    gen_ntau_pt_2     = -99;
    gen_ntau_eta_2    = -99;
    gen_ntau_phi_2    = -99;
    gen_ntau_e_2      = -99;
    gen_ntau_pt_3     = -99;
    gen_ntau_eta_3    = -99;
    gen_ntau_phi_3    = -99;
    gen_ntau_e_3      = -99;

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

    for(int j=0; j<212; j++){
        pweight[j]=0.0;
    }


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
        ak4jet_deepcsvudsg[ii] = -99;
        ak4jet_deepcsvb[ii] = -99;
        ak4jet_deepcsvc[ii] = -99;
        ak4jet_deepcsvbb[ii] = -99;
        ak4jet_deepcsvcc[ii] = -99;
        ak4jet_IDLoose[ii] = -99;
        ak4jet_IDTight[ii] = -99;
    }
    
    ak8sj11.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj12.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj13.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj14.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj15.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj21.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj22.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj23.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj24.SetPtEtaPhiM(0,-99,-99,-99);
    ak8sj25.SetPtEtaPhiM(0,-99,-99,-99);

    for(int i=0;i<4;i++){
        jetAK8puppi_mass1[i] = -99;
        jetAK8puppi_pt1[i] = -99;
        jetAK8puppi_eta1[i] = -99;

        pttau[i] = -99 ;etatau[i] = -99 ;phitau[i] = -99 ;etau[i] = -99 ;pdgidtau[i] = -99 ;
        pttau_2[i] = -99 ;etatau_2[i] = -99 ;phitau_2[i] = -99 ;etau_2[i] = -99 ;pdgidtau_2[i] = -99 ;
        pttau_3[i] = -99 ;etatau_3[i] = -99 ;phitau_3[i] = -99 ;etau_3[i] = -99 ;pdgidtau_3[i] = -99 ;
    }
 
    for(int i=0;i<3;i++){
        ptq[i] = -99 ;etaq[i] = -99 ;phiq[i] = -99 ;eq[i] = -99 ;pdgidq[i] = -99 ;
        ptq_2[i] = -99 ;etaq_2[i] = -99 ;phiq_2[i] = -99 ;eq_2[i] = -99 ;pdgidq_2[i] = -99 ;
        ptq_3[i] = -99 ;etaq_3[i] = -99 ;phiq_3[i] = -99 ;eq_3[i] = -99 ;pdgidq_3[i] = -99 ;
    }

    IDLoose = false;
    IDTight = false;
    IDLoose_2 = false;
    IDTight_2 = false;
    IDLoose_3 = false;
    IDTight_3 = false;

    isHighPt = false;
    isHEEP = false;
    //rho = -99;
    iso = -99;
    isoCut = -99;
    et = -99;
    trackIso = -99;
    muchaiso=-99.;
    muneuiso=-99.;
    muphoiso=-99.;
    muPU=-99.;
    muisolation=-99.;

    METraw_et = -99;
    METraw_phi = -99;
    METraw_sumEt = -99;
    MET_et = -99;
    MET_phi = -99;
    MET_sumEt = -99;
    MET_corrPx = -99;
    MET_corrPy = -99;

    candMasspuppiJEC     =  -99;
    m_jlv     =  -99;

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

    for(int i=0;i<5;i++){
        ptgenwl[i]=-99;etagenwl[i]=-99;phigenwl[i]=-99;massgenwl[i]=-99;taggenwl[i]=-99;taggenwmother[i]=-99;
        genw_q1_pt[i]=-99;genw_q1_phi[i]=-99;genw_q1_eta[i]=-99;genw_q1_e[i]=-99;genw_q1_pdg[i]=-99;
        genw_q2_pt[i]=-99;genw_q2_phi[i]=-99;genw_q2_eta[i]=-99;genw_q2_e[i]=-99;genw_q2_pdg[i]=-99;
        ptgenzl[i]=-99;etagenzl[i]=-99;phigenzl[i]=-99;massgenzl[i]=-99;taggenzl[i]=-99;
        ptgenwf[i]=-99;etagenwf[i]=-99;phigenwf[i]=-99;massgenwf[i]=-99;
        ptgenzf[i]=-99;etagenzf[i]=-99;phigenzf[i]=-99;massgenzf[i]=-99;
    }
    for(int i=0;i<10;i++){
        ptgengl[i]=-99;etagengl[i]=-99;phigengl[i]=-99;egengl[i]=-99;
        ptgengf[i]=-99;etagengf[i]=-99;phigengf[i]=-99;egengf[i]=-99;
        mothergengf[i]=-99;mmothergengf[i]=-99;
    }
    for(int i=0;i<5;i++){
        ptgenq1l[i]=-99;etagenq1l[i]=-99;phigenq1l[i]=-99;egenq1l[i]=-99;
        ptgenq1f[i]=-99;etagenq1f[i]=-99;phigenq1f[i]=-99;egenq1f[i]=-99;
        ptgenq2l[i]=-99;etagenq2l[i]=-99;phigenq2l[i]=-99;egenq2l[i]=-99;
        ptgenq2f[i]=-99;etagenq2f[i]=-99;phigenq2f[i]=-99;egenq2f[i]=-99;
        ptgenq3l[i]=-99;etagenq3l[i]=-99;phigenq3l[i]=-99;egenq3l[i]=-99;
        ptgenq3f[i]=-99;etagenq3f[i]=-99;phigenq3f[i]=-99;egenq3f[i]=-99;
        ptgenq4l[i]=-99;etagenq4l[i]=-99;phigenq4l[i]=-99;egenq4l[i]=-99;
        ptgenq4f[i]=-99;etagenq4f[i]=-99;phigenq4f[i]=-99;egenq4f[i]=-99;
        ptgenq5l[i]=-99;etagenq5l[i]=-99;phigenq5l[i]=-99;egenq5l[i]=-99;
        ptgenq5f[i]=-99;etagenq5f[i]=-99;phigenq5f[i]=-99;egenq5f[i]=-99;
        mmothergenq1f[i]=-99;mmothergenq2f[i]=-99;mmothergenq3f[i]=-99;mmothergenq4f[i]=-99;mmothergenq5f[i]=-99;

    }
    gent_b_pt=-99;gent_b_phi=-99;gent_b_eta=-99;gent_b_mass=-99;
    genantit_b_pt=-99;genantit_b_phi=-99;genantit_b_eta=-99;genantit_b_mass=-99;
    gent_w_pt=-99;gent_w_phi=-99;gent_w_eta=-99;gent_w_mass=-99;
    genantit_w_pt=-99;genantit_w_phi=-99;genantit_w_eta=-99;genantit_w_mass=-99;
    gent_w_q1_pt=-99;gent_w_q1_phi=-99;gent_w_q1_eta=-99;gent_w_q1_e=-99;gent_w_q1_pdg=-99;
    genantit_w_q1_pt=-99;genantit_w_q1_phi=-99;genantit_w_q1_eta=-99;genantit_w_q1_e=-99;genantit_w_q1_pdg=-99;
    gent_w_q2_pt=-99;gent_w_q2_phi=-99;gent_w_q2_eta=-99;gent_w_q2_e=-99;gent_w_q2_pdg=-99;
    genantit_w_q2_pt=-99;genantit_w_q2_phi=-99;genantit_w_q2_eta=-99;genantit_w_q2_e=-99;genantit_w_q2_pdg=-99;
    gent_w_tag=-99;genantit_w_tag=-99;

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
	/*
     edm::Handle<LHERunInfoProduct> runw;
        typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
        
        iRun.getByToken(LhestrToken_,runw);
        LHERunInfoProduct myLHERunInfoProduct = *(runw.product());
        
        for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
            std::cout << iter->tag() << std::endl;
            std::vector<std::string> lines = iter->lines();
            for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
                std::cout << lines.at(iLine);
            }
        }*/
    std::cout << "EDBRTreeMaker endJob()... endRun" << std::endl;
}


void
EDBRTreeMaker::endJob() {
    std::cout << "EDBRTreeMaker endJob()..." << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(EDBRTreeMaker);
