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

void printUsage(int& argc, char** argv, bool killProcess);
void processArgs(int& argc, char** argv, std::string& inputFileName, int& savePlots);
bool isMissingPartDoubleColumn(const int& firstCol, const int& firstRow, const int& secondCol, const int& secondRow);
ClusterStats getClusterStats(const Cluster& clusterField);

int main(int argc, char** argv) try
{
	std::string inputFileName("");
	int savePlots                 = 0;
	processArgs(argc, argv, inputFileName, savePlots);
	std::cout << debug_prompt << "PlotEventClusters started..." << std::endl;
	gStyle -> SetPalette(1);
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
	TH1D clusterAngle_H                    ("ClusterAngleDistribution",         "Cluster Angle Distribution;angle;# of clusters",                                                                     100, -3.2, 3.2);
	TH1D clusterPairIndAngle_H             ("ClusterPairAngleIndDistribution",  "Cluster Pair Relative Angle Distribution;angle;# of clusters",                                                       100, -3.2, 3.2);
	TH1D clusterPairEndAdc_H               ("ClusterPairEndAdcDifferences",     "Cluster Pair Closest Col. Adc Difference;adc difference;# of clusters",                                              200, 0, 200);
	TH2D clusterPairRelAngle_H             ("ClusterPairAngleRelDistribution",  "Cluster Pair Individual Angle Correspondence;angle of first cluster;angle of second cluster;# of clusters",          100, -3.2, 3.2, 100, -3.2, 3.2);
	TH2D clusterPairRelAngle_vs_EndAdc_H   ("ClusterPairAngleRelAngleVSEndAdc", "Cluster Pair Relative Angle vs Summed ADC on the closest cols;rel. angle;adc",                                       100, -3.2, 3.2, 200, 0, 200);
	TH3D clusterPairAngle_vs_clusterAngle_H("ClusterPairAngleVSClusterAngle",   "Cluster Pair Angle vs Cluster Pair Angle;angle of first cluster;angle of second cluster;relative cluster angle",     100, -3.2, 3.2, 100, -3.2, 3.2, 100, -3.2, 3.2);
	// Loop on data
	std::map<int, std::vector<Cluster>> eventClustersMap;
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
	// Looping on events
	for(auto eventClustersIt = eventClustersMap.begin(); eventClustersIt != eventClustersMap.end(); ++eventClustersIt)
	{
		std::vector<Cluster>& eventClusters = eventClustersIt -> second;
		// Looping on clusters
		for(auto firstClusterIt = eventClusters.begin(); firstClusterIt != eventClusters.end(); ++firstClusterIt)
		{
			ClusterStats clusterStats1 = getClusterStats(*firstClusterIt);
			// Looping on possible pairs
			for(auto secondClusterIt = firstClusterIt + 1; secondClusterIt != eventClusters.end(); ++secondClusterIt)
			{
				// Check if clusters are on the same module 
				if(firstClusterIt -> mod == secondClusterIt -> mod) continue;
				// Quick checks
				if(12 < std::abs(firstClusterIt -> x - secondClusterIt -> x)) continue;
				if(12 < std::abs(firstClusterIt -> y - secondClusterIt -> y)) continue;
				ClusterStats clusterStats2 = getClusterStats(*secondClusterIt);
				std::pair<int, int> dist(0, 0);
				int endAdcDifference = 0;
				if(clusterStats1.startCol <= clusterStats2.startCol) 
				{
					if(!isMissingPartDoubleColumn(clusterStats1.endRow, clusterStats1.endCol, clusterStats2.startRow, clusterStats2.startCol)) continue;
					dist = std::pair<int, int>(clusterStats1.startRow - clusterStats2.endRow, clusterStats1.startCol - clusterStats2.startCol);
					endAdcDifference = std::abs(clusterStats1.endColAdc - clusterStats2.startColAdc);
				}
				if(clusterStats2.startCol <  clusterStats1.startCol) 
				{
					if(!isMissingPartDoubleColumn(clusterStats1.endRow, clusterStats1.endCol, clusterStats2.startRow, clusterStats2.startCol)) continue;
					dist = std::pair<int, int>(clusterStats1.startRow - clusterStats2.endRow, clusterStats1.startCol - clusterStats2.startCol);
					endAdcDifference = std::abs(clusterStats1.startColAdc - clusterStats2.endColAdc);
				}
				int clusterPairAngle;
				clusterPairAngle = std::atan2(dist.second, dist.first);
				clusterAngle_H.Fill(clusterStats1.dir);
				clusterPairIndAngle_H.Fill(clusterPairAngle);
				clusterPairEndAdc_H.Fill(endAdcDifference);
				clusterPairRelAngle_H.Fill(clusterStats1.dir, clusterStats2.dir);
				clusterPairRelAngle_vs_EndAdc_H.Fill(clusterPairAngle, endAdcDifference);
				clusterPairAngle_vs_clusterAngle_H.Fill(clusterStats1.dir, clusterStats2.dir, clusterPairAngle);
			}
		}
	}
	TCanvas canvas("canvas", "canvas", 10, 10, 1600, 800);
	canvas.Divide(4, 2);
	canvas.cd(1);
	clusterAngle_H.Draw();
	canvas.cd(2);
	clusterPairIndAngle_H.Draw();
	canvas.cd(3);
	clusterPairRelAngle_H.Draw("COLZ");
	canvas.cd(4);
	clusterPairAngle_vs_clusterAngle_H.Draw("LEGO");
	canvas.cd(5);
	clusterPairEndAdc_H.Draw();
	canvas.cd(6);
	clusterPairRelAngle_vs_EndAdc_H.Draw("COLZ");
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
			clusterStats.endRow    += clusterField.pixelsRow[i] * clusterField.pixelsAdc[i];
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