// Data structure
#include "../../../interface/EventClustersTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"

// Root
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
// #include <TH1F.h>
// #include <TH2I.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <exception>
#include <memory>

int main(int argc, char** argv)
{
	std::cout << "PlotEventClusters started." << std::endl;
	TFile* inputFile = TFile::Open("/data/hunyadi/CMSSW/SplitClusterStudy/CMSSW_8_0_18/src/TestSplitClusterStudy/Ntuple_scm.root", "READ");
	TTree* eventClustersTree = (TTree*)(inputFile -> Get("eventClustersTree"));
	TTreeTools::treeCheck(eventClustersTree, "Tree missing.", true);
	// FIXME: This is awkward, but otherwise eats away more the available stack memory (segfault)
	// std::auto_ptr<EventClustersDataArrays> eventClusterFieldPtr(new EventClustersDataArrays());
	EventClustersDataArrays* eventClusterFieldPtr = new EventClustersDataArrays();
	EventClustersTree::associateDataFieldsFromTree(eventClustersTree, *eventClusterFieldPtr);
	// Get number of entries
	Int_t totalNumEntries = eventClustersTree -> GetEntries();
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	eventClustersTree -> GetEntry(1);
	// // Check if data is present
	if(totalNumEntries == 0) {std::cerr << error_prompt << "No entries found in tree: eventClustersTree." << std::endl; exit(-1);}
	// // Loop on data
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		eventClustersTree -> GetEntry(entryIndex);
		std::cout << debug_prompt << "nClusters: "   << eventClusterFieldPtr -> size            << std::endl;
		std::cout << debug_prompt << "x: "           << eventClusterFieldPtr -> x[0]            << std::endl;
		std::cout << debug_prompt << "y: "           << eventClusterFieldPtr -> y[0]            << std::endl;
		std::cout << debug_prompt << "sizeX: "       << eventClusterFieldPtr -> sizeX[0]        << std::endl;
		std::cout << debug_prompt << "sizeY: "       << eventClusterFieldPtr -> sizeY[0]        << std::endl;
		std::cout << debug_prompt << "clusterSize: " << eventClusterFieldPtr -> clusterSize[0]  << std::endl;
		std::cout << debug_prompt << "charge: "      << eventClusterFieldPtr -> charge[0]       << std::endl;
	}
	inputFile -> Close();
	delete eventClusterFieldPtr;
	return 0; 
}
