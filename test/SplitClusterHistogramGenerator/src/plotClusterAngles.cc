// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../interface/HelperFunctionsCommon.h"
#include "../../../interface/CanvasExtras.h"

#include "../../../TimerForBenchmarking/interface/TimerColored.h"
#include "../interface/ClusterPairFunctions.h"

// Root
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TH3D.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <functional>
#include <complex>
#include <algorithm>

struct ClusterStats
{
	int   startCol = NOVAL_I;
	int   endCol   = NOVAL_I;
	float startRow = NOVAL_F;
	float endRow   = NOVAL_F;
	float dir      = NOVAL_F;
};

void printUsage(int& argc, char** argv, bool killProcess);
void processArgs(int& argc, char** argv, std::string& inputFileName, int& savePlots);
std::map<int, std::vector<Cluster>> getClusterCollectionSortedByEvtnum(TTree* clusterTree, EventData& eventField, Cluster& clusterField);

int main(int argc, char** argv) try
{
	std::string inputFileName("");
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, savePlots);
	std::cout << process_prompt << "plotClusterAngles started..." << std::endl;
	gStyle -> SetPalette(1);
	TimerColored timer(timer_prompt);
	TApplication* theApp = new TApplication("App", &argc, argv);
	TFile* inputFile = TFile::Open(inputFileName.c_str(), "READ");
	TTree* clusterTree = (TTree*)(inputFile -> Get("clustTree"));
	TTreeTools::treeCheck(clusterTree, "Tree missing.", true);
	Cluster  clusterField;
	EventData eventField;
	std::vector<int>* pixelsColWrapper    = &clusterField.pixelsCol;
	std::vector<int>* pixelsRowWrapper    = &clusterField.pixelsRow;
	std::vector<int>* pixelsAdcWrapper    = &clusterField.pixelsAdc;
	std::vector<int>* pixelsMarkerWrapper = &clusterField.pixelsMarker;
	// Check if data is present
	Int_t totalNumEntries = clusterTree -> GetEntries();
	if(totalNumEntries == 0) throw std::runtime_error("No entries found in tree: clusterTree.");
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Get the indecies of the first "numEventsToMerge" events
	ClusterDataTree::associateDataFieldsFromTree(clusterTree, eventField, clusterField);
	TTreeTools::checkGetBranch(clusterTree, "pixelsCol")    -> SetAddress(&pixelsColWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsRow")    -> SetAddress(&pixelsRowWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsAdc")    -> SetAddress(&pixelsAdcWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsMarker") -> SetAddress(&pixelsMarkerWrapper);
	timer.restart("Measuring the time required for separating clusters by events...");
	std::map<int, std::vector<Cluster>> eventClustersMap(getClusterCollectionSortedByEvtnum(clusterTree, eventField, clusterField));
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	// STL classes require addresses of pointers as accessors, unfortunately I can't do this in the associateDataFields function
	// Histogram definitions
	TH1D clusterAngle_H                    ("ClusteAngleDistribution",        "Cluster Angle Distribution;angle;nclusters",                                                                     100, 0.0, 3.15);
	TH1D clusterPairIndAngle_H             ("ClustePairIndAngleDistribution", "Cluster Pair Individual Angle Distribution;angle;nclusters",                                                     100, 0.0, 3.15);
	TH1D clusterPairRelAngle_H             ("ClustePairRelAngleDistribution", "Cluster Pair Relative Angle Distribution;angle;nclusters",                                                       100, 0.0, 3.15);
	TH2D clusterPairAngleCorresondence_H   ("ClustePairAngleCorrespondence",  "Cluster Pair Individual Angle Correspondence;angle of first cluster;angle of second cluster;nclusters",          100, 0.0, 3.15, 100, 0.0, 3.15);
	TH3D clusterPairAngle_vs_clusterAngle_H("ClustePairAngleVSClusterAngle",  "Cluster Pair Angle vs Cluster Pair Angle;angle of first cluster;angle of second cluster;relative cluster angle", 100, 0.0, 3.15, 100, 0.0, 3.15, 100, 0.0, 3.15);
	std::vector<std::shared_ptr<TH1*>> histograms;
	histograms.emplace_back(std::make_shared<TH1*>(&clusterAngle_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterPairIndAngle_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterPairRelAngle_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterPairAngleCorresondence_H));
	histograms.emplace_back(std::make_shared<TH1*>(&clusterPairAngle_vs_clusterAngle_H));
	// Main loop
	timer.restart("Measuring the time required to create the event angle plots...");
	int eventNum = 0;
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		for(const Cluster& cluster: clusterCollection)
		{
			float angle = ClusterPairFunctions::getClusterIndAngle(cluster);
			clusterAngle_H.Fill(angle);
		}
		ClusterPairFunctions::PairCollectionType pairCollection(ClusterPairFunctions::getClusterPairCollection(clusterCollection));
		for(const ClusterPairFunctions::PairType& pair: pairCollection)
		{
			float firstAngle  = ClusterPairFunctions::getClusterIndAngle(*pair.first);
			float secondAngle = ClusterPairFunctions::getClusterIndAngle(*pair.second);
			float pairAngle   = ClusterPairFunctions::getClusterPairAngle(pair);
			clusterPairIndAngle_H.Fill(firstAngle);
			clusterPairIndAngle_H.Fill(secondAngle);
			clusterPairRelAngle_H.Fill(pairAngle);
			clusterPairAngleCorresondence_H.Fill(std::min(firstAngle, secondAngle), std::max(firstAngle, secondAngle));
			clusterPairAngle_vs_clusterAngle_H.Fill(std::min(firstAngle, secondAngle), std::max(firstAngle, secondAngle), pairAngle);
		}
		++eventNum;
	}
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	std::vector<std::shared_ptr<TCanvas>> canvases;
	for(const auto& i: range(histograms.size()))
	{
		auto& histogram = *(*histograms[i]);
		canvases.emplace_back(std::make_shared<TCanvas>(("canvas_" + std::to_string(i)).c_str(), histogram.GetTitle(), 50 + i * 303, 50, 300, 300));
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
	theApp -> Run();
	inputFile -> Close();
	std::cout << process_prompt << "PlotEventClusters terminated succesfully." << std::endl;
	return 0; 
}
catch(const std::exception& e)
{
	std::cout << error_prompt << "Exception thrown: " << e.what() << std::endl;
	return -1;
}

void printUsage(int& argc, char** argv, bool killProcess)
{
	std::cout << "Usage: " << argv[0] << " <Ntuple path> <optional: --savePlots>" << std::endl;
	if(killProcess) exit(-1);
}

void processArgs(int& argc, char** argv, std::string& inputFileName, int& savePlots)
{
	if(argc != 2 && argc != 3)
	{
		printUsage(argc, argv, true);
	}
	inputFileName = argv[1];
	if(argc == 3)
	{
		if(std::string(argv[2]) == std::string("--savePlots"))
		{
			savePlots = 1;
		}
		else
		{
			printUsage(argc, argv, true);
		}
	}
	else
	{
		savePlots = 0;
	}
}

std::map<int, std::vector<Cluster>> getClusterCollectionSortedByEvtnum(TTree* clusterTree, EventData& eventField, Cluster& clusterField)
{
	std::map<int, std::vector<Cluster>> eventClustersMap;
	Int_t totalNumEntries = clusterTree -> GetEntries();
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		clusterTree -> GetEntry(entryIndex);
		int eventNum = eventField.evt;
		auto eventClustersIt = eventClustersMap.find(eventNum);
		// If key does not exist yet: add key
		if(eventClustersIt == eventClustersMap.end())
		{
			eventClustersIt = eventClustersMap.emplace(eventField.evt, std::vector<Cluster>()).first;
		}
		eventClustersIt -> second.push_back(clusterField);
	}
	return eventClustersMap;
}
