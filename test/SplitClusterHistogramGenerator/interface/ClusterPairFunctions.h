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
	SharedPtrCollectionType getClustersWithXLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getClustersWithXLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getClustersWithXLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getClustersWithYLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getClustersWithYLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getClustersWithYLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getHealthyClustersWithXLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getHealthyClustersWithXLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getHealthyClustersWithXLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getHealthyClustersWithYLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getHealthyClustersWithYLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getHealthyClustersWithYLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getUnhealthyClustersWithXLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getUnhealthyClustersWithXLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getUnhealthyClustersWithXLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getUnhealthyClustersWithYLength(const std::vector<Cluster>&  clusterCollection, const int& length);
	SharedPtrCollectionType getUnhealthyClustersWithYLength(      std::vector<Cluster>&  clusterCollection, const int& length) = delete; // Only bind this to constant collections
	SharedPtrCollectionType getUnhealthyClustersWithYLength(      std::vector<Cluster>&& clusterCollection, const int& length) = delete; // Only bind this to constant collections
	PairCollectionType      getRealPairsWithXLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength);
	PairCollectionType      getRealPairsWithXLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getRealPairsWithXLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getRealPairsWithYLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength);
	PairCollectionType      getRealPairsWithYLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getRealPairsWithYLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getFakePairsWithXLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength);
	PairCollectionType      getFakePairsWithXLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getFakePairsWithXLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getFakePairsWithYLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength);
	PairCollectionType      getFakePairsWithYLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getFakePairsWithYLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	TH1D                    getClusterPairAngles(const std::vector<Cluster>&  clusterCollection, const std::string& histoName, const std::string& histoTitle);
	TH1D                    getClusterPairAngles(      std::vector<Cluster>&  clusterCollection, const std::string& histoName, const std::string& histoTitle) = delete; // Only bind this to constant collections
	TH1D                    getClusterPairAngles(      std::vector<Cluster>&& clusterCollection, const std::string& histoName, const std::string& histoTitle) = delete; // Only bind this to constant collections
	PairCollectionType      getClusterPairsWithMergingLength(const std::vector<Cluster>&  clusterCollection, const int& mergedLength); 
	PairCollectionType      getClusterPairsWithMergingLength(      std::vector<Cluster>&  clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
	PairCollectionType      getClusterPairsWithMergingLength(      std::vector<Cluster>&& clusterCollection, const int& mergedLength) = delete; // Only bind this to constant collections
} // ClusterPairFunctions

#endif