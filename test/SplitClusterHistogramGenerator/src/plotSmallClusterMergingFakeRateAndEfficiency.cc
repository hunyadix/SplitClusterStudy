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
#include <TH1I.h>
#include <TH1F.h>
#include <TH2D.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <functional>
#include <algorithm>

bool isMissingPartDoubleColumn(const int& firstCol, const int& firstRow, const int& secondCol, const int& secondRow);
int isTagged(const Cluster& clusterField);
int isTaggedInColumn(const Cluster& clusterField, int col);
int isTaggedOnOneEndOnly(const Cluster& clusterField);

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
	TH1I healthyUnhealthySmallClusters_H("healthyUnhealthySmallClusters", "# of six long healthy clusters and estimated # of unhealthy 6 long clusters;healthy/unhealthy;nclusters", 2, 0, 1);
	TH1I sixLongClustersFakeRate_H("sixLongClustersFakeRate",             "# of six long split cluster pairs found, # of fake six long clusters;real/fake;nclusters", 2, 0, 1);
	TH1I sixLongClustersEfficiency_H("sixLongClustersEfficiency",         "# of six long split cluster pairs found, # of non-restorable six long clusters with pixel-loss;restoreable/not restoreable;nclusters", 2, 0, 1);
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
	int numFiveLongHealthy                = 0;
	int numSevenLongHealthy               = 0;
	int numSixLongHealthy                 = 0;
	int numFourLongUnrestoreableCandidate = 0;
	int numFiveLongUnrestoreableCandidate = 0;
	int numFourLongOneEndMarked           = 0;
	int numFiveLongOneEndMarked           = 0;
	int numSixLongRealPairs               = 0;
	int numSixLongFakePairs               = 0;
	for(auto eventClustersIt = eventClustersMap.begin(); eventClustersIt != eventClustersMap.end(); ++eventClustersIt)
	{
		std::vector<Cluster>& eventClusters = eventClustersIt -> second;
		// Looping on clusters
		for(auto clusterIt = eventClusters.begin(); clusterIt != eventClusters.end(); ++clusterIt)
		{
			const Cluster& clusterField = *clusterIt;
			if(clusterField.sizeY == 4 && isTaggedOnOneEndOnly(clusterField)) ++numFourLongUnrestoreableCandidate;
			if(clusterField.sizeY == 5 && isTaggedOnOneEndOnly(clusterField)) ++numFiveLongUnrestoreableCandidate;
			if(clusterField.sizeY == 5 && !isTagged(clusterField))            ++numFiveLongHealthy;
			if(clusterField.sizeY == 7 && !isTagged(clusterField))            ++numSevenLongHealthy;
			if(clusterField.sizeY == 6 && !isTagged(clusterField))            ++numSixLongHealthy;
			auto minMaxCol = std::minmax_element(clusterField.pixelsCol.begin(), clusterField.pixelsCol.end());
			int clusterMinCol = *(minMaxCol.first);
			int clusterMaxCol = *(minMaxCol.second);
			int isMarkedOnMin = isTaggedInColumn(clusterField, clusterMinCol);
			int isMarkedOnMax = isTaggedInColumn(clusterField, clusterMaxCol);
			if(clusterField.sizeY == 4 && (isMarkedOnMax ^ isMarkedOnMin)) ++numFourLongOneEndMarked;
			if(clusterField.sizeY == 5 && (isMarkedOnMax ^ isMarkedOnMin)) ++numFiveLongOneEndMarked;
			for(auto pairCandidateIt = clusterIt + 1; pairCandidateIt != eventClusters.end(); ++pairCandidateIt)
			{
				const Cluster& pairCandidate = *pairCandidateIt;
				if(clusterField.mod == pairCandidate.mod) continue;
				// Quick checks
				if(6 < std::abs(clusterField.x - pairCandidate.x)) continue;
				if(6 < std::abs(clusterField.y - pairCandidate.y)) continue;
				auto pairMinMaxCol = std::minmax_element(pairCandidate.pixelsCol.begin(), pairCandidate.pixelsCol.end());
				int pairMinCol = *(pairMinMaxCol.first);
				int pairMaxCol = *(pairMinMaxCol.second);
				int pairIsMarkedOnMin = isTaggedInColumn(pairCandidate, pairMinCol);
				int pairIsMarkedOnMax = isTaggedInColumn(pairCandidate, pairMaxCol);
				// Real cluster pair
				if(( isMarkedOnMin && !isMarkedOnMax && !pairIsMarkedOnMin &&  pairIsMarkedOnMax && isMissingPartDoubleColumn(pairMaxCol,    pairCandidate.x, clusterMinCol, clusterField.x )) ||
				   (!isMarkedOnMin &&  isMarkedOnMax &&  pairIsMarkedOnMin && !pairIsMarkedOnMax && isMissingPartDoubleColumn(clusterMaxCol, clusterField.x,  pairMinCol,    pairCandidate.x))) 
				{
					if(clusterField.sizeY + pairCandidate.sizeY == 4) ++numSixLongRealPairs;
				}
				// Fake cluster pair
				if( !( isMarkedOnMin && !isMarkedOnMax && !pairIsMarkedOnMin &&  pairIsMarkedOnMax) && isMissingPartDoubleColumn(pairMaxCol,    pairCandidate.x, clusterMinCol, clusterField.x ) ||
				   (!(!isMarkedOnMin &&  isMarkedOnMax &&  pairIsMarkedOnMin && !pairIsMarkedOnMax) && isMissingPartDoubleColumn(clusterMaxCol, clusterField.x,  pairMinCol,    pairCandidate.x)))
				{
					if(clusterField.sizeY + pairCandidate.sizeY == 4) ++numSixLongFakePairs;
				}
			}
		}
	}
	// std::cout << "numFiveLongHealthy: "                << numFiveLongHealthy                << std::endl;
	// std::cout << "numSevenLongHealthy: "               << numSevenLongHealthy               << std::endl;
	// std::cout << "numSixLongHealthy: "                 << numSixLongHealthy                 << std::endl;
	// std::cout << "numFourLongUnrestoreableCandidate: " << numFourLongUnrestoreableCandidate << std::endl;
	// std::cout << "numFourLongOneEndMarked: "           << numFourLongOneEndMarked           << std::endl;
	// std::cout << "numFiveLongOneEndMarked: "           << numFiveLongOneEndMarked           << std::endl;
	// std::cout << "numSixLongRealPairs: "               << numSixLongRealPairs               << std::endl;
	// std::cout << "numSixLongFakePairs: "               << numSixLongFakePairs               << std::endl;
	healthyUnhealthySmallClusters_H.AddBinContent(1, numSixLongHealthy);
	healthyUnhealthySmallClusters_H.AddBinContent(2, numFourLongUnrestoreableCandidate * (numSixLongHealthy / static_cast<double>(numSixLongHealthy + numFiveLongHealthy)) + numFiveLongUnrestoreableCandidate * numSixLongHealthy / static_cast<double>(numSixLongHealthy + numSevenLongHealthy));
	sixLongClustersFakeRate_H      .AddBinContent(1, numSixLongRealPairs);
	sixLongClustersFakeRate_H      .AddBinContent(2, numSixLongFakePairs);
	sixLongClustersEfficiency_H    .AddBinContent(1, numSixLongRealPairs);
	sixLongClustersEfficiency_H    .AddBinContent(2, numFourLongOneEndMarked * (numSixLongHealthy / static_cast<double>(numSixLongHealthy + numFiveLongHealthy)) + numFiveLongOneEndMarked * (numSixLongHealthy / static_cast<double>(numSixLongHealthy + numSevenLongHealthy)));
	TCanvas canvas("canvas", "canvas", 10, 10, 1500, 500);
	canvas.Divide(3, 1);
	canvas.cd(1);
	healthyUnhealthySmallClusters_H.Draw();
	canvas.cd(2);
	sixLongClustersFakeRate_H.Draw();
	canvas.cd(3);
	sixLongClustersEfficiency_H.Draw();
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

int isTagged(const Cluster& clusterField)
{
	if(std::any_of(clusterField.pixelsMarker.begin(), clusterField.pixelsMarker.end(), [] (int marker) {return marker == 0;})) return 1;
	return 0;
}

int isTaggedInColumn(const Cluster& clusterField, int col)
{
	for(int i: range(clusterField.pixelsCol.size()))
	{
		if(clusterField.pixelsCol[i] == col && clusterField.pixelsMarker[i] != 0) return 1;		
	}
	return 0;
}

int isTaggedOnOneEndOnly(const Cluster& clusterField)
{
	auto minMaxCol = std::minmax_element(clusterField.pixelsCol.begin(), clusterField.pixelsCol.end());
	int clusterMinCol = *(minMaxCol.first);
	int clusterMaxCol = *(minMaxCol.second);
	int isMarkedOnMin = isTaggedInColumn(clusterField, clusterMinCol);
	int isMarkedOnMax = isTaggedInColumn(clusterField, clusterMaxCol);
	return isMarkedOnMin ^ isMarkedOnMax;
}