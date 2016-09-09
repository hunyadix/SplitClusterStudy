#ifndef CLUSTER_GEOMETRY_H
#define CLUSTER_GEOMETRY_H

#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include <cmath>

namespace ClusterGeometry
{
	// Pixel distances
	int   pixelSignedXDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs);
	int   pixelSignedYDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs);
	int   pixelAbsXDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs);
	int   pixelAbsYDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs);
	int   pixelAbsSquareDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs);
	float pixelAbsDistance(const SiPixelCluster::Pixel& lhs, const SiPixelCluster::Pixel& rhs);
	// Cluster distances
	float signedDistanceX(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
	float signedDistanceY(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
	float absDistanceX(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
	float absDistanceY(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
	float distanceSquared(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
	bool  isSameCluster(const SiPixelCluster& lhs, const SiPixelCluster& rhs);
	bool  isCloserThan(float dist, const SiPixelCluster& lhs, const SiPixelCluster& rhs);
};

#endif