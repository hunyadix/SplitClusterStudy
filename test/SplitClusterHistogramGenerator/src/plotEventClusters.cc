// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../TimerForBenchmarking/interface/TimerColored.h"
#include "../../../interface/HelperFunctionsCommon.h"
#include "../../../interface/CanvasExtras.h"

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

class ModuleClusterPlot
{
	public:
		enum Type {digi = 0, digiFromMarker, digiFromMarkerWithNeighbours, cluster};
	private:
		static constexpr std::array<const char*, 4> histogramTypePrefixes = {{ "Digis", "Marked digis", "Ntuple markers with neighbours", "Clusters" }};
		static constexpr float BASE_DIGI_FILL_VALUE    = 0.5f;
		static constexpr float MISSING_NEIGHBOUR_VALUE = 1000.0f;
		static constexpr float PALETTE_MINIMUM    = 0.5f;
		static constexpr float PALETTE_MAXIMUM    = 1500.0f;
		TH2D histogram;
		Type type;
		int layer;
		int module;
		int ladder;
		int startEvent;
		int endEvent;
		static std::vector<ModuleClusterPlot*> moduleClusterPlotCollection;
	private:
		static void markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers);
		static void fillMissingPixels(const int& col, const int& row, const int& markerState, TH2D& histo, const int& weight);
	public:
		ModuleClusterPlot(Type typeArg, const int& layer, const int& module, const int& ladder, const int& startEventArg, const int& endEventArg);
		int         isEventNumInRange(const int& eventNum);
		static void fillAll(Cluster Cluster, const int& eventNum);
		static void saveAllFinished(const int& eventNum);
};

constexpr std::array<const char*, 4> ModuleClusterPlot::histogramTypePrefixes;
std::vector<ModuleClusterPlot*> ModuleClusterPlot::moduleClusterPlotCollection;

ModuleClusterPlot::ModuleClusterPlot(Type typeArg, const int& layerArg, const int& moduleArg, const int& ladderArg, const int& startEventArg, const int& endEventArg):
	histogram(
		(std::string("ModuleClusterPlot") + "_" + std::to_string(moduleClusterPlotCollection.size()) + "_" + std::to_string(layerArg) + "_" + std::to_string(moduleArg) +  + "_" + std::to_string(ladderArg) + "_" + std::to_string(startEventArg) + "_" + std::to_string(endEventArg)).c_str(), 
		(std::string(histogramTypePrefixes[typeArg]) + " on layer " + std::to_string(layerArg) + ", module " + std::to_string(moduleArg) + ", ladder " + std::to_string(ladderArg) + ";module pix. (col);ladder pix. (row)").c_str(),
		416, 0, 416,
		160, 0, 160),
	type(typeArg),
	layer(layerArg),
	module(moduleArg),
	ladder(ladderArg),
	startEvent(startEventArg),
	endEvent(endEventArg)
{
	moduleClusterPlotCollection.push_back(this);
}

void ModuleClusterPlot::markerToRowColModifierArrays(const int& markerState, std::vector<int>& colModifiers, std::vector<int>& rowModifiers)
{
	colModifiers.clear();
	rowModifiers.clear();
	static auto checkInsertIndexPair = [&colModifiers, &rowModifiers] (const int& pixelIsMarked, const int& col, const int& row)
	{
		if(pixelIsMarked)
		{
			colModifiers.push_back(col);
			rowModifiers.push_back(row);
		}
	};
	checkInsertIndexPair(markerState & (1 << 0), -1, -1);
	checkInsertIndexPair(markerState & (1 << 1), -1,  0);
	checkInsertIndexPair(markerState & (1 << 2), -1, +1);
	checkInsertIndexPair(markerState & (1 << 3),  0, -1);
	checkInsertIndexPair(markerState & (1 << 4),  0, +1);
	checkInsertIndexPair(markerState & (1 << 5), +1, -1);
	checkInsertIndexPair(markerState & (1 << 6), +1,  0);
	checkInsertIndexPair(markerState & (1 << 7), +1, +1);
}

void ModuleClusterPlot::fillMissingPixels(const int& col, const int& row, const int& markerState, TH2D& histo, const int& weight)
{
	std::vector<int> colModifiers;
	std::vector<int> rowModifiers;
	float xAxisBinWidth = histo.GetXaxis() -> GetBinWidth(1);
	float yAxisBinWidth = histo.GetYaxis() -> GetBinWidth(1);
	markerToRowColModifierArrays(markerState, colModifiers, rowModifiers);
	for(unsigned int markedNeighbourIndex = 0; markedNeighbourIndex < colModifiers.size(); ++markedNeighbourIndex)
	{
		int bin = histo.GetBin(col + colModifiers[markedNeighbourIndex] * xAxisBinWidth, row + rowModifiers[markedNeighbourIndex] * yAxisBinWidth);
		if(histo.GetBinContent(bin) == 0) histo.SetBinContent(bin, weight);
	}
}

int ModuleClusterPlot::isEventNumInRange(const int& eventNum)
{
	if(startEvent <= eventNum && eventNum <= endEvent) return true;
	return false;
}

void ModuleClusterPlot::fillAll(Cluster clusterField, const int& eventNum)
{
	auto digisInRange = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> isEventNumInRange(eventNum); });
	ModuleData& mod_on = clusterField.mod_on;
	if(mod_on.det    != 0) return;
	auto filteredList = filter(digisInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.layer  == plotToCheck -> layer;   });
	filteredList      = filter(digisInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.module == plotToCheck -> module; });
	filteredList      = filter(digisInRange, [&mod_on] (ModuleClusterPlot* plotToCheck) { return mod_on.ladder == plotToCheck -> ladder; });
	for(ModuleClusterPlot* plotDefinitionPtr: filteredList)
	{
		TH2D& histogramToFill = plotDefinitionPtr -> histogram;
		for(const int& digiIndex: range(clusterField.pixelsCol.size()))
		{
			int col         = clusterField.pixelsCol[digiIndex];
			int row         = clusterField.pixelsRow[digiIndex];
			int markerState = clusterField.pixelsMarker[digiIndex];
			switch(plotDefinitionPtr -> type)
			{
				case digi:
					histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					break;
				case digiFromMarker:
					if(markerState == 0) histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					else                 histogramToFill.SetBinContent(col, row, markerState);
					break;
				case digiFromMarkerWithNeighbours:
					if(markerState == 0) histogramToFill.SetBinContent(col, row, BASE_DIGI_FILL_VALUE);
					else                 histogramToFill.SetBinContent(col, row, markerState);
					fillMissingPixels(col, row, markerState, histogramToFill, MISSING_NEIGHBOUR_VALUE);
					break;
				case cluster:
					break;
				default:
					std::cerr << "Error in ModuleClusterPlot::fill(): Error deducing type for histogram type." << std::endl; 
					break;
			}
		}
	}
}

void ModuleClusterPlot::saveAllFinished(const int& eventNum)
{
	auto filteredList = filter(moduleClusterPlotCollection, [&eventNum] (ModuleClusterPlot* plotToCheck) { return plotToCheck -> endEvent == eventNum; });
	if(filteredList.size() == 0) return;
	CanvasExtras::setMulticolorColzPalette();
	std::vector<std::shared_ptr<TCanvas>> canvases;
	for(auto moduleClusterPlotToSave: filteredList)
	{
		// std::cout << "Saving plot!!!" << std::endl;
		TH2D& histogram = moduleClusterPlotToSave -> histogram;
		canvases.emplace_back(std::make_shared<TCanvas>(histogram.GetName(), (histogram.GetTitle() + std::string(" ") + std::to_string(moduleClusterPlotToSave -> startEvent) + " " + std::to_string(moduleClusterPlotToSave -> endEvent)).c_str(), 50, 50, 400, 300));
		canvases.back() -> cd();
		CanvasExtras::redesignCanvas(canvases.back().get(), &histogram);
		histogram.GetZaxis() -> SetRangeUser(PALETTE_MINIMUM, PALETTE_MAXIMUM);
		histogram.GetZaxis() -> SetRangeUser(PALETTE_MINIMUM, PALETTE_MAXIMUM);
		histogram.GetZaxis() -> SetRangeUser(PALETTE_MINIMUM, PALETTE_MAXIMUM);
		canvases.back() -> SetLogz();
		histogram.Draw("COLZ");
		TText* xlabel = new TText();
		xlabel -> SetNDC();
		xlabel -> SetTextFont(1);
		xlabel -> SetTextColor(1);
		xlabel -> SetTextSize(0.04);
		xlabel -> SetTextAlign(22);
		xlabel -> SetTextAngle(0);
		xlabel -> DrawText(0.22, 0.98, ("Events: [" + std::to_string(moduleClusterPlotToSave -> startEvent) + " - " + std::to_string(moduleClusterPlotToSave -> endEvent) + "]").c_str());
		// std::cout << histogram.GetTitle() << " is ready." << std::endl;
	}
	for(const auto& canvas: canvases)
	{
		canvas -> Update();
		std::string filename = canvas -> GetTitle();
		std::transform(filename.begin(), filename.end(), filename.begin(), [] (char ch) { return ch == ' ' ? '_' : ch; });
		filename =  "results/" + filename + ".eps";
		canvas -> SaveAs(filename.c_str());
	}
}


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

	std::vector<std::shared_ptr<ModuleClusterPlot>> moduleClusterPlots;

	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 0, 0));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 1, 1));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 2, 2));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 3, 3));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 4, 4));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 5, 5));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 6, 6));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 7, 7));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 8, 8));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 9, 9));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 0, 1));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 2, 2, 0, 9));

	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 0, 0));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 1, 1));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 2, 2));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 3, 3));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 4, 4));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 5, 5));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 6, 6));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 7, 7));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 8, 8));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 9, 9));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 0, 1));
	moduleClusterPlots.push_back(std::make_shared<ModuleClusterPlot>(ModuleClusterPlot::Type::digiFromMarkerWithNeighbours, 1, 4, 6, 0, 9));
	// Loop on data
	timer.restart("Measuring the time required to create the event plots...");
	int eventNum = 0;
	for(const std::pair<int, std::vector<Cluster>>& eventNumClusterCollectionPair: eventClustersMap)
	{
		// const int&                  eventNum          = eventNumClusterCollectionPair.first;
		const std::vector<Cluster>& clusterCollection = eventNumClusterCollectionPair.second;
		for(const Cluster& cluster: clusterCollection)
		{
			ModuleClusterPlot::fillAll(cluster, eventNum);
		}
		ModuleClusterPlot::saveAllFinished(eventNum);
		++eventNum;
	}
	timer.printSeconds("Loop done. Took about: ", " second(s).");
	std::cout << process_prompt << "PlotEventClusters terminated succesfully." << std::endl;
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