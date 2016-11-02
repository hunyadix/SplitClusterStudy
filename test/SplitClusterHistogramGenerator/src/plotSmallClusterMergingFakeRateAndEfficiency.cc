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
	TH1I healthyUnhealthySmallClusters_H("healthyUnhealthySmallClusters", "# of 4-6 long healthy clusters, # of 4-6 long split cluster pairs;healthy/unhealthy;nclusters", 2, 0, 1);
	TH1I sixLongClustersFakeRate_H("sixLongClustersFakeRate",             "# of 4-6 long split cluster pairs, # of fake 4-6 long cluster pairs;real/fake;nclusters", 2, 0, 1);
	TH1I sixLongClustersEfficiency_H("sixLongClustersEfficiency",         "# of 4-6 long healthy clusters, # of 4-6 long unhealthy clusters;restoreable/not restoreable;nclusters", 2, 0, 1);
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
	int numFourLongHealthy      = 0;
	int numFiveLongHealthy      = 0;
	int numSixLongHealthy       = 0;
	int numFourLongUnhealthy    = 0;
	int numFiveLongUnhealthy    = 0;
	int numSixLongUnhealthy     = 0;
	int numFourLongOneEndMarked = 0;
	int numFiveLongOneEndMarked = 0;
	int numSixLongRealPairs     = 0;
	int numFiveLongRealPairs    = 0;
	int numFourLongRealPairs    = 0;
	int numSixLongFakePairs     = 0;
	int numFiveLongFakePairs    = 0;
	int numFourLongFakePairs    = 0;
	int numSixMarkerlessPairs   = 0;
	int numFiveMarkerlessPairs  = 0;
	int numFourMarkerlessPairs  = 0;
	for(auto eventClustersIt = eventClustersMap.begin(); eventClustersIt != eventClustersMap.end(); ++eventClustersIt)
	{
		std::vector<Cluster>& eventClusters = eventClustersIt -> second;
		// Looping on clusters
		for(auto clusterIt = eventClusters.begin(); clusterIt != eventClusters.end(); ++clusterIt)
		{
			const Cluster& clusterField = *clusterIt;
			if(clusterField.sizeY == 4 && !isTagged(clusterField)) ++numFourLongHealthy;
			if(clusterField.sizeY == 5 && !isTagged(clusterField)) ++numFiveLongHealthy;
			if(clusterField.sizeY == 6 && !isTagged(clusterField)) ++numSixLongHealthy;
			if(clusterField.sizeY == 4 && isTagged(clusterField))  ++numFourLongUnhealthy;
			if(clusterField.sizeY == 5 && isTagged(clusterField))  ++numFiveLongUnhealthy;
			if(clusterField.sizeY == 6 && isTagged(clusterField))  ++numSixLongUnhealthy;
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
					if(clusterField.sizeY + pairCandidate.sizeY == 2) ++numFourLongRealPairs;
					if(clusterField.sizeY + pairCandidate.sizeY == 3) ++numFiveLongRealPairs;
					if(clusterField.sizeY + pairCandidate.sizeY == 4) ++numSixLongRealPairs;
				}
				// Not "only-middle-markers" pairs
				if((!( isMarkedOnMin && !isMarkedOnMax  && !pairIsMarkedOnMin &&  pairIsMarkedOnMax) && isMissingPartDoubleColumn(pairMaxCol,    pairCandidate.x, clusterMinCol, clusterField.x )) ||
				   (!(!isMarkedOnMin &&  isMarkedOnMax &&  pairIsMarkedOnMin  && !pairIsMarkedOnMax) && isMissingPartDoubleColumn(clusterMaxCol, clusterField.x,  pairMinCol,    pairCandidate.x)))
				{
					if(clusterField.sizeY + pairCandidate.sizeY == 2) ++numFourLongFakePairs;
					if(clusterField.sizeY + pairCandidate.sizeY == 3) ++numFiveLongFakePairs;
					if(clusterField.sizeY + pairCandidate.sizeY == 4) ++numSixLongFakePairs;
				}
				// Non-tagged pairs
				if(!isTagged(clusterField) && !isTagged(pairCandidate) && (isMissingPartDoubleColumn(pairMaxCol, pairCandidate.x, clusterMinCol, clusterField.x ) || isMissingPartDoubleColumn(clusterMaxCol, clusterField.x,  pairMinCol,  pairCandidate.x)))
				{
					if(clusterField.sizeY + pairCandidate.sizeY == 2) ++numFourMarkerlessPairs;
					if(clusterField.sizeY + pairCandidate.sizeY == 3) ++numFiveMarkerlessPairs;
					if(clusterField.sizeY + pairCandidate.sizeY == 4) ++numSixMarkerlessPairs;
				}
			}
		}
	}
	int hc = numFourLongHealthy     + numFiveLongHealthy     + numSixLongHealthy;
	int sc = numFourLongRealPairs   + numFiveLongRealPairs   + numSixLongRealPairs;
	int fc = numFourMarkerlessPairs + numFiveMarkerlessPairs + numSixMarkerlessPairs;
	std::cout << "hc: " << hc << std::endl;
	std::cout << "sc: " << sc << std::endl;
	std::cout << "fc: " << fc << std::endl;
	std::cout << "fc / (sc + fc): " << fc / static_cast<double>(sc + fc) << std::endl;
	std::cout << "sc / (sc + hc): " << sc / static_cast<double>(sc + hc) << std::endl;
	std::cout << "fc / (hc + fc): " << fc / static_cast<double>(hc + fc) << std::endl;

	healthyUnhealthySmallClusters_H.AddBinContent(1, numFourLongHealthy   + numFiveLongHealthy   + numSixLongHealthy);
	healthyUnhealthySmallClusters_H.AddBinContent(2, numSixLongRealPairs  + numFiveLongRealPairs + numFourLongRealPairs);
	// healthyUnhealthySmallClusters_H.AddBinContent(2, numFourLongUnhealthy, numFiveLongUnhealthy, numSixLongUnhealthy);
	sixLongClustersFakeRate_H      .AddBinContent(1, numFiveLongRealPairs + numSixLongRealPairs  + numFourLongRealPairs);
	sixLongClustersFakeRate_H      .AddBinContent(2, numFiveLongFakePairs + numSixLongFakePairs  + numFourLongFakePairs);
	sixLongClustersEfficiency_H    .AddBinContent(1, numFourLongHealthy   + numFiveLongHealthy   + numSixLongHealthy);
	sixLongClustersEfficiency_H    .AddBinContent(2, numFourLongUnhealthy + numFiveLongUnhealthy + numSixLongUnhealthy);
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