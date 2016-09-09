#ifndef EVENT_DATA_TREE_H
#define EVENT_DATA_TREE_H

//////////////////////
// Tree definitions //
//////////////////////

#include "../interface/EventData.h"
#include "../interface/MergingStatisticsData.h"

////////////////////
// Root libraries //
////////////////////

#include <TROOT.h>
#include <TTree.h>

class EventDataTree
{
	public:
		static void defineEventTreeBranches(TTree*& eventTree, EventData& eventField);
		static void setEventTreeDataFields (TTree*& eventTree, EventData& eventField);

		static void defineEventClusterPairsTreeBranches(TTree*& eventTree, EventData& eventField, MergingStatisticsDataArrays eventMergingStatisticsField);
		static void setEventClusterPairsTreeDataFields (TTree*& eventTree, EventData& eventField, MergingStatisticsDataArrays mergingStatisticsDataArrays);
		// static void setEventClusterPairsTreeDataFields (TTree*& eventTree, EventData& eventField, std::vector<MergingStatisticsData>& eventMergingStatisticsField);
};

#endif