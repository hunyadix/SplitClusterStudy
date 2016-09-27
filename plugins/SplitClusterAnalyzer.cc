#include "SplitClusterAnalyzer.h"

#define EDM_ML_LOGDEBUG
#define ML_DEBUG

// #pragma message("Recompiling SplitClusterAnalyzer.cc...")

SplitClusterAnalyzer::SplitClusterAnalyzer(edm::ParameterSet const& iConfigArg) : iConfig(iConfigArg)
{
	// Tokens
	rawDataErrorToken        = consumes<edm::DetSetVector<SiPixelRawDataError>>(edm::InputTag("siPixelDigis"));
	digiFlagsToken           = consumes<edm::DetSetVector<PixelDigi>>          (edm::InputTag("simSiPixelDigis", "dcolLostNeighbourDigiFlags"));
	clustersToken            = consumes<edmNew::DetSetVector<SiPixelCluster>>  (edm::InputTag("siPixelClusters"));
	trajTrackCollectionToken = consumes<TrajTrackAssociationCollection>(iConfig.getParameter<edm::InputTag>("trajectoryInput"));

#ifdef USE_TIMER
	timer.reset(new TimerColored(timer_prompt));
#endif
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
	EventDataTree::defineEventClusterPairsTreeBranches(eventTree, eventField, eventMergingStatisticsField);
	eventClustersTree = new TTree("eventClustersTree", "Event cluster collection");
	EventClustersTree::defineEventClustersTreeBranches(eventClustersTree, eventClustersField);
	clusterTree = new TTree("clustTree", "Pixel clusters");
	ClusterDataTree::defineClusterTreeBranches(clusterTree, clusterField);
	mergeTree   = new TTree("mergeTree", "Cluster merging informations");
	MergingStatisticsTree::defineMergingStatTreeBranches(mergeTree, mergeStatField);
	pixelTree = new TTree("pixelTree", "Pixel data");
	PixelDataTree::definePixelTreeBranches(pixelTree, pixelField, eventField);
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
	// Accessing the tracker topology for module informations
	edm::ESHandle<TrackerTopology> trackerTopologyHandle;
	iSetup.get<TrackerTopologyRcd>().get(trackerTopologyHandle);
	const TrackerTopology* const trackerTopology = trackerTopologyHandle.product();
	// FED errors
	std::map<uint32_t, int> fedErrors = FedErrorFetcher::getFedErrors(iEvent, rawDataErrorToken);
	// Fetching the markers for digis that have a dcol lost digi next to them
	edm::Handle<edm::DetSetVector<PixelDigi>> digiFlagsCollection;
	iEvent.getByToken(digiFlagsToken,         digiFlagsCollection);
	// std::cout << "Succesfully fetched digiflags." << std::endl;
	// std::cout << "digiFlagsCollection -> size(): " << digiFlagsCollection -> size() << std::endl;
	// Fetching the clusters by token
	edm::Handle<edmNew::DetSetVector<SiPixelCluster>> clusterCollection;
	iEvent.getByToken(clustersToken,                  clusterCollection);
	// Fetching the tracks by token
	edm::Handle<TrajTrackAssociationCollection> trajTrackCollection;
	iEvent.getByToken(trajTrackCollectionToken, trajTrackCollection);
	// Trying to access the clusters
	// if(!dcolLostDigiCollection.isValid()) handleDefaultError("data access", "data_access", "Failed to fetch dcol lost digis.");
	if(!clusterCollection.isValid())      handleDefaultError("data access", "data_access", "Failed to fetch clusters.");
	if(!trajTrackCollection.isValid())    handleDefaultError("data access", "data_access", "Failed to fetch trajectory measurements.");
	// Resetting data fields
	clearAllContainers();
	// Processing data
	// Preparing a trajectory to closest cluster map
	TrajClusterMap trajClosestClustMap;
	trajClosestClustMap = getTrajClosestClusterMap(trajTrackCollection, clusterCollection, trackerTopology);
	handleClusters(clusterCollection, digiFlagsCollection, trackerTopology, fedErrors);
	handleEvent(iEvent);
}

SplitClusterAnalyzer::TrajClusterMap SplitClusterAnalyzer::getTrajClosestClusterMap(const edm::Handle<TrajTrackAssociationCollection>& trajTrackCollection, const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const TrackerTopology* const trackerTopology)
{
	auto mapComparator = [] (const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs)
	{
		return lhs.estimate() < rhs.estimate();
	};
	TrajClusterMap trajClosestClustMap(mapComparator);
	for(const auto& currentTrackKeypair: *trajTrackCollection)
	{
		// Trajectory segments + corresponding track informations
		const edm::Ref<std::vector<Trajectory>> traj = currentTrackKeypair.key;
		const reco::TrackRef track                   = currentTrackKeypair.val; // TrackRef is actually a pointer type
		// Discarding tracks without pixel measurements
		if(TrajAnalyzer::trajectoryHasPixelHit(traj)) continue;
		// Looping again to check hit efficiency of pixel hits
		for(auto& measurement: traj -> measurements())
		{
			// Check measurement validity
			if(!measurement.updatedState().isValid()) continue;
			auto hit = measurement.recHit();
			// Det id
			DetId detId = hit -> geographicalId();
			uint32_t subdetid = (detId.subdetId());
			// Looking for pixel hits
			bool is_pixel_hit = false;
			is_pixel_hit |= subdetid == PixelSubdetector::PixelBarrel;
			is_pixel_hit |= subdetid == PixelSubdetector::PixelEndcap;
			if(!is_pixel_hit) continue;
			// Fetch the hit
			const SiPixelRecHit* pixhit = dynamic_cast<const SiPixelRecHit*>(hit -> hit());
			// Check hit qualty
			if(!pixhit) continue;
			// Position measurements
			TrajectoryStateCombiner  trajStateComb;
			TrajectoryStateOnSurface trajStateOnSurface = trajStateComb(measurement.forwardPredictedState(), measurement.backwardPredictedState());
			LocalPoint localPosition = trajStateOnSurface.localPosition();
			float lx = localPosition.x();
			float ly = localPosition.y();
			SiPixelCluster closestCluster = findClosestCluster(clusterCollection, detId.rawId(), lx, ly);
			// Do nothing if no cluster is found
			if(closestCluster.minPixelRow() == SiPixelCluster::MAXPOS && closestCluster.minPixelCol() == SiPixelCluster::MAXPOS) continue;
			// float lz = localPosition.z()
			trajClosestClustMap.insert(std::pair<TrajectoryMeasurement, SiPixelCluster>(measurement, closestCluster));
		}
	}
	return trajClosestClustMap;
}

SiPixelCluster SplitClusterAnalyzer::findClosestCluster(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const uint32_t& rawId, const float& lx, const float& ly)
{
	auto getDistanceSquared = [] (float x1, float y1, float x2, float y2) {return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);};
	const SiPixelCluster* closestCluster = nullptr;
	float minDistanceSquared;
	// Looping on all the clusters
	for(const auto& clusterSetOnModule: *clusterCollection)
	{
		DetId detId(clusterSetOnModule.id());
		// Discarding clusters on a different module
		if(detId.rawId() != rawId) continue;
		unsigned int subdetId = detId.subdetId();
		// Discarding non-pixel clusters
		if(subdetId != PixelSubdetector::PixelBarrel && subdetId != PixelSubdetector::PixelEndcap) continue;
		// Guess that the first cluster is the closest
		closestCluster = clusterSetOnModule.begin();
		// First set the minimum distance to the distance of the first cluster
		minDistanceSquared = getDistanceSquared(closestCluster -> x(), closestCluster -> y(), lx, ly);
		// Looping on clusters on the same detector_part
		for(const auto& currentCluster: clusterSetOnModule)
		{
			float distanceSquared = getDistanceSquared(currentCluster.x(), currentCluster.y(), lx, ly);
			if(distanceSquared < minDistanceSquared)
			{
				closestCluster = &currentCluster;
				minDistanceSquared = distanceSquared;
			}
		}
	}
	if(closestCluster == nullptr) 
	{
		return SiPixelCluster();
	}
	return *closestCluster;
}

void SplitClusterAnalyzer::handleClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection, const edm::Handle<edm::DetSetVector<PixelDigi>>& digiFlagsCollection, const TrackerTopology* const trackerTopology, const std::map<uint32_t, int>& fedErrors)
{
	// Reserve place for the variables storing the cluster data
	// reserveMemoryForEventClusters(clusterCollection);
	// Generate det id to marker set map to fetch the marked pixels on the modules
	std::map<DetId, const edm::DetSet<PixelDigi>*> detIdToMarkerPtrMap;
	for(const auto& markerSet: *digiFlagsCollection)
	{
		detIdToMarkerPtrMap.insert(std::pair<DetId, const edm::DetSet<PixelDigi>*>(markerSet.detId(), &markerSet));
	}
	#ifdef USE_TIMER
		timer -> restart("Measuring the time required to handle the clusters.");
	#endif
	// Looping on all the clusters
	for(const auto& clusterSetOnModule: *clusterCollection)
	{
		// std::cerr << "Num clusters on module: " << clusterSetOnModule.size() << std::endl;
		DetId detId(clusterSetOnModule.id());
		const edm::DetSet<PixelDigi>* digiFlagsOnModulePtr = nullptr;
		try
		{
			digiFlagsOnModulePtr = detIdToMarkerPtrMap.at(detId);
		}
		catch(const std::out_of_range& error)
		{
			std::cout << c_red << "Error: " << c_def << "check marker detId iteration! (permissive)" << std::endl;
		}
		unsigned int subdetId = detId.subdetId();
		// Take only pixel clusters
		if((subdetId != PixelSubdetector::PixelBarrel) && (subdetId != PixelSubdetector::PixelEndcap)) continue;
		// Get module data
		ModuleData mod    = ModuleDataProducer::getPhaseZeroOfflineModuleData(detId.rawId(), trackerTopology, fedErrors);
		ModuleData mod_on = ModuleDataProducer::convertPhaseZeroOfflineOnline(mod);
		// Map for mergeable cluster pairs
		std::map<const SiPixelCluster*, const SiPixelCluster*> clusterPairsToMerge;
		// addMarkersToEventPlot(mod_on, *digiFlagsOnModulePtr);
		// Looping on clusters on the same detector_part
		for(const auto& currentCluster: clusterSetOnModule)
		{
			// Perform test for marker
			int isCurrentClusterSplit = getMaxDigiMarkerValue(currentCluster, *digiFlagsOnModulePtr);
			// Saving the pixel vectors to save computing time
			std::vector<SiPixelCluster::Pixel> currentClusterPixels = currentCluster.pixels();
			// Save cluster data
			saveClusterData(currentCluster, mod, mod_on, *digiFlagsOnModulePtr);
			eventClustersField.fill(clusterField, clusterField.mod_on);
			// Save digis data
			for(const auto& pixel: currentClusterPixels)
			{
				savePixelData(pixel, mod, mod_on, *digiFlagsOnModulePtr);
			}
			// Find pixels that are close to the current cluster
			std::vector<const SiPixelCluster*> closeClusters;
			for(const auto& otherCluster: clusterSetOnModule)
			{
				// Discard the cluster itself
				if(ClusterGeometry::isSameCluster(currentCluster, otherCluster)) continue;
				// Filter pixels closer than a given distance
				if(ClusterGeometry::isCloserThan(14.0, currentCluster, otherCluster))
				{
					closeClusters.push_back(&otherCluster);
				}
			}
			const SiPixelCluster* clusterToMergePtr = nullptr;
			// Filtering
			for(const SiPixelCluster* closeClusterPtr: closeClusters)
			{
				ClusterPairMergingValidator validator(currentCluster, *closeClusterPtr);
				if(!(validator.isMissingPartDoubleColumn())) continue;
				if(!(validator.isPairNotTooLong()))          continue;
				// if(!(validator.isDirSimilar()))              continue;
				if(clusterToMergePtr != nullptr)
				{
					std::cerr << c_red << "Error: " << c_def << "One of the clusters has more than one mergeable cluster candidates. Merging only the last one..." << std::endl;
				}
				clusterToMergePtr = closeClusterPtr;
			}
			// Stop if no mergeable clusters found
			if(clusterToMergePtr == nullptr) continue;
			const SiPixelCluster& clusterToMerge = *clusterToMergePtr;
			int isMergeableClusterSplit = getMaxDigiMarkerValue(clusterToMerge, *digiFlagsOnModulePtr);
			// Save every cluster pair only once: check the cluster order
			if(checkClusterPairOrder(currentCluster, clusterToMerge))
			{
				clusterPairsToMerge.insert(std::make_pair(&currentCluster, &clusterToMerge));
				// Saving the pixel vector to save computing time
				std::vector<SiPixelCluster::Pixel> clusterToMergePixels = clusterToMerge.pixels();
				// Saving off informations about the mergeable pixels
				saveMergingData(currentCluster, clusterToMerge, currentClusterPixels, clusterToMergePixels, isCurrentClusterSplit, isMergeableClusterSplit, mod, mod_on);
				eventMergingStatisticsField.fill(mergeStatField);
			}
		}
	}
	#ifdef USE_TIMER
		timer -> printSeconds("Took about: ", " seconds.");
	#endif
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
	EventDataTree::setEventClusterPairsTreeDataFields(eventTree, eventField, eventMergingStatisticsField);
	eventTree -> Fill();
	#ifdef USE_TIMER
		timer -> restart("Measuring the time required to fill the data tree.");
	#endif
	eventClustersTree -> Fill();
	#ifdef USE_TIMER
		timer -> printSeconds("Took about: ", " seconds.");
	#endif
}

void SplitClusterAnalyzer::saveClusterData(const SiPixelCluster& cluster, const ModuleData& mod, const ModuleData& mod_on, const edm::DetSet<PixelDigi>& digiFlags)
{
	clusterField.mod         = mod;
	clusterField.mod_on      = mod_on;
	clusterField.x           = cluster.x();
	clusterField.y           = cluster.y();
	clusterField.sizeX       = cluster.sizeX();
	clusterField.sizeY       = cluster.sizeY();
	clusterField.clusterSize = cluster.size();
	clusterField.charge      = cluster.charge();
	// Info of the pixels in the cluster
	const auto currentPixelPositions = cluster.pixels();
	const auto currentAdcs           = cluster.pixelADC();
	for(int numPixel = 0; numPixel < clusterField.clusterSize && numPixel < 100; ++numPixel)
	{
		clusterField.pixelsCol    .push_back(currentPixelPositions[numPixel].x);
		clusterField.pixelsRow    .push_back(currentPixelPositions[numPixel].y);
		clusterField.pixelsAdc    .push_back(currentAdcs[numPixel] / 1000.0);
		clusterField.pixelsMarker .push_back(getDigiMarkerValue(currentPixelPositions[numPixel], digiFlags));
	}
	ClusterDataTree::setClusterTreeDataFields(clusterTree, clusterField);
	clusterTree -> Fill();
}

void SplitClusterAnalyzer::saveMergingData(const SiPixelCluster& currentCluster, const SiPixelCluster& clusterToMerge, const std::vector<SiPixelCluster::Pixel>& currentClusterPixels, const std::vector<SiPixelCluster::Pixel>& clusterToMergePixels, bool isCurrentClusterSplit, bool isMergeableClusterSplit, const ModuleData& mod, const ModuleData& mod_on)
{
	// Checking the charge of the cluster and the neighbour
	std::vector<uint16_t> currentClPixelADCs;
	std::vector<uint16_t> clusterToMergeADCs;
	double currentClusterTotalPixelADC  = 0;
	double clusterToMergeTotalPixelADC = 0;
	for(const auto pixel: currentClusterPixels)
	{
		currentClPixelADCs.push_back(pixel.adc);
		currentClusterTotalPixelADC += pixel.adc;
	}
	std::sort(currentClPixelADCs.begin(), currentClPixelADCs.end());
	for(const auto pixel: clusterToMerge.pixels())
	{
		clusterToMergeADCs.push_back(pixel.adc);
		clusterToMergeTotalPixelADC += pixel.adc;
	}
	std::sort(clusterToMergeADCs.begin(), clusterToMergeADCs.end());
	// Saving informations about the mergeable clusters
	mergeStatField.clusterSize_1            = currentCluster.size();
	mergeStatField.clusterSize_2            = clusterToMerge.size();
	mergeStatField.sizeDifference           = clusterToMerge.size() - currentCluster.size();
	mergeStatField.clusterCharge_1          = currentCluster.charge();
	mergeStatField.clusterCharge_2          = clusterToMerge.charge();
	mergeStatField.chargeDifference         = clusterToMerge.charge() - currentCluster.charge();
	mergeStatField.clusterAngle_1           = ClusterPairMergingValidator::getClusterAngle(currentCluster);
	mergeStatField.clusterAngle_2           = ClusterPairMergingValidator::getClusterAngle(clusterToMerge);
	mergeStatField.angleDifference          = mergeStatField.clusterAngle_1 - mergeStatField.clusterAngle_2;
	mergeStatField.isMarkedAsSplitCluster_1 = isCurrentClusterSplit;
	mergeStatField.isMarkedAsSplitCluster_2 = isMergeableClusterSplit;
	mergeStatField.distanceInPixels         = ClusterPairMergingValidator::getShortestPathBetweenClusters(currentCluster, clusterToMerge).size() - 1;
	mergeStatField.mod                      = mod;
	mergeStatField.mod_on                   = mod_on;
	MergingStatisticsTree::setMergingStatTreeDataFields(mergeTree, mergeStatField);
	mergeTree -> Fill();
}

void SplitClusterAnalyzer::savePixelData(const SiPixelCluster::Pixel& pixelToSave, const ModuleData& mod, const ModuleData& mod_on, const edm::DetSet<PixelDigi>& digiFlagsCollection)
{
	pixelField.mod      = mod;
	pixelField.mod_on   = mod_on;
	pixelField.col      = static_cast<int>(pixelToSave.x);
	pixelField.row      = static_cast<int>(pixelToSave.y);
	pixelField.adc      = static_cast<int>(pixelToSave.adc);
	pixelField.isMarked = getDigiMarkerValue(pixelToSave, digiFlagsCollection);
	PixelDataTree::setPixelTreeDataFields(pixelTree, pixelField, eventField);
	pixelTree -> Fill();
}

// void SplitClusterAnalyzer::reserveMemoryForEventClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection)
// {
// #ifdef USE_TIMER
// 	timer -> restart("Measuring the time required to reserve space for the vector containing the clusters to draw.");
// #endif
// 	unsigned int numClusters = 0;
// 	for(const auto& clusterSetOnModule: *clusterCollection)
// 	{
// 		numClusters += clusterSetOnModule.size();
// 	}
// 	eventClustersField.reserveAllContainers(numClusters);
// 	for(const auto& clusterSetOnModule: *clusterCollection)
// 	{
// 		const auto beginIt = clusterSetOnModule.begin();
// 		for(auto clusterIt = clusterSetOnModule.begin(); clusterIt != clusterSetOnModule.end(); ++clusterIt)
// 		{
// 			int clusterIndex = clusterIt - beginIt;
// 			int numPixels = clusterIt -> size();
// 			eventClustersField.pixelsCol[clusterIndex]    .reserve(numPixels);
// 			eventClustersField.pixelsRow[clusterIndex]    .reserve(numPixels);
// 			eventClustersField.pixelsAdc[clusterIndex]    .reserve(numPixels);
// 			eventClustersField.pixelsMarker[clusterIndex] .reserve(numPixels);
// 		}
// 	}
// #ifdef USE_TIMER
// 	timer -> printSeconds("Took about: ", " seconds.");
// #endif
// }

// unsigned int SplitClusterAnalyzer::getNumClusters(const edm::Handle<edmNew::DetSetVector<SiPixelCluster>>& clusterCollection)
// {
// 	unsigned int numClusters = 0;
// 	for(const auto& clusterSetOnModule: *clusterCollection)
// 	{
// 		numClusters += clusterSetOnModule.size();
// 	}
// 	return numClusters;
// }

bool SplitClusterAnalyzer::checkClusterPairOrder(const SiPixelCluster& lhs, const SiPixelCluster& rhs)
{
	// Perform checks once per cluster pairs
	if(lhs.size() < rhs.size()) return true;
	if(lhs.size() > rhs.size()) return false;
	if(lhs.charge() < rhs.charge()) return true;
	if(lhs.charge() > rhs.charge()) return false;
	if(lhs.charge() < rhs.charge()) return true;
	if(lhs.charge() > rhs.charge()) return false;
	if(lhs.x() < rhs.x()) return true;
	if(lhs.x() > rhs.x()) return false;
	if(lhs.y() < rhs.y()) return true;
	if(lhs.y() > rhs.y()) return false;
	std::cerr << c_red << "Error: " << c_def << "SplitClusterMergerProducer::checkClusterPairOrder() called with the same cluster twice as argument. Check code!" << std::endl;
	return false;
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

void SplitClusterAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
	// LogDebug("step") << "Executing SplitClusterAnalyzer::beginRun()..." << std::endl;
}

void SplitClusterAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
	// LogDebug("step") << "Executing SplitClusterAnalyzer::endRun()..." << std::endl;
}

void SplitClusterAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
	// LogDebug("step") << "Executing SplitClusterAnalyzer::beginLuminosityBlock()..." << std::endl;
}

void SplitClusterAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
	// LogDebug("step") << "Executing SplitClusterAnalyzer::endLuminosityBlock()..." << std::endl;
}

/////////////
// Utility //
/////////////

void SplitClusterAnalyzer::clearAllContainers()
{
	eventField.init();
	clusterField.init();
	mergeStatField.init();
	pixelField.init();
	eventMergingStatisticsField.clear();
	eventClustersField.clear();
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
