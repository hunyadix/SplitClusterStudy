//////////////////////////
// EDM plugin libraries //
//////////////////////////

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// For the tracker topology handle
#include "FWCore/Framework/interface/ESHandle.h"

///////////
// Tools //
///////////

// Tracker topology
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"

// Digi token
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
// Clusters token
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

////////////////////
// Root libraries //
////////////////////

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
// #include <TH1F.h>
// #include <TH2I.h>
// #include <TRandom3.h>

////////////////
// C++ system //
////////////////

#include <iostream>
#include <string>
#include <vector>

#include "../ModuleDataFetcher/interface/ModuleDataProducer.h"

class ClustData
{
	public:
		float x;
		float y;
		int sizeX;
		int sizeY;
		int clusterIndex;
		int clusterSize;
		float charge;
		std::vector<int> pixelsCol;
		std::vector<int> pixelsRow;
		std::vector<int> pixelsAdc;
		std::vector<int> pixelsMarker;
		ClustData() { init(); }
		~ClustData() {}
		void init()
		{
			x            = NOVAL_F;
			y            = NOVAL_F;
			sizeX        = NOVAL_I;
			sizeY        = NOVAL_I;
			clusterIndex = NOVAL_I;
			clusterSize  = 0;
			charge       = NOVAL_F;
			pixelsCol    .clear();
			pixelsRow    .clear();
			pixelsAdc    .clear();
			pixelsMarker .clear();
		};
};

class Cluster : public ClustData
{
	public:

		ModuleData mod; // offline module number
		ModuleData mod_on; // online module number

		Cluster() { mod.init(); mod_on.init();};
		void init() 
		{
			ClustData::init();
			mod.init();
			mod_on.init();
		}
};

class EventClustersDataArrays
{
	public:
		int                           size;
		std::vector<float>            x;
		std::vector<float>            y;
		std::vector<int>              sizeX;
		std::vector<int>              sizeY;
		std::vector<int>              clusterIndex;
		std::vector<int>              clusterSize;
		std::vector<float>            charge;
		std::vector<std::vector<int>> pixelsCol;
		std::vector<std::vector<int>> pixelsRow;
		std::vector<std::vector<int>> pixelsAdc;
		std::vector<std::vector<int>> pixelsMarker;
		// Module
		std::vector<int>              det;
		std::vector<int>              layer;
		std::vector<int>              ladder;
		std::vector<int>              module;
		std::vector<int>              half;
		std::vector<int>              outer;
		std::vector<int>              side;
		std::vector<int>              disk;
		std::vector<int>              blade;
		std::vector<int>              panel;
		std::vector<int>              ring;
		std::vector<int>              shl;
		std::vector<int>              federr;
		EventClustersDataArrays()
		{
			clear();
		}
		~EventClustersDataArrays() {}
		void clear()
		{
			size = 0;
			x             .clear();
			y             .clear();
			sizeX         .clear();
			sizeY         .clear();
			clusterIndex  .clear();
			clusterSize   .clear();
			charge        .clear();
			pixelsCol     .clear();
			pixelsRow     .clear();
			pixelsAdc     .clear();
			pixelsMarker  .clear();
			// Module
			det          .clear();
			layer        .clear();
			ladder       .clear();
			module       .clear();
			half         .clear();
			outer        .clear();
			side         .clear();
			disk         .clear();
			blade        .clear();
			panel        .clear();
			ring         .clear();
			shl          .clear();
			federr       .clear();
		}
		void fill(const ClustData& clustData, const ModuleData& module_on)
		{
			x            .push_back(clustData.x);
			y            .push_back(clustData.y);
			sizeX        .push_back(clustData.sizeX);
			sizeY        .push_back(clustData.sizeY);
			clusterIndex .push_back(clustData.clusterIndex);
			clusterSize  .push_back(clustData.clusterSize);
			charge       .push_back(clustData.charge);
			pixelsCol    .push_back(clustData.pixelsCol);
			pixelsRow    .push_back(clustData.pixelsRow);
			pixelsAdc    .push_back(clustData.pixelsAdc);
			pixelsMarker .push_back(clustData.pixelsMarker);	
			// Module
			det          .push_back(module_on.det);
			layer        .push_back(module_on.layer);
			ladder       .push_back(module_on.ladder);
			module       .push_back(module_on.module);
			half         .push_back(module_on.half);
			outer        .push_back(module_on.outer);
			side         .push_back(module_on.side);
			disk         .push_back(module_on.disk);
			blade        .push_back(module_on.blade);
			panel        .push_back(module_on.panel);
			ring         .push_back(module_on.ring);
			shl          .push_back(module_on.shl);
			federr       .push_back(module_on.federr);
			++size;
		}
};

class BenchmarkingTest : public edm::EDAnalyzer
{
	private:
	// Data Fields
		edm::ParameterSet       iConfig;
		std::string             ntupleOutputFilename = "Ntuple_benchmarking.root";
		TFile*                  ntupleOutputFile;
		TTree*                  clusterTree;
		TTree*                  eventClustersTree;
		Cluster                 clusterField;
		EventClustersDataArrays eventClustersField;
	// Tokens
		edm::EDGetTokenT<edm::DetSetVector<PixelDigi>>         digiFlagsToken;
		edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>> clustersToken;
	public:
		BenchmarkingTest(edm::ParameterSet const& iConfigArg);
		virtual ~BenchmarkingTest();
	// From EDAnalyzer
		virtual void beginJob();
		virtual void endJob();
		virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
		virtual void beginRun(edm::Run const&, edm::EventSetup const&);
		virtual void endRun(edm::Run const&, edm::EventSetup const&);
		virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
		virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

		static int                  getDigiMarkerValue(const SiPixelCluster::Pixel& pixelToCheck, const edm::DetSet<PixelDigi>& digiFlags);
	//Utility
		void clearAllContainers();
};

BenchmarkingTest::BenchmarkingTest(edm::ParameterSet const& iConfigArg) : iConfig(iConfigArg)
{
	// Tokens
	digiFlagsToken           = consumes<edm::DetSetVector<PixelDigi>>          (edm::InputTag("simSiPixelDigis", "dcolLostNeighbourDigiFlags"));
	clustersToken            = consumes<edmNew::DetSetVector<SiPixelCluster>>  (edm::InputTag("siPixelClusters"));
}

BenchmarkingTest::~BenchmarkingTest() {}

void BenchmarkingTest::beginJob()
{
	ntupleOutputFile = new TFile(ntupleOutputFilename.c_str(), "RECREATE");
	clusterTree = new TTree("clustTree", "Pixel clusters");
	// Clusters
	// clusterTree -> Branch("module",       &clusterField.mod,          ModuleData::list.c_str());
	// clusterTree -> Branch("module_on",    &clusterField.mod_on,       ModuleData::list.c_str());
	clusterTree -> Branch("x",            &clusterField.x,            "x/F");
	clusterTree -> Branch("y",            &clusterField.y,            "y/F");
	clusterTree -> Branch("clust_sizeX",  &clusterField.sizeX,        "sizeX/I");
	clusterTree -> Branch("clust_sizeY",  &clusterField.sizeY,        "sizeY/I");
	clusterTree -> Branch("clust_index",  &clusterField.clusterIndex, "i/I");
	clusterTree -> Branch("clust_size",   &clusterField.clusterSize,  "size/I");
	clusterTree -> Branch("clust_charge", &clusterField.charge,       "charge/F");
	// Event Clusters
	eventClustersTree = new TTree("eventClustersTree", "Event cluster collection");
	eventClustersTree -> Branch("x",            &(eventClustersField.x));
	eventClustersTree -> Branch("y",            &(eventClustersField.y));
	eventClustersTree -> Branch("sizeX",        &(eventClustersField.sizeX));
	eventClustersTree -> Branch("sizeY",        &(eventClustersField.sizeY));
	eventClustersTree -> Branch("clusterSize",  &(eventClustersField.clusterSize));
	eventClustersTree -> Branch("charge",       &(eventClustersField.charge));
	eventClustersTree -> Branch("pixelsCol",    &(eventClustersField.pixelsCol));
	eventClustersTree -> Branch("pixelsRow",    &(eventClustersField.pixelsRow));
	eventClustersTree -> Branch("pixelsAdc",    &(eventClustersField.pixelsAdc));
	eventClustersTree -> Branch("pixelsMarker", &(eventClustersField.pixelsMarker));
	eventClustersTree -> Branch("det",          &(eventClustersField.det));
	eventClustersTree -> Branch("layer",        &(eventClustersField.layer));
	eventClustersTree -> Branch("ladder",       &(eventClustersField.ladder));
	eventClustersTree -> Branch("module",       &(eventClustersField.module));
	eventClustersTree -> Branch("half",         &(eventClustersField.half));
	eventClustersTree -> Branch("outer",        &(eventClustersField.outer));
	eventClustersTree -> Branch("side",         &(eventClustersField.side));
	eventClustersTree -> Branch("disk",         &(eventClustersField.disk));
	eventClustersTree -> Branch("blade",        &(eventClustersField.blade));
	eventClustersTree -> Branch("panel",        &(eventClustersField.panel));
	eventClustersTree -> Branch("ring",         &(eventClustersField.ring));
	eventClustersTree -> Branch("shl",          &(eventClustersField.shl));
	eventClustersTree -> Branch("federr",       &(eventClustersField.federr));
}

void BenchmarkingTest::endJob()
{
	if(!ntupleOutputFile) return;
	LogDebug("file_operations") << "Writing plots to file: \"" << ntupleOutputFilename << "\"." << std::endl;
	ntupleOutputFile -> Write();
	ntupleOutputFile -> Close();
	LogDebug("file_operations") << "File succesfully closed: \"" << ntupleOutputFilename << "\"." << std::endl;
}

void BenchmarkingTest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	// Accessing the tracker topology for module informations
	edm::ESHandle<TrackerTopology> trackerTopologyHandle;
	iSetup.get<TrackerTopologyRcd>().get(trackerTopologyHandle);
	const TrackerTopology* const trackerTopology = trackerTopologyHandle.product();
	// Fetching the markers for digis that have a dcol lost digi next to them
	edm::Handle<edm::DetSetVector<PixelDigi>> digiFlagsCollection;
	iEvent.getByToken(digiFlagsToken,         digiFlagsCollection);
	// Fetching the clusters by token
	edm::Handle<edmNew::DetSetVector<SiPixelCluster>> clusterCollection;
	iEvent.getByToken(clustersToken,                  clusterCollection);
	// Resetting data fields
	clearAllContainers();
	// Processing data
	std::map<DetId, const edm::DetSet<PixelDigi>*> detIdToMarkerPtrMap;
	for(const auto& markerSet: *digiFlagsCollection)
	{
		detIdToMarkerPtrMap.insert(std::pair<DetId, const edm::DetSet<PixelDigi>*>(markerSet.detId(), &markerSet));
	}
	for(const auto& clusterSetOnModule: *clusterCollection)
	{
		DetId detId(clusterSetOnModule.id());
		const edm::DetSet<PixelDigi>* digiFlagsOnModulePtr = nullptr;
		try {digiFlagsOnModulePtr = detIdToMarkerPtrMap.at(detId);}
		catch(const std::out_of_range& error) {std::cout << "Error: check marker detId iteration! (permissive)" << std::endl;}
		unsigned int subdetId = detId.subdetId();
		// Take only pixel clusters
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		// Get module data
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology);
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);
		for(const auto& currentCluster: clusterSetOnModule)
		{
			clusterField.mod         = mod;
			clusterField.mod_on      = mod_on;
			clusterField.x           = currentCluster.x();
			clusterField.y           = currentCluster.y();
			clusterField.sizeX       = currentCluster.sizeX();
			clusterField.sizeY       = currentCluster.sizeY();
			clusterField.clusterSize = currentCluster.size();
			clusterField.charge      = currentCluster.charge();
			clusterTree -> Fill();
			const auto currentPixelPositions = currentCluster.pixels();
			const auto currentAdcs           = currentCluster.pixelADC();
			for(int numPixel = 0; numPixel < clusterField.clusterSize && numPixel < 100; ++numPixel)
			{
				clusterField.pixelsCol    .push_back(currentPixelPositions[numPixel].x);
				clusterField.pixelsRow    .push_back(currentPixelPositions[numPixel].y);
				clusterField.pixelsAdc    .push_back(currentAdcs[numPixel] / 1000.0);
				clusterField.pixelsMarker .push_back(getDigiMarkerValue(currentPixelPositions[numPixel], *digiFlagsOnModulePtr));
			}
			eventClustersField.fill(clusterField, clusterField.mod_on);
		}
	}
	eventClustersTree -> Fill();
}

int BenchmarkingTest::getDigiMarkerValue(const SiPixelCluster::Pixel& pixelToCheck, const edm::DetSet<PixelDigi>& digiFlags)
{
	int markerValue = 0;
	int channel = PixelDigi::pixelToChannel(pixelToCheck.x, pixelToCheck.y);
	for(const auto& digiInSet: digiFlags)
	{
		int channelToCompareWith = digiInSet.channel();
		if(channel == channelToCompareWith)
		{
			markerValue = digiInSet.adc();
		}
	}
	return markerValue;
}

void BenchmarkingTest::beginRun(edm::Run const&, edm::EventSetup const&) {}
void BenchmarkingTest::endRun(edm::Run const&, edm::EventSetup const&) {}
void BenchmarkingTest::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}
void BenchmarkingTest::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

/////////////
// Utility //
/////////////

void BenchmarkingTest::clearAllContainers()
{
	clusterField.init();
	eventClustersField.clear();
}

DEFINE_FWK_MODULE(BenchmarkingTest);