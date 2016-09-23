// Data structure
#include "../../../interface/Cluster_structure.h"

// Utility
#include "../../../interface/TTreeTools.h"
#include "../../../interface/CommonActors.h"

// Root
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
// #include <TH1F.h>
// #include <TH2I.h>
// #include <TRandom3.h>

// C++ libraries
#include <iostream>
#include <exception>

int main(int argc, char** argv)
{
	std::cout << "PlotEventClusters started." << std::endl;
	TFile* inputFile = TFile::Open("/data/hunyadi/CMSSW/SplitClusterStudy/CMSSW_8_0_18/src/TestSplitClusterStudy/Ntuple_scm.root", "READ");
	TTree* eventClustersTree = (TTree*)(inputFile -> Get("eventClustersTree"));

	EventClustersDataArrays eventClusterField;
	TTreeTools::treeCheck(eventClustersTree, "This error message should be shown with a prompt before it.", true);
	inputFile -> Close();
	return 0; 
}
