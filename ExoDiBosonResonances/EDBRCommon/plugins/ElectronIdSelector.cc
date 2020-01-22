
/*****************************************************************************
 * Project: CMS detector at the CERN
 *
 * Package: ElectroWeakAnalysis/VPlusJets
 *
 *
 * Authors:
 *
 *   Chayanit Asawatangtrakuldee chayanit@cern.ch 
 *
 * Description:
 *   - Selects "loose" and "tight" electrons needed for V-boson analysis.
 *   - Saves collection of the reference vectors of electrons passing the 
 *     required electron ID.
 * History:
 *   
 *
 *****************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
//#include "ElectroWeakAnalysis/VPlusJets/interface/ElectronEffectiveArea.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include <memory>
#include <vector>
#include <sstream>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class ElectronIdSelector : public edm::EDProducer
{
public:
  // construction/destruction
  ElectronIdSelector(const edm::ParameterSet& iConfig);
  virtual ~ElectronIdSelector();
  
  // member functions
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob();

private:  
  // member data
  edm::InputTag  src_;
  std::string    moduleLabel_;
  std::string    idLabel_; 
  bool           useDetectorIsolation_;
  bool           applyTightID_;
  bool           applyMediumID_;
  bool           applyLooseID_;
  bool           applyVetoID_;

  unsigned int nTot_;
  unsigned int nPassed_;
  float dEtaInSeed( const pat::Electron&  ele) ;
  int nPassPteta_;
  edm::EDGetTokenT<edm::View<pat::Electron>> ElectronToken_;

  edm::EDGetTokenT<reco::VertexCollection> VertexToken_;
  edm::EDGetTokenT<double> RhoToken_;

  edm::EDGetTokenT<edm::ValueMap<bool> > vidToken_; //VID is versioned ID, is the standard E/gamma ID producer which we have configured for HEEP
  edm::EDGetTokenT<edm::ValueMap<float> > trkIsolMapToken_;
};



////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
ElectronIdSelector::ElectronIdSelector(const edm::ParameterSet& iConfig)
  : moduleLabel_(iConfig.getParameter<std::string>   ("@module_label"))
  , idLabel_(iConfig.existsAs<std::string>("idLabel") ? iConfig.getParameter<std::string>("idLabel") : "loose")
  , useDetectorIsolation_(iConfig.existsAs<bool>("useDetectorIsolation") ? iConfig.getParameter<bool>("useDetectorIsolation") : false)
  , nTot_(0)
  , nPassed_(0)
  , ElectronToken_(consumes<edm::View<pat::Electron>> (iConfig.getParameter<edm::InputTag>( "src" ) ) )
  , VertexToken_(consumes<reco::VertexCollection> (iConfig.getParameter<edm::InputTag>( "vertex" ) ) )
  , RhoToken_(consumes<double> (iConfig.getParameter<edm::InputTag>( "rho") ) )
  , vidToken_(consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("vid")))
  , trkIsolMapToken_(consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("trkIsolMap")))
{
  produces<std::vector<pat::Electron> >();


  /// ------- Decode the ID criteria --------
  applyTightID_ = false;
  applyMediumID_ = false;
  applyLooseID_ = false;
  applyVetoID_ = false;

  if( (idLabel_.compare("tight")==0) || 
      (idLabel_.compare("Tight")==0) || 
      (idLabel_.compare("TIGHT")==0) ||
      (idLabel_.compare("WP70")==0) ||
      (idLabel_.compare("wp70")==0) )  
    applyTightID_ = true;
  else if( (idLabel_.compare("medium")==0) ||
      (idLabel_.compare("Medium")==0) ||
      (idLabel_.compare("MEDIUM")==0) ||
      (idLabel_.compare("WP80")==0) ||
      (idLabel_.compare("wp80")==0) )  applyMediumID_ = true;
  else if( (idLabel_.compare("loose")==0) || 
      (idLabel_.compare("Loose")==0) || 
      (idLabel_.compare("LOOSE")==0) ||
      (idLabel_.compare("WP90")==0) ||
      (idLabel_.compare("wp90")==0) )  applyLooseID_ = true;
  else if( (idLabel_.compare("veto")==0) || 
      (idLabel_.compare("Veto")==0) || 
      (idLabel_.compare("VETO")==0) ||
      (idLabel_.compare("VETOid")==0) ||
      (idLabel_.compare("VetoId")==0) )  applyVetoID_ = true;
}

 
//______________________________________________________________________________
ElectronIdSelector::~ElectronIdSelector(){}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////



//float
//ElectronIdSelector::dEtaInSeed( const pat::Electron*  ele ){
//  return ele.superCluster().isNonnull() && ele.superCluster()->seed().isNonnull() ?
//    ele.deltaEtaSuperClusterTrackAtVtx() - ele.superCluster()->eta() + ele.superCluster()->seed()->eta() : std::numeric_limits<float>::max();
//}


 
float
ElectronIdSelector::dEtaInSeed( const pat::Electron&  ele ){
		return ele.superCluster().isNonnull() && ele.superCluster()->seed().isNonnull() ?
                        ele.deltaEtaSuperClusterTrackAtVtx() - ele.superCluster()->eta() + ele.superCluster()->seed()->eta() : std::numeric_limits<float>::max();}
//______________________________________________________________________________

void ElectronIdSelector::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{

  /////// Pileup density "rho" in the event from fastJet pileup calculation /////
  double fastJetRho;
  fastJetRho=-99.;
  edm::Handle<double> rho;
  iEvent.getByToken(RhoToken_,rho);
  fastJetRho = *rho;

   edm::Handle<reco::VertexCollection> vtxs;
   iEvent.getByToken(VertexToken_, vtxs);

   reco::VertexCollection::const_iterator firstGoodVertex = vtxs->begin();
  int firstGoodVertexIdx = 0;
  for( reco::VertexCollection::const_iterator vtx = vtxs->begin(); vtx != vtxs->end(); ++vtx, ++firstGoodVertexIdx){
    bool isFake = (vtx->chi2()==0 && vtx->ndof()==0);
    // Check the goodness
    if( !isFake && vtx->ndof()>=4. && vtx->position().Rho()<=2.0 && fabs(vtx->position().Z())<=24.0) {
      firstGoodVertex = vtx;
      break;
    }
  } 

  std::unique_ptr<std::vector<pat::Electron> > passingElectrons(new std::vector<pat::Electron >);

   edm::Handle<edm::View<pat::Electron> > electrons;
   iEvent.getByToken(ElectronToken_, electrons);
 

  edm::Handle<edm::ValueMap<bool> > vid;
  edm::Handle<edm::ValueMap<float> > trkIsolMap;
  iEvent.getByToken(vidToken_,vid);
  iEvent.getByToken(trkIsolMapToken_,trkIsolMap);


  bool* isPassing = new bool[electrons->size()];
  nPassPteta_ = 0;
//  int nEl = -1;

  for(unsigned int iElec=0; iElec<electrons->size(); iElec++) { 

    isPassing[iElec]=false;

    const pat::Electron& ele = electrons->at(iElec);

    const auto elo = electrons->ptrAt(iElec);
    //std::cout<<(*vid)[elo]<<" lilili  "<<(*trkIsolMap)[elo]<<std::endl;

    // -------- Make sure that the electron is within acceptance ------
    float eta = ele.superCluster()->eta();
    bool isEB = ele.isEB() && fabs(eta) < 1.442;
    bool isEE = ele.isEE() && fabs(eta) > 1.566 && fabs(eta) < 2.5;
    //bool inAcceptance = (isEB || isEE) && (ele.ecalDrivenSeed()==1);
    float pt  = ele.pt();

    // -------- Compute Detector isolation ------
    const double PI = 4.0*atan(1.0);
    float detector_isolation = (/*ele.dr03TkSumPt()*/ (*trkIsolMap)[elo] + 
			       std::max(0.,ele.dr03EcalRecHitSumEt()-1.0) + 
			       ele.dr03HcalTowerSumEt() - 
			       PI*0.3*0.3*fastJetRho) / pt;

    float isolation = 100.;
    if(useDetectorIsolation_) isolation = detector_isolation;

    // -------- Compute ID ------
    double sigmaIEtaIEta   = ele.sigmaIetaIeta();
    double dPhiIn    = fabs(ele.deltaPhiSuperClusterTrackAtVtx());
    double dEtaIn    = fabs(ele.deltaEtaSuperClusterTrackAtVtx());
    double hoe     = ele.hadronicOverEm();
    double ooemoop = fabs((1.0/ele.ecalEnergy() - ele.eSuperClusterOverP()/ele.ecalEnergy()));
//
    // impact parameter variables
    float d0vtx         = 0.0;
    float dzvtx         = 0.0;
    if (vtxs->size() > 0) {
        reco::VertexRef vtx(vtxs, 0);    
        d0vtx = ele.gsfTrack()->dxy((*firstGoodVertex).position());
        dzvtx = ele.gsfTrack()->dz((*firstGoodVertex).position());
    } else {
        d0vtx = ele.gsfTrack()->dxy();
        dzvtx = ele.gsfTrack()->dz();
    }
    // conversion rejection variables
    bool vtxFitConversion = !(ele.passConversionVeto()==1);//ConversionTools::hasMatchedConversion(ele, conversions, beamspot.position());
    float mHits = ele.gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS);
    
//    double iso = ele.dr03EcalRecHitSumEt() + ele.dr03HcalDepth1TowerSumEt();
//    double isoCut = 2 + 0.03*et + 0.28*fastJetRho;
//    double e_el = ele.superCluster()->energy();
 

//    float dEtaInSeed = ele.superCluster().isNonnull() && ele.superCluster()->seed().isNonnull() ?
    bool isTight  = false;  /////// <--- equivalent to WP70
    bool isTight1  = false;  /////// <--- equivalent to WP70
    bool isMedium = false;  /////// <--- equivalent to WP80
    bool isLoose  = false;  /////// <--- equivalent to WP90
    bool isVeto    = false;  /////// <--- the loosest cut for veto
    double iso,isoCut;

    // ---------- cut-based ID -----------------
    double et = ele.energy()!=0. ? 
                              ele.et()/ele.energy()*ele.caloEnergy() : 0.;
if (ele.gsfTrack().isNonnull()){
    if( et > 35. ) {
         if( fabs(eta) < 1.4442 ){
              iso = ele.dr03EcalRecHitSumEt() + ele.dr03HcalDepth1TowerSumEt();
              isoCut = 2 + 0.03*et + 0.28*fastJetRho;
              if(dEtaInSeed( ele ) < 0.004 &&//dEtaInSeed < 0.004 &&
		 ele.ecalDriven() == 1 && ele.deltaPhiSuperClusterTrackAtVtx() < 0.06 &&
                 ele.hadronicOverEm() < (1./ele.superCluster()->energy()+0.05) &&
                 //ele.hadronicOverEm() < (2./ele.superCluster()->energy()+0.05) &&  //HEEPV5
                 (ele.full5x5_e2x5Max()/ele.full5x5_e5x5() > 0.94 || ele.full5x5_e1x5()/ele.full5x5_e5x5() > 0.83) &&
                 /*ele.dr03TkSumPt()*/ (*trkIsolMap)[elo] < 5. && ele.gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
                 iso < isoCut && fabs(d0vtx) < 0.02 ) isTight1 = true;
              }
         if( fabs(eta) > 1.566 && fabs(eta) < 2.5 ){
              iso = ele.dr03EcalRecHitSumEt() + ele.dr03HcalDepth1TowerSumEt();
              if( et <= 50 )
                   isoCut = 2.5 + 0.28*fastJetRho;
              else
                   isoCut = 2.5+0.03*(et-50.) + 0.28*fastJetRho;
              if( dEtaInSeed( ele ) < 0.006 &&//dEtaInSeed < 0.006 &&
		  ele.ecalDriven() == 1 &&  ele.deltaPhiSuperClusterTrackAtVtx() < 0.06 &&
                  ele.hadronicOverEm() < (5./ele.superCluster()->energy()+0.05) && ele.full5x5_sigmaIetaIeta() < 0.03 &&
                  //ele.hadronicOverEm() < (12.5/ele.superCluster()->energy()+0.05) && ele.full5x5_sigmaIetaIeta() < 0.03 && //HEEPV5
                  /*ele.dr03TkSumPt()*/(*trkIsolMap)[elo] < 5. && ele.gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS) <= 1 &&
                  iso < isoCut && fabs(d0vtx) < 0.05 ) isTight1 = true;
               }
//		std::cout<<"isTight:"<<isTight<<std::endl;
}
}
    isMedium = (pt>20.) && (mHits<=1) &&
        (isolation<0.15) && (!vtxFitConversion) &&
        ((isEB && sigmaIEtaIEta<0.01 && dPhiIn<0.06 && dEtaIn<0.004 && hoe<0.120 && ooemoop<0.050 && fabs(d0vtx)<0.020 && fabs(dzvtx)<0.100) ||
         (isEE && sigmaIEtaIEta<0.03 && dPhiIn<0.03 && dEtaIn<0.007 && hoe<0.100 && ooemoop<0.050 && fabs(d0vtx)<0.020 && fabs(dzvtx)<0.100));
    isLoose = (pt>20.) && (mHits<=1) &&
        (isolation<0.15) && (!vtxFitConversion) &&
        ((isEB && sigmaIEtaIEta<0.01 && dPhiIn<0.15 && dEtaIn<0.007 && hoe<0.120 && ooemoop<0.050 && fabs(d0vtx)<0.020 && fabs(dzvtx)<0.200) ||
         (isEE && sigmaIEtaIEta<0.03 && dPhiIn<0.10 && dEtaIn<0.009 && hoe<0.100 && ooemoop<0.050 && fabs(d0vtx)<0.020 && fabs(dzvtx)<0.200));
    isVeto = (pt>10.) && (mHits<=999) &&
        (isolation<0.15) && //(vtxFitConversion) &&
        ((isEB && sigmaIEtaIEta<0.01 && dPhiIn<0.800 && dEtaIn<0.007 && hoe<0.150 && ooemoop<999.9 && fabs(d0vtx)<0.040 && fabs(dzvtx)<0.200) ||
         (isEE && sigmaIEtaIEta<0.03 && dPhiIn<0.700 && dEtaIn<0.010 && hoe<999.9 && ooemoop<999.9 && fabs(d0vtx)<0.040 && fabs(dzvtx)<0.200));

    /// ------- Finally apply selection --------
    if( (pt > 45) && isTight1 ) isTight = true; 
    if( (pt > 35) && isTight1 )  isLoose = true;
    if(applyTightID_ && isTight)   isPassing[iElec]= true;
    if(applyMediumID_ && isMedium) isPassing[iElec]= true;
    if(applyLooseID_ && isLoose)   isPassing[iElec]= true;
    if(applyVetoID_ && isVeto) isPassing[iElec]= true;

    
 }

/*  unsigned int counter=0;
  edm::View<pat::Electron>::const_iterator tIt, endcands = electrons->end();
  for (tIt = electrons->begin(); tIt != endcands; ++tIt, ++counter) {
    //if(isPassing[counter] && (nPassAEl==0)) passingElectrons->push_back( *tIt );  
    if(isPassing[counter] ) passingElectrons->push_back( *tIt );  
  }
*/

 for (unsigned int iElectron = 0; iElectron < electrons -> size(); iElectron ++)
   {     if(isPassing[iElectron]) passingElectrons->push_back( electrons -> at(iElectron) );       
  }

  nTot_  +=electrons->size();
  nPassed_+=passingElectrons->size();

  delete [] isPassing;  
  iEvent.put(std::move(passingElectrons));
}

 
//______________________________________________________________________________
void ElectronIdSelector::endJob()
{
  std::stringstream ss;
  ss<<"nTot="<<nTot_<<" nPassed="<<nPassed_
    <<" effPassed="<<100.*(nPassed_/(double)nTot_)<<"%\n";
  std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++"
	   <<"\n"<<moduleLabel_<<"(ElectronIdSelector) SUMMARY:\n"<<ss.str()
	   <<"++++++++++++++++++++++++++++++++++++++++++++++++++"
	   << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// plugin definition
////////////////////////////////////////////////////////////////////////////////
typedef ElectronIdSelector   			    PATElectronIdSelector;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATElectronIdSelector);
