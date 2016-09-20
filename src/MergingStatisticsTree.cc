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
