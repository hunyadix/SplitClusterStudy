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
	using PairType                = std::pair<std::shared_ptr<Cluster>, std::shared_ptr<Cluster>>;
	using PairCollectionType      = std::vector<PairType>;
	using SharedPtrCollectionType = std::vector<std::shared_ptr<Cluster>>;

	int                    isClusterTagged(const Cluster& cluster);
	int                    isClusterTaggedInColumn(const Cluster& clusterField, int col);
	int                    areClustersPair(const Cluster& lhs, const Cluster& rhs);
	int                    areClustersEndTaggedPair(const Cluster& first, const Cluster& second);
	PairCollectionType     getClusterPairCollection(const std::vector<Cluster>&  clusterCollection);
	PairCollectionType     getClusterPairCollection(      std::vector<Cluster>&  clusterCollection) = delete; // Only bind this to constant collections
	PairCollectionType     getClusterPairCollection(      std::vector<Cluster>&& clusterCollection) = delete; // Only bind this to constant collections
	float                  getAdcWeightedRowCoordinateInCol(const Cluster& cluster, const int& col);
	float                  getClusterIndAngle(const Cluster& cluster);
	float                  getClusterPairAngle(const PairType& pair);
	std::vector<float>     getClusterPairAngles(const std::vector<Cluster>&  clusterCollection);
	std::vector<float>     getClusterPairAngles(      std::vector<Cluster>&  clusterCollection) = delete; // Only bind this to constant collections
	std::vector<float>     getClusterPairAngles(      std::vector<Cluster>&& clusterCollection) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getClustersWithLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getClustersWithLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getClustersWithLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getHealthyClustersWithLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getHealthyClustersWithLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getHealthyClustersWithLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getUnhealthyClustersWithLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getUnhealthyClustersWithLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getUnhealthyClustersWithLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	PairCollectionType      getRealPairsWithLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength);
	PairCollectionType      getRealPairsWithLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getRealPairsWithLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getFakePairsWithLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength);
	PairCollectionType      getFakePairsWithLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getFakePairsWithLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	TH1D                    getClusterPairAngles(const std::vector<Cluster>&  clusterCollection, const std::string& histoName, const std::string& histoTitle);
	TH1D                    getClusterPairAngles(      std::vector<Cluster>&  clusterCollection, const std::string& histoName, const std::string& histoTitle) = delete; // Only bind this to constant collections
	TH1D                    getClusterPairAngles(      std::vector<Cluster>&& clusterCollection, const std::string& histoName, const std::string& histoTitle) = delete; // Only bind this to constant collections
	PairCollectionType      getClusterPairsWithMergingLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength); 
	PairCollectionType      getClusterPairsWithMergingLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getClusterPairsWithMergingLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
} // ClusterPairFunctions

#endif