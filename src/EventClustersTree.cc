#include "../interface/EventClustersTree.h"

void EventClustersTree::defineEventClustersTreeBranches(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	// Number of clusters
	// eventClustersTree -> Branch("nClusters",    "int", &eventClusters);
	// Cluster data
	eventClustersTree -> Branch("x",            "std::vector<float>", &(eventClusters.x));
	eventClustersTree -> Branch("y",            "std::vector<float>", &(eventClusters.y));
	eventClustersTree -> Branch("sizeX",        "std::vector<int>",   &(eventClusters.sizeX));
	eventClustersTree -> Branch("sizeY",        "std::vector<int>",   &(eventClusters.sizeY));
	eventClustersTree -> Branch("clusterSize",  "std::vector<int>",   &(eventClusters.clusterSize));
	eventClustersTree -> Branch("charge",       "std::vector<float>", &(eventClusters.charge));
	// Pixel data
	eventClustersTree -> Branch("pixelsCol",    "std::vector<std::vector<int>>", &(eventClusters.pixelsCol));
	eventClustersTree -> Branch("pixelsRow",    "std::vector<std::vector<int>>", &(eventClusters.pixelsRow));
	eventClustersTree -> Branch("pixelsAdc",    "std::vector<std::vector<int>>", &(eventClusters.pixelsAdc));
	eventClustersTree -> Branch("pixelsMarker", "std::vector<std::vector<int>>", &(eventClusters.pixelsMarker));
	// Module data for mergeable cluster pairs
	eventClustersTree -> Branch("det",          "std::vector<int>", &(eventClusters.det));
	eventClustersTree -> Branch("layer",        "std::vector<int>", &(eventClusters.layer));
	eventClustersTree -> Branch("ladder",       "std::vector<int>", &(eventClusters.ladder));
	eventClustersTree -> Branch("module",       "std::vector<int>", &(eventClusters.module));
	eventClustersTree -> Branch("half",         "std::vector<int>", &(eventClusters.half));
	eventClustersTree -> Branch("outer",        "std::vector<int>", &(eventClusters.outer));
	eventClustersTree -> Branch("side",         "std::vector<int>", &(eventClusters.side));
	eventClustersTree -> Branch("disk",         "std::vector<int>", &(eventClusters.disk));
	eventClustersTree -> Branch("blade",        "std::vector<int>", &(eventClusters.blade));
	eventClustersTree -> Branch("panel",        "std::vector<int>", &(eventClusters.panel));
	eventClustersTree -> Branch("ring",         "std::vector<int>", &(eventClusters.ring));
	eventClustersTree -> Branch("shl",          "std::vector<int>", &(eventClusters.shl));
	eventClustersTree -> Branch("federr",       "std::vector<int>", &(eventClusters.federr));
}

void EventClustersTree::setEventClustersTreeDataFields (TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	// Cluster data
	eventClustersTree -> SetBranchAddress("x",            &(eventClusters.x));
	eventClustersTree -> SetBranchAddress("y",            &(eventClusters.y));
	eventClustersTree -> SetBranchAddress("sizeX",        &(eventClusters.sizeX));
	eventClustersTree -> SetBranchAddress("sizeY",        &(eventClusters.sizeY));
	eventClustersTree -> SetBranchAddress("clusterSize",  &(eventClusters.clusterSize));
	eventClustersTree -> SetBranchAddress("charge",       &(eventClusters.charge));
	// Pixel data
	eventClustersTree -> SetBranchAddress("pixelsCol",    &(eventClusters.pixelsCol));
	eventClustersTree -> SetBranchAddress("pixelsRow",    &(eventClusters.pixelsRow));
	eventClustersTree -> SetBranchAddress("pixelsAdc",    &(eventClusters.pixelsAdc));
	eventClustersTree -> SetBranchAddress("pixelsMarker", &(eventClusters.pixelsMarker));
	// Module data for mergeable cluster pairs
	eventClustersTree -> SetBranchAddress("det",          &(eventClusters.det));
	eventClustersTree -> SetBranchAddress("layer",        &(eventClusters.layer));
	eventClustersTree -> SetBranchAddress("ladder",       &(eventClusters.ladder));
	eventClustersTree -> SetBranchAddress("module",       &(eventClusters.module));
	eventClustersTree -> SetBranchAddress("half",         &(eventClusters.half));
	eventClustersTree -> SetBranchAddress("outer",        &(eventClusters.outer));
	eventClustersTree -> SetBranchAddress("side",         &(eventClusters.side));
	eventClustersTree -> SetBranchAddress("disk",         &(eventClusters.disk));
	eventClustersTree -> SetBranchAddress("blade",        &(eventClusters.blade));
	eventClustersTree -> SetBranchAddress("panel",        &(eventClusters.panel));
	eventClustersTree -> SetBranchAddress("ring",         &(eventClusters.ring));
	eventClustersTree -> SetBranchAddress("shl",          &(eventClusters.shl));
	eventClustersTree -> SetBranchAddress("federr",       &(eventClusters.federr));
}

// void EventClustersTree::associateDataFieldsFromTree(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
// {
// 	TTreeTools::treeCheck(eventClustersTree, "Error opening the clustTree.");
// 		// Number of clusters
// 	TTreeTools::checkGetBranch(eventClustersTree, "nClusters")    -> SetAddress(&eventClusters.size);
// 	// Cluster Data
// 	TTreeTools::checkGetBranch(eventClustersTree, "x")            -> SetAddress(&(eventClusters.x[0]));
// 	TTreeTools::checkGetBranch(eventClustersTree, "y")            -> SetAddress(&(eventClusters.y[0]));
// 	TTreeTools::checkGetBranch(eventClustersTree, "sizeX")        -> SetAddress(&(eventClusters.sizeX[0]));
// 	TTreeTools::checkGetBranch(eventClustersTree, "sizeY")        -> SetAddress(&(eventClusters.sizeY[0]));
// 	TTreeTools::checkGetBranch(eventClustersTree, "clusterSize")  -> SetAddress(&(eventClusters.clusterSize[0]));
// 	TTreeTools::checkGetBranch(eventClustersTree, "charge")       -> SetAddress(&(eventClusters.charge[0]));
// 	// // Pixel Data
// 	TTreeTools::checkGetBranch(eventClustersTree, "pixelsCol")    -> SetAddress(eventClusters.pixelsCol[0]);
// 	TTreeTools::checkGetBranch(eventClustersTree, "pixelsRow")    -> SetAddress(eventClusters.pixelsRow[0]);
// 	TTreeTools::checkGetBranch(eventClustersTree, "pixelsAdc")    -> SetAddress(eventClusters.pixelsAdc[0]);
// 	TTreeTools::checkGetBranch(eventClustersTree, "pixelsMarker") -> SetAddress(eventClusters.pixelsMarker[0]);
// 	// // Module data for mergeable cluster pairs
// 	TTreeTools::checkGetBranch(eventClustersTree, "det")          -> SetAddress(&(eventClusters.mod_on[0]).det);   // Működik
// 	TTreeTools::checkGetBranch(eventClustersTree, "layer")        -> SetAddress(&(eventClusters.mod_on[0]).layer); // Szegmentálási hiba
// 	// TTreeTools::checkGetBranch(eventClustersTree, "ladder")       -> SetAddress(&(eventClusters.mod_on[0]).ladder);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "module")       -> SetAddress(&(eventClusters.mod_on[0]).module);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "half")         -> SetAddress(&(eventClusters.mod_on[0]).half);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "outer")        -> SetAddress(&(eventClusters.mod_on[0]).outer);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "side")         -> SetAddress(&(eventClusters.mod_on[0]).side);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "disk")         -> SetAddress(&(eventClusters.mod_on[0]).disk);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "blade")        -> SetAddress(&(eventClusters.mod_on[0]).blade);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "panel")        -> SetAddress(&(eventClusters.mod_on[0]).panel);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "ring")         -> SetAddress(&(eventClusters.mod_on[0]).ring);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "shl")          -> SetAddress(&(eventClusters.mod_on[0]).shl);
// 	// TTreeTools::checkGetBranch(eventClustersTree, "federr")       -> SetAddress(&(eventClusters.mod_on[0]).federr);
// }
