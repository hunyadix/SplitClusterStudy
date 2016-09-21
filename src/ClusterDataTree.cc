#include "../interface/ClusterDataTree.h"

////////////////////
// Tree branching //
////////////////////

void ClusterDataTree::defineClusterTreeBranches(TTree*& clusterTree, Cluster& clusterField)
{
	// clusterTree -> Branch("event",        &event_field,               event_field.list.c_str());
	// Module
	clusterTree -> Branch("module",       &clusterField.mod,          ModuleData::list.c_str());
	clusterTree -> Branch("module_on",    &clusterField.mod_on,       ModuleData::list.c_str());
	// Cluster data
	clusterTree -> Branch("x",            &clusterField.x,            "x/F");
	clusterTree -> Branch("y",            &clusterField.y,            "y/F");
	clusterTree -> Branch("clust_sizeX",  &clusterField.sizeX,        "sizeX/I");
	clusterTree -> Branch("clust_sizeY",  &clusterField.sizeY,        "sizeY/I");
	clusterTree -> Branch("clust_index",  &clusterField.clusterIndex, "i/I");
	clusterTree -> Branch("clust_size",   &clusterField.clusterSize,  "size/I");
	clusterTree -> Branch("clust_charge", &clusterField.charge,       "charge/F");
	// Pixel data
	clusterTree -> Branch("pixelsCol",    &clusterField.pixelsCol,    "pixelsCol[100]/I");
	clusterTree -> Branch("pixelsRow",    &clusterField.pixelsRow,    "pixelsRow[100]/I");
	clusterTree -> Branch("pixelsAdc",    &clusterField.pixelsAdc,    "pixelsAdc[100]/I");
	clusterTree -> Branch("pixelsMarker", &clusterField.pixelsMarker, "pixelsMarker[100]/I");
}

////////////////////////////////////
// Tree branch address assignment //
////////////////////////////////////

void ClusterDataTree::setClusterTreeDataFields(TTree*& clusterTree, Cluster& clusterField)
{
	// Module data
	clusterTree -> SetBranchAddress("module",       &clusterField.mod);
	clusterTree -> SetBranchAddress("module_on",    &clusterField.mod_on);
	// Cluster data
	clusterTree -> SetBranchAddress("x",            &clusterField.x);
	clusterTree -> SetBranchAddress("y",            &clusterField.y);
	clusterTree -> SetBranchAddress("clust_sizeX",  &clusterField.sizeX);
	clusterTree -> SetBranchAddress("clust_sizeY",  &clusterField.sizeY);
	clusterTree -> SetBranchAddress("clust_index",  &clusterField.clusterIndex);
	clusterTree -> SetBranchAddress("clust_size",   &clusterField.clusterSize);
	clusterTree -> SetBranchAddress("clust_charge", &clusterField.charge);
	// Pixel data
	clusterTree -> SetBranchAddress("pixelsCol",    &clusterField.pixelsCol);
	clusterTree -> SetBranchAddress("pixelsRow",    &clusterField.pixelsRow);
	clusterTree -> SetBranchAddress("pixelsAdc",    &clusterField.pixelsAdc);
	clusterTree -> SetBranchAddress("pixelsMarker", &clusterField.pixelsMarker);
}
