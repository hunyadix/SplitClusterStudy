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
	int   startCol = NOVAL_I;
	int   endCol   = NOVAL_I;
	float startRow = NOVAL_F;
	float endRow   = NOVAL_F;
	float dir      = NOVAL_F;
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
	std::cout << process_prompt << "plotClusterAngles started..." << std::endl;
	gStyle -> SetPalette(1);
	TimerColored timer(timer_prompt);
	TApplication* theApp = new TApplication("App", &argc, argv);
	TFile* inputFile = TFile::Open(inputFileName.c_str(), "READ");
	TTree* clusterTree = (TTree*)(inputFile -> Get("clustTree"));
	TTreeTools::treeCheck(clusterTree, "Tree missing.", true);
	Cluster  clusterField;
	EventData eventField;
	auto eventBranch = TTreeTools::checkGetBranch(clusterTree, "event");
	eventBranch -> SetAddress(&eventField);
	// Check if data is present
	Int_t totalNumEntries = clusterTree -> GetEntries();
	if(totalNumEntries == 0) throw std::runtime_error("No entries found in tree: clusterTree.");
	std::cout << debug_prompt << "Total entries in the tree: " << totalNumEntries << std::endl;
	// Get the indecies of the first "numEventsToMerge" events
	std::map<int, std::vector<Cluster>> eventClustersMap;
	std::cout << process_prompt << "Sorting the clusters by event numbers..." << std::endl;
	for(Int_t entryIndex = 0; entryIndex < totalNumEntries; ++entryIndex) 
	{
		eventBranch -> GetEntry(entryIndex);
		int eventNum = eventField.evt;
		auto eventClustersIt = eventClustersMap.find(eventNum);
		// If key does not exist yet: add key
		if(eventClustersIt == eventClustersMap.end())
		{
			eventClustersIt = eventClustersMap.emplace(eventField.evt, std::vector<Cluster>()).first;
		}
		eventClustersIt -> second.push_back(clusterField);
	}
	std::cout << process_prompt << "Done." << std::endl;
	// STL classes require addresses of pointers as accessors, unfortunately I can't do this in the associateDataFields function
	std::vector<int>* pixelsColWrapper    = &clusterField.pixelsCol;
	std::vector<int>* pixelsRowWrapper    = &clusterField.pixelsRow;
	std::vector<int>* pixelsAdcWrapper    = &clusterField.pixelsAdc;
	std::vector<int>* pixelsMarkerWrapper = &clusterField.pixelsMarker;
	// ClusterDataTree::associateDataFieldsFromTree(clusterTree, eventField, clusterField);
	TTreeTools::checkGetBranch(clusterTree, "event")        -> SetAddress(&eventField);
	TTreeTools::checkGetBranch(clusterTree, "mod_on")       -> SetAddress(&clusterField.mod_on);
	TTreeTools::checkGetBranch(clusterTree, "pixelsCol")    -> SetAddress(&pixelsColWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsRow")    -> SetAddress(&pixelsRowWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsAdc")    -> SetAddress(&pixelsAdcWrapper);
	TTreeTools::checkGetBranch(clusterTree, "pixelsMarker") -> SetAddress(&pixelsMarkerWrapper);
	// Histogram definitions
	TH1D clusterAngle_H                    ("ClusteAngleDistribution",        "Cluster Angle Distribution;angle;nclusters",                                                                     100, 0, 3.6);
	TH1D clusterPairIndAngle_H             ("ClustePairAngleIndDistribution", "Cluster Pair Relative Angle Distribution;angle;nclusters",                                                       100, 0, 3.6);
	TH2D clusterPairRelAngle_H             ("ClustePairAngleRelDistribution", "Cluster Pair Individual Angle Correspondence;angle of first cluster;angle of second cluster;nclusters",          100, 0, 3.6, 100, 0, 3.6);
	TH3D clusterPairAngle_vs_clusterAngle_H("ClustePairAngleVSClusterAngle",  "Cluster Pair Angle vs Cluster Pair Angle;angle of first cluster;angle of second cluster;relative cluster angle", 100, 0, 3.6, 100, 0, 3.6, 100, 0, 3.6);
	// Looping on events
	for(auto eventClustersIt = eventClustersMap.begin(); eventClustersIt != eventClustersMap.end(); ++eventClustersIt)
	{
		std::vector<Cluster>& eventClusters = eventClustersIt -> second;
		// Looping on clusters
		for(auto firstClusterIt = eventClusters.begin(); firstClusterIt != eventClusters.end(); ++firstClusterIt)
		{
			if(firstClusterIt -> mod_on.det != 0) continue;
			ClusterStats clusterStats1 = getClusterStats(*firstClusterIt);
			// Looping on possible pairs
			for(auto secondClusterIt = firstClusterIt + 1; secondClusterIt != eventClusters.end(); ++secondClusterIt)
			{
				if(secondClusterIt -> mod_on.det != 0) continue;
				// Check if clusters are on the same module 
				if(firstClusterIt -> mod_on == secondClusterIt -> mod_on) continue;
				// Quick checks
				if(12 < std::abs(firstClusterIt -> x - secondClusterIt -> x)) continue;
				if(12 < std::abs(firstClusterIt -> y - secondClusterIt -> y)) continue;
				ClusterStats clusterStats2 = getClusterStats(*secondClusterIt);
				std::pair<int, int> dist(0, 0);
				if(clusterStats1.startCol <= clusterStats2.startCol) 
				{
					if(!isMissingPartDoubleColumn(clusterStats1.endRow, clusterStats1.endCol, clusterStats2.startRow, clusterStats2.startCol)) continue;
					dist = std::pair<int, int>(clusterStats1.startRow - clusterStats2.endRow, clusterStats1.startCol - clusterStats2.startCol);
				}
				if(clusterStats2.startCol <  clusterStats1.startCol) 
				{
					if(!isMissingPartDoubleColumn(clusterStats1.endRow, clusterStats1.endCol, clusterStats2.startRow, clusterStats2.startCol)) continue;
					dist = std::pair<int, int>(clusterStats1.startRow - clusterStats2.endRow, clusterStats1.startCol - clusterStats2.startCol);
				}
				int clusterPairAngle;
				clusterPairAngle = std::atan2(dist.second, dist.first);
				clusterAngle_H.Fill(clusterStats1.dir);
				clusterPairIndAngle_H.Fill(clusterPairAngle);
				clusterPairRelAngle_H.Fill(clusterStats1.dir, clusterStats2.dir);
				clusterPairAngle_vs_clusterAngle_H.Fill(clusterStats1.dir, clusterStats2.dir, clusterPairAngle);
			}
		}
	}
	TCanvas canvas("canvas", "canvas", 10, 10, 1600, 500);
	canvas.Divide(4, 1);
	canvas.cd(1);
	clusterAngle_H.Draw();
	canvas.cd(2);
	clusterPairIndAngle_H.Draw();
	canvas.cd(3);
	clusterPairRelAngle_H.Draw("COLZ");
	canvas.cd(4);
	clusterPairAngle_vs_clusterAngle_H.Draw("COLZ");
	canvas.Update();
	theApp -> Run();
	inputFile -> Close();
	std::cout << process_prompt << "PlotEventClusters terminated succesfully." << std::endl;
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
	auto colMinmax = std::minmax_element(clusterField.pixelsCol.begin(), clusterField.pixelsCol.end());
	int colMin = *(colMinmax.first);
	int colMax = *(colMinmax.second);
	clusterStats.startCol   = colMin;
	clusterStats.endCol     = colMax;
	if(colMin == colMax)
	{
		clusterStats.startRow = clusterField.pixelsRow[0];
		clusterStats.endRow   = clusterStats.startRow;
		clusterStats.dir      = NOVAL_F;
		return clusterStats;
	}
	clusterStats.startRow   = 0;
	clusterStats.endRow     = 0;
	int numEdgePixelsMinCol = 0;
	int numEdgePixelsMaxCol = 0;
	int adcMinCol           = 0;
	int adcMaxCol           = 0;
	for(int i: range(clusterField.pixelsCol.size()))
	{
		if(clusterField.pixelsCol[i] == colMin)
		{
			clusterStats.startRow += clusterField.pixelsRow[i] * clusterField.pixelsAdc[i];
			adcMinCol += clusterField.pixelsAdc[i];
			++numEdgePixelsMinCol;
		}
		if(clusterField.pixelsCol[i] == colMax)
		{
			clusterStats.endRow   += clusterField.pixelsRow[i] * clusterField.pixelsAdc[i];
			adcMaxCol += clusterField.pixelsAdc[i];
			++numEdgePixelsMaxCol;
		}
	}
	clusterStats.startRow /= adcMinCol;
	clusterStats.endRow   /= adcMaxCol;
	clusterStats.dir = std::atan2(clusterStats.endCol - clusterStats.startCol, clusterStats.endRow - clusterStats.startRow);
	return clusterStats;
}

