#include "../interface/MergingStatisticsTree.h"

///////////////
// Info tree //
///////////////

void MergingStatisticsTree::defineMergingStatTreeBranches(TTree*& mergeTree, MergingStatisticsData& mergeStatField)
{
	mergeTree -> Branch("mergingInfo", &mergeStatField,        mergeStatField.list.c_str());
	mergeTree -> Branch("module",      &mergeStatField.mod,    ModuleData::list.c_str());
	mergeTree -> Branch("module_on",   &mergeStatField.mod_on, ModuleData::list.c_str());
}


void MergingStatisticsTree::setMergingStatTreeDataFields (TTree*& mergeTree, MergingStatisticsData& mergeStatField)
{
	mergeTree -> SetBranchAddress("mergingInfo", &mergeStatField);
	mergeTree -> SetBranchAddress("module",      &mergeStatField.mod);
	mergeTree -> SetBranchAddress("module_on",   &mergeStatField.mod_on);
}

///////////////////
// Snapshot tree //
///////////////////

void MergingStatisticsTree::defineMergingSnapshotTreeBranches(TTree*& mergeSnapshotTree, int (&mergeSnapshotField)[1024])
{
	mergeSnapshotTree -> Branch("clusterMergingSnapshots", &mergeSnapshotField, "mergeSnapshotField[1024]/I");

}

void MergingStatisticsTree::setMergingSnapshotTreeDataFields (TTree*& mergeSnapshotTree, int (&mergeSnapshotField)[1024])
{
	mergeSnapshotTree -> SetBranchAddress("clusterMergingSnapshots", &mergeSnapshotField);
}

