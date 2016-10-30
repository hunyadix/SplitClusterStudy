// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../TimerForBenchmarking/interface/TimerColored.h"
#include "../../../interface/HelperFunctionsCommon.h"
// #include "../../../interface/CanvasExtras.h"
#include "../interface/ModuleClusterPlot.h"

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

void printUsage(int& argc, char** argv, bool killProcess);
void processArgs(int& argc, char** argv, std::string& inputFileName, unsigned int& numEventsToMerge, int& savePlots);
std::map<int, std::vector<Cluster>> getClusterCollectionSortedByEvtnum(TTree* clusterTree, EventData& eventField, Cluster& clusterField);
void printClusterFieldInfo(const Cluster& clusterField);

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
	timer.restart("Measuring the time required for separating clusters by events...");
	std::map<int, std::vector<Cluster>> eventClustersMap(getClusterCollectionSortedByEvtnum(clusterTree, eventField, clusterField));
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	// Histogram definitions
	const auto& digisType                    = ModuleClusterPlot::Type::digis;
	const auto& pairsType                    = ModuleClusterPlot::Type::pairs;
	const auto& pairsWithMarkersType         = ModuleClusterPlot::Type::pairsWithMarkers;
	const auto& pairsWithNeighboursType      = ModuleClusterPlot::Type::pairsWithNeighbours;
	const auto& pairsWithAngleColorCodesType = ModuleClusterPlot::Type::pairsWithAngleColorCodes;
	std::vector<std::shared_ptr<ModuleClusterPlot>> moduleClusterPlots;
	const auto defineStandardPlots = [&moduleClusterPlots] (const ModuleClusterPlot::Type& type, const int& layer, const int& module, const int& ladder)
	{
		for(const int& i: range(10))
		{
			moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, i, i));
		}
		moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, 0, 1));
		moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(type, layer, module, ladder, 0, 9));
	};
	defineStandardPlots(digisType,                    1, 1, 2);
	defineStandardPlots(pairsType,                    1, 1, 2);
	defineStandardPlots(pairsWithMarkersType,         1, 1, 2);
	defineStandardPlots(pairsWithNeighboursType,      1, 1, 2);
	defineStandardPlots(pairsWithAngleColorCodesType, 1, 1, 2);
	defineStandardPlots(digisType,                    1, 4, 6);
	defineStandardPlots(pairsType,                    1, 4, 6);
	defineStandardPlots(pairsWithMarkersType,         1, 4, 6);
	defineStandardPlots(pairsWithNeighboursType,      1, 4, 6);
	defineStandardPlots(pairsWithAngleColorCodesType, 1, 4, 6);
	// Loop on data
	timer.restart("Measuring the time required to create the event plots...");
	int eventNum = 0;
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		ModuleClusterPlot::fillDigisMarkers(clusterCollection, eventNum);
		ModuleClusterPlot::fillAllPairs(clusterCollection, eventNum);
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

void printUsage(int& argc, char** argv, bool killProcess)
{
	std::cout << "Usage: " << argv[0] << " <Ntuple path> <num. events to merge> <optional: --savePlots>" << std::endl;
	if(killProcess) exit(-1);
}

void processArgs(int& argc, char** argv, std::string& inputFileName, unsigned int& numEventsToMerge, int& savePlots)
{
	if(argc != 3 && argc != 4)
	{
		printUsage(argc, argv, true);
	}
	inputFileName = argv[1];
	numEventsToMerge = atoi(argv[2]);
	if(argc == 4)
	{
		if(std::string(argv[3]) == std::string("--savePlots"))
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

void printClusterFieldInfo(const Cluster& clusterField)
{
	std::cout << "--- Begin cluster field info ---" << std::endl;

	std::cout << "det: "          << clusterField.mod_on.det    << "." << std::endl;
	std::cout << "layer: "        << clusterField.mod_on.layer  << "." << std::endl;
	std::cout << "module: "       << clusterField.mod_on.module << "." << std::endl;
	std::cout << "ladder: "       << clusterField.mod_on.ladder << "." << std::endl;
	std::cout << "x: "            << clusterField.x                << "." << std::endl;
	std::cout << "y: "            << clusterField.y                << "." << std::endl;
	std::cout << "clustSizeX: "   << clusterField.sizeX        << "." << std::endl;
	std::cout << "clustSizeY: "   << clusterField.sizeY        << "." << std::endl;
	std::cout << "clustIndex: "   << clusterField.clusterIndex       << "." << std::endl;
	std::cout << "clustSize: "    << clusterField.clusterSize        << "." << std::endl;
	std::cout << "charge: "       << clusterField.charge        << "." << std::endl;

	std::cout << "pixelsCol:    vector of size: "  << clusterField.pixelsCol.size()    << ". First element: " << clusterField.pixelsCol[0]    << "." << std::endl;
	std::cout << "pixelsRow:    vector of size: "  << clusterField.pixelsRow.size()    << ". First element: " << clusterField.pixelsRow[0]    << "." << std::endl;
	std::cout << "pixelsAdc:    vector of size: "  << clusterField.pixelsAdc.size()    << ". First element: " << clusterField.pixelsAdc[0]    << "." << std::endl;
	std::cout << "pixelsMarker: vector of size: "  << clusterField.pixelsMarker.size() << ". First element: " << clusterField.pixelsMarker[0] << "." << std::endl;

	std::cout << "--- End cluster field info ---" << std::endl;
}