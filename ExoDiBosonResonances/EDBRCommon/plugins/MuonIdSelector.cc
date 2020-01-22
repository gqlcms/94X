
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
 *   - Smucts "loose" and "tight" muons needed for V-boson analysis.
 *   - Saves collection of the reference vectors of muons passing the 
 *     required muctron ID.
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
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonCocktails.h"
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
class MuonIdSelector : public edm::EDProducer
{
public:
  // construction/destruction
  MuonIdSelector(const edm::ParameterSet& iConfig);
  virtual ~MuonIdSelector();
  
  // member functions
  void produce(edm::Event& iEvent,const edm::EventSetup& iSetup);
  void endJob();

private:  
  // member data
  // edm::InputTag  src_;
  std::string    moduleLabel_;
  std::string    idLabel_;  
  bool           useDetectorIsolation_;
  bool           applyTightID_;
  bool           applyMediumID_;
  bool           applyLooseID_;
  bool           applyVetoID_;

  unsigned int nTot_;
  unsigned int nPassed_;
  int nPassPteta_;//qun
  edm::EDGetTokenT<pat::MuonCollection> MuonToken_;
  edm::EDGetTokenT<reco::VertexCollection> VertexToken_;

};



////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

	//______________________________________________________________________________
MuonIdSelector::MuonIdSelector(const edm::ParameterSet& iConfig)
  : moduleLabel_(iConfig.getParameter<std::string>   ("@module_label"))
  , idLabel_(iConfig.existsAs<std::string>("idLabel") ? iConfig.getParameter<std::string>("idLabel") : "loose")
  , useDetectorIsolation_(iConfig.existsAs<bool>("useDetectorIsolation") ? iConfig.getParameter<bool>("useDetectorIsolation") : false)
  , nTot_(0)
  , nPassed_(0)
  , MuonToken_ (consumes<pat::MuonCollection> (iConfig.getParameter<edm::InputTag>( "src" ) ) )
  , VertexToken_ (consumes<reco::VertexCollection> (iConfig.getParameter<edm::InputTag>( "vertex" ) ) )
{

  produces<std::vector<pat::Muon> >();
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
MuonIdSelector::~MuonIdSelector(){}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////
 
//______________________________________________________________________________
void MuonIdSelector::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{

  /////// Pileup density "rho" in the event from fastJet pileup calculation /////
//  double fastJetRho;
//  fastJetRho=-99.;
//  edm::Handle<double> rho;
//  iEvent.getByLabel("fixedGridRhoFastjetAll",rho);
//  fastJetRho = *rho;
 
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

  std::unique_ptr<std::vector<pat::Muon> > passingMuons(new std::vector<pat::Muon >);

  edm::Handle<pat::MuonCollection > muons;
  iEvent.getByToken(MuonToken_, muons);
  
  bool* isPassing = new bool[muons->size()];
  int nPassPteta_ = 0;
  bool isPassPteta =false;
//  int muIndex = -1;

  for(unsigned int iMuon=0; iMuon<muons->size(); iMuon++) { 

    isPassing[iMuon]=false;

    const pat::Muon& mu = muons->at(iMuon);

    // -------- Make sure that the muctron is within acceptance ------

//    if (vtxs->size() > 0) {
//        reco::VertexRef vtx(vtxs, 0);}
    bool isTight  = false;  /////// <--- equivalent to WP70
    bool isMedium = false;  /////// <--- equivalent to WP80
    bool isLoose  = false;  /////// <--- equivalent to WP90
    bool isVeto    = false;  /////// <--- the loosest cut for veto

    double trackIso = mu.trackIso();

    double pt = mu.pt();
    double eta = mu.eta();
   // double pt = mu.tunePMuonBestTrack()->pt(); //mu.pt();
   // double eta = mu.tunePMuonBestTrack()->eta(); //mu.eta();

    isTight = mu.isHighPtMuon( *firstGoodVertex ) && (pt > 45) && (fabs(eta) < 2.4) && (trackIso/pt<0.1);//&& (trackIso/pt<0.1);// && (abs(eta)<2.4);//->position());
    isMedium = mu.isHighPtMuon( *firstGoodVertex );
    isLoose = mu.isHighPtMuon( *firstGoodVertex ) && (pt > 20) && (fabs(eta) < 2.4) && (trackIso/pt<0.1);
    // ---------- cut-based ID -----------------

    isPassPteta = (pt > 45) && (fabs(eta) < 2.4) && (trackIso/pt<0.1);
    if(isPassPteta && isTight) nPassPteta_ = nPassPteta_ +1;
    /// ------- Finally apply smuction --------
    if(applyTightID_ && isTight)   isPassing[iMuon]= true;
    if(applyMediumID_ && isMedium) isPassing[iMuon]= true;
    if(applyLooseID_ && isLoose)   isPassing[iMuon]= true;
    if(applyVetoID_ && isVeto) isPassing[iMuon]= true;
    
 }
/*  int nPassAMu = 0;
  for(unsigned int iMuon=0; iMuon<muons->size(); iMuon++) {

    const pat::Muon& mu1 = muons->at(iMuon);
    bool isTight  = false;  /////// <--- equivalent to WP70
    double trackIso = mu1.trackIso();
    double pt = mu1.pt();
    double eta = mu1.eta();
    isTight = mu1.isHighPtMuon(vtxs->at(0)) ;
    isPassPteta = (pt > 20) && (fabs(eta) < 2.4) && (trackIso/pt<0.1) && (int(iMuon) != muIndex);
    if(isPassPteta && isTight) nPassAMu = nPassAMu +1;
}
*/



/*  unsigned int counter=0;
  edm::View<pat::Muon>::const_iterator tIt, endcands = muons->end();
  for (tIt = muons->begin(); tIt != endcands; ++tIt, ++counter) {
    if(isPassing[counter] ) passingMuons->push_back( *tIt );  
    //if(isPassing[counter] && (nPassAMu==0)) passingMuons->push_back( *tIt );  
  }
*/

 for (unsigned int iMuon = 0; iMuon < muons -> size(); iMuon ++)
   {     if(isPassing[iMuon]) passingMuons->push_back( muons -> at(iMuon) );  
  }

  nTot_  +=muons->size();
  nPassed_+=passingMuons->size();
  //std::cout<<"isPassing"<< isPassing << "nPassPteta_" << nPassPteta_ << std::endl;

  delete [] isPassing;  
  iEvent.put(std::move(passingMuons));
}

 
//______________________________________________________________________________
void MuonIdSelector::endJob()
{
  std::stringstream ss;
  ss<<"nTot="<<nTot_<<" nPassed="<<nPassed_
    <<" effPassed="<<100.*(nPassed_/(double)nTot_)<<"%\n";
  std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++"
	   <<"\n"<<moduleLabel_<<"(MuonIdSelector) SUMMARY:\n"<<ss.str()
	   <<"++++++++++++++++++++++++++++++++++++++++++++++++++"
	   << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// plugin definition
////////////////////////////////////////////////////////////////////////////////
typedef MuonIdSelector   			    PATMuonIdSelector;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATMuonIdSelector);
