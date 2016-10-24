// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../TimerForBenchmarking/interface/TimerColored.h"
#include "../../../interface/HelperFunctionsCommon.h"
#include "../../../interface/CanvasExtras.h"

#include "../../../interface/LayerEventPlotTriplet.h"

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

void printUsage(int& argc, char** argv, bool killProcess);
void processArgs(int& argc, char** argv, std::string& inputFileName, unsigned int& numEventsToMerge, int& savePlots);
int moduleAndColToXCoordinate(const int& module, const int& col);
int ladderAndRowToYCoordinate(const int& ladder, const int& row);
void markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers);
void printFillEventPlotError(const TH2D& histogram, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, const int& moduleCoordinate, const int& ladderCoordinate, const int& isReversedModule);
void fillEventPlot(LayerEventPlotTriplet& histogramTriplet, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, bool fillMissingPixels = false);
void printClusterFieldInfo(const Cluster& clusterField);

int main(int argc, char** argv) try
{
	std::string inputFileName("");
	unsigned int numEventsToMerge = 1;
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, numEventsToMerge, savePlots);
	std::cout << process_prompt << "PlotEventClusters started..." << std::endl;
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
	std::map<int, LayerEventPlotTriplet> eventPlots;
	auto eventBranch = TTreeTools::checkGetBranch(clusterTree, "event");
	eventBranch -> SetAddress(&eventField);
	// Check if data is present
	Int_t totalNumEntries = clusterTree -> GetEntries();
	if(totalNumEntries == 0) throw std::runtime_error("No entries found in tree: clusterTree.");
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Get the indecies of the first "numEventsToMerge" events
	std::cout << process_prompt << "Filtering the event numbers of the first " << numEventsToMerge << " events..." << std::endl;
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		eventBranch -> GetEntry(entryIndex);
		int eventNum = eventField.evt;
		auto plotTripletIt = eventPlots.find(eventNum);
		// If key does not exist yet: add key
		if(plotTripletIt == eventPlots.end())
		{
			if(eventPlots.size() < numEventsToMerge)
			{
				eventPlots.emplace(eventNum, eventNum);
			}
		}
	}
	std::cout << process_prompt << "Done." << std::endl;
	ClusterDataTree::associateDataFieldsFromTree(clusterTree, eventField, clusterField);
	TTreeTools::checkGetBranch(clusterTree, "pixelsCol")    -> SetAddress(&pixelsColWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsRow")    -> SetAddress(&pixelsRowWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsAdc")    -> SetAddress(&pixelsAdcWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsMarker") -> SetAddress(&pixelsMarkerWrapper);
	// Loop on data
	eventBranch -> GetEntry(0);
	timer.restart("Measuring the time required for the looping...");
	auto plotTripletIt = eventPlots.find(eventField.evt);
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		clusterTree -> GetEntry(entryIndex);
		int eventNum = eventField.evt;
		// Search only when a change happens in the eventNum
		if(eventNum != plotTripletIt -> first)
		{
			auto searchResults = eventPlots.find(eventNum);
			if(searchResults == eventPlots.end()) continue;
			plotTripletIt = std::move(searchResults);
		}
		for(unsigned int digiIndex = 0; digiIndex < clusterField.pixelsCol.size(); ++digiIndex)
		{
			// Only plotting barrel clusters
			if(clusterField.mod_on.det != 0) continue;
			int col         = clusterField.pixelsCol[digiIndex];
			int row         = clusterField.pixelsRow[digiIndex];
			int markerState = clusterField.pixelsMarker[digiIndex];
			// printClusterFieldInfo(clusterField);
			plotTripletIt -> second.fillEventPlot(clusterField.mod_on, col, row, markerState, true);
		}
	}
	if(eventPlots.size() < numEventsToMerge)
	{
		std::cout << error_prompt << "The ntuple file only contains " << eventPlots.size() << " events. Merging only those..." << std::endl;
	}
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	LayerEventPlotTriplet mergedEvents_H(-1);
	mergedEvents_H.layer1.SetNameTitle("merged_event_plot_layer_1", "Merged event plot layer 1;module pix. (col);ladder pix. (row)");
	mergedEvents_H.layer2.SetNameTitle("merged_event_plot_layer_2", "Merged event plot layer 2;module pix. (col);ladder pix. (row)");
	mergedEvents_H.layer3.SetNameTitle("merged_event_plot_layer_3", "Merged event plot layer 3;module pix. (col);ladder pix. (row)");
	gStyle -> SetPalette(1);
	// int numPlot = 1;
	std::vector<std::shared_ptr<TCanvas>> canvases;
	// std::vector<std::pair<int, int>> lowEtaXRanges = 
	// {
	// 	std::make_pair<int, int>(200, 350),
	// 	std::make_pair<int, int>(200, 350),
	// 	std::make_pair<int, int>(200, 350)
	// };
	// std::vector<std::pair<int, int>> lowEtaYRanges =
	// {
	// 	std::make_pair<int, int>(200, 350),
	// 	std::make_pair<int, int>(200, 350),
	// 	std::make_pair<int, int>(200, 350)
	// };
	// std::vector<std::pair<int, int>> highEtaXRanges = 
	// {
	// 	std::make_pair<int, int>(-1800, -1650),
	// 	std::make_pair<int, int>(-1800, -1650),
	// 	std::make_pair<int, int>(-1800, -1650)
	// };
	// std::vector<std::pair<int, int>> highEtaYRanges =
	// {
	// 	std::make_pair<int, int>(-1600, -1450),
	// 	std::make_pair<int, int>(-2600, -2450),
	// 	std::make_pair<int, int>(-3500, -3350)
	// };
	for(auto eventNumHistoTripletPair: eventPlots)
	{
		mergedEvents_H.layer1.Add(&eventNumHistoTripletPair.second.layer1);
		mergedEvents_H.layer2.Add(&eventNumHistoTripletPair.second.layer2);
		mergedEvents_H.layer3.Add(&eventNumHistoTripletPair.second.layer3);
		// // Added to save individual events
		// canvases.emplace_back(std::make_shared<TCanvas>(("canvas_layer_2_lowEta_evt_" + std::to_string(eventNumHistoTripletPair.first)).c_str(), eventNumHistoTripletPair.second.layer2.GetTitle(), 200, 200, 300, 300));
		// canvases.back() -> cd();
		// eventNumHistoTripletPair.second.layer2.GetXaxis() -> SetRangeUser(lowEtaXRanges[1].first, lowEtaXRanges[1].second);
		// eventNumHistoTripletPair.second.layer2.GetYaxis() -> SetRangeUser(lowEtaYRanges[1].first, lowEtaYRanges[1].second);
		// eventNumHistoTripletPair.second.layer2.Draw("COLZ");
		// canvases.emplace_back(std::make_shared<TCanvas>(("canvas_layer_2_highEta_evt_" + std::to_string(eventNumHistoTripletPair.first)).c_str(), eventNumHistoTripletPair.second.layer2.GetTitle(), 200, 200, 300, 300));
		// canvases.back() -> cd();
		// eventNumHistoTripletPair.second.layer2.GetXaxis() -> SetRangeUser(highEtaXRanges[1].first, highEtaXRanges[1].second);
		// eventNumHistoTripletPair.second.layer2.GetYaxis() -> SetRangeUser(highEtaYRanges[1].first, highEtaYRanges[1].second);
		// eventNumHistoTripletPair.second.layer2.Draw("COLZ");
	}
	canvases.emplace_back(std::make_shared<TCanvas>("canvas_1", mergedEvents_H.layer1.GetTitle(), 50, 50, 300, 300));
	canvases.back() -> cd();
	redesignCanvas(canvases.back().get(), &mergedEvents_H.layer1);
	mergedEvents_H.layer1.Draw("COLZ");
	canvases.emplace_back(std::make_shared<TCanvas>("canvas_2", mergedEvents_H.layer2.GetTitle(), 353, 50, 300, 300));
	canvases.back() -> cd();
	redesignCanvas(canvases.back().get(), &mergedEvents_H.layer2);
	mergedEvents_H.layer2.Draw("COLZ");
	canvases.emplace_back(std::make_shared<TCanvas>("canvas_3", mergedEvents_H.layer3.GetTitle(), 656, 50, 300, 300));
	canvases.back() -> cd();
	redesignCanvas(canvases.back().get(), &mergedEvents_H.layer3);
	// Axes
	mergedEvents_H.layer3.Draw("COLZ");
	mergedEvents_H.layer1.GetXaxis() -> SetRangeUser(-330, -280);
	mergedEvents_H.layer1.GetYaxis() -> SetRangeUser(-1040, -900);
	mergedEvents_H.layer2.GetXaxis() -> SetRangeUser(1240, 1420);
	mergedEvents_H.layer2.GetYaxis() -> SetRangeUser(-1950, -1830);
	mergedEvents_H.layer3.GetXaxis() -> SetRangeUser(860, 940);
	mergedEvents_H.layer3.GetYaxis() -> SetRangeUser(-1000, -850);
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
	std::cout << process_prompt << "PlotEventClusters terminated succesfully." << std::endl;
	theApp -> Run();
	inputFile -> Close();
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