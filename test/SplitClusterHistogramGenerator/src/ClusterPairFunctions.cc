#include "../interface/ClusterPairFunctions.h"

namespace ClusterPairFunctions
{
	int areClustersPair(const Cluster& lhs, const Cluster& rhs)
	{
		if(5 < std::abs(rhs.x - lhs.x)) return 0;
		auto lhsColMinmax = std::minmax_element(lhs.pixelsCol.begin(), lhs.pixelsCol.end());
		const int &lhsColMin = *(lhsColMinmax.first);
		const int &lhsColMax = *(lhsColMinmax.second);
		auto rhsColMinmax = std::minmax_element(rhs.pixelsCol.begin(), rhs.pixelsCol.end());
		const int &rhsColMin = *(rhsColMinmax.first);
		const int &rhsColMax = *(rhsColMinmax.second);
		int lhsFirst = (lhsColMin < rhsColMin);
		if(lhsFirst)
		{
			if(lhsColMax % 2 != 1 || lhsColMax + 3 != rhsColMin) return 0;
		}
		else
		{
			if(rhsColMax % 2 != 1 || rhsColMax + 3 != lhsColMin) return 0;
		}
		return 1;
	}
	
	// std::vector<float> getClusterPairAngles(const std::vector<Cluster>& clusterCollection)
	// {
	// 	for(auto firstClusterIt = clusterCollection.begin(); firstClusterIt != clusterCollection.end(); ++firstClusterIt)
	// 	{
	// 		const ModuleData& mod1 = firstClusterIt -> mod_on;
	// 		if(mod1.det != 0) continue;
	// 		for(auto secondClusterIt = firstClusterIt + 1; secondClusterIt != clusterCollection.end(); ++secondClusterIt)
	// 		{
	// 			const ModuleData& mod2 = secondClusterIt -> mod_on;
	// 			if(!(mod1 == mod2)) continue;
	// 			if(!areClustersPair(*firstClusterIt, *secondClusterIt)) continue;
	// 		}
	// 	}
	// }
	// TH2D getClusterPairAngles(const std::vector<Cluster>& clusterCollection)
	// {
	// 	std::vector<float> pairAngles = getClusterPairAngles(clusterCollection);
	// 	TH2D pairAnglesPlot
	// }
} // ClusterPairFunctions
