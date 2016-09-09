#include "../interface/EventDataTree.h"

// Normal event tree

void EventDataTree::defineEventTreeBranches(TTree*& eventTree, EventData& eventField)
{
	eventTree -> Branch("event", &eventField, eventField.list.c_str());
}

void EventDataTree::setEventTreeDataFields(TTree*& eventTree, EventData& eventField)
{
	eventTree -> SetBranchAddress("event",                          &eventField);
}

// Event based cluster pair statistics
// void EventDataTree::defineEventClusterPairsTreeBranches(TTree*& eventTree, EventData& eventField, std::vector<MergingStatisticsData>& eventMergingStatisticsField)
void EventDataTree::defineEventClusterPairsTreeBranches(TTree*& eventTree, EventData& eventField, MergingStatisticsDataArrays mergingStatisticsDataArrays)
{
	eventTree -> Branch("event", &eventField, eventField.list.c_str());
	// Number of mergeable cluster pairs
	eventTree -> Branch("nMergeableClusterPairs", &mergingStatisticsDataArrays.size, "nMergeableClusterPairs/I");
	// // Mergeable cluster pair data
	eventTree -> Branch("clusterSize_1",            &(mergingStatisticsDataArrays.clusterSize_1[0]),            "clusterSize_1[nMergeableClusterPairs]/I");
	eventTree -> Branch("clusterSize_2",            &(mergingStatisticsDataArrays.clusterSize_2[0]),            "clusterSize_2[nMergeableClusterPairs]/I");
	eventTree -> Branch("sizeDifference",           &(mergingStatisticsDataArrays.sizeDifference[0]),           "sizeDifference[nMergeableClusterPairs]/I");
	eventTree -> Branch("clusterCharge_1",          &(mergingStatisticsDataArrays.clusterCharge_1[0]),          "clusterCharge_1[nMergeableClusterPairs]/F");
	eventTree -> Branch("clusterCharge_2",          &(mergingStatisticsDataArrays.clusterCharge_2[0]),          "clusterCharge_2[nMergeableClusterPairs]/F");
	eventTree -> Branch("chargeDifference",         &(mergingStatisticsDataArrays.chargeDifference[0]),         "chargeDifference[nMergeableClusterPairs]/F");
	eventTree -> Branch("clusterAngle_1",           &(mergingStatisticsDataArrays.clusterAngle_1[0]),           "clusterAngle_1[nMergeableClusterPairs]/F");
	eventTree -> Branch("clusterAngle_2",           &(mergingStatisticsDataArrays.clusterAngle_2[0]),           "clusterAngle_2[nMergeableClusterPairs]/F");
	eventTree -> Branch("angleDifference",          &(mergingStatisticsDataArrays.angleDifference[0]),          "angleDifference[nMergeableClusterPairs]/F");
	eventTree -> Branch("isMarkedAsSplitCluster_1", &(mergingStatisticsDataArrays.isMarkedAsSplitCluster_1[0]), "isMarkedAsSplitCluster_1[nMergeableClusterPairs]/I");
	eventTree -> Branch("isMarkedAsSplitCluster_2", &(mergingStatisticsDataArrays.isMarkedAsSplitCluster_2[0]), "isMarkedAsSplitCluster_2[nMergeableClusterPairs]/I");
	eventTree -> Branch("distanceInPixels",         &(mergingStatisticsDataArrays.distanceInPixels[0]),         "distanceInPixels[nMergeableClusterPairs]/I");
	// Module data for mergeable cluster pairs
	eventTree -> Branch("det",    &(mergingStatisticsDataArrays.mod_on[0]).det,    "det[nMergeableClusterPairs]/I");
	eventTree -> Branch("layer",  &(mergingStatisticsDataArrays.mod_on[0]).layer,  "layer[nMergeableClusterPairs]/I");
	eventTree -> Branch("ladder", &(mergingStatisticsDataArrays.mod_on[0]).ladder, "ladder[nMergeableClusterPairs]/I");
	eventTree -> Branch("module", &(mergingStatisticsDataArrays.mod_on[0]).module, "module[nMergeableClusterPairs]/I");
	eventTree -> Branch("half",   &(mergingStatisticsDataArrays.mod_on[0]).half,   "half[nMergeableClusterPairs]/I");
	eventTree -> Branch("outer",  &(mergingStatisticsDataArrays.mod_on[0]).outer,  "outer[nMergeableClusterPairs]/I");
	eventTree -> Branch("side",   &(mergingStatisticsDataArrays.mod_on[0]).side,   "side[nMergeableClusterPairs]/I");
	eventTree -> Branch("disk",   &(mergingStatisticsDataArrays.mod_on[0]).disk,   "disk[nMergeableClusterPairs]/I");
	eventTree -> Branch("blade",  &(mergingStatisticsDataArrays.mod_on[0]).blade,  "blade[nMergeableClusterPairs]/I");
	eventTree -> Branch("panel",  &(mergingStatisticsDataArrays.mod_on[0]).panel,  "panel[nMergeableClusterPairs]/I");
	eventTree -> Branch("ring",   &(mergingStatisticsDataArrays.mod_on[0]).ring,   "ring[nMergeableClusterPairs]/I");
	eventTree -> Branch("shl",    &(mergingStatisticsDataArrays.mod_on[0]).shl,    "shl[nMergeableClusterPairs]/I");
	eventTree -> Branch("federr", &(mergingStatisticsDataArrays.mod_on[0]).federr, "federr[nMergeableClusterPairs]/I");
}

void EventDataTree::setEventClusterPairsTreeDataFields (TTree*& eventTree, EventData& eventField, MergingStatisticsDataArrays mergingStatisticsDataArrays)
{
	eventTree -> SetBranchAddress("event", &eventField);
	// Number of mergeable cluster pairs
	eventTree -> SetBranchAddress("nMergeableClusterPairs", &mergingStatisticsDataArrays.size);
	// // Mergeable cluster pair data
	eventTree -> SetBranchAddress("clusterSize_1",            &(mergingStatisticsDataArrays.clusterSize_1[0]));
	eventTree -> SetBranchAddress("clusterSize_2",            &(mergingStatisticsDataArrays.clusterSize_2[0]));
	eventTree -> SetBranchAddress("sizeDifference",           &(mergingStatisticsDataArrays.sizeDifference[0]));
	eventTree -> SetBranchAddress("clusterCharge_1",          &(mergingStatisticsDataArrays.clusterCharge_1[0]));
	eventTree -> SetBranchAddress("clusterCharge_2",          &(mergingStatisticsDataArrays.clusterCharge_2[0]));
	eventTree -> SetBranchAddress("chargeDifference",         &(mergingStatisticsDataArrays.chargeDifference[0]));
	eventTree -> SetBranchAddress("clusterAngle_1",           &(mergingStatisticsDataArrays.clusterAngle_1[0]));
	eventTree -> SetBranchAddress("clusterAngle_2",           &(mergingStatisticsDataArrays.clusterAngle_2[0]));
	eventTree -> SetBranchAddress("angleDifference",          &(mergingStatisticsDataArrays.angleDifference[0]));
	eventTree -> SetBranchAddress("isMarkedAsSplitCluster_1", &(mergingStatisticsDataArrays.isMarkedAsSplitCluster_1[0]));
	eventTree -> SetBranchAddress("isMarkedAsSplitCluster_2", &(mergingStatisticsDataArrays.isMarkedAsSplitCluster_2[0]));
	eventTree -> SetBranchAddress("distanceInPixels",         &(mergingStatisticsDataArrays.distanceInPixels[0]));
	// Module data for mergeable cluster pairs
	eventTree -> SetBranchAddress("det",    &(mergingStatisticsDataArrays.mod_on[0]).det);
	eventTree -> SetBranchAddress("layer",  &(mergingStatisticsDataArrays.mod_on[0]).layer);
	eventTree -> SetBranchAddress("ladder", &(mergingStatisticsDataArrays.mod_on[0]).ladder);
	eventTree -> SetBranchAddress("module", &(mergingStatisticsDataArrays.mod_on[0]).module);
	eventTree -> SetBranchAddress("half",   &(mergingStatisticsDataArrays.mod_on[0]).half);
	eventTree -> SetBranchAddress("outer",  &(mergingStatisticsDataArrays.mod_on[0]).outer);
	eventTree -> SetBranchAddress("side",   &(mergingStatisticsDataArrays.mod_on[0]).side);
	eventTree -> SetBranchAddress("disk",   &(mergingStatisticsDataArrays.mod_on[0]).disk);
	eventTree -> SetBranchAddress("blade",  &(mergingStatisticsDataArrays.mod_on[0]).blade);
	eventTree -> SetBranchAddress("panel",  &(mergingStatisticsDataArrays.mod_on[0]).panel);
	eventTree -> SetBranchAddress("ring",   &(mergingStatisticsDataArrays.mod_on[0]).ring);
	eventTree -> SetBranchAddress("shl",    &(mergingStatisticsDataArrays.mod_on[0]).shl);
	eventTree -> SetBranchAddress("federr", &(mergingStatisticsDataArrays.mod_on[0]).federr);
}


// // void EventDataTree::defineEventClusterPairsTreeBranches(TTree*& eventTree, EventData& eventField, std::vector<MergingStatisticsData>& eventMergingStatisticsField)
// void EventDataTree::defineEventClusterPairsTreeBranches(TTree*& eventTree, EventData& eventField, 
// 	int clusterSize_1,
// 	int clusterSize_2,
// 	int sizeDifference,
// 	float clusterCharge_1,
// 	float clusterCharge_2,
// 	float chargeDifference,
// 	float clusterAngle_1,
// 	float clusterAngle_2,
// 	float angleDifference,
// 	int isMarkedAsSplitCluster_1,
// 	int isMarkedAsSplitCluster_2,
// 	int distanceInPixels)
// {
// 	eventTree -> Branch("event", &eventField, eventField.list.c_str());
// 	// Number of mergeable cluster pairs
// 	int size = eventMergingStatisticsField.size();
// 	eventTree -> Branch("nMergeableClusterPairs", &size, "nMergeableClusterPairs/I");
// 	// // Mergeable cluster pair data
// 	eventTree -> Branch("clusterSize_1",            &eventMergingStatisticsField.clusterSize_1,            "clusterSize_1[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("clusterSize_2",            &eventMergingStatisticsField.clusterSize_2,            "clusterSize_2[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("sizeDifference",           &eventMergingStatisticsField.sizeDifference,           "sizeDifference[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("clusterCharge_1",          &eventMergingStatisticsField.clusterCharge_1,          "clusterCharge_1[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("clusterCharge_2",          &eventMergingStatisticsField.clusterCharge_2,          "clusterCharge_2[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("chargeDifference",         &eventMergingStatisticsField.chargeDifference,         "chargeDifference[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("clusterAngle_1",           &eventMergingStatisticsField.clusterAngle_1,           "clusterAngle_1[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("clusterAngle_2",           &eventMergingStatisticsField.clusterAngle_2,           "clusterAngle_2[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("angleDifference",          &eventMergingStatisticsField.angleDifference,          "angleDifference[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("isMarkedAsSplitCluster_1", &eventMergingStatisticsField.isMarkedAsSplitCluster_1, "isMarkedAsSplitCluster_1[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("isMarkedAsSplitCluster_2", &eventMergingStatisticsField.isMarkedAsSplitCluster_2, "isMarkedAsSplitCluster_2[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("distanceInPixels",         &eventMergingStatisticsField.distanceInPixels,         "distanceInPixels[nMergeableClusterPairs]/I");
// 	// Module data for mergeable cluster pairs
// 	eventTree -> Branch("det",    &eventMergingStatisticsField.mod_on.det,    "det[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("layer",  &eventMergingStatisticsField.mod_on.layer,  "layer[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("ladder", &eventMergingStatisticsField.mod_on.ladder, "ladder[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("module", &eventMergingStatisticsField.mod_on.module, "module[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("half",   &eventMergingStatisticsField.mod_on.half,   "half[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("outer",  &eventMergingStatisticsField.mod_on.outer,  "outer[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("side",   &eventMergingStatisticsField.mod_on.side,   "side[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("disk",   &eventMergingStatisticsField.mod_on.disk,   "disk[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("blade",  &eventMergingStatisticsField.mod_on.blade,  "blade[nMergeableClusterPairs]/F");
// 	eventTree -> Branch("panel",  &eventMergingStatisticsField.mod_on.panel,  "panel[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("ring",   &eventMergingStatisticsField.mod_on.ring,   "ring[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("shl",    &eventMergingStatisticsField.mod_on.shl,    "shl[nMergeableClusterPairs]/I");
// 	eventTree -> Branch("federr", &eventMergingStatisticsField.mod_on.federr, "federr[nMergeableClusterPairs]/I");
// 	// auto data = eventMergingStatisticsField.data();
// 	// eventTree -> Branch("mergeableClusterPairStatistics", &data, "eventMergingStatisticsField[12][nMergeableClusterPairs]/F");

// 	// eventTree -> Branch("module",      &eventMergingStatisticsField.mod,    ModuleData::list.c_str());
// 	// eventTree -> Branch("module_on",   &eventMergingStatisticsField.mod_on, ModuleData::list.c_str());
// }

// void EventDataTree::setEventClusterPairsTreeDataFields (TTree*& eventTree, EventData& eventField, std::vector<MergingStatisticsData>& eventMergingStatisticsField)
// {
// 	int size = eventMergingStatisticsField.size();
// 	eventTree -> SetBranchAddress("nMergeableClusterPairs",         &size);
// 	auto data = eventMergingStatisticsField.data();
// 	eventTree -> SetBranchAddress("mergeableClusterPairStatistics", &data);
// 	// eventTree -> SetBranchAddress("module",      &eventMergingStatisticsField.mod);
// 	// eventTree -> SetBranchAddress("module_on",   &eventMergingStatisticsField.mod_on);
// }
