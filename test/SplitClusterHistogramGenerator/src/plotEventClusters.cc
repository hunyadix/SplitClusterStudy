// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"

#include "../../../TimerForBenchmarking/interface/TimerColored.h"

// Root
#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2D.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <functional>

// Chip size: 80 rows, 52 columns
// Ladder coordinate range:
// Layer 1: [-10, 10];
// Layer 2: [-16, 16];
// Layer 3: [-22, 22];
// Pixels per module in ladder coordinate direction: 80 * 2 = 160
// Histo definition:
// Layer 1: [-10, 10] -> [1679, 1679];
// Layer 2: [-16, 16] -> [2639, 2639];
// Layer 3: [-22, 22] -> [3599, 3599];
// Module coordinate range:
// All layers: [-4, 4] + extra for 0
// Pixels per module in ladder coordinate direction: 52 * 8 = 416
// Histo definition:
// All layers: [-1871, 1871]

struct LayerEventPlotTriplet
{
	LayerEventPlotTriplet(int nameIndex) : 
		layer1(("event_plot_layer_1_" + std::to_string(nameIndex)).c_str(), ("event_plot_layer_1_" + std::to_string(nameIndex) + ";module pix. (col);ladder pix. (row)").c_str(), 3743, -1871.5, 1871.5, 3359, -1679.5, 1679.5),
		layer2(("event_plot_layer_2_" + std::to_string(nameIndex)).c_str(), ("event_plot_layer_2_" + std::to_string(nameIndex) + ";module pix. (col);ladder pix. (row)").c_str(), 3743, -1871.5, 1871.5, 5279, -2639.5, 2639.5),
		layer3(("event_plot_layer_3_" + std::to_string(nameIndex)).c_str(), ("event_plot_layer_3_" + std::to_string(nameIndex) + ";module pix. (col);ladder pix. (row)").c_str(), 3743, -1871.5, 1871.5, 7199, -3599.5, 3599.5) {}
	TH2D layer1;
	TH2D layer2;
	TH2D layer3;
};

void printFillEventPlotError(const TH2D& histogram, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, const int& moduleCoordinate, const int& ladderCoordinate, const int& isReversedModule);
void fillEventPlot(LayerEventPlotTriplet& histogramTriplet, const ModuleData& mod_on, const int& col, const int& row, const int& markerState);
void printClusterFieldInfo(const Cluster& clusterField);

int main(int argc, char** argv)
{
	try
	{
		std::cout << debug_prompt << "PlotEventClusters started..." << std::endl;
		gStyle -> SetPalette(1);
		TimerColored timer(timer_prompt);
		TApplication* theApp = new TApplication("App", &argc, argv);
		TFile* inputFile = TFile::Open("/data/hunyadi/CMSSW/SplitClusterStudy/CMSSW_8_0_18/src/TestSplitClusterStudy/Ntuple_scs.root", "READ");
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
		// Histogram definitions
		std::map<int, LayerEventPlotTriplet> eventPlots;
		// Loop on data
		timer.restart("Measuring the time required for the looping...");
		for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
		{
			clusterTree -> GetEntry(entryIndex);
			int eventNum = eventField.evt;
			auto plotTriplet_it = eventPlots.find(eventNum);
			// If key does not exist yet: add key
			if(plotTriplet_it == eventPlots.end())
			{
				std::cout << debug_prompt << "Adding event plots for event: " << eventNum << std::endl;
				plotTriplet_it = eventPlots.emplace(eventNum, eventNum).first;
			}
			// std::cout << clusterField.clusterSize << " should equal " << clusterField.pixelsCol.size() << std::endl;
			for(unsigned int digiIndex = 0; digiIndex < clusterField.pixelsCol.size(); ++digiIndex)
			{
				int col         = clusterField.pixelsCol[digiIndex];
				int row         = clusterField.pixelsRow[digiIndex];
				int markerState = clusterField.pixelsMarker[digiIndex];
				// printClusterFieldInfo(clusterField);
				fillEventPlot(plotTriplet_it -> second, clusterField.mod_on, col, row, markerState);
			}
		}
		timer.printSeconds("Loop done. Took about: ", " second(s).");
		TCanvas canvas("canvas", "canvas", 10, 10, 1500, 500);
		canvas.Divide(3, 1);
		canvas.cd(1);
		eventPlots.at(1).layer1.Draw("COLZ");
		canvas.cd(2);
		eventPlots.at(1).layer2.Draw("COLZ");
		canvas.cd(3);
		eventPlots.at(1).layer3.Draw("COLZ");
		canvas.Update();
		theApp -> Run();
		inputFile -> Close();
		std::cout << debug_prompt << "PlotEventClusters terminated succesfully." << std::endl;
	}
	catch(const std::out_of_range& e)
	{
		std::cout << error_prompt << "Out of range exception was thrown: " << e.what() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << error_prompt << "Exception thrown: " << e.what() << std::endl;
	}
	return 0; 
}

void printFillEventPlotError(const TH2D& histogram, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, const int& moduleCoordinate, const int& ladderCoordinate, const int& isReversedModule)
{
	std::cout << c_blue << "Warning: " << c_def << "Filling (" << moduleCoordinate << ", " << ladderCoordinate << ") that has already been filled. Value before filling: " << histogram.GetBinContent(moduleCoordinate, ladderCoordinate) << ". Reversed module? " << isReversedModule << std::endl; 
	std::cout << c_blue << "Warning: " << c_def << "mod_on.layer: "  << mod_on.layer  << ". mod_on.ladder: " << mod_on.ladder << ". mod_on.module: " << mod_on.module << std::endl;
	std::cout << c_blue << "Warning: " << c_def << "row: " << row << ". col: " << col << std::endl;
};

void fillEventPlot(LayerEventPlotTriplet& histogramTriplet, const ModuleData& mod_on, const int& col, const int& row, const int& markerState)
{
	if(mod_on.det == 1) return;
	int isReversedModule = (mod_on.ladder + (0 < mod_on.ladder)) % 2;
	int moduleCoordinate = NOVAL_I;
	int ladderCoordinate = NOVAL_I;
	if(isReversedModule)
	{
		if(mod_on.ladder < 0) ladderCoordinate = (mod_on.ladder + 0.5) * 160 - row;
		if(0 < mod_on.ladder) ladderCoordinate = (mod_on.ladder + 0.5) * 160 - row - 1;
		if(mod_on.ladder == 1)  ladderCoordinate += 80;
		if(mod_on.ladder == -1) ladderCoordinate -= 80;
	}
	else
	{
		if(mod_on.ladder < 0) ladderCoordinate = (mod_on.ladder - 0.5) * 160 + row + 1;
		if(0 < mod_on.ladder) ladderCoordinate = (mod_on.ladder - 0.5) * 160 + row;
		if(mod_on.ladder == 1)  ladderCoordinate += 80;
		if(mod_on.ladder == -1) ladderCoordinate -= 80;
	}
	if(mod_on.module < 0) moduleCoordinate = (mod_on.module - 0.5) * 416 + col + 1;
	if(0 < mod_on.module) moduleCoordinate = (mod_on.module - 0.5) * 416 + col;
	int fillWeight = markerState ? 2 : 1;
	switch(mod_on.layer)
	{
		case 1:
			if(histogramTriplet.layer1.GetBinContent(moduleCoordinate, ladderCoordinate) != 0) printFillEventPlotError(histogramTriplet.layer1, mod_on, col, row, markerState, moduleCoordinate, ladderCoordinate, isReversedModule);
			histogramTriplet.layer1.Fill(moduleCoordinate, ladderCoordinate, fillWeight);
			break;
		case 2:
			if(histogramTriplet.layer2.GetBinContent(moduleCoordinate, ladderCoordinate) != 0) printFillEventPlotError(histogramTriplet.layer2, mod_on, col, row, markerState, moduleCoordinate, ladderCoordinate, isReversedModule);
			histogramTriplet.layer2.Fill(moduleCoordinate, ladderCoordinate, fillWeight);
			break;
		case 3:
			if(histogramTriplet.layer3.GetBinContent(moduleCoordinate, ladderCoordinate) != 0) printFillEventPlotError(histogramTriplet.layer3, mod_on, col, row, markerState, moduleCoordinate, ladderCoordinate, isReversedModule);
			histogramTriplet.layer3.Fill(moduleCoordinate, ladderCoordinate, fillWeight);
			break;
		default:
			std::cout << c_red << "Error: " << c_def << "layer coordinate of a pixel is invalid: " << mod_on.layer << std::endl;
			std::cout << "Info: Det: " << mod_on.det << ". Ladder:" << mod_on.ladder << ". Module:" << mod_on.module << "." << std::endl;
			break;
	}
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