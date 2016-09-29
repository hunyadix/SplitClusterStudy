#ifndef CLUSTER_DATA_TREE_H
#define CLUSTER_DATA_TREE_H

#include "../interface/Cluster.h"
#include "../interface/EventData.h"

#include "../interface/TTreeTools.h"

#include <TROOT.h>
#include <TTree.h>

class ClusterDataTree
{
	public:
		static void defineClusterTreeBranches  (TTree*& clusterTree, EventData& eventField, Cluster& clusterField);
		static void associateDataFieldsFromTree(TTree*& clusterTree, EventData& eventField, Cluster& clusterField);
};

#endif