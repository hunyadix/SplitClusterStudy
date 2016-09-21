#ifndef EVENT_CLUSTERS_TREE_TREE_H
#define EVENT_CLUSTERS_TREE_TREE_H

// #include "../interface/EventData.h"
#include "../interface/Cluster.h"

#include <TROOT.h>
#include <TTree.h>

class EventClustersTree
{
	public:
		static void defineEventClustersTreeBranches(TTree*& eventClustersTree, EventClustersDataArrays& eventClusters);
		static void setEventClustersTreeDataFields (TTree*& eventClustersTree, EventClustersDataArrays& eventClusters);
};


#endif