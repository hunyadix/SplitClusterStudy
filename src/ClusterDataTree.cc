#include "../interface/ClusterDataTree.h"

////////////////////
// Tree branching //
////////////////////

void ClusterDataTree::defineClusterTreeBranches(TTree*& clusterTree, Cluster& clusterField)
{
	// LogDebug("tree_branching") << "Defining cluster tree branches..." << std::endl;
	// clusterTree -> Branch("event",        &event_field,          event_field.list.c_str());
	clusterTree -> Branch("module",       &clusterField.mod,    ModuleData::list.c_str());
	clusterTree -> Branch("module_on",    &clusterField.mod_on, ModuleData::list.c_str());
	// Serial number of cluster in the given module
	clusterTree -> Branch("clust_i",      &clusterField.i,      "i/I");
	// // Set if there is a valid hits
	// clusterTree -> Branch("clust_edge",   &clusterField.edge,   "edge/I");
	// clusterTree -> Branch("clust_badpix", &clusterField.badpix, "badpix/I");
	// clusterTree -> Branch("clust_tworoc", &clusterField.tworoc, "tworoc/I");
	// Position and size
	clusterTree -> Branch("clust_xy",     &clusterField.x,      "x/F:y");
	clusterTree -> Branch("clust_size",   &clusterField.size,   "size/I");
	clusterTree -> Branch("clust_sizeXY", &clusterField.sizeX,  "sizeX/I:sizeY");
	// Charge
	clusterTree -> Branch("clust_charge", &clusterField.charge, "charge/F");
	// Misc.
	clusterTree -> Branch("clust_adc",    &clusterField.adc,    "adc[size]/F");
	clusterTree -> Branch("clust_pix",    &clusterField.pix,    "pix[size][2]/F");
	// LogDebug("tree_branching") << "Done defining cluster tree branches..." << std::endl;
}

////////////////////////////////////
// Tree branch address assignment //
////////////////////////////////////

void ClusterDataTree::setClusterTreeDataFields(TTree*& clusterTree, Cluster& clusterField)
{
	// LogDebug("tree_branching") << "Setting branch addresses for cluster tree..." << std::endl;
	// clusterTree -> SetBranchAddress("event",        &event_field);
	clusterTree -> SetBranchAddress("module",       &clusterField.mod);
	clusterTree -> SetBranchAddress("module_on",    &clusterField.mod_on);
	// Serial number of cluster in the given module
	clusterTree -> SetBranchAddress("clust_i",      &clusterField.i);
	// // Set if there is a valid hits
	// clusterTree -> SetBranchAddress("clust_edge",   &clusterField.edge);
	// clusterTree -> SetBranchAddress("clust_badpix", &clusterField.badpix);
	// clusterTree -> SetBranchAddress("clust_tworoc", &clusterField.tworoc);
	// Position and size
	clusterTree -> SetBranchAddress("clust_xy",     &clusterField.x);
	clusterTree -> SetBranchAddress("clust_size",   &clusterField.size);
	clusterTree -> SetBranchAddress("clust_sizeXY", &clusterField.sizeX);
	// Charge
	clusterTree -> SetBranchAddress("clust_charge", &clusterField.charge);
	// Misc.
	clusterTree -> SetBranchAddress("clust_adc",    &clusterField.adc);
	clusterTree -> SetBranchAddress("clust_pix",    &clusterField.pix);
	// LogDebug("tree_branching") << "Done setting branch addresses for cluster tree..." << std::endl;
}
