// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../interface/HelperFunctionsCommon.h"
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
#include <TH1F.h>
#include <TH1D.h>
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
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, savePlots);
	std::cout << process_prompt << argv[0] << " started..." << std::endl;
	TimerColored timer(timer_prompt);
	TApplication* theApp = new TApplication("App", &argc, argv);
	TFile* inputFile = TFile::Open(inputFileName.c_str(), "READ");
	TTree* clusterTree = (TTree*)(inputFile -> Get("clustTree"));
	TTreeTools::treeCheck(clusterTree, "Tree missing.", true);
	Cluster  clusterField;
	EventData eventField;
	auto clusterSizeBranch  = TTreeTools::checkGetBranch(clusterTree, "clusterSize");
	auto clusterSizeYBranch = TTreeTools::checkGetBranch(clusterTree, "sizeY");
	auto moduleOnBranch     = TTreeTools::checkGetBranch(clusterTree, "mod_on");
	clusterSizeBranch  -> SetAddress(&clusterField.clusterSize);
	clusterSizeYBranch -> SetAddress(&clusterField.sizeY);
	moduleOnBranch     -> SetAddress(&(clusterField.mod_on));
	// Histogram definitions
	std::vector<std::shared_ptr<TH1D>>    histograms;
	std::vector<std::shared_ptr<TCanvas>> canvases;
	int canvasSizeX = 300;
	int canvasSizeY = 300;
	for(int layerNum = 1; layerNum <= 3; ++layerNum)
	{
		histograms.emplace_back(std::make_shared<TH1D>(("Sizes_layer_" + std::to_string(layerNum)).c_str(), ("Cluster sizes on layer " + std::to_string(layerNum) + ";size (pixels); num. clusters").c_str(), 50, 0, 50));
		canvases.emplace_back(std::make_shared<TCanvas>(("canvas_" + std::to_string(layerNum)).c_str(), ("canvas_" + std::to_string(layerNum)).c_str(), 50 + (layerNum - 1) * (canvasSizeX + 3), 50, canvasSizeX, canvasSizeY));
		CanvasExtras::redesignCanvas(canvases.back().get(), histograms.back().get());
	}
	for(int layerNum = 1; layerNum <= 3; ++layerNum)
	{
		histograms.emplace_back(std::make_shared<TH1D>(("Y sizes_layer_" + std::to_string(layerNum)).c_str(), ("Cluster Y sizes on layer " + std::to_string(layerNum) + ";size (pixels); num. clusters").c_str(), 50, 0, 50));
		canvases.emplace_back(std::make_shared<TCanvas>(("canvas_" + std::to_string(layerNum + 3)).c_str(), ("canvas_" + std::to_string(layerNum + 3)).c_str(), 50 + (layerNum - 1) * (canvasSizeX + 3), 50 + (canvasSizeY + 30), canvasSizeX, canvasSizeY));
	}
	// Get number of entries
	auto eventBranch = TTreeTools::checkGetBranch(clusterTree, "event");
	eventBranch -> SetAddress(&eventField);
	Int_t totalNumEntries = clusterTree -> GetEntries();
	// Check if data is present
	if(totalNumEntries == 0) throw std::runtime_error("No entries found in tree: clusterTree.");
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Loop on data
	timer.restart("Measuring the time required for the looping...");
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		moduleOnBranch     -> GetEntry(entryIndex);
		if(clusterField.mod_on.det != 0) continue;
		clusterSizeBranch  -> GetEntry(entryIndex);
		clusterSizeYBranch -> GetEntry(entryIndex);
		int layer = clusterField.mod_on.layer;
		if(layer < 1 || 3 < layer)
		{
			std::cout << error_prompt << "Unset or bad layer number: " << layer << std::endl;
		}
		histograms[layer - 1]     -> Fill(clusterField.clusterSize);
		histograms[layer + 3 - 1] -> Fill(clusterField.sizeY);
	}
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	gStyle -> SetPalette(1);
	for(int histogram_index: range(histograms.size()))
	{
		canvases[histogram_index] -> cd();
		histograms[histogram_index] -> GetXaxis() -> SetRangeUser(0, 25);
		histograms[histogram_index] -> Draw("PHE1");
		canvases[histogram_index] -> Update();
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
