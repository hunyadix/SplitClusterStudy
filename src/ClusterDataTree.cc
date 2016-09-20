#include "../interface/ClusterDataTree.h"

////////////////////
// Tree branching //
////////////////////

void ClusterDataTree::defineClusterTreeBranches(TTree*& clusterTree, Cluster& clusterField)
{
	// clusterTree -> Branch("event",        &event_field,          event_field.list.c_str());
	clusterTree -> Branch("module",       &clusterField.mod,    ModuleData::list.c_str());
	clusterTree -> Branch("module_on",    &clusterField.mod_on, ModuleData::list.c_str());
	// Serial number of cluster in the given module
	clusterTree -> Branch("clust_i",      &clusterField.i,      "i/I");
	// // Set if there is a valid hits
	// Position and size
	clusterTree -> Branch("clust_xy",     &clusterField.x,      "x/F:y");
	clusterTree -> Branch("clust_size",   &clusterField.size,   "size/I");
	clusterTree -> Branch("clust_sizeXY", &clusterField.sizeX,  "sizeX/I:sizeY");
	// Charge
	clusterTree -> Branch("clust_charge", &clusterField.charge, "charge/F");
	// Pixel data
	clusterTree -> Branch("pixelsCol",    &clusterField.pixelsCol,    "pixelsCol[250]/I");
	clusterTree -> Branch("pixelsRow",    &clusterField.pixelsRow,    "pixelsRow[250]/I");
	clusterTree -> Branch("pixelsAdc",    &clusterField.pixelsAdc,    "pixelsAdc[250]/I");
	clusterTree -> Branch("pixelsMarker",    &clusterField.pixelsMarker, "pixelsMarker[250]/I");
}

////////////////////////////////////
// Tree branch address assignment //
////////////////////////////////////

void ClusterDataTree::setClusterTreeDataFields(TTree*& clusterTree, Cluster& clusterField)
{
	clusterTree -> SetBranchAddress("module",       &clusterField.mod);
	clusterTree -> SetBranchAddress("module_on",    &clusterField.mod_on);
	clusterTree -> SetBranchAddress("clust_i",      &clusterField.i);
	// Position and size
	clusterTree -> SetBranchAddress("clust_xy",     &clusterField.x);
	clusterTree -> SetBranchAddress("clust_size",   &clusterField.size);
	clusterTree -> SetBranchAddress("clust_sizeXY", &clusterField.sizeX);
	// Charge
	clusterTree -> SetBranchAddress("clust_charge", &clusterField.charge);
	// Pixel data
	clusterTree -> SetBranchAddress("pixelsCol",    &clusterField.pixelsCol);
	clusterTree -> SetBranchAddress("pixelsRow",    &clusterField.pixelsRow);
	clusterTree -> SetBranchAddress("pixelsAdc",    &clusterField.pixelsAdc);
	clusterTree -> SetBranchAddress("pixelsMarker",    &clusterField.pixelsMarker);
}
