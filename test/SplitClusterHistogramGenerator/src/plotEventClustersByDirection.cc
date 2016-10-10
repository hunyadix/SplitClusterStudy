// Data structure
#include "../../../interface/ClusterDataTree.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"
#include "../../../interface/HelperFunctionsCommon.h"

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

struct ClusterStats
{
	int   startCol    = NOVAL_I;
	int   endCol      = NOVAL_I;
	float startRow    = NOVAL_F;
	float endRow      = NOVAL_F;
	int   startColAdc = NOVAL_I;
	int   endColAdc   = NOVAL_I;
	float length      = NOVAL_F;
	float dir         = NOVAL_F;
	int   error       = 0;
};

bool isMissingPartDoubleColumn(const int& firstCol, const int& firstRow, const int& secondCol, const int& secondRow);
ClusterStats getClusterStats(const Cluster& clusterField);
void printFillEventPlotError(const TH2D& histogram, const ModuleData& mod_on, const int& col, const int& row, const int& markerState, const int& moduleCoordinate, const int& ladderCoordinate, const int& isReversedModule);
void fillEventPlot(LayerEventPlotTriplet& histogramTriplet, const ModuleData& mod_on, const int& col, const int& row, const int& markerState);

int main(int argc, char** argv) try
{
	std::cout << debug_prompt << "PlotEventClusters started..." << std::endl;
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
	// Histogram definitions
	std::map<int, LayerEventPlotTriplet> lowPhiClusters;
	std::map<int, LayerEventPlotTriplet> highPhiClusters;
	// Loop on data
	std::map<int, std::vector<Cluster>> eventClustersMap;
	clusterTree -> GetEntry(1);
	int eventNum = eventField.evt;
	eventClustersMap.emplace(eventNum, std::vector<Cluster>());
	lowPhiClusters.emplace(eventNum, eventNum * 2);
	lowPhiClusters.at(eventNum).layer1.SetTitle("Layer_1: Low phi clusters;module pix. (col);ladder pix. (row)");
	lowPhiClusters.at(eventNum).layer2.SetTitle("Layer_2: Low phi clusters;module pix. (col);ladder pix. (row)");
	lowPhiClusters.at(eventNum).layer3.SetTitle("Layer_3: Low phi clusters;module pix. (col);ladder pix. (row)");
	highPhiClusters.emplace(eventNum, eventNum * 2 + 1);
	highPhiClusters.at(eventNum).layer1.SetTitle("Layer_1: High phi clusters;module pix. (col);ladder pix. (row)");
	highPhiClusters.at(eventNum).layer2.SetTitle("Layer_2: High phi clusters;module pix. (col);ladder pix. (row)");
	highPhiClusters.at(eventNum).layer3.SetTitle("Layer_3: High phi clusters;module pix. (col);ladder pix. (row)");
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		clusterTree -> GetEntry(entryIndex);
		if(eventField.evt != eventClustersMap.begin() -> first) continue;
		eventClustersMap.at(eventField.evt).push_back(clusterField);
	}
	// Looping on events
	// for(auto eventClustersIt = eventClustersMap.begin(); eventClustersIt != eventClustersMap.end(); ++eventClustersIt)
	// {
	auto eventClustersIt = eventClustersMap.begin();
	std::vector<Cluster>& eventClusters = eventClustersIt -> second;
	// Looping on clusters
	for(const auto& clusterField: eventClusters)
	{
		float phi   = atan2(tan(clusterField.beta), tan(clusterField.alpha));
		if(!((phi < -2.94) || (-1.77 < phi && phi < -1.37) || (-0.2 < phi && phi < 0.2) || (1.37 < phi && phi < 1.77) || (2.94 < phi))) continue;
		for(unsigned int digiIndex = 0; digiIndex < clusterField.pixelsCol.size(); ++digiIndex)
		{
			int col         = clusterField.pixelsCol[digiIndex];
			int row         = clusterField.pixelsRow[digiIndex];
			int markerState = clusterField.pixelsMarker[digiIndex];
			// printClusterFieldInfo(clusterField);
			if((phi < -2.94) || (-0.2 < phi && phi < 0.2) || (2.94 < phi)) fillEventPlot(lowPhiClusters .at(eventClustersIt -> first), clusterField.mod_on, col, row, markerState);
			if((-1.77 < phi && phi < -1.37) || (1.37 < phi && phi < 1.77)) fillEventPlot(highPhiClusters.at(eventClustersIt -> first), clusterField.mod_on, col, row, markerState);
		}
	}
	// }
	TCanvas canvas("canvas", "canvas", 10, 10, 1500, 900);
	canvas.Divide(2, 3);
	canvas.cd(1);
	lowPhiClusters.at(eventNum).layer1.Draw("COLZ");
	canvas.cd(2);
	highPhiClusters.at(eventNum).layer1.Draw("COLZ");
	canvas.cd(3);
	lowPhiClusters.at(eventNum).layer2.Draw("COLZ");
	canvas.cd(4);
	highPhiClusters.at(eventNum).layer2.Draw("COLZ");
	canvas.cd(5);
	lowPhiClusters.at(eventNum).layer3.Draw("COLZ");
	canvas.cd(6);
	highPhiClusters.at(eventNum).layer3.Draw("COLZ");
	canvas.Update();
	theApp -> Run();
	inputFile -> Close();
	std::cout << debug_prompt << "PlotEventClusters terminated succesfully." << std::endl;
	return 0; 
}
catch(const std::exception& e)
{
	std::cout << error_prompt << "Exception thrown: " << e.what() << std::endl;
	return -1;
}

bool isMissingPartDoubleColumn(const int& firstCol, const int& firstRow, const int& secondCol, const int& secondRow)
{
	if(firstCol % 2 == 1 && secondCol == firstCol + 3)
	{
		if(std::abs(secondRow - firstRow) < 4)
		{
			return true;
		}
		return false;
	}
	return false;
}

ClusterStats getClusterStats(const Cluster& clusterField)
{
	ClusterStats clusterStats;
	float maxLengthSquared = 0;
	// int maxPixelPairCharge = 0;
	if(clusterField.pixelsCol.size() == 1)
	{
		clusterStats.length     = 0;
		clusterStats.dir        = -1;
		clusterStats.error = 1;
		return clusterStats;
	}
	auto colMinmax = std::minmax_element(clusterField.pixelsCol.begin(), clusterField.pixelsCol.end());
	int colMin = *(colMinmax.first);
	int colMax = *(colMinmax.second);
	clusterStats.startCol = colMin;
	clusterStats.endCol   = colMax;
	clusterStats.startRow = 0;
	clusterStats.endRow   = 0;
	float totalAdcStart = 0;
	float totalAdcEnd = 0;
	for(int i: range(clusterField.pixelsCol.size()))
	{
		if(clusterField.pixelsCol[i] == colMin)
		{
			clusterStats.startRow    += clusterField.pixelsRow[i] * clusterField.pixelsAdc[i];
			clusterStats.startColAdc += clusterField.pixelsAdc[i];
			totalAdcStart += clusterField.pixelsAdc[i];
		}
		if(clusterField.pixelsCol[i] == colMax)
		{
			clusterStats.endRow    += (clusterField.pixelsRow[i] * clusterField.pixelsAdc[i]);
			clusterStats.endColAdc += clusterField.pixelsAdc[i];
			totalAdcEnd += clusterField.pixelsAdc[i];
		}
	}
	clusterStats.startRow /= totalAdcStart;
	clusterStats.endRow   /= totalAdcEnd;
	// Save cluster data
	clusterStats.length = sqrt(maxLengthSquared);
	clusterStats.dir = std::atan2(clusterStats.endCol - clusterStats.startCol, clusterStats.endRow - clusterStats.startRow);
	return clusterStats;
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