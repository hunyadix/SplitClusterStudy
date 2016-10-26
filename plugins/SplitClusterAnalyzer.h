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
#include "../interface/EventClustersTree.h"
#include "../interface/ClusterDataTree.h"
#include "../interface/MergingStatisticsTree.h"
#include "../interface/PixelDataTree.h"
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
// For the merging
#include "../interface/ClusterPairMergingValidator.h"

#include "../interface/CanvasExtras.h"
#include "../interface/LayerEventPlotTriplet.h"
#include "../interface/ModuleClusterPlot.h"

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
	struct PlotDefinition
	{
		enum Type {digi, digiFromMarker, digiFromMarkerWithNeighbours, cluster};
		int startEventIndex;
		int endEventIndex;
		std::string plotTitle;
		Type type;
		std::vector<std::pair<int, int>> xAxisRange;
		std::vector<std::pair<int, int>> yAxisRange;
		LayerEventPlotTriplet histograms;
		PlotDefinition(const int& startEventIndexArg, const int& endEventIndexArg, const std::string& plotTitleArg, Type typeArg, const std::vector<std::pair<int, int>>& xAxisRangeArg, const std::vector<std::pair<int, int>>& yAxisRangeArg) :
			startEventIndex(startEventIndexArg),
			endEventIndex(endEventIndexArg),
			plotTitle(plotTitleArg),
			type(typeArg),
			xAxisRange(xAxisRangeArg),
			yAxisRange(yAxisRangeArg),
			histograms(plotTitle, plotTitle)
			{
				if(xAxisRange.size() != 3 || yAxisRange.size() != 3)
				{
					std::cout << "Check axis range sizes for plot definitions!" << std::endl;
					exit(-1);
				}
			}
	};

	std::vector<std::shared_ptr<ModuleClusterPlot>> moduleClusterPlots;

	int eventCounter = 0;
	std::vector<PlotDefinition> plotDefinitionCollection;

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
		TTree*                      mergeTree;
		MergingStatisticsData       mergeStatField;

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

		///////////////////////////////////////
		// Added for plotting event clusters //
		///////////////////////////////////////

		void        defineEventPlots();
		void        updateEventPlots(const edm::Handle<edm::DetSetVector<PixelDigi>>& digisCollection, const edm::Handle<edm::DetSetVector<PixelDigi>>& digiFlagsCollection, const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const TrackerTopology* const trackerTopology, const std::map<uint32_t, int>& fedErrors);
		void        saveReadyEventPlots();
		static int  moduleAndColToXCoordinate(const int& module, const int& col);
		static int  ladderAndRowToYCoordinate(const int& ladder, const int& row);
		static void markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers);
		static void printFillEventPlotError(const TH2D& histogram, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, const int& moduleCoordinate, const int& ladderCoordinate, const int& isReversedModule);
		static void fillEventPlot(LayerEventPlotTriplet& histogramTriplet, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, bool fillMissingPixels = false);
		static void printClusterFieldInfo(const Cluster& clusterField);
		void        fillEventPlotWithDigis(LayerEventPlotTriplet& histogramTriplet, const edm::Handle<edm::DetSetVector<PixelDigi>>& digisCollection, const TrackerTopology* const trackerTopology, const std::map<uint32_t, int>& fedErrors, bool fillMissingPixels = false);
		void        fillEventPlotWithClusters(LayerEventPlotTriplet& histogramTriplet, const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const TrackerTopology* const trackerTopology, const std::map<uint32_t, int>& fedErrors);

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