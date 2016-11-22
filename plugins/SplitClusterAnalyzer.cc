#include "SplitClusterAnalyzer.h"

#define EDM_ML_LOGDEBUG
#define ML_DEBUG

// #pragma message("Recompiling SplitClusterAnalyzer.cc...")

SplitClusterAnalyzer::SplitClusterAnalyzer(edm::ParameterSet const& iConfigArg) : iConfig(iConfigArg)
{
	// Tokens
	rawDataErrorToken        = consumes<edm::DetSetVector<SiPixelRawDataError>>(edm::InputTag("siPixelDigis"));
	pixelDigisToken         = consumes<edm::DetSetVector<PixelDigi>>          (edm::InputTag("simSiPixelDigis"));
	digiFlagsToken           = consumes<edm::DetSetVector<PixelDigi>>          (edm::InputTag("simSiPixelDigis", "dcolLostNeighbourDigiFlags"));
	clustersToken            = consumes<edmNew::DetSetVector<SiPixelCluster>>  (edm::InputTag("siPixelClusters"));
	trajTrackCollectionToken = consumes<TrajTrackAssociationCollection>(iConfig.getParameter<edm::InputTag>("trajectoryInput"));
}

SplitClusterAnalyzer::~SplitClusterAnalyzer() {}

void SplitClusterAnalyzer::beginJob()
{
	clearAllContainers();
	// Override output filename
	if(iConfig.exists("fileName"))
	{
		ntupleOutputFilename = iConfig.getParameter<std::string>("filename");
	}
	// Create output file
	ntupleOutputFile = new TFile(ntupleOutputFilename.c_str(), "RECREATE");
	if(!(ntupleOutputFile -> IsOpen()))
	{
		handleDefaultError("file_operations", "file_operations", {"Failed to open output file: ", ntupleOutputFilename});
	}
	LogDebug("file_operations") << "Output file: \"" << ntupleOutputFilename << "\" created." << std::endl;
	// Tree definitions
	eventTree = new TTree("eventTree", "Event informations");
	// EventDataTree::defineEventClusterPairsTreeBranches(eventTree, eventField, eventMergingStatisticsField);
	EventDataTree::defineEventTreeBranches(eventTree, eventField);
	clusterTree = new TTree("clustTree", "Pixel clusters");
	ClusterDataTree::defineClusterTreeBranches(clusterTree, eventField, clusterField);
	// Plots
	eventCounter = 0;
	// defineEventPlots();
	// ModuleClusterPlot(Type typeArg, const int& layerArg, const int& moduleArg, const int& ladderArg, const int& startEventArg, const int& endEventArg)

	const auto& digisType = ModuleClusterPlot::Type::digis;
	const auto& digisFromMarkersType = ModuleClusterPlot::Type::digisFromMarkers;
	const auto& digisFromMarkersWithNeighboursType = ModuleClusterPlot::Type::digisFromMarkersWithNeighbours;
	// const auto& digisAndMarkersType = ModuleClusterPlot::Type::digisAndMarkers;

	const auto defineStandardPlots = [this] (const ModuleClusterPlot::Type& type, const int& layer, const int& module, const int& ladder)
	{
		for(const int& i: range(10))
		{
			this -> moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, i, i));
		}
		this -> moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, 0, 1));
		this -> moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, 0, 9));
	};

	defineStandardPlots(digisType,                          1, 1, 2);
	defineStandardPlots(digisFromMarkersType,               1, 1, 2);
	defineStandardPlots(digisFromMarkersWithNeighboursType, 1, 1, 2);
	// defineStandardPlots(digisAndMarkersType,                1, 1, 2);
	defineStandardPlots(digisType,                          1, 4, 6);
	defineStandardPlots(digisFromMarkersType,               1, 4, 6);
	defineStandardPlots(digisFromMarkersWithNeighboursType, 1, 4, 6);
	// defineStandardPlots(digisAndMarkersType,                1, 4, 6);
}

void SplitClusterAnalyzer::endJob()
{
	if(!ntupleOutputFile) return;
	LogDebug("file_operations") << "Writing plots to file: \"" << ntupleOutputFilename << "\"." << std::endl;
	ntupleOutputFile -> Write();
	ntupleOutputFile -> Close();
	LogDebug("file_operations") << "File succesfully closed: \"" << c_blue << ntupleOutputFilename << c_def << "\"." << std::endl;
}

void SplitClusterAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	isEventMc = iEvent.id().run() == 1;
	// Accessing the tracker topology for module informations
	edm::ESHandle<TrackerTopology> trackerTopologyHandle;
	iSetup.get<TrackerTopologyRcd>().get(trackerTopologyHandle);
	const TrackerTopology* const trackerTopology = trackerTopologyHandle.product();
	// FED errors
	std::map<uint32_t, int> fedErrors = FedErrorFetcher::getFedErrors(iEvent, rawDataErrorToken);
	// Fetching pixel digis
	edm::Handle<edm::DetSetVector<PixelDigi>> digiCollection;
	iEvent.getByToken(pixelDigisToken,        digiCollection);
	// Fetching the markers for digis that have a dcol lost digi next to them
	edm::Handle<edm::DetSetVector<PixelDigi>> digiFlagsCollection;
	iEvent.getByToken(digiFlagsToken,         digiFlagsCollection);
	// Fetching the clusters by token
	edm::Handle<edmNew::DetSetVector<SiPixelCluster>> clusterCollection;
	iEvent.getByToken(clustersToken,                  clusterCollection);
	// Fetching the tracks by token
	edm::Handle<TrajTrackAssociationCollection> trajTrackCollection;
	iEvent.getByToken(trajTrackCollectionToken, trajTrackCollection);
	// Trying to access the data
	if(!digiCollection.isValid())      handleDefaultError("data access", "data_access", "Failed to fetch digi collection.");
	if(!clusterCollection  .isValid()) handleDefaultError("data access", "data_access", "Failed to fetch clusters.");
	if(!trajTrackCollection.isValid()) handleDefaultError("data access", "data_access", "Failed to fetch trajectory measurements.");
	if(isEventMc)
	{
		if(!digiFlagsCollection.isValid()) handleDefaultError("data access", "data_access", "Failed to fetch dcol lost digis.");
	}
	// Info prompts
	std::cout << process_prompt << "Deduced data type: " << c_blue << (isEventMc ? "REAL RAW DATA" : "MONTE-CARLO") << c_def << "." << std::endl;
	std::cout << "Trajectory measurements in this event: " << CMSSWPluginTools::getNumTrajCollectionEntries(trajTrackCollection) << std::endl;
	std::cout << "Clusters in this event: " << CMSSWPluginTools::getNumClusterCollectionEntries(clusterCollection) << std::endl;
	std::cout << "Digis in this event: " << CMSSWPluginTools::getNumDigiCollectionEntries(digiCollection) << std::endl;
	if(isEventMc)
	{
		std::cout << "Markers in this event (should be the same number as digis): " << CMSSWPluginTools::getNumDigiCollectionEntries(digiFlagsCollection) << std::endl;
	}
	// Resetting data fields
	clearAllContainers();
	// Processing data
	handleEvent(iEvent);
	// Preparing a trajectory to closest cluster map
	std::vector<TrajClusterAssociationData> onTrackClusters;
	handleTrajectories(trajTrackCollection, clusterCollection, trackerTopology, onTrackClusters);
	handleClusters(clusterCollection, digiFlagsCollection, onTrackClusters, trackerTopology, fedErrors);
	eventTree -> Fill();
	// updateEventPlots(digiCollection, digiFlagsCollection, clusterCollection, trackerTopology, fedErrors);
	// saveReadyEventPlots();
	// CMSSWPluginTools::printDigiCollectionInfo(digiCollection);
	// CMSSWPluginTools::printDigiCollectionInfo(digiFlagsCollection);
	ModuleClusterPlot::fillAll(digiCollection, digiFlagsCollection, trackerTopology, eventCounter);
	ModuleClusterPlot::saveAllFinished(eventCounter);
	++eventCounter;
}

void SplitClusterAnalyzer::handleTrajectories(const edm::Handle<TrajTrackAssociationCollection>& trajTrackCollection, const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const TrackerTopology* const trackerTopology, std::vector<TrajClusterAssociationData>& onTrackClusters) try
{
	if(!onTrackClusters.empty()) throw new std::runtime_error("onTrackClusters should be an empty container before filling in handleTrajectories().");
	for(const auto& currentTrackKeypair: *trajTrackCollection)
	{
		// Trajectory segments + corresponding track informations
		const edm::Ref<std::vector<Trajectory>> traj = currentTrackKeypair.key;
		const reco::TrackRef track                   = currentTrackKeypair.val; // TrackRef is actually a pointer type
		// Discarding tracks without pixel measurements
		if(!TrajAnalyzer::trajectoryHasPixelHit(traj)) continue;
		// Looping again to check hit efficiency of pixel hits
		for(auto& measurement: traj -> measurements())
		{
			// Check measurement validity
			if(!measurement.updatedState().isValid()) continue;
			// Fetch the hit
			// TransientTrackingRecHit::ConstRecHitPointer recHit = measurement.recHit();
			auto recHit = measurement.recHit();
			if(recHit -> hit() == nullptr) continue;
			const SiPixelRecHit* pixhit = dynamic_cast<const SiPixelRecHit*>(recHit -> hit());
			// Check hit qualty
			if(!recHit -> isValid())       continue;
			// Det id
			DetId detId = recHit -> geographicalId();
			uint32_t subdetid = (detId.subdetId());
			// Looking for pixel hits
			if(!TrajAnalyzer::subdetidIsOnPixel(subdetid)) continue;
			SiPixelRecHit::ClusterRef const& clusterRef = pixhit -> cluster();
			if(!clusterRef.isNonnull()) continue;
			// Position measurements
			TrajectoryStateCombiner  trajStateComb;
			TrajectoryStateOnSurface trajStateOnSurface = trajStateComb(measurement.forwardPredictedState(), measurement.backwardPredictedState());
			// LocalPoint localPosition = trajStateOnSurface.localPosition();
			// float lx = localPosition.x();
			// float ly = localPosition.y();
			// SiPixelCluster closestCluster = findClosestCluster(clusterCollection, detId.rawId(), lx, ly);
			// // Do nothing if no cluster is found
			// if(closestCluster.minPixelRow() == SiPixelCluster::MAXPOS && closestCluster.minPixelCol() == SiPixelCluster::MAXPOS) continue;
			// // float lz = localPosition.z()
			LocalTrajectoryParameters trajectoryParameters = trajStateOnSurface.localParameters();
			auto trajectoryMomentum = trajectoryParameters.momentum();
			LocalVector localTrackDirection = trajectoryMomentum / trajectoryMomentum.mag();
			float alpha = atan2(localTrackDirection.z(), localTrackDirection.x());
			float beta  = atan2(localTrackDirection.z(), localTrackDirection.y());
			// Save data
			onTrackClusters.emplace_back(clusterRef, alpha, beta);
		}
	}
}
catch(const std::exception& e) { std::cerr << error_prompt << e.what() << std::endl; }

void fillDetIdToMarkerPtrMap(std::map<DetId, const edm::DetSet<PixelDigi>*>& detIdToMarkerPtrMap, const edm::Handle<edm::DetSetVector<PixelDigi>>& digiFlagsCollection)
{
	for(const auto& markerSet: *digiFlagsCollection)
	{
		detIdToMarkerPtrMap.insert(std::make_pair<DetId, const edm::DetSet<PixelDigi>*>(markerSet.detId(), &markerSet));
	}
}

void SplitClusterAnalyzer::handleClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const edm::Handle<edm::DetSetVector<PixelDigi>>& digiFlagsCollection, std::vector<TrajClusterAssociationData>& onTrackClusters, const TrackerTopology* const trackerTopology, const std::map<uint32_t, int>& fedErrors)
{
	// Generate det id to marker set map to fetch the marked pixels on the modules
	std::map<DetId, const edm::DetSet<PixelDigi>*> detIdToMarkerPtrMap;
	if(!isEventMc) { fillDetIdToMarkerPtrMap(detIdToMarkerPtrMap, digiFlagsCollection); }
	// Looping on all the clusters
	for(const edmNew::DetSet<SiPixelCluster>& clusterSetOnModule: *clusterCollection)
	{
		// std::cerr << "Num clusters on module: " << clusterSetOnModule.size() << std::endl;
		DetId detId(clusterSetOnModule.id());
		const edm::DetSet<PixelDigi>* digiFlagsOnModulePtr = nullptr;
		try                                   { digiFlagsOnModulePtr = detIdToMarkerPtrMap.at(detId); }
		catch(const std::out_of_range& error) { std::cout << c_red << "Error: " << c_def << "check marker detId iteration! (permissive)" << std::endl; }
		unsigned int subdetId = detId.subdetId();
		// Take only pixel clusters
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		// Get module data
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, fedErrors);
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);
		// Looping on clusters on the same detector_part
		for(const auto& currentCluster: clusterSetOnModule)
		{
			// Saving the pixel vectors to save computing time
			std::vector<SiPixelCluster::Pixel> currentClusterPixels = currentCluster.pixels();
			// Save cluster data
			saveClusterData(currentCluster, mod, mod_on, *digiFlagsOnModulePtr, onTrackClusters);
		}
	}
}

void SplitClusterAnalyzer::handleEvent(const edm::Event& iEvent)
{
	// Save event data
	eventField.fill         = static_cast<int>(0); // FIXME
	eventField.run          = static_cast<int>(iEvent.id().run());
	eventField.ls           = static_cast<int>(iEvent.luminosityBlock());
	eventField.orb          = static_cast<int>(iEvent.orbitNumber());
	eventField.bx           = static_cast<int>(iEvent.bunchCrossing());
	eventField.evt          = static_cast<int>(iEvent.id().event());
}

void SplitClusterAnalyzer::saveClusterData(const SiPixelCluster& cluster, const ModuleData& mod, const ModuleData& mod_on, const edm::DetSet<PixelDigi>& digiFlags, std::vector<TrajClusterAssociationData>& onTrackClusters)
{
	clusterField.init();
	// FIXME: change this to a global cluster counting
	static int clusterIndex = 0;
	clusterField.clusterIndex = ++clusterIndex;
	clusterField.mod          = mod;
	clusterField.mod_on       = mod_on;
	clusterField.x            = cluster.x();
	clusterField.y            = cluster.y();
	clusterField.sizeX        = cluster.sizeX();
	clusterField.sizeY        = cluster.sizeY();
	clusterField.clusterSize  = cluster.size();
	clusterField.charge       = cluster.charge();
	// Info of the pixels in the cluster
	const auto currentPixelPositions = cluster.pixels();
	const auto currentAdcs           = cluster.pixelADC();
	if(static_cast<unsigned int>(clusterField.clusterSize) != currentPixelPositions.size())
	{
		std::cout << error_prompt << "Size conflict in saveClusterData()." << std::endl;
	}
	for(int numPixel = 0; numPixel < clusterField.clusterSize; ++numPixel)
	{
		clusterField.pixelsCol.push_back(currentPixelPositions[numPixel].y);
		clusterField.pixelsRow.push_back(currentPixelPositions[numPixel].x);
		clusterField.pixelsAdc.push_back(currentAdcs[numPixel] / 1000.0);
		if(isEventMc) clusterField.pixelsMarker.push_back(getDigiMarkerValue(currentPixelPositions[numPixel], digiFlags));
		else          clusterField.pixelsMarker.push_back(NOVAL_I);
	}
	auto searchResult = std::find_if(onTrackClusters.begin(), onTrackClusters.end(), [&cluster] (const TrajClusterAssociationData& toCheck) {return toCheck.clusterRef.get() == &cluster;});
	if(searchResult != onTrackClusters.end())
	{
		// std::cout << debug_prompt << "Found cluster reference in track cluster association data." << std::endl;
		// std::cin.get();
		clusterField.isOnHit = 1;
		clusterField.alpha   = searchResult -> alpha;
		clusterField.beta    = searchResult -> beta;
	}
	clusterTree -> Fill();
}

int SplitClusterAnalyzer::getDigiMarkerValue(const SiPixelCluster::Pixel& pixelToCheck, const edm::DetSet<PixelDigi>& digiFlags)
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

int SplitClusterAnalyzer::getMaxDigiMarkerValue(const SiPixelCluster& clusterToCheck, const edm::DetSet<PixelDigi>& digiFlags)
{
	int markerValue = 0;
	for(const auto& digi: clusterToCheck.pixels())
	{
		markerValue = std::max(markerValue, getDigiMarkerValue(digi, digiFlags));
	}
	return false;
}

bool SplitClusterAnalyzer::checkIfDigiIsInDetSet(const PixelDigi& digi, const edm::DetSet<PixelDigi>& digiDetSet)
{
	int markerValue = 0;
	int channel = digi.channel();
	for(const auto& digiInSet: digiDetSet)
	{
		int channelToCompareWith = digiInSet.channel();
		if(channel == channelToCompareWith)
		{
			markerValue = digiInSet.adc();
		}
	}
	return markerValue;
}

void SplitClusterAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&) {}
void SplitClusterAnalyzer::endRun(edm::Run const&, edm::EventSetup const&) {}
void SplitClusterAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}
void SplitClusterAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

/////////////
// Utility //
/////////////

void SplitClusterAnalyzer::clearAllContainers()
{
	eventField.init();
	clusterField.init();
	// eventClustersField.clear();
}

////////////////////
// Error handling //
////////////////////

void SplitClusterAnalyzer::handleDefaultError(const std::string& exceptionType, const std::string& streamType, std::string msg)
{
	edm::LogError(streamType.c_str()) << c_red << msg << c_def << std::endl;
	printEvtInfo(streamType);
	throw cms::Exception(exceptionType.c_str());
}

void SplitClusterAnalyzer::handleDefaultError(const std::string& exceptionType, const std::string& streamType, std::vector<std::string> msg)
{
	edm::LogError(streamType.c_str()) << c_red;
	for(const auto& msg_part: msg)
	{
		edm::LogError(streamType.c_str()) << msg_part;
	}
	edm::LogError(streamType.c_str()) << c_def << std::endl;
	printEvtInfo(streamType);
	throw cms::Exception(exceptionType.c_str());
}

void SplitClusterAnalyzer::printEvtInfo(const std::string& streamType)
{
	edm::LogError(streamType.c_str()) << c_blue <<
		"Run: "    << currentEvent -> id().run()        << 
		" Ls: "    << currentEvent -> luminosityBlock() << 
		" Event: " << currentEvent -> id().event()      << c_def << std::endl;
}

DEFINE_FWK_MODULE(SplitClusterAnalyzer);

///////////////////////
// CODE DUMP AREA :) //
///////////////////////
