// Data structure
#include "../../../interface/EventClustersTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"

// Root
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TCanvas.h>
// #include <TH2I.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <exception>
#include <memory>

int main(int argc, char** argv)
{
	TApplication* theApp = new TApplication("App", 0, 0);
	std::cout << debug_prompt << "PlotEventClusters started..." << std::endl;
	TFile* inputFile = TFile::Open("/data/hunyadi/CMSSW/SplitClusterStudy/CMSSW_8_0_18/src/TestSplitClusterStudy/Ntuple_scm.root", "READ");
	TTree* eventClustersTree = (TTree*)(inputFile -> Get("eventClustersTree"));
	TTreeTools::treeCheck(eventClustersTree, "Tree missing.", true);
	EventClustersDataArrays eventClusterField;
	EventClustersTree::associateDataFieldsFromTree(eventClustersTree, eventClusterField);
	// Get number of entries
	Int_t totalNumEntries = eventClustersTree -> GetEntries();
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Check if data is present
	if(totalNumEntries == 0) {std::cerr << error_prompt << "No entries found in tree: eventClustersTree." << std::endl; exit(-1);}
	// Histogram definitions
	TH1F* chargeDistribution = new TH1F("charge_distribution", "charge_distribution", 50, 0, 50);
	// Loop on data
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		eventClustersTree   -> GetEntry(entryIndex);
		for(const float& charge: *(eventClusterField.charge))
		{
			chargeDistribution -> Fill(charge);
		}
	}
	std::cout << debug_prompt << "Loop done." << std::endl;
	TCanvas canvas("canvas", "canvas");
	chargeDistribution -> Draw();
	std::cin.get();
	std::cin.get();
	inputFile -> Close();
	std::cout << debug_prompt << "PlotEventClusters terminated succesfully." << std::endl;
	return 0; 
}
