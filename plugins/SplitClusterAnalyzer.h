#ifndef SPLIT_CLUSTER_ANALYZER_H
#define SPLIT_CLUSTER_ANALYZER_H

//////////////////////////
// EDM plugin libraries //
//////////////////////////

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// For the tracker topology handle
#include "FWCore/Framework/interface/ESHandle.h"

////////////////////////////
// Message logger service //
////////////////////////////

#include "FWCore/MessageLogger/interface/MessageLogger.h"
// Adding some colors :)
#include "../interface/ConsoleColors.h"

///////////
// Tools //
///////////

// Common tools
#include "../CMSSWPluginToolkit/interface/CMSSWPluginTools.h"
// Fed errors
#include "../FedErrorFetcher/interface/FedErrorFetcher.h"
// Tree branching
#include "../interface/EventDataTree.h"
#include "../interface/ClusterDataTree.h"
// Module data
#include "../ModuleDataFetcher/interface/ModuleDataProducer.h"
// Cluster/Pixel geometry
#include "../interface/ClusterGeometry.h"
// Tracker topology
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
// Tracks
#include "DataFormats/TrackReco/interface/Track.h"
#include "../TrajAnalyzer/interface/TrajAnalyzer.h"
// Hits
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"

// Position
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
// #include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
// Timer
#include "../TimerForBenchmarking/interface/TimerColored.h"

///////////
// Other //
///////////

// Errors token
#include "DataFormats/SiPixelRawData/interface/SiPixelRawDataError.h"
// Digi token
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
// Clusters token
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
// Tracjectories token
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"

#include "../interface/CanvasExtras.h"
#include "../interface/LayerEventPlotTriplet.h"
#include "../interface/ModuleClusterPlotForEDAnalyzers.h"

////////////////////
// Root libraries //
////////////////////

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
// #include <TH1F.h>
// #include <TH2I.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TAttFill.h>
// #include <TRandom3.h>

////////////////
// C++ system //
////////////////

#include <iostream>
#include <string>
#include <vector>

class SplitClusterAnalyzer : public edm::EDAnalyzer
{
	std::vector<std::shared_ptr<ModuleClusterPlot>> moduleClusterPlots;
	int eventCounter = 0;
	private:
		struct TrajClusterAssociationData
		{
			SiPixelRecHit::ClusterRef clusterRef;
			float                     alpha;
			float                     beta;
			TrajClusterAssociationData(SiPixelRecHit::ClusterRef clusterRefArg, float alphaArg, float betaArg) : clusterRef(clusterRefArg), alpha(alphaArg), beta(betaArg) {};
		};
		edm::ParameterSet iConfig;
		// For easier debug message generation
		int isEventMc;
		const edm::Event* currentEvent;
		// Tokens
		edm::EDGetTokenT<edm::DetSetVector<SiPixelRawDataError>> rawDataErrorToken;
		edm::EDGetTokenT<edm::DetSetVector<PixelDigi>>           pixelDigisToken;
		edm::EDGetTokenT<edm::DetSetVector<PixelDigi>>           digiFlagsToken;
		edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>>   clustersToken;
		edm::EDGetTokenT<TrajTrackAssociationCollection>         trajTrackCollectionToken;
		// Output file path
		// Default: "Ntuple_scs.root"
		std::string ntupleOutputFilename = "Ntuple_scs.root";
		TFile*      ntupleOutputFile;
		// Tree definition
		TTree*                      eventTree;
		EventData                   eventField;
		// TTree*                      eventClustersTree;
		// EventClustersDataArrays     eventClustersField;
		TTree*                      clusterTree;
		Cluster                     clusterField;

		/////////////////////
		// Data processing //
		/////////////////////

		SiPixelCluster              findClosestCluster(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const uint32_t& rawId, const float& lx, const float& ly);
		void                        handleEvent(const edm::Event& iEvent);
		void                        handleTrajectories(const edm::Handle<TrajTrackAssociationCollection>& trajTrackCollection, const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const TrackerTopology* const trackerTopology, std::vector<TrajClusterAssociationData>& onTrackClusters);
		void                        handleClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const edm::Handle<edm::DetSetVector<PixelDigi>>& digiFlagsCollection, std::vector<TrajClusterAssociationData>& onTrackClusters, const TrackerTopology* const trackerTopology, const std::map<uint32_t, int>& fedErrors);
		void                        saveClusterData(const SiPixelCluster& cluster, const ModuleData& mod, const ModuleData& mod_on, const edm::DetSet<PixelDigi>& digiFlags, std::vector<TrajClusterAssociationData>& onTrackClusters);
		void                        saveMergingData(const SiPixelCluster& currentCluster, const SiPixelCluster& clusterToMerge, const std::vector<SiPixelCluster::Pixel>& currentClusterPixels, const std::vector<SiPixelCluster::Pixel>& clusterToMergePixels, bool isCurrentClusterSplit, bool isMergeableClusterSplit, const ModuleData& mod, const ModuleData& mod_on);
		void                        savePixelData(const SiPixelCluster::Pixel& pixelToSave, const ModuleData& mod, const ModuleData& mod_on, const edm::DetSet<PixelDigi>& digiFlagsCollection);
		void                        reserveMemoryForEventClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection);

		// static unsigned int         getNumClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection);
		static bool                 checkClusterPairOrder(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
		static int                  getDigiMarkerValue(const SiPixelCluster::Pixel& pixelToCheck, const edm::DetSet<PixelDigi>& digiFlags);
		static int                  getMaxDigiMarkerValue(const SiPixelCluster& clusterToCheck, const edm::DetSet<PixelDigi>& digiFlags);
		static bool                 checkIfDigiIsInDetSet(const PixelDigi& digi, const edm::DetSet<PixelDigi>& digiFlags);
		////////////////////
		// Error handling //
		////////////////////

		void handleDefaultError(const std::string& exceptionType, const std::string& streamType, std::string msg);
		void handleDefaultError(const std::string& exceptionType, const std::string& streamType, std::vector<std::string> msg);
		void printEvtInfo(const std::string& streamType);

		/////////////
		// Utility //
		/////////////

		void clearAllContainers();

	public:
		SplitClusterAnalyzer(edm::ParameterSet const& iConfigArg);
		virtual ~SplitClusterAnalyzer();
		virtual void beginJob();
		virtual void endJob();
		virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
		virtual void beginRun(edm::Run const&, edm::EventSetup const&);
		virtual void endRun(edm::Run const&, edm::EventSetup const&);
		virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
		virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
};

#endif