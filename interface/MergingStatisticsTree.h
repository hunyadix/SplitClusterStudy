#ifndef MERGING_STATISTICS_TREE_H
#define MERGING_STATISTICS_TREE_H

#include "../interface/MergingStatisticsData.h"

#include <TROOT.h>
#include <TTree.h>

class MergingStatisticsTree
{
	public:
		static void defineMergingStatTreeBranches(TTree*& mergeTree, MergingStatisticsData& mergeStatField);
		static void defineMergingSnapshotTreeBranches(TTree*& mergeSnapshotTree, int (&mergeSnapshotField)[1024]);
		// static void defineMergingSignalTreeBranches(TTree*& mergeSnapshotTree, );
		// static void defineMergingBackgroundTreeBranches(TTree*& mergeSnapshotTree, );

		static void setMergingStatTreeDataFields (TTree*& mergeTree, MergingStatisticsData& mergeStatField);
		static void setMergingSnapshotTreeDataFields (TTree*& mergeSnapshotTree, int (&mergeSnapshotField)[1024]);
};


#endif