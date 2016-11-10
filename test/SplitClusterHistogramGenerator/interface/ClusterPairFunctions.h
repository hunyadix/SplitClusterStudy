#ifndef CLUSTER_PAIR_FUNCTIONS_H
#define CLUSTER_PAIR_FUNCTIONS_H

#include "../../../interface/Cluster.h"
#include "../../../interface/HelperFunctionsCommon.h"

#include <TH2D.h>

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace ClusterPairFunctions
{
	using PairType                 = std::pair<std::shared_ptr<Cluster>, std::shared_ptr<Cluster>>;
	using PairCollectionType       = std::vector<PairType>;

	int                areClustersPair(const Cluster& lhs, const Cluster& rhs);
	PairCollectionType getClusterPairCollection(const std::vector<Cluster>&  clusterCollection);
	PairCollectionType getClusterPairCollection(      std::vector<Cluster>&  clusterCollection) = delete; // Only bind this to constant collections
	PairCollectionType getClusterPairCollection(      std::vector<Cluster>&& clusterCollection) = delete; // Only bind this to constant collections
	float              getAdcWeightedRowCoordinateInCol(const Cluster& cluster, const int& col);
	float              getClusterIndAngle(const Cluster& cluster);
	float              getClusterPairAngle(const PairType& pair);
	std::vector<float> getClusterPairAngles(const std::vector<Cluster>&  clusterCollection);
	std::vector<float> getClusterPairAngles(      std::vector<Cluster>&  clusterCollection) = delete; // Only bind this to constant collections
	std::vector<float> getClusterPairAngles(      std::vector<Cluster>&& clusterCollection) = delete; // Only bind this to constant collections
	TH1D               getClusterPairAngles(const std::vector<Cluster>& clusterCollection, const std::string& histoName, const std::string& histoTitle);
	PairCollectionType getClusterPairsWithMergingLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength); 
	PairCollectionType getClusterPairsWithMergingLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType getClusterPairsWithMergingLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections

} // ClusterPairFunctions

#endif