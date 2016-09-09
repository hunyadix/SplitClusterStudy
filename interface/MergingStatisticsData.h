#ifndef MERGING_STATISTICS_STRUCTURE_H
#define MERGING_STATISTICS_STRUCTURE_H

#include "../ModuleDataFetcher/interface/ModuleData.h"

#include <string>
#include <vector>

#ifndef NOVAL_I
#define NOVAL_I -9999
#endif
#ifndef NOVAL_F
#define NOVAL_F -9999
#endif

class MergingStatisticsData
{
	public:
		int clusterSize_1;
		int clusterSize_2;
		int sizeDifference;
		float clusterCharge_1;
		float clusterCharge_2;
		float chargeDifference;
		float clusterAngle_1;
		float clusterAngle_2;
		float angleDifference;
		int isMarkedAsSplitCluster_1;
		int isMarkedAsSplitCluster_2;
		int distanceInPixels;
		// Module
		ModuleData mod;
		ModuleData mod_on;
		// Instantiation
		MergingStatisticsData() { init(); };
		void init()
		{
			mod.init();
			mod_on.init();
			clusterSize_1 = NOVAL_I;
			clusterSize_2 = NOVAL_I;
			sizeDifference = NOVAL_I;
			clusterCharge_1 = NOVAL_F;
			clusterCharge_2 = NOVAL_F;
			chargeDifference = NOVAL_F;
			clusterAngle_1 = NOVAL_F;
			clusterAngle_2 = NOVAL_F;
			angleDifference = NOVAL_F;
			isMarkedAsSplitCluster_1 = NOVAL_I;
			isMarkedAsSplitCluster_2 = NOVAL_I;
			distanceInPixels = NOVAL_I;
		};
		// Data list
		// std::string list = "clusterSize_1/F:clusterSize_2/F:sizeDifference/F:clusterCharge_1/F:clusterCharge_2/F:chargeDifference/F:clusterAngle_1/F:clusterAngle_2/F:angleDifference/F:isMarkedAsSplitCluster_1/F:isMarkedAsSplitCluster_2/F:distanceFnPixels/F";
		std::string list = "clusterSize_1/I:clusterSize_2/I:sizeDifference/I:clusterCharge_1/F:clusterCharge_2/F:chargeDifference/F:clusterAngle_1/F:clusterAngle_2/F:angleDifference/F:isMarkedAsSplitCluster_1/I:isMarkedAsSplitCluster_2/I:distanceInPixels/I";
};
 
class MergingStatisticsDataArrays
{
	public:
		int size = 0;
		std::vector<int> clusterSize_1;
		std::vector<int> clusterSize_2;
		std::vector<int> sizeDifference;
		std::vector<float> clusterCharge_1;
		std::vector<float> clusterCharge_2;
		std::vector<float> chargeDifference;
		std::vector<float> clusterAngle_1;
		std::vector<float> clusterAngle_2;
		std::vector<float> angleDifference;
		std::vector<int> isMarkedAsSplitCluster_1;
		std::vector<int> isMarkedAsSplitCluster_2;
		std::vector<int> distanceInPixels;
		// Module
		std::vector<ModuleData> mod;
		std::vector<ModuleData> mod_on;
		void clear()
		{
			size = 0;
			clusterSize_1.clear();
			clusterSize_2.clear();
			sizeDifference.clear();
			clusterCharge_1.clear();
			clusterCharge_2.clear();
			chargeDifference.clear();
			clusterAngle_1.clear();
			clusterAngle_2.clear();
			angleDifference.clear();
			isMarkedAsSplitCluster_1.clear();
			isMarkedAsSplitCluster_2.clear();
			distanceInPixels.clear();
			mod.clear();
			mod_on.clear();
		}
		void fill(const MergingStatisticsData& mergeStatField)
		{
			size++;
			clusterSize_1.push_back(mergeStatField.clusterSize_1);
			clusterSize_2.push_back(mergeStatField.clusterSize_2);
			sizeDifference.push_back(mergeStatField.sizeDifference);
			clusterCharge_1.push_back(mergeStatField.clusterCharge_1);
			clusterCharge_2.push_back(mergeStatField.clusterCharge_2);
			chargeDifference.push_back(mergeStatField.chargeDifference);
			clusterAngle_1.push_back(mergeStatField.clusterAngle_1);
			clusterAngle_2.push_back(mergeStatField.clusterAngle_2);
			angleDifference.push_back(mergeStatField.angleDifference);
			isMarkedAsSplitCluster_1.push_back(mergeStatField.isMarkedAsSplitCluster_1);
			isMarkedAsSplitCluster_2.push_back(mergeStatField.isMarkedAsSplitCluster_2);
			distanceInPixels.push_back(mergeStatField.distanceInPixels);
			mod.push_back(mergeStatField.mod);
			mod_on.push_back(mergeStatField.mod_on);
		}
};

#endif