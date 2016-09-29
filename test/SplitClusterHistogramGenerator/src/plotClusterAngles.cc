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
#include <TH3D.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <functional>
#include <complex>

struct ClusterStats
{
	int startIndex = NOVAL_I;
	int endIndex   = NOVAL_I;
	float length   = NOVAL_F;
	float dir      = NOVAL_F;
	int error      = 0;
};

ClusterStats getClusterStats(Cluster& clusterField);

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
		TH1D clusterAngle_H    ("ClusteAngleDistribution",     "Cluster Angle Distribution;angle;nclusters",      100, 0, 3.6);
		TH1D clusterPairAngle_H("ClustePairAngleDistribution", "Cluster Pair Angle Distribution;angle;nclusters", 100, 0, 3.6);

		TH3D clusterPairAngle_vs_clusterAngle_H("ClustePairAngleVSClusterAngle", "Cluster Pair Angle vs Cluster Pair Angle;angle of first cluster;angle of second cluster;relative cluster angle", 100, 0, 3.6, 100, 0, 3.6, 100, 0, 3.6);
		// Loop on data
		timer.restart("Measuring the time required for the looping...");
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
				std::complex<float> pos1(firstClusterIt -> x, firstClusterIt -> y);
				ClusterStats clusterStats1 = getClusterStats(*firstClusterIt);
				clusterAngle_H.Fill(clusterStats1.dir);
				// Looping on possible pairs
				for(auto secondClusterIt = firstClusterIt + 1; secondClusterIt != eventClusters.end(); ++secondClusterIt)
				{
					// Check if clusters are on the same module
					if(firstClusterIt -> mod == secondClusterIt -> mod) continue;
					std::complex<float> pos2(secondClusterIt -> x, secondClusterIt -> y);
					std::complex<float> dist = pos2 - pos1;
					// Check if clusters are close enough
					if(15 < std::abs(dist)) continue;
					ClusterStats clusterStats2 = getClusterStats(*secondClusterIt);
					float clusterPairAngle;
					clusterPairAngle = std::atan2(dist.imag(), dist.real());
					clusterPairAngle_H.Fill(clusterPairAngle);
					clusterPairAngle_vs_clusterAngle_H.Fill(clusterStats1.dir, clusterStats2.dir, clusterPairAngle);
				}
			}
		}
		TCanvas canvas("canvas", "canvas", 10, 10, 1500, 500);
		canvas.Divide(3, 1);
		canvas.cd(1);
		clusterAngle_H.Draw("COLZ");
		canvas.cd(2);
		clusterPairAngle_H.Draw("COLZ");
		canvas.cd(3);
		clusterPairAngle_vs_clusterAngle_H.Draw("COLZ");
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

ClusterStats getClusterStats(Cluster& clusterField)
{
	ClusterStats clusterStats;
	float maxLengthSquared = 0;
	int maxPixelPairCharge = 0;
	if(clusterField.pixelsCol.size() == 1)
	{
		clusterStats.length     = 0;
		clusterStats.dir        = -1;
		clusterStats.error = 1;
		return clusterStats;
	}
	for(unsigned int firstIndex = 0; firstIndex < clusterField.pixelsCol.size(); ++firstIndex)
	{
		for(unsigned int secondIndex = firstIndex + 1; secondIndex < clusterField.pixelsCol.size(); ++secondIndex)
		{
			bool saveAsNewMax = false;
			float xDistance   = clusterField.pixelsRow[firstIndex] - clusterField.pixelsRow[secondIndex];
			float yDistance   = clusterField.pixelsCol[firstIndex] - clusterField.pixelsCol[secondIndex]; 
			float distanceSquared = xDistance * xDistance + yDistance * yDistance;
			// Save the furthermost pixelpair
			if(maxLengthSquared < distanceSquared) saveAsNewMax = true;
			// Check charge for pixels with the same distanceSquared
			else if(distanceSquared == maxLengthSquared)
			{
				int pixelPairCharge = clusterField.pixelsAdc[firstIndex] + clusterField.pixelsAdc[secondIndex];
				if(maxPixelPairCharge < pixelPairCharge) saveAsNewMax = true;
			}
			if(saveAsNewMax == true)
			{
				clusterStats.startIndex    = firstIndex;
				clusterStats.endIndex      = secondIndex;
				maxLengthSquared   = distanceSquared;
				maxPixelPairCharge = clusterField.pixelsAdc[firstIndex] + clusterField.pixelsAdc[secondIndex];
			}
		}
	}
	// Check start-end order
	if(clusterField.pixelsCol[clusterStats.startIndex] < clusterField.pixelsCol[clusterStats.endIndex]) std::swap(clusterStats.startIndex, clusterStats.endIndex);
	// Save cluster data
	clusterStats.length = sqrt(maxLengthSquared);
	clusterStats.dir = std::atan2(static_cast<float>(clusterField.pixelsRow[clusterStats.endIndex] - clusterField.pixelsRow[clusterStats.startIndex]), static_cast<float>(clusterField.pixelsCol[clusterStats.endIndex] - clusterField.pixelsCol[clusterStats.startIndex]));
	return clusterStats;
}