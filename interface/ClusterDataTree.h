#ifndef CLUSTER_DATA_TREE_H
#define CLUSTER_DATA_TREE_H

//////////////////////
// Tree definitions //
//////////////////////

#include "../interface/Cluster.h"

////////////////////////////
// Message logger service //
////////////////////////////

#include "FWCore/MessageLogger/interface/MessageLogger.h"

////////////////////
// Root libraries //
////////////////////

#include <TROOT.h>
#include <TTree.h>

class ClusterDataTree
{
	public:
		static void defineClusterTreeBranches(TTree*& clusterTree, Cluster& clusterField);
		static void setClusterTreeDataFields (TTree*& clusterTree, Cluster& clusterField);
};

#endif