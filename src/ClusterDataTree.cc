#include "../interface/ClusterDataTree.h"

////////////////////
// Tree branching //
////////////////////

void ClusterDataTree::defineClusterTreeBranches(TTree*& clusterTree, EventData& eventField, Cluster& clusterField)
{
	// Event Data
	clusterTree -> Branch("event",        &eventField,                eventField.list.c_str());
	// Module
	clusterTree -> Branch("mod",          &clusterField.mod,          ModuleData::list.c_str());
	clusterTree -> Branch("mod_on",       &clusterField.mod_on,       ModuleData::list.c_str());
	// Cluster data
	clusterTree -> Branch("x",            &clusterField.x,            "x/F");
	clusterTree -> Branch("y",            &clusterField.y,            "y/F");
	clusterTree -> Branch("sizeX",        &clusterField.sizeX,        "sizeX/I");
	clusterTree -> Branch("sizeY",        &clusterField.sizeY,        "sizeY/I");
	clusterTree -> Branch("clusterIndex", &clusterField.clusterIndex, "i/I");
	clusterTree -> Branch("clusterSize",  &clusterField.clusterSize,  "size/I");
	clusterTree -> Branch("charge",       &clusterField.charge,       "charge/F");
	// Pixel data
	clusterTree -> Branch("pixelsCol",    &clusterField.pixelsCol);
	clusterTree -> Branch("pixelsRow",    &clusterField.pixelsRow);
	clusterTree -> Branch("pixelsAdc",    &clusterField.pixelsAdc);
	clusterTree -> Branch("pixelsMarker", &clusterField.pixelsMarker);
}

void ClusterDataTree::associateDataFieldsFromTree(TTree*& clusterTree, EventData& eventField, Cluster& clusterField)
{
	TTreeTools::treeCheck(clusterTree, "Error opening the clustTree.");
	// Event data
	TTreeTools::checkGetBranch(clusterTree, "event")        -> SetAddress(&eventField);
	// Module data
	TTreeTools::checkGetBranch(clusterTree, "mod")          -> SetAddress(&(clusterField.mod));
	TTreeTools::checkGetBranch(clusterTree, "mod_on")       -> SetAddress(&(clusterField.mod_on));
	// Cluster Data
	TTreeTools::checkGetBranch(clusterTree, "x")            -> SetAddress(&clusterField.x);
	TTreeTools::checkGetBranch(clusterTree, "y")            -> SetAddress(&clusterField.y);
	TTreeTools::checkGetBranch(clusterTree, "sizeX")        -> SetAddress(&clusterField.sizeX);
	TTreeTools::checkGetBranch(clusterTree, "sizeY")        -> SetAddress(&clusterField.sizeY);
	TTreeTools::checkGetBranch(clusterTree, "clusterIndex") -> SetAddress(&clusterField.clusterIndex);
	TTreeTools::checkGetBranch(clusterTree, "clusterSize")  -> SetAddress(&clusterField.clusterSize);
	TTreeTools::checkGetBranch(clusterTree, "charge")       -> SetAddress(&clusterField.charge);
	// Pixel Data
	// TTreeTools::checkGetBranch(clusterTree, "pixelsCol")    -> SetAddress(&(clusterField.pixelsCol));
	// TTreeTools::checkGetBranch(clusterTree, "pixelsRow")    -> SetAddress(&(clusterField.pixelsRow));
	// TTreeTools::checkGetBranch(clusterTree, "pixelsAdc")    -> SetAddress(&(clusterField.pixelsAdc));
	// TTreeTools::checkGetBranch(clusterTree, "pixelsMarker") -> SetAddress(&(clusterField.pixelsMarker));
}