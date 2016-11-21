// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../interface/HelperFunctionsCommon.h"
#include "../interface/ClusterPairFunctions.h"
#include "../../../interface/CanvasExtras.h"
#include "../interface/ModuleClusterPlot.h"
#include "../interface/TestsCommon.h"

#include "../../../TimerForBenchmarking/interface/TimerColored.h"

// Root
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1I.h>
#include <TH1F.h>
#include <TH2D.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <functional>
#include <algorithm>

// #define PLOT_FAKE_PAIR_SNAPSHOTS

int isTagged(const Cluster& clusterField);
int isTaggedInColumn(const Cluster& clusterField, int col);
int isTaggedOnOneEndOnly(const Cluster& clusterField);

constexpr int HIGH_ETA_ABS_LOWER_LIMIT = 4;

int main(int argc, char** argv) try
{
	std::string inputFileName("");
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, savePlots);
	std::cout << process_prompt << argv[0] << " started..." << std::endl;
	gStyle -> SetPalette(1);
	TimerColored timer(timer_prompt);
	TApplication* theApp = new TApplication("App", &argc, argv);
	TFile* inputFile = TFile::Open(inputFileName.c_str(), "READ");
	TTree* clusterTree = (TTree*)(inputFile -> Get("clustTree"));
	TTreeTools::treeCheck(clusterTree, "Tree missing.", true);
	Cluster  clusterField;
	// STL classes require addresses of pointers as accessors, unfortunately I can't do this in the associateDataFields function
	std::vector<int>* pixelsColWrapper    = &clusterField.pixelsCol;
	std::vector<int>* pixelsRowWrapper    = &clusterField.pixelsRow;
	std::vector<int>* pixelsAdcWrapper    = &clusterField.pixelsAdc;
	std::vector<int>* pixelsMarkerWrapper = &clusterField.pixelsMarker;
	EventData eventField;
	ClusterDataTree::associateDataFieldsFromTree(clusterTree, eventField, clusterField);
	TTreeTools::checkGetBranch(clusterTree, "pixelsCol")    -> SetAddress(&pixelsColWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsRow")    -> SetAddress(&pixelsRowWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsAdc")    -> SetAddress(&pixelsAdcWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsMarker") -> SetAddress(&pixelsMarkerWrapper);
	clusterTree -> SetBranchAddress("pixelsCol",    &pixelsColWrapper);
	clusterTree -> SetBranchAddress("pixelsRow",    &pixelsRowWrapper);
	clusterTree -> SetBranchAddress("pixelsAdc",    &pixelsAdcWrapper);
	clusterTree -> SetBranchAddress("pixelsMarker", &pixelsMarkerWrapper);
	// Get number of entries
	Int_t totalNumEntries = clusterTree -> GetEntries();
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Check if data is present
	if(totalNumEntries == 0) throw std::runtime_error("No entries found in tree: clusterTree.");
	timer.restart("Measuring the time required for separating clusters by events...");
	std::map<int, std::vector<Cluster>> eventClustersMap(getClusterCollectionSortedByEvtnum(clusterTree, eventField, clusterField));
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	// Histogram definitions
	// Cluster sizes
	TH1I clusterSizesLayer1HighEta_H ("clusterSizesLayer1HighEta",  "Cluster sizes [L1-HE];size (pixels); num. clusters", 31, 0, 30);
	TH1I clusterXSizesLayer1HighEta_H("clusterXSizesLayer1HighEta", "Cluster X sizes [L1-HE];size (pixels); num. clusters", 31, 0, 30);
	TH1I clusterYSizesLayer1HighEta_H("clusterYSizesLayer1HighEta", "Cluster Y sizes [L1-HE];size (pixels); num. clusters", 31, 0, 30);
	// Healthy, and fake rate
	TH1I healthyClustersLengthXDistr_H(  "healthyClustersLengthXDistr",   "Healthy cluster X length distribution [L1-HE];size (pixels);nclusters",   31, 0, 30);
	TH1I healthyClustersLengthYDistr_H(  "healthyClustersLengthYDistr",   "Healthy cluster Y length distribution [L1-HE];size (pixels);nclusters",   31, 0, 30);
	TH1I unHealthyClustersLengthXDistr_H("unHealthyClustersLengthXDistr", "Unhealthy cluster X length distribution [L1-HE];size (pixels);nclusters", 31, 0, 30);
	TH1I unHealthyClustersLengthYDistr_H("unHealthyClustersLengthYDistr", "Unhealthy cluster Y length distribution [L1-HE];size (pixels);nclusters", 31, 0, 30);
	TH1I realPairsLengthXDistr_H(        "realPairsLengthXDistr",         "Real split cluster pair X length distribution [L1-HE];size (pixels);nclusters", 31, 0, 30);
	TH1I realPairsLengthYDistr_H(        "realPairsLengthYDistr",         "Real split cluster pair Y length distribution [L1-HE];size (pixels);nclusters", 31, 0, 30);
	TH1I fakePairsLengthXDistr_H(        "fakePairsLengthXDistr",         "Fake split cluster pair X length distribution [L1-HE];size (pixels);nclusters", 31, 0, 30);
	TH1I fakePairsLengthYDistr_H(        "fakePairsLengthYDistr",         "Fake split cluster pair Y length distribution [L1-HE];size (pixels);nclusters", 31, 0, 30);
	// Size vs indcidence angles
	TH2I clusterSizeVsAlpha_H(           "clusterSizeVsAlpha",            "Cluster size distribution for trk. alpha inc. angles[L1-HE];size (pixels);alpha", 31, 0, 30, 64, -3.2, 3.2);
	TH2I clusterSizeVsBeta_H (           "clusterSizeVsBeta",             "Cluster size distribution for trk. beta inc. angles[L1-HE];size (pixels);beta",   31, 0, 30, 64, -3.2, 3.2);
	TH2I clusterSizeVsTheta_H(           "clusterSizeVsTheta",            "Cluster size distribution for trk. theta inc. angles[L1-HE];size (pixels);theta", 31, 0, 30, 64, -3.2, 3.2);
	TH2I clusterSizeVsPhi_H  (           "clusterSizeVsPhi",              "Cluster size distribution for trk. phi inc. angles[L1-HE];size (pixels);phi",     31, 0, 30, 64, -3.2, 3.2);
	std::vector<std::shared_ptr<TH1*>> histograms;
	histograms.emplace_back(std::make_shared<TH1*>(&clusterSizesLayer1HighEta_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterXSizesLayer1HighEta_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterYSizesLayer1HighEta_H));
	histograms.emplace_back(std::make_shared<TH1*>(&healthyClustersLengthXDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&healthyClustersLengthYDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&unHealthyClustersLengthXDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&unHealthyClustersLengthYDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&realPairsLengthXDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&realPairsLengthYDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&fakePairsLengthXDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&fakePairsLengthYDistr_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterSizeVsAlpha_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterSizeVsBeta_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterSizeVsTheta_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterSizeVsPhi_H));
	// High eta filter lambda
	auto layer1highEtaFilter = [] (const Cluster& cluster) { return cluster.mod_on.layer == 1 && HIGH_ETA_ABS_LOWER_LIMIT <= std::abs(cluster.mod_on.module); };
	int eventNum = 0;
#ifdef PLOT_FAKE_PAIR_SNAPSHOTS
	const auto& digisFromMarkersWithNeighboursType = ModuleClusterPlot::Type::digisFromMarkersWithNeighbours;
	const auto& fakePairsWithMarkersType = ModuleClusterPlot::Type::fakePairsWithMarkers;
	std::vector<std::shared_ptr<ModuleClusterPlot>> moduleClusterPlots;
#endif
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		using CollectionType     = ClusterPairFunctions::SharedPtrCollectionType;
		using PairCollectionType = ClusterPairFunctions::PairCollectionType;
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		const std::vector<Cluster> layer1HighEtaClusterCollection = filter(clusterCollection, layer1highEtaFilter);
		// Cluster sizes
		for(const Cluster& cluster: layer1HighEtaClusterCollection)
		{
			clusterSizesLayer1HighEta_H .Fill(cluster.clusterSize);
			clusterXSizesLayer1HighEta_H.Fill(cluster.sizeX);
			clusterYSizesLayer1HighEta_H.Fill(cluster.sizeY);
			const float& alpha = cluster.alpha;
			const float& beta  = cluster.beta;
			const float  theta = atan(sqrt(1.0 / (1.0 / (tan(alpha) * tan(alpha)) + 1.0 / (tan(beta) * tan(beta))))); // Imported from TimingStudy
			const float  phi   = atan2(tan(beta), tan(alpha));                                                        // Imported from TimingStudy
			// Inc. angles
			clusterSizeVsAlpha_H.Fill(cluster.clusterSize, alpha);
			clusterSizeVsBeta_H .Fill(cluster.clusterSize, beta);
			clusterSizeVsTheta_H.Fill(cluster.clusterSize, theta);
			clusterSizeVsPhi_H  .Fill(cluster.clusterSize, phi);
		}
		// Healthy, unhealthy
		for(int clusterSize: range(30))
		{
			const CollectionType healthyClustersOfXSize   = ClusterPairFunctions::getHealthyClustersWithXLength(  layer1HighEtaClusterCollection, clusterSize);
			const CollectionType healthyClustersOfYSize   = ClusterPairFunctions::getHealthyClustersWithYLength(  layer1HighEtaClusterCollection, clusterSize);
			const CollectionType unhealthyClustersOfXSize = ClusterPairFunctions::getUnhealthyClustersWithXLength(layer1HighEtaClusterCollection, clusterSize);
			const CollectionType unhealthyClustersOfYSize = ClusterPairFunctions::getUnhealthyClustersWithYLength(layer1HighEtaClusterCollection, clusterSize);
			healthyClustersLengthXDistr_H  .AddBinContent(clusterSize + 1, healthyClustersOfXSize  .size());
			healthyClustersLengthYDistr_H  .AddBinContent(clusterSize + 1, healthyClustersOfYSize  .size());
			unHealthyClustersLengthXDistr_H.AddBinContent(clusterSize + 1, unhealthyClustersOfXSize.size());
			unHealthyClustersLengthYDistr_H.AddBinContent(clusterSize + 1, unhealthyClustersOfYSize.size());
			// std::cout << clusterSize << ": " << healthyClustersOfGivenSize.size() << std::endl;
			// std::cout << "Summed at " << clusterSize << ": " << healthyClustersLengthDistr_H.GetBinContent(clusterSize) << std::endl;
		}
		// Real and fake pairs
		for(int clusterPairMergedSize: range(30))
		{
			const PairCollectionType realPairsOfXSize = ClusterPairFunctions::getRealPairsWithXLength(layer1HighEtaClusterCollection, clusterPairMergedSize);
			const PairCollectionType realPairsOfYSize = ClusterPairFunctions::getRealPairsWithYLength(layer1HighEtaClusterCollection, clusterPairMergedSize);
			const PairCollectionType fakePairsOfXSize = ClusterPairFunctions::getFakePairsWithXLength(layer1HighEtaClusterCollection, clusterPairMergedSize);
			const PairCollectionType fakePairsOfYSize = ClusterPairFunctions::getFakePairsWithYLength(layer1HighEtaClusterCollection, clusterPairMergedSize);
			realPairsLengthXDistr_H.AddBinContent(clusterPairMergedSize + 1, realPairsOfXSize.size());
			realPairsLengthYDistr_H.AddBinContent(clusterPairMergedSize + 1, realPairsOfYSize.size());
			fakePairsLengthXDistr_H.AddBinContent(clusterPairMergedSize + 1, fakePairsOfXSize.size());
			fakePairsLengthYDistr_H.AddBinContent(clusterPairMergedSize + 1, fakePairsOfYSize.size());
			if(clusterPairMergedSize == 0 || clusterPairMergedSize == 1 || clusterPairMergedSize == 2)
			{
				if(realPairsOfXSize.size() != 0) std::cout << "realPairsOfXSize.size() = " << realPairsOfXSize.size() << std::endl;
			}
#ifdef PLOT_FAKE_PAIR_SNAPSHOTS
			for(const auto pair: fakePairsOfGivenSize)
			{
				const int& event  = eventNum; 
				const int& layer  = pair.first -> mod_on.layer; 
				const int& module = pair.first -> mod_on.module; 
				const int& ladder = pair.first -> mod_on.ladder; 
				moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, layer,  module, ladder, event, event));
				moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(fakePairsWithMarkersType,           layer,  module, ladder, event, event));
			}
#endif
		}
		++eventNum;
	}
#ifdef PLOT_FAKE_PAIR_SNAPSHOTS
	// Saving snapshots
	eventNum = 0;
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		for(const Cluster& cluster: clusterCollection)
		{
			ModuleClusterPlot::fillDigisMarkers(cluster, eventNum);
		}
		ModuleClusterPlot::fillAllPairs(clusterCollection, eventNum);
		ModuleClusterPlot::saveAllFinished(eventNum);
		++eventNum;
	}
#endif
	CanvasExtras::setMulticolorColzPalette();
	std::vector<std::shared_ptr<TCanvas>> canvases;
	for(const auto& i: range(histograms.size()))
	{
		auto& histogram = *(*histograms[i]);
		canvases.emplace_back(std::make_shared<TCanvas>(("canvas_" + std::to_string(i)).c_str(), histogram.GetTitle(), 50 + i * 403, 50, 400, 300));
		canvases.back() -> cd();
		CanvasExtras::redesignCanvas(canvases.back().get(), &histogram);
		// histogram.GetYaxis() -> SetNdivisions(507);
		histogram.SetFillColor(38);
		if(dynamic_cast<TH2I*>(*histograms[i])) histogram.Draw("COLZ");
		else                                    histogram.Draw("BHIST");
		TText layer1highEtaLabel;
		CanvasExtras::setLabelStyleNote(layer1highEtaLabel);		
		layer1highEtaLabel.DrawText(0.79, 0.88, "[Layer: 1, Module: -4 and +4]");
		TText integralValueLabel;
		CanvasExtras::setLabelStyleNote(integralValueLabel);
		layer1highEtaLabel.DrawText(0.79, 0.83, ("[Integral: " + std::to_string(histogram.Integral()) + "]").c_str());
	}
	for(const auto& canvas: canvases)
	{
		canvas -> Update();
		if(savePlots)
		{
			std::string filename = canvas -> GetTitle();
			std::transform(filename.begin(), filename.end(), filename.begin(), [] (char ch) { return ch == ' ' ? '_' : ch; });
			filename = "results/" + filename + ".eps";
			canvas -> SaveAs(filename.c_str());
		}
	}
	inputFile -> Close();
	std::cout << process_prompt << argv[0] << " terminated succesfully." << std::endl;
	theApp -> Run();
	return 0; 
}
catch(const std::exception& e)
{
	std::cout << error_prompt << "Exception thrown: " << e.what() << std::endl;
	return -1;
}

int isTagged(const Cluster& clusterField)
{
	if(std::any_of(clusterField.pixelsMarker.begin(), clusterField.pixelsMarker.end(), [] (int marker) {return marker == 0;})) return 1;
	return 0;
}

int isTaggedInColumn(const Cluster& clusterField, int col)
{
	for(int i: range(clusterField.pixelsCol.size()))
	{
		if(clusterField.pixelsCol[i] == col && clusterField.pixelsMarker[i] != 0) return 1;		
	}
	return 0;
}

int isTaggedOnOneEndOnly(const Cluster& clusterField)
{
	auto minMaxCol = std::minmax_element(clusterField.pixelsCol.begin(), clusterField.pixelsCol.end());
	int clusterMinCol = *(minMaxCol.first);
	int clusterMaxCol = *(minMaxCol.second);
	int isMarkedOnMin = isTaggedInColumn(clusterField, clusterMinCol);
	int isMarkedOnMax = isTaggedInColumn(clusterField, clusterMaxCol);
	return isMarkedOnMin ^ isMarkedOnMax;
}