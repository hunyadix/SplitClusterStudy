#include "../interface/EventClustersTree.h"

void EventClustersTree::defineEventClustersTreeBranches(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
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

void EventClustersTree::associateDataFieldsFromTree(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	TTreeTools::treeCheck(eventClustersTree, "Error opening the clustTree.");
	// Cluster Data
	TTreeTools::checkGetBranch(eventClustersTree, "x")            -> SetAddress(&(eventClusters.x));
	TTreeTools::checkGetBranch(eventClustersTree, "y")            -> SetAddress(&(eventClusters.y));
	TTreeTools::checkGetBranch(eventClustersTree, "sizeX")        -> SetAddress(&(eventClusters.sizeX));
	TTreeTools::checkGetBranch(eventClustersTree, "sizeY")        -> SetAddress(&(eventClusters.sizeY));
	TTreeTools::checkGetBranch(eventClustersTree, "clusterSize")  -> SetAddress(&(eventClusters.clusterSize));
	TTreeTools::checkGetBranch(eventClustersTree, "charge")       -> SetAddress(&(eventClusters.charge));
	// // Pixel Data
	TTreeTools::checkGetBranch(eventClustersTree, "pixelsCol")    -> SetAddress(&(eventClusters.pixelsCol));
	TTreeTools::checkGetBranch(eventClustersTree, "pixelsRow")    -> SetAddress(&(eventClusters.pixelsRow));
	TTreeTools::checkGetBranch(eventClustersTree, "pixelsAdc")    -> SetAddress(&(eventClusters.pixelsAdc));
	TTreeTools::checkGetBranch(eventClustersTree, "pixelsMarker") -> SetAddress(&(eventClusters.pixelsMarker));
	// // Module data for mergeable cluster pairs
	TTreeTools::checkGetBranch(eventClustersTree, "det")          -> SetAddress(&(eventClusters.det));   // Működik
	TTreeTools::checkGetBranch(eventClustersTree, "layer")        -> SetAddress(&(eventClusters.layer)); // Szegmentálási hiba
	TTreeTools::checkGetBranch(eventClustersTree, "ladder")       -> SetAddress(&(eventClusters.ladder));
	TTreeTools::checkGetBranch(eventClustersTree, "module")       -> SetAddress(&(eventClusters.module));
	TTreeTools::checkGetBranch(eventClustersTree, "half")         -> SetAddress(&(eventClusters.half));
	TTreeTools::checkGetBranch(eventClustersTree, "outer")        -> SetAddress(&(eventClusters.outer));
	TTreeTools::checkGetBranch(eventClustersTree, "side")         -> SetAddress(&(eventClusters.side));
	TTreeTools::checkGetBranch(eventClustersTree, "disk")         -> SetAddress(&(eventClusters.disk));
	TTreeTools::checkGetBranch(eventClustersTree, "blade")        -> SetAddress(&(eventClusters.blade));
	TTreeTools::checkGetBranch(eventClustersTree, "panel")        -> SetAddress(&(eventClusters.panel));
	TTreeTools::checkGetBranch(eventClustersTree, "ring")         -> SetAddress(&(eventClusters.ring));
	TTreeTools::checkGetBranch(eventClustersTree, "shl")          -> SetAddress(&(eventClusters.shl));
	TTreeTools::checkGetBranch(eventClustersTree, "federr")       -> SetAddress(&(eventClusters.federr));
}
