#include "../interface/EventClustersTree.h"

void EventClustersTree::defineEventClustersTreeBranches(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	// Cluster data
	eventClustersTree -> Branch("x",            &(eventClusters.x));
	eventClustersTree -> Branch("y",            &(eventClusters.y));
	eventClustersTree -> Branch("sizeX",        &(eventClusters.sizeX));
	eventClustersTree -> Branch("sizeY",        &(eventClusters.sizeY));
	eventClustersTree -> Branch("clusterSize",  &(eventClusters.clusterSize));
	eventClustersTree -> Branch("charge",       &(eventClusters.charge));
	// Pixel data
	eventClustersTree -> Branch("pixelsCol",    &(eventClusters.pixelsCol));
	eventClustersTree -> Branch("pixelsRow",    &(eventClusters.pixelsRow));
	eventClustersTree -> Branch("pixelsAdc",    &(eventClusters.pixelsAdc));
	eventClustersTree -> Branch("pixelsMarker", &(eventClusters.pixelsMarker));
	// Module data for mergeable cluster pairs
	eventClustersTree -> Branch("det",          &(eventClusters.det));
	eventClustersTree -> Branch("layer",        &(eventClusters.layer));
	eventClustersTree -> Branch("ladder",       &(eventClusters.ladder));
	eventClustersTree -> Branch("module",       &(eventClusters.module));
	eventClustersTree -> Branch("half",         &(eventClusters.half));
	eventClustersTree -> Branch("outer",        &(eventClusters.outer));
	eventClustersTree -> Branch("side",         &(eventClusters.side));
	eventClustersTree -> Branch("disk",         &(eventClusters.disk));
	eventClustersTree -> Branch("blade",        &(eventClusters.blade));
	eventClustersTree -> Branch("panel",        &(eventClusters.panel));
	eventClustersTree -> Branch("ring",         &(eventClusters.ring));
	eventClustersTree -> Branch("shl",          &(eventClusters.shl));
	eventClustersTree -> Branch("federr",       &(eventClusters.federr));
}

void EventClustersTree::setEventClustersTreeDataFields (TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	// FIXME: this does not work.
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
	TTreeTools::checkGetBranch(eventClustersTree, "det")          -> SetAddress(&(eventClusters.det));
	TTreeTools::checkGetBranch(eventClustersTree, "layer")        -> SetAddress(&(eventClusters.layer));
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
