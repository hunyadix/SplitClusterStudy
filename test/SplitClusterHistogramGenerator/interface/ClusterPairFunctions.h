#ifndef CLUSTER_PAIR_FUNCTIONS_H
#define CLUSTER_PAIR_FUNCTIONS_H

#include "../../../interface/Cluster.h"

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace ClusterPairFunctions
{
	int areClustersPair(const Cluster& lhs, const Cluster& rhs);
} // ClusterPairFunctions

#endif