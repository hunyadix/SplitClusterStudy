#include "../interface/ClusterPairFunctions.h"

namespace ClusterPairFunctions
{
	void calcPairEndsAndOrderLeftRight_(Cluster const* lhs, Cluster const* rhs, int& lhsColMin, int& lhsColMax, int& rhsColMin, int& rhsColMax)
	{
		std::tie(lhsColMin, lhsColMax) = deref_minmax_element(lhs -> pixelsCol.begin(), lhs -> pixelsCol.end());
		std::tie(rhsColMin, rhsColMax) = deref_minmax_element(rhs -> pixelsCol.begin(), rhs -> pixelsCol.end());
		if(rhsColMin < lhsColMin)
		{
			std::swap(lhs, rhs);
			std::swap(lhsColMin, rhsColMin);
			std::swap(lhsColMax, rhsColMax);
		}
	}

	int isClusterTaggedInColumn(const Cluster& clusterField, int col)
	{
		for(int i: range(clusterField.pixelsCol.size()))
		{
			if(clusterField.pixelsCol[i] == col && clusterField.pixelsMarker[i] != 0) return 1;		
		}
		return 0;
	}

	int areClustersPair(const Cluster& first, const Cluster& second)
	{
		if(5 < std::abs(first.x - second.x)) return 0;
		const Cluster* lhs = &first;
		const Cluster* rhs = &second;
		int lhsColMin, lhsColMax, rhsColMin, rhsColMax;
		calcPairEndsAndOrderLeftRight_(lhs, rhs, lhsColMin, lhsColMax, rhsColMin, rhsColMax);
		if(lhsColMax % 2 != 1 || lhsColMax + 3 != rhsColMin) return 0;
		return 1;
	}

	int areClustersEndTaggedPair(const Cluster& first, const Cluster& second)
	{
		if(5 < std::abs(first.x - second.x)) return 0;
		const Cluster* lhs = &first;
		const Cluster* rhs = &second;
		int lhsColMin, lhsColMax, rhsColMin, rhsColMax;
		calcPairEndsAndOrderLeftRight_(lhs, rhs, lhsColMin, lhsColMax, rhsColMin, rhsColMax);
		if(lhsColMax % 2 != 1 || lhsColMax + 3 != rhsColMin) return 0;
		if(!isClusterTaggedInColumn(*lhs, lhsColMax)) return 0;
		if(!isClusterTaggedInColumn(*rhs, rhsColMin)) return 0;
		return 1;
	}

	PairCollectionType getClusterPairCollection(const std::vector<Cluster>& clusterCollection)
	{
		PairCollectionType clusterPairCollection;
		for(auto firstClusterIt = clusterCollection.begin(); firstClusterIt != clusterCollection.end(); ++firstClusterIt)
		{
			const ModuleData& mod1 = firstClusterIt -> mod_on;
			if(mod1.det != 0) continue;
			for(auto secondClusterIt = firstClusterIt + 1; secondClusterIt != clusterCollection.end(); ++secondClusterIt)
			{
				const ModuleData& mod2 = secondClusterIt -> mod_on;
				if(!(mod1 == mod2)) continue;
				if(!areClustersPair(*firstClusterIt, *secondClusterIt)) continue;
				clusterPairCollection.emplace_back(std::make_shared<Cluster>(*firstClusterIt), std::make_shared<Cluster>(*secondClusterIt));
			}
		}
		return clusterPairCollection;
	}

	// The complexity of the calculations can be decreased by looping once and looking for every weighted avarage to calculate
	float getAdcWeightedRowCoordinateInCol(const Cluster& cluster, const int& col)
	{
		int rowSum      = 0;
		int adcSum      = 0;
		for(int pixelIndex: range(cluster.pixelsCol.size())) {
			if(cluster.pixelsCol[pixelIndex] == col)
			{
				rowSum += cluster.pixelsRow[pixelIndex] * cluster.pixelsAdc[pixelIndex];
				adcSum += cluster.pixelsAdc[pixelIndex];
			}
		}
		return static_cast<float>(rowSum) / adcSum;
	}

	float getClusterIndAngle(const Cluster& cluster)
	{
		int colMin, colMax;
		std::tie(colMin, colMax) = deref_minmax_element(cluster.pixelsCol.begin(), cluster.pixelsCol.end());
		float startRow = getAdcWeightedRowCoordinateInCol(cluster, colMin);
		float endRow   = getAdcWeightedRowCoordinateInCol(cluster, colMax);
		return std::atan2(colMax - colMin, endRow - startRow);
	}

	// If this is slow, the speed can be increased to almost 2x by only looping once on the collection and only calculating
	// the min and max values required. Finding both the min and max together should not be that slow though.
	float getClusterPairAngle(const PairType& pair)
	{
		int lhsColMin, lhsColMax, rhsColMin, rhsColMax;
		std::tie(lhsColMin, lhsColMax) = deref_minmax_element(pair.first  -> pixelsCol.begin(), pair.first  -> pixelsCol.end());
		std::tie(rhsColMin, rhsColMax) = deref_minmax_element(pair.second -> pixelsCol.begin(), pair.second -> pixelsCol.end());
		int lhsFirst = (lhsColMin < rhsColMin);
		const int& startCol = lhsFirst ? lhsColMin : rhsColMin;
		const int& endCol   = lhsFirst ? rhsColMax : lhsColMax;
		float startRow = getAdcWeightedRowCoordinateInCol(*pair.first,  startCol);
		float endRow   = getAdcWeightedRowCoordinateInCol(*pair.second, endCol);
		return std::atan2(endCol - startCol, endRow - startRow); 
	}

	std::vector<float> getClusterPairAngles(const std::vector<Cluster>& clusterCollection)
	{
		std::vector<float> clusterPairAngles;
		PairCollectionType clusterPairCollection(getClusterPairCollection(clusterCollection));
		for(const PairType& clusterPair: clusterPairCollection)
		{
			clusterPairAngles.emplace_back(getClusterPairAngle(clusterPair));
		}
		return clusterPairAngles;
	}
	TH1D getClusterPairAngles(const std::vector<Cluster>& clusterCollection, const std::string& histoName, const std::string& histoTitle) 
	{
		std::vector<float> pairAngles = getClusterPairAngles(clusterCollection);
		TH1D pairAnglesPlot(histoName.c_str(), histoTitle.c_str(), 100, 0.0, 3.15);
		for(const float& angle: pairAngles)
		{
			pairAnglesPlot.Fill(angle);
		}
		return pairAnglesPlot;
	}

	std::vector<std::shared_ptr<Cluster>> getClustersWithLength(const std::vector<Cluster>& clusterCollection, const int& length) 
	{
		return(fmap(
			filter(clusterCollection, [&length] (const Cluster& cluster) {
				return cluster.sizeY == length;
			}), 
			[] (Cluster& cluster) {
				return std::make_shared<Cluster>(cluster);
			}));
	}

	// Healthy: not tagged, unhealthy: tagged
	std::vector<std::shared_ptr<Cluster>> getHealthyClustersWithLength(const std::vector<Cluster>& clusterCollection, const int& length)
	{
		return filter(getClustersWithLength(clusterCollection, length), [] (const std::shared_ptr<Cluster>& clusterPtr) 
		{
			// return !isClusterTagged(clusterPtr);
			return true;
		});
	}

	std::vector<std::shared_ptr<Cluster>> getUnhealthyClustersWithLength(const std::vector<Cluster>& clusterCollection, const int& length)
	{
		return filter(getClustersWithLength(clusterCollection, length), [] (const std::shared_ptr<Cluster>& clusterPtr) 
		{
			// return isClusterTagged(*clusterPtr);
			return false;
		});
	}

	// Merging length: Y length of the cluster pairs after merging. It is basically the lengths of the 
	// clusters added together plus two (for the dcol loss)
	PairCollectionType getClusterPairsWithMergingLength(const std::vector<Cluster>& clusterCollection, const int& mergedLength) 
	{
		return PairCollectionType(filter(
			getClusterPairCollection(clusterCollection), [&mergedLength] (PairType pair) 
			{
				return pair.first -> sizeY - pair.second -> sizeY + 2 == mergedLength;
			}));
	}
} // ClusterPairFunctions
