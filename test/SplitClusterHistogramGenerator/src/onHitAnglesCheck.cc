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

bool isMissingPartDoubleColumn(const int& firstCol, const int& firstRow, const int& secondCol, const int& secondRow);
ClusterStats getClusterStats(const Cluster& clusterField);

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
	TH1D clusterAngleAllFromShape_H    ("ClusteAngleDistributionFromShape",         "Cluster Angle Distribution (all tagged clusters, shape based);angle;nclusters",                          100, -3.2, 3.2);
	TH1D clusterAngleAlpha_H           ("ClusteAngleAlpha",                         "Cluster Angle Alpha (on hit tagged clusters);angle;nclusters",                                           100, -3.2, 3.2);
	TH1D clusterAngleBeta_H            ("ClusteAngleBeta",                          "Cluster Angle Beta (on hit tagged clusters);angle;nclusters",                                            100, -3.2, 3.2);
	TH1D clusterAngleOnTrackFromShape_H("OnHitClusteAngleDistributionFromShape",    "Cluster Angle Distribution (on hit tagged clusters, shape based);angle;nclusters",                       100, -3.2, 3.2);
	TH1D clusterAngleOnTrackFromTrack_H("OnHitClusteAngleDistributionFromTrack",    "Cluster Angle Distribution (on hit tagged clusters, track inclination based);angle;nclusters",           100, -3.2, 3.2);
	TH1D clusterAngleDifference_H      ("OnHitClusteAngleDifferenceBetweenSources", "Cluster Angle Differences  (on hit tagged clusters, shape and track inclination based);angle;nclusters", 100, -3.2, 3.2);
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
		for(const auto& clusterField: eventClusters)
		{
			// Exclude non-tagged clusters
			if(std::all_of(clusterField.pixelsMarker.begin(), clusterField.pixelsMarker.end(), [] (int marker) {return marker == 0;})) continue;
			auto clusterStats = getClusterStats(clusterField);
			clusterAngleAllFromShape_H.Fill(clusterStats.dir);
			if(clusterField.isOnHit)
			{
				clusterAngleAlpha_H.Fill(clusterField.alpha);
				clusterAngleBeta_H.Fill(clusterField.beta);
				clusterAngleOnTrackFromShape_H.Fill(clusterStats.dir);
				float phi   = atan2(tan(clusterField.beta), tan(clusterField.alpha));
				// float theta = atan(sqrt(1.0 / ( 1.0 / (tan(clusterField.alpha) * tan(clusterField.alpha)) + 1.0 / (tan(clusterField.beta) * tan(clusterField.beta)))));
				clusterAngleOnTrackFromTrack_H.Fill(phi);
				clusterAngleDifference_H.Fill(std::abs(phi - clusterStats.dir));
			}
		}
	}
	TCanvas canvas("canvas", "canvas", 10, 10, 1600, 900);
	canvas.Divide(3, 2);
	canvas.cd(1);
	clusterAngleAllFromShape_H.Draw();
	canvas.cd(2);
	clusterAngleAlpha_H.Draw();
	canvas.cd(3);
	clusterAngleBeta_H.Draw();
	canvas.cd(4);
	clusterAngleOnTrackFromShape_H.Draw();
	canvas.cd(5);
	clusterAngleOnTrackFromTrack_H.Draw();
	canvas.cd(6);
	clusterAngleDifference_H.Draw();
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