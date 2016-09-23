#include "../interface/EventClustersTree.h"

void EventClustersTree::defineEventClustersTreeBranches(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	// eventClustersTree -> Branch("event", &eventField, eventField.list.c_str());
	// Number of clusters
	eventClustersTree -> Branch("nClusters",    &(eventClusters.size), "nClusters/I");
	// Cluster data
	eventClustersTree -> Branch("x",            &(eventClusters.x[0]),             "x[nClusters]/F");
	eventClustersTree -> Branch("y",            &(eventClusters.y[0]),             "y[nClusters]/F");
	eventClustersTree -> Branch("sizeX",        &(eventClusters.sizeX[0]),         "sizeX[nClusters]/I");
	eventClustersTree -> Branch("sizeY",        &(eventClusters.sizeY[0]),         "sizeY[nClusters]/I");
	eventClustersTree -> Branch("clusterSize",  &(eventClusters.clusterSize[0]),   "clusterSize[nClusters]/I");
	eventClustersTree -> Branch("charge",       &(eventClusters.charge[0]),        "charge[nClusters]/F");
	// Pixel data
	eventClustersTree -> Branch("pixelsCol",    eventClusters.pixelsCol[0],        "pixelsCol[nClusters][100]/I");
	eventClustersTree -> Branch("pixelsRow",    eventClusters.pixelsRow[0],        "pixelsRow[nClusters][100]/I");
	eventClustersTree -> Branch("pixelsAdc",    eventClusters.pixelsAdc[0],        "pixelsAdc[nClusters][100]/I");
	eventClustersTree -> Branch("pixelsMarker", eventClusters.pixelsMarker[0],     "pixelsMarker[nClusters][100]/I");
	// Module data for mergeable cluster pairs
	eventClustersTree -> Branch("det",          &(eventClusters.mod_on[0]).det,    "det[nClusters]/I");
	eventClustersTree -> Branch("layer",        &(eventClusters.mod_on[0]).layer,  "layer[nClusters]/I");
	eventClustersTree -> Branch("ladder",       &(eventClusters.mod_on[0]).ladder, "ladder[nClusters]/I");
	eventClustersTree -> Branch("module",       &(eventClusters.mod_on[0]).module, "module[nClusters]/I");
	eventClustersTree -> Branch("half",         &(eventClusters.mod_on[0]).half,   "half[nClusters]/I");
	eventClustersTree -> Branch("outer",        &(eventClusters.mod_on[0]).outer,  "outer[nClusters]/I");
	eventClustersTree -> Branch("side",         &(eventClusters.mod_on[0]).side,   "side[nClusters]/I");
	eventClustersTree -> Branch("disk",         &(eventClusters.mod_on[0]).disk,   "disk[nClusters]/I");
	eventClustersTree -> Branch("blade",        &(eventClusters.mod_on[0]).blade,  "blade[nClusters]/I");
	eventClustersTree -> Branch("panel",        &(eventClusters.mod_on[0]).panel,  "panel[nClusters]/I");
	eventClustersTree -> Branch("ring",         &(eventClusters.mod_on[0]).ring,   "ring[nClusters]/I");
	eventClustersTree -> Branch("shl",          &(eventClusters.mod_on[0]).shl,    "shl[nClusters]/I");
	eventClustersTree -> Branch("federr",       &(eventClusters.mod_on[0]).federr, "federr[nClusters]/I");
}

void EventClustersTree::setEventClustersTreeDataFields (TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
{
	// eventClustersTree -> SetBranchAddress("event", &eventField);
	// Number of clusters
	eventClustersTree -> SetBranchAddress("nClusters",    &eventClusters.size);
	// Cluster Data
	eventClustersTree -> SetBranchAddress("x",            &(eventClusters.x[0]));
	eventClustersTree -> SetBranchAddress("y",            &(eventClusters.y[0]));
	eventClustersTree -> SetBranchAddress("sizeX",        &(eventClusters.sizeX[0]));
	eventClustersTree -> SetBranchAddress("sizeY",        &(eventClusters.sizeY[0]));
	eventClustersTree -> SetBranchAddress("clusterSize",  &(eventClusters.clusterSize[0]));
	eventClustersTree -> SetBranchAddress("charge",       &(eventClusters.charge[0]));
	// Pixel Data
	eventClustersTree -> SetBranchAddress("pixelsCol",    eventClusters.pixelsCol[0]);
	eventClustersTree -> SetBranchAddress("pixelsRow",    eventClusters.pixelsRow[0]);
	eventClustersTree -> SetBranchAddress("pixelsAdc",    eventClusters.pixelsAdc[0]);
	eventClustersTree -> SetBranchAddress("pixelsMarker", eventClusters.pixelsMarker[0]);
	// Module data for mergeable cluster pairs
	eventClustersTree -> SetBranchAddress("det",          &(eventClusters.mod_on[0]).det);
	eventClustersTree -> SetBranchAddress("layer",        &(eventClusters.mod_on[0]).layer);
	eventClustersTree -> SetBranchAddress("ladder",       &(eventClusters.mod_on[0]).ladder);
	eventClustersTree -> SetBranchAddress("module",       &(eventClusters.mod_on[0]).module);
	eventClustersTree -> SetBranchAddress("half",         &(eventClusters.mod_on[0]).half);
	eventClustersTree -> SetBranchAddress("outer",        &(eventClusters.mod_on[0]).outer);
	eventClustersTree -> SetBranchAddress("side",         &(eventClusters.mod_on[0]).side);
	eventClustersTree -> SetBranchAddress("disk",         &(eventClusters.mod_on[0]).disk);
	eventClustersTree -> SetBranchAddress("blade",        &(eventClusters.mod_on[0]).blade);
	eventClustersTree -> SetBranchAddress("panel",        &(eventClusters.mod_on[0]).panel);
	eventClustersTree -> SetBranchAddress("ring",         &(eventClusters.mod_on[0]).ring);
	eventClustersTree -> SetBranchAddress("shl",          &(eventClusters.mod_on[0]).shl);
	eventClustersTree -> SetBranchAddress("federr",       &(eventClusters.mod_on[0]).federr);
}

// void EventClustersTree::getDataFieldsFromTree(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters)
// {
	
// }
