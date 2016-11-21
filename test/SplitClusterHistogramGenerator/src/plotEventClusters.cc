// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../TimerForBenchmarking/interface/TimerColored.h"
#include "../../../interface/HelperFunctionsCommon.h"
// #include "../../../interface/CanvasExtras.h"
#include "../interface/ModuleClusterPlot.h"
#include "../interface/TestsCommon.h"

// Root
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TText.h>
#include <TH1F.h>
#include <TH2D.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <functional>

int main(int argc, char** argv) try
{
	std::string inputFileName("");
	unsigned int numEventsToMerge = 1;
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, numEventsToMerge, savePlots);
	std::cout << process_prompt << argv[0] << " started..." << std::endl;
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
	// Histogram definitions
	auto eventBranch = TTreeTools::checkGetBranch(clusterTree, "event");
	eventBranch -> SetAddress(&eventField);
	// Check if data is present
	Int_t totalNumEntries = clusterTree -> GetEntries();
	if(totalNumEntries == 0) throw std::runtime_error("No entries found in tree: clusterTree.");
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Get the indecies of the first "numEventsToMerge" events
	std::cout << process_prompt << "Filtering the event numbers of the first " << numEventsToMerge << " events..." << std::endl;
	std::cout << process_prompt << "Done." << std::endl;
	ClusterDataTree::associateDataFieldsFromTree(clusterTree, eventField, clusterField);
	TTreeTools::checkGetBranch(clusterTree, "pixelsCol")    -> SetAddress(&pixelsColWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsRow")    -> SetAddress(&pixelsRowWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsAdc")    -> SetAddress(&pixelsAdcWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsMarker") -> SetAddress(&pixelsMarkerWrapper);
	// Loop to separate events
	timer.restart("Measuring the time required for separating clusters by events...");
	std::map<int, std::vector<Cluster>> eventClustersMap(getClusterCollectionSortedByEvtnum(clusterTree, eventField, clusterField));
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	// Histogram definitions
	// const auto& digisType                          = ModuleClusterPlot::Type::digis;
	// const auto& digisFromMarkersType               = ModuleClusterPlot::Type::digisFromMarkers;
	const auto& digisFromMarkersWithNeighboursType = ModuleClusterPlot::Type::digisFromMarkersWithNeighbours;
	std::vector<std::shared_ptr<ModuleClusterPlot>> moduleClusterPlots;
	// const auto defineStandardPlots = [&moduleClusterPlots] (const ModuleClusterPlot::Type& type, const int& layer, const int& module, const int& ladder)
	// {
	// 	for(const int& i: range(10))
	// 	{
	// 		moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, i, i));
	// 	}
	// 	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, 0, 1));
	// 	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, 0, 9));
	// };
	// defineStandardPlots(digisType, 1, 1, 2);
	// defineStandardPlots(digisFromMarkersType, 1, 1, 2);
	// defineStandardPlots(digisFromMarkersWithNeighboursType, 1, 1, 2);
	// defineStandardPlots(digisType, 1, 4, 6);
	// defineStandardPlots(digisFromMarkersType, 1, 4, 6);
	// defineStandardPlots(digisFromMarkersWithNeighboursType, 1, 4, 6);
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1,  2, -10, 0, 0));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1, -1,   2, 3, 3));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1, -3,   7, 3, 3));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1,  3,   7, 3, 3));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 2, -4,  -4, 3, 3));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1, -1,   6, 4, 4));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 2, -3,   8, 4, 4));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1,  3,  -7, 4, 4));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1, -3,   7, 5, 5));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1, -4,   1, 5, 5));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1,  2,  -2, 6, 6));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1,  4,   4, 8, 8));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1,  1,  -2, 9, 9));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(digisFromMarkersWithNeighboursType, 1, -3,   9, 9, 9));
	// Primary loop
	timer.restart("Measuring the time required to create the event plots...");
	int eventNum = 0;
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		for(const Cluster& cluster: clusterCollection)
		{
			ModuleClusterPlot::fillDigisMarkers(cluster, eventNum);
		}
		ModuleClusterPlot::saveAllFinished(eventNum);
		++eventNum;
	}
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	std::cout << process_prompt << argv[0] << " terminated succesfully." << std::endl;
	inputFile -> Close();
	theApp -> Run();
	return 0; 
}
catch(const std::out_of_range& e)
{
	std::cout << error_prompt << "Out of range exception was thrown: " << e.what() << std::endl;
	return -1;
}
catch(const std::exception& e)
{
	std::cout << error_prompt << "Exception thrown: " << e.what() << std::endl;
	return -1;
}
