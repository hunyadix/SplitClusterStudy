// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../interface/HelperFunctionsCommon.h"
#include "../interface/ClusterPairFunctions.h"
#include "../../../interface/CanvasExtras.h"
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

int isTagged(const Cluster& clusterField);
int isTaggedInColumn(const Cluster& clusterField, int col);
int isTaggedOnOneEndOnly(const Cluster& clusterField);

int main(int argc, char** argv) try
{
	std::string inputFileName("");
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, savePlots);
	std::cout << process_prompt << argv[0] << " started..." << std::endl;
	gStyle -> SetPalette(1);
	TimerColored timer(timer_prompt);
	TApplication* theApp = new TApplication("App", &argc, argv);
	TFile* inputFile = TFile::Open("/data_ssd_120gb/hunyadi/CMSSW/SplitClusterStudy/CMSSW_8_0_18/src/TestSplitClusterStudy/Ntuple_scs.root", "READ");
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
	TH1I healthyUnhealthySmallClusters_H("healthyUnhealthySmallClusters", "Num. of 4-6 long healthy clusters, # of 4-6 long split cluster pairs;healthy/unhealthy;nclusters", 2, 0, 1);
	TH1I sixLongClustersFakeRate_H("sixLongClustersFakeRate",             "Num. of 4-6 long split cluster pairs, # of fake 4-6 long cluster pairs;real/fake;nclusters", 2, 0, 1);
	TH1I sixLongClustersEfficiency_H("sixLongClustersEfficiency",         "Num. of 4-6 long healthy clusters, # of 4-6 long unhealthy clusters;restoreable/not restoreable;nclusters", 2, 0, 1);
	std::vector<std::shared_ptr<TH1*>> histograms;
	histograms.emplace_back(std::make_shared<TH1*>(&healthyUnhealthySmallClusters_H));
	histograms.emplace_back(std::make_shared<TH1*>(&sixLongClustersFakeRate_H));
	histograms.emplace_back(std::make_shared<TH1*>(&sixLongClustersEfficiency_H));
	int eventNum = 0;
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		using CollectionType     = ClusterPairFunctions::SharedPtrCollectionType;
		using PairCollectionType = ClusterPairFunctions::PairCollectionType;
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		// Healthy
		{
			const CollectionType healthyFourLongClusters = ClusterPairFunctions::getHealthyClustersWithLength(clusterCollection, 4);
			const CollectionType healthyFiveLongClusters = ClusterPairFunctions::getHealthyClustersWithLength(clusterCollection, 5);
			const CollectionType healthySixLongClusters  = ClusterPairFunctions::getHealthyClustersWithLength(clusterCollection, 6);
			CollectionType healthySmallClusters;
			std::copy(healthyFourLongClusters.begin(), healthyFourLongClusters.end(), std::back_inserter(healthySmallClusters));
			std::copy(healthyFiveLongClusters.begin(), healthyFiveLongClusters.end(), std::back_inserter(healthySmallClusters));
			std::copy(healthySixLongClusters .begin(), healthySixLongClusters .end(), std::back_inserter(healthySmallClusters));
			std::cout << "Healthy small clusters: " << healthySmallClusters.size() << std::endl;
			healthyUnhealthySmallClusters_H.AddBinContent(0, healthySmallClusters.size());
		}
		// Unhealthy
		{
			const CollectionType unhealthyFourLongClusters = ClusterPairFunctions::getUnhealthyClustersWithLength(clusterCollection, 4);
			const CollectionType unhealthyFiveLongClusters = ClusterPairFunctions::getUnhealthyClustersWithLength(clusterCollection, 5);
			const CollectionType unhealthySixLongClusters  = ClusterPairFunctions::getUnhealthyClustersWithLength(clusterCollection, 6);
			CollectionType unhealthySmallClusters;
			std::copy(unhealthyFourLongClusters.begin(), unhealthyFourLongClusters.end(), std::back_inserter(unhealthySmallClusters));
			std::copy(unhealthyFiveLongClusters.begin(), unhealthyFiveLongClusters.end(), std::back_inserter(unhealthySmallClusters));
			std::copy(unhealthySixLongClusters .begin(), unhealthySixLongClusters .end(), std::back_inserter(unhealthySmallClusters));
			std::cout << "Unhealthy small clusters: " << unhealthySmallClusters.size() << std::endl;
			healthyUnhealthySmallClusters_H.AddBinContent(1, unhealthySmallClusters.size());
		}
		// Real pairs
		{
			const PairCollectionType fourLongRealPairs = ClusterPairFunctions::getRealPairsWithLength(clusterCollection, 4);
			const PairCollectionType fiveLongRealPairs = ClusterPairFunctions::getRealPairsWithLength(clusterCollection, 5);
			const PairCollectionType sixLongRealPairs  = ClusterPairFunctions::getRealPairsWithLength(clusterCollection, 6);
			PairCollectionType smallRealPairs;
			std::copy(fourLongRealPairs.begin(), fourLongRealPairs.end(), std::back_inserter(smallRealPairs));
			std::copy(fiveLongRealPairs.begin(), fiveLongRealPairs.end(), std::back_inserter(smallRealPairs));
			std::copy(sixLongRealPairs .begin(), sixLongRealPairs .end(), std::back_inserter(smallRealPairs));
			std::cout << "Small real cluster pairs: " << smallRealPairs.size() << std::endl;
			sixLongClustersFakeRate_H.AddBinContent(0, smallRealPairs.size());
		}
		// Fake pairs
		{
			const PairCollectionType fourLongFakePairs = ClusterPairFunctions::getFakePairsWithLength(clusterCollection, 4);
			const PairCollectionType fiveLongFakePairs = ClusterPairFunctions::getFakePairsWithLength(clusterCollection, 5);
			const PairCollectionType sixLongFakePairs  = ClusterPairFunctions::getFakePairsWithLength(clusterCollection, 6);
			PairCollectionType smallFakePairs;
			std::copy(fourLongFakePairs.begin(), fourLongFakePairs.end(), std::back_inserter(smallFakePairs));
			std::copy(fiveLongFakePairs.begin(), fiveLongFakePairs.end(), std::back_inserter(smallFakePairs));
			std::copy(sixLongFakePairs .begin(), sixLongFakePairs .end(), std::back_inserter(smallFakePairs));
			std::cout << "Small fake cluster pairs: " << smallFakePairs.size() << std::endl;
			sixLongClustersFakeRate_H.AddBinContent(1, smallFakePairs.size());
		}
		++eventNum;
	}
	std::cout << "---\n"; 
	std::cout << "Statistics total: \n"; 
	std::cout << "Number of healthy   clusters with size of [4, 6]: " << healthyUnhealthySmallClusters_H.GetBinContent(0) << "\n";
	std::cout << "Number of unhealthy clusters with size of [4, 6]: " << healthyUnhealthySmallClusters_H.GetBinContent(1) << "\n";
	std::cout << "Number of real pairs         with size of [4, 6]: " << sixLongClustersFakeRate_H.GetBinContent(0) << "\n";
	std::cout << "Number of fake pairs         with size of [4, 6]: " << sixLongClustersFakeRate_H.GetBinContent(1) << "\n";
	std::cout << "---" << std::endl; 
	std::vector<std::shared_ptr<TCanvas>> canvases;
	for(const auto& i: range(histograms.size()))
	{
		auto& histogram = *(*histograms[i]);
		canvases.emplace_back(std::make_shared<TCanvas>(("canvas_" + std::to_string(i)).c_str(), histogram.GetTitle(), 50 + i * 403, 50, 400, 300));
		canvases.back() -> cd();
		CanvasExtras::redesignCanvas(canvases.back().get(), &histogram);
		histogram.Draw("COLZ");
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
	// int hc = numFourLongHealthy     + numFiveLongHealthy     + numSixLongHealthy;
	// int sc = numFourLongRealPairs   + numFiveLongRealPairs   + numSixLongRealPairs;
	// int fc = numFourMarkerlessPairs + numFiveMarkerlessPairs + numSixMarkerlessPairs;
	// std::cout << "hc: " << hc << std::endl;
	// std::cout << "sc: " << sc << std::endl;
	// std::cout << "fc: " << fc << std::endl;
	// std::cout << "fc / (sc + fc): " << fc / static_cast<double>(sc + fc) << std::endl;
	// std::cout << "sc / (sc + hc): " << sc / static_cast<double>(sc + hc) << std::endl;
	// std::cout << "fc / (hc + fc): " << fc / static_cast<double>(hc + fc) << std::endl;

	// TCanvas canvas("canvas", "canvas", 10, 10, 1500, 500);
	// canvas.Divide(3, 1);
	// canvas.cd(1);
	// healthyUnhealthySmallClusters_H.Draw();
	// canvas.cd(2);
	// sixLongClustersFakeRate_H.Draw();
	// canvas.cd(3);
	// sixLongClustersEfficiency_H.Draw();
	// canvas.Update();
	theApp -> Run();
	inputFile -> Close();
	std::cout << process_prompt << argv[0] << " terminated succesfully." << std::endl;
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